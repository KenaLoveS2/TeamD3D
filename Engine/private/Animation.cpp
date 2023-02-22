#include "stdafx.h"
#include "..\public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "Function_Manager.h"
#include "Utile.h"

CAnimation::CAnimation()
	: m_isLooping(true)
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_isLooping(true)
	, m_pModel(rhs.m_pModel)
{
	strcpy_s(m_szName, rhs.m_szName);
	m_Duration = rhs.m_Duration;
	m_TickPerSecond = rhs.m_TickPerSecond;

	m_iNumChannels = rhs.m_iNumChannels;
	for (_uint i = 0; i < m_iNumChannels; i++)
		m_Channels.push_back(rhs.m_Channels[i]->Clone());
}

HRESULT CAnimation::Save_Animation(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = _uint(strlen(m_szName)) + 1;
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_szName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	for (auto& pChannel : m_Channels)
	{
		if (FAILED(pChannel->Save_Channel(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Channel");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CAnimation::Load_Animation(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);

	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	strcpy_s(m_szName, pName);

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	m_Channels.reserve(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(nullptr, m_pModel);
		NULL_CHECK_RETURN(pChannel, E_FAIL);
		FAILED_CHECK_RETURN(pChannel->Load_Channel(hFile, dwByte), E_FAIL);

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Add_Event(_float fPlayTime, const string & strFuncName)
{
	if (fPlayTime > m_Duration)
		return E_FAIL;

	m_mapEvent.emplace(fPlayTime, strFuncName);

	return S_OK;
}

HRESULT CAnimation::Initialize_Prototype(HANDLE hFile, CModel* pModel)
{
	m_pModel = pModel;

	if (hFile == nullptr)
		return S_OK;

	_ulong dwByte = 0;
	_uint iLen = 0;
	ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, m_szName, iLen + 1, &dwByte, nullptr);
	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(hFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(void* pArg)
{
	return S_OK;
}

void CAnimation::ImGui_RenderEvents(_int & iSelectEvent)
{
	_uint		iEventCnt = (_uint)m_mapEvent.size();
	char**		ppEvents = new char*[iEventCnt];

	string		strShowTag = "";
	_uint		i = 0;
	for (auto& Pair : m_mapEvent)
	{
		strShowTag = to_string(Pair.first) + " | " + Pair.second;
		ppEvents[i] = new char[(_uint)strShowTag.length() + 1];
		strcpy_s(ppEvents[i++], (_uint)strShowTag.length() + 1, strShowTag.c_str());
	}

	ImGui::ListBox("Events List", &iSelectEvent, ppEvents, iEventCnt);

	if (iSelectEvent != -1)
	{
		if (ImGui::Button("Erase"))
		{
			auto	iter = m_mapEvent.begin();
			for (_uint i = 0; i < iSelectEvent; ++i)
				++iter;

			m_mapEvent.erase(iter);
			iSelectEvent = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Exit"))
		{
			iSelectEvent = -1;
		}
	}

	for (_uint j = 0; j < iEventCnt; ++j)
		Safe_Delete_Array(ppEvents[j]);
	Safe_Delete_Array(ppEvents);
}

void CAnimation::Update_Bones(_float fTimeDelta)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
			m_isFinished = false;
		else
			return;
	}

	_float		fLastPlayTime = (_float)m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	Call_Event(fLastPlayTime, fTimeDelta);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		m_Channels[i]->Update_TransformMatrix((_float)m_PlayTime);
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;
}

void CAnimation::Update_Bones_Blend(_float fTimeDelta, _float fBlendRatio)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
			m_isFinished = false;
		else
			return;
	}

	_float		fLastPlayTime = (_float)m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	Call_Event(fLastPlayTime, fTimeDelta);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		m_Channels[i]->Blend_TransformMatrix((_float)m_PlayTime, fBlendRatio);
	}
}

void CAnimation::Update_Bones_Addtive(_float fTimeDelta, _float fRatio)
{
	_float		fLastPlayTime = (_float)m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	Call_Event(fLastPlayTime, fTimeDelta);

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Additive_TransformMatrix((_float)m_PlayTime, fRatio);
		m_Channels[i]->Reset_KeyFrameIndex();
	}
}

void CAnimation::Reset_Animation()
{
	for (auto& pChannel : m_Channels)
		pChannel->Reset_KeyFrameIndex();

	m_PlayTime = 0.0;
	m_isFinished = false;
}

void CAnimation::Call_Event(_float fLastPlayTime, _float fTimeDelta)
{
	if (m_mapEvent.empty() == true || fTimeDelta == 0.f)
		return;

	NULL_CHECK_RETURN(m_pModel, );

	for (auto& Pair : m_mapEvent)
	{
		_tchar		wszFunctionTag[128] = L"";

		if (fLastPlayTime <= Pair.first && Pair.first <= m_PlayTime)
		{
			wsprintf(wszFunctionTag, L"");
			CUtile::CharToWideChar(Pair.second.c_str(), wszFunctionTag);
			CFunction_Manager::GetInstance()->Call_Function((CBase*)m_pModel->Get_Owner(), wszFunctionTag, fTimeDelta);
		}
	}
}

CAnimation * CAnimation::Create(HANDLE hFile, CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone(void *pArg)
{
	CAnimation* pInstance = new CAnimation(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

HRESULT CAnimation::SetUp_ChannelsBonePtr(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		if (FAILED(m_Channels[i]->SetUp_BonePtr(pModel)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation::Synchronization_ChannelsBonePtr(CModel * pModel, const string & strRootNodeName)
{
	for (auto iter = m_Channels.begin(); iter != m_Channels.end();)
	{
		if (iter == m_Channels.begin())
			(*iter)->Set_ChannelName(strRootNodeName);

		if ((*iter)->Synchronization_BonePtr(pModel) == S_FALSE)
		{
			Safe_Release(*iter);
			iter = m_Channels.erase(iter);
		}
		else
			++iter;
	}

	m_iNumChannels = (_uint)m_Channels.size();

	return S_OK;
}


