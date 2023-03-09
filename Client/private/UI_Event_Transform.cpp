#include "stdafx.h"
#include "..\public\UI_Event_Transform.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include "UI_Node.h"

CUI_Event_Transform::CUI_Event_Transform(CUI * pUI)
{
	m_szEventName	= "Transform";
	m_pParent		= pUI;

	m_bStart		= false;
	m_bFinished		= false;
	m_fSpeed		= 0.f;
	m_fAlpha		= 1.f;
	m_eTag			= TAG_END;
}

void CUI_Event_Transform::Call_Event(_uint iTag, _float fData)
{
	m_eTag = (TAG)iTag;
	m_bStart = true;
	m_bFinished = false;
	m_fTimeAcc = 0.f;
}

HRESULT CUI_Event_Transform::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_Transform::Late_Tick(_float fTimeDelta)
{
	if (!m_bStart)
		return S_OK;

	if (m_bFinished)
	{
		m_bStart = false;
		return S_OK;
	}


	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc <= m_fTime)
	{

	}
	

	return S_OK;
}

HRESULT CUI_Event_Transform::SetUp_ShaderResources(CShader * pShader)
{
	return S_OK;
}

void CUI_Event_Transform::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Event: "); ImGui::SameLine();
	ImGui::Text(m_szEventName);


	/* Duration */
	static float fDuration;
	fDuration = m_fTime;
	if (ImGui::SliderFloat("Transform Duration", &fDuration, 0.f, 30.f))
		m_fTime = fDuration;

}

HRESULT CUI_Event_Transform::Save_Data(Json * json)
{
	if (json == nullptr)
		return E_FAIL;

	//(*json)["Duration"] = m_fTime;
	//for (_uint i = 0; i < m_iTextureNum; ++i)
	//{
	//	(*json)["FramesX"].push_back(m_vecFramesX[i]);
	//	(*json)["FramesY"].push_back(m_vecFramesY[i]);
	//}

	return S_OK;
}

HRESULT CUI_Event_Transform::Load_Data(wstring fileName)
{
	Json jLoad;

	wstring name = L"../Bin/Data/UI/";
	name += fileName;
	name += L"_Property.json";
	string filePath;
	filePath.assign(name.begin(), name.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	//if (jLoad.contains("Duration"))
	//	jLoad["Duration"].get_to<_float>(m_fTime);

	//_uint i = 0;
	//if (jLoad.contains("FramesX"))
	//	for (_int iFrame : jLoad["FramesX"])
	//	{
	//		m_vecFramesX[i] = iFrame;
	//		++i;
	//	}

	return S_OK;
}

_bool CUI_Event_Transform::Appear(_float fTimeDelta)
{
	return _bool();
}

_bool CUI_Event_Transform::DisAppear(_float fTimeDelta)
{
	return _bool();
}

CUI_Event_Transform * CUI_Event_Transform::Create(CUI * pUI)
{
	CUI_Event_Transform* pInstance = new CUI_Event_Transform(pUI);
	return pInstance;
}

CUI_Event_Transform * CUI_Event_Transform::Create(wstring fileName, CUI * pUI)
{
	CUI_Event_Transform* pInstance = new CUI_Event_Transform(pUI);
	if (pInstance != nullptr)
		pInstance->Load_Data(fileName);
	return pInstance;
}

void CUI_Event_Transform::Free()
{
	__super::Free();
}
