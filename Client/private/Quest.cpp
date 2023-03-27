#include "stdafx.h"
#include "..\public\Quest.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>

CQuest::CQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_iCurSubIndex(-1)
{
}

CQuest::CQuest(const CQuest & rhs)
	: CGameObject(rhs)
	, m_iCurSubIndex(-1)
{
}

HRESULT CQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuest::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Load_File(((QUESTDESC*)pArg)->filePath)))
	{
		MSG_BOX("Failed To Load : Quest");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To Setup Components : Quest");
		return E_FAIL;
	}

	return S_OK;
}

void CQuest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;

	//CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	//CUI_ClientManager::UI_FUNCTION funcLevelup = CUI_ClientManager::FUNC_LEVELUP;
	//CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;

	//static _float fNum = 0.f;

	//if (pGameInstance->Key_Down(DIK_L))
	//{
	//	m_QuestDelegator.broadcast(eQuest, funcLevelup, fNum);
	//}

	//RELEASE_INSTANCE(CGameInstance);


}

void CQuest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CQuest::Render()
{
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CQuest::Load_File(const _tchar * fileName)
{
	Json	jLoad;

	wstring name = L"../Bin/Data/Quest/";
	name += fileName;
	string filePath;
	filePath.assign(name.begin(), name.end());
	m_FilePath = filePath;

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring_convert<convert_type> utf8_conv;
	
	string strMain;
	jLoad["MainQuest"].get_to<string>(strMain);
	m_Main.first = CUtile::utf8_to_wstring(strMain);

	size_t iNumSubs;
	jLoad["NumSubs"].get_to<size_t>(iNumSubs);

	for (size_t i = 1; i <= iNumSubs; ++i)
	{
		for (auto jSub : jLoad["SubQuest"])
		{
			string str = "sub" + to_string(i);
			string subQuest;
			jSub[str].get_to<string>(subQuest);

			wstring wstr = CUtile::utf8_to_wstring(subQuest);
			m_vecSub.push_back({ wstr,false });
		}
	}

	return S_OK;
}

HRESULT CQuest::Save_File()
{
	Json	json;

	//using convert_type = codecvt_utf8<wchar_t>;

	wstring main = m_Main.first;
	//wstring_convert<convert_type> utf8_conv;
	json["MainQuest"] = CUtile::wstring_to_utf8(main);
	json.dump();

	json["NumSubs"] = m_vecSub.size();

	Json jSub;
	_int i = 1;
	for (auto pair : m_vecSub)
	{
		string str = "sub" + to_string(i);
		jSub[str] = CUtile::wstring_to_utf8(pair.first);
		json.dump();
		i++;
	}
	json["SubQuest"].push_back(jSub);


	ofstream	file(m_FilePath);
	file << json;
	file.close();


	return S_OK;
}

HRESULT CQuest::SetUp_Components()
{
	return S_OK;
}

HRESULT CQuest::SetUp_ShaderResources()
{

	return S_OK;
}

CQuest * CQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CQuest*		pInstance = new CQuest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CQuest::Clone(void * pArg)
{
	CQuest*		pInstance = new CQuest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuest::Free()
{
	__super::Free();

	//Save_File();

	m_vecSub.clear();
}
