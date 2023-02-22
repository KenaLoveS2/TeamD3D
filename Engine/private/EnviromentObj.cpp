#include "stdafx.h"
#include "..\public\EnviromentObj.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Enviroment_Manager.h"

CEnviromentObj::CEnviromentObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pEnviroment_Manager(CEnviroment_Manager::GetInstance())
{
}

CEnviromentObj::CEnviromentObj(const CEnviromentObj & rhs)
	: CGameObject(rhs)
	, m_pEnviroment_Manager(rhs.m_pEnviroment_Manager)
{	
}

HRESULT CEnviromentObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;	

	return S_OK;
}

HRESULT CEnviromentObj::Initialize(void * pArg)
{

	if (pArg) 
	{		
		ENVIROMENT_DESC* Desc = reinterpret_cast<ENVIROMENT_DESC*>(pArg);

		m_EnviromentDesc.szProtoObjTag = Desc->szProtoObjTag;
		m_EnviromentDesc.szModelTag = Desc->szModelTag;
		m_EnviromentDesc.szTextureTag = Desc->szTextureTag;
		m_EnviromentDesc.iRoomIndex = Desc->iRoomIndex;

		m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = 90.f;
		m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	}
	else
	{
		m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = 90.f;
		m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	}

	if (FAILED(__super::Initialize(&m_EnviromentDesc.ObjectDesc))) return E_FAIL;


	HRESULT SetUp_Component();


	return S_OK;
}

void CEnviromentObj::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEnviromentObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_bRenderActive = m_pEnviroment_Manager->Is_RenderIndex(m_EnviromentDesc.iRoomIndex);
}

HRESULT CEnviromentObj::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CEnviromentObj::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

//void CEnviromentObj::Imgui_RenderComponentProperties()
//{
//	__super::Imgui_RenderComponentProperties();
//
//	for (const auto& com : m_AdditionalComponent)
//	{
//		ImGui::Separator();
//		char szName[128];
//		CUtile::WideCharToChar(com.first, szName);
//
//		ImGui::Text("%s", szName);
//		com.second->Imgui_RenderProperty();
//	}
//
//}
//
//HRESULT CEnviromentObj::Add_AdditionalComponent(const _tchar * pComTag, CComponent * pComponent)
//{
//	CComponent* pAfterAddCom = Find_AdditionalCom(pComTag);
//	assert(pAfterAddCom == nullptr && "CEnviromentObj::Add_AdditionalComponent"); // must nullptr !!!
//
//	m_AdditionalComponent.emplace(pComTag, pComponent);
//
//	return S_OK;
//}
//
//CComponent * CEnviromentObj::Find_AdditionalCom(const _tchar * pComTag)
//{
//	auto &Pair = find_if(m_AdditionalComponent.begin(), m_AdditionalComponent.end(), CTag_Finder(pComTag));
//
//	if (Pair == m_AdditionalComponent.end())
//		return nullptr;
//
//	return Pair->second;
//}

void CEnviromentObj::Free()
{
	__super::Free();
	
	//for (auto &pAddCom : m_AdditionalComponent)
	//	Safe_Release(pAddCom.second);
	//m_AdditionalComponent.clear();
}