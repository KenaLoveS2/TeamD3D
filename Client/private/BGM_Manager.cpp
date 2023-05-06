#include "stdafx.h"
#include "BGM_Manager.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>

IMPLEMENT_SINGLETON(CBGM_Manager)

CBGM_Manager::CBGM_Manager()
	: m_pSoundManager(CSound_Manager::GetInstance())
{
	Safe_AddRef(m_pSoundManager);
}

HRESULT CBGM_Manager::Initialize(const string & strFilePath)
{
	NULL_CHECK_RETURN(m_pSoundManager, E_FAIL);

	m_Channel = m_pSoundManager->Get_Channel((_uint)BGMChannel);
	NULL_CHECK_RETURN(m_Channel, E_FAIL);
	
	if (strFilePath != "")
		Load_FromFile(strFilePath);

	//m_fBGMVolume = 0.f;

	return S_OK;
}

void CBGM_Manager::Tick(_float fTimeDelta)
{
	m_eCurLevel = (LEVEL)CGameInstance::GetInstance()->Get_CurLevelIndex();
	if (m_eCurLevel == LEVEL_LOADING || m_eCurLevel == LEVEL_END)
	{
		m_pSoundManager->Stop_Sound((_uint)BGMChannel);
		m_eCurState = CBGM_Manager::FIELD_STATE_END;
		m_ePreState = CBGM_Manager::FIELD_STATE_END;
		return;
	}

	if (m_eCurState == CBGM_Manager::FIELD_FROM_BATTLE || m_eCurState == CBGM_Manager::FIELD_LEVELUP)
	{
		if (m_pSoundManager->Is_StopSound(BGMChannel))
			Change_FieldState(CBGM_Manager::FIELD_IDLE);
	}

	if (m_ePreState != m_eCurState)
	{
		m_ePreState = m_eCurState;
	}
	else
	{
		_bool bStateFlag = m_eCurState != CBGM_Manager::FIELD_INTO_BATTLE && m_eCurState != CBGM_Manager::FIELD_FROM_BATTLE;
		if (m_pSoundManager->Is_StopSound(BGMChannel) == true && bStateFlag == true)
			Change_BGM(m_eCurState);
	}
}

void CBGM_Manager::Change_FieldState(FIELD_STATE eState)
{
	if (eState == m_eCurState || eState >= CBGM_Manager::FIELD_STATE_END)
		return;

	_uint	iSize = (_uint)m_BGMs[eState].size();
	if (iSize == 0)
		return;

	_uint	iRand = (_uint)floor(CUtile::Get_RandomFloat(0.f, _float(iSize) - 0.1f));
	
	auto	iter = m_BGMs[eState].begin();
	for (_uint i = 0; i < iRand; ++i)
		iter++;

	m_pSoundManager->Stop_Sound(BGMChannel);
	m_pSoundManager->Play_ManualSound(iter->first, m_fBGMVolume, false, BGMChannel);

	m_eCurState = eState;
}

void CBGM_Manager::Change_BGM(FIELD_STATE eState)
{
	if (m_eCurState != eState || eState == CBGM_Manager::FIELD_STATE_END)
		return;

	_uint	iSize = (_uint)m_BGMs[eState].size();
	if (iSize == 0)
		return;
	else if (iSize == 1)
	{
		if (m_pSoundManager->Is_StopSound(BGMChannel) == false)
			return;
		
		_uint	iRand = (_uint)floor(CUtile::Get_RandomFloat(0.f, _float(iSize) - 0.1f));

		auto	iter = m_BGMs[eState].begin();
		for (_uint i = 0; i < iRand; ++i)
			iter++;

		m_pSoundManager->Stop_Sound(BGMChannel);
		m_pSoundManager->Play_ManualSound(iter->first, m_fBGMVolume, false, BGMChannel);
	}
	else
	{
		if (m_pSoundManager->Is_StopSound(BGMChannel) == false)
			return;

		_uint	iRand = (_uint)floor(CUtile::Get_RandomFloat(0.f, _float(iSize) - 0.1f));

		auto	iter = m_BGMs[eState].begin();
		for (_uint i = 0; i < iRand; ++i)
			iter++;

		m_pSoundManager->Play_ManualSound(iter->first, m_fBGMVolume, false, BGMChannel);
	}
}

HRESULT CBGM_Manager::Load_FromFile(const string& strFilePath)
{
	Json	jBGMInfo;

	ifstream file(strFilePath.c_str());
	file >> jBGMInfo;
	file.close();

	string	strStateName = "";
	string	strAudioName = "";
	FIELD_STATE	eState = CBGM_Manager::FIELD_STATE_END;
	CSound_Manager::SOUNDS*		pBGMs = nullptr;

	for (auto jState : jBGMInfo)
	{
		eState = CBGM_Manager::FIELD_STATE_END;
		pBGMs = nullptr;

		jState["0. State Name"].get_to<string>(strStateName);

		if (strStateName == "IDLE")
			eState = CBGM_Manager::FIELD_IDLE;
		else if (strStateName == "VILLAGE")
			eState = CBGM_Manager::FIELD_VILLAGE;
		else if (strStateName == "SHOP")
			eState = CBGM_Manager::FIELD_SHOP;
		else if (strStateName == "PUZZLE")
			eState = CBGM_Manager::FIELD_PUZZLE;
		else if (strStateName == "LEVEL_UP")
			eState = CBGM_Manager::FIELD_LEVELUP;
		else if (strStateName == "INTO_BATTLE")
			eState = CBGM_Manager::FIELD_INTO_BATTLE;
		else if (strStateName == "FROM_BATTLE")
			eState = CBGM_Manager::FIELD_FROM_BATTLE;
		else if (strStateName == "BATTLE")
			eState = CBGM_Manager::FIELD_BATTLE;
		else if (strStateName == "BOSS_BATTLE_WARRIOR_PHASE_1")
			eState = CBGM_Manager::FIELD_BOSS_BATTLE_WARRIOR_PHASE_1;
		else if (strStateName == "BOSS_BATTLE_WARRIOR_PHASE_2")
			eState = CBGM_Manager::FIELD_BOSS_BATTLE_WARRIOR_PHASE_2;
		else if (strStateName == "BOSS_BATTLE_HUNTER")
			eState = CBGM_Manager::FIELD_BOSS_BATTLE_HUNTER;
		else if (strStateName == "BOSS_BATTLE_SHAMAN")
			eState = CBGM_Manager::FIELD_BOSS_BATTLE_SHAMAN;
		else
			return E_FAIL;

		pBGMs = &m_BGMs[eState];

		for (_uint i = 0; i < (_uint)jState["1. File Names"].size(); ++i)
		{
			jState["1. File Names"][i].get_to<string>(strAudioName);
			
			_tchar* pAudioName = CUtile::CharToWideChar(const_cast<char*>(strAudioName.c_str()));
			m_pSoundManager->Copy_Sound(pAudioName, pBGMs);

			delete [] pAudioName;
			pAudioName = nullptr;
		}
	}

	return S_OK;
}

void CBGM_Manager::Free()
{
	Safe_Release(m_pSoundManager);
}