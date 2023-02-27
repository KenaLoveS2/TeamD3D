#include "stdafx.h"
#include "..\public\UI_Event_Transform.h"
#include "GameInstance.h"
#include "UI.h"
#include "Json/json.hpp"

CUI_Event_Transform::CUI_Event_Transform(CUI * pUI)
{
	m_szEventName			= "Transform";
	m_pParent				= pUI;

	m_bStart				= false;
	m_bFinished				= false;

	m_vSourScale			= { 1.f ,1.f, 1.f };
	m_vSourTranslation		= { 0.f, 0.f, 0.f };

	m_vDestScale			= { 1.f ,1.f, 1.f };
	m_vDestTranslation		= { 0.f, 0.f, 0.f };

	m_fScaleSpeed			= 0.f;
	m_fTransSpeed			= 0.f;
}

void CUI_Event_Transform::Call_Event(_uint iTag, _float fData)
{
	switch (iTag)
	{
	case TAG_SCALE_BY_PERCENT:
		m_vDestScale = fData * m_pParent->Get_OriginalSettingScale();
		break;
	}

	m_bStart = true;
	m_bFinished = false;
	m_fTimeAcc = 0.f;
}

HRESULT CUI_Event_Transform::Tick(_float fTimeDelta)
{
	if (m_bStart)
	{
		m_fTimeAcc += fTimeDelta;
		_float3 vScaleNow = m_pParent->Get_WorldScale();
	//	if(vScaleNow.x >= m_vDestScale)
	}

	return S_OK;
}

HRESULT CUI_Event_Transform::Late_Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_Transform::SetUp_ShaderResources(CShader * pShader)
{
	return S_OK;
}

void CUI_Event_Transform::Imgui_RenderProperty()
{
}

HRESULT CUI_Event_Transform::Save_Data(Json * json)
{
	return S_OK;
}

HRESULT CUI_Event_Transform::Load_Data(wstring fileName)
{
	return S_OK;
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
}
