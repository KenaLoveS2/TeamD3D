#include "stdafx.h"
#include "..\public\UI_Event_Transform.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include "UI_Node.h"

CUI_Event_Transform::CUI_Event_Transform(CUI * pUI)
{
	m_szEventName			= "Transform";
	m_pParent				= pUI;

	m_bStart				= false;
	m_bFinished				= false;
	m_eTag					= TAG_END;

	m_vSourScale			= { 1.f ,1.f, 1.f };
	m_vSourTranslation		= { 0.f, 0.f, 0.f };

	m_vDestScale			= { 1.f ,1.f, 1.f };
	m_vDestTranslation		= { 0.f, 0.f, 0.f };

	m_vRotAxis				= { 0.f, 1.f ,0.f };
	m_fRotAngle				= 0.f;

	m_fScaleSpeed			= 0.f;
	m_fTransSpeed			= 0.f;
	m_fRotSpeed				= 0.f;

	m_vScaleNow				= m_vSourScale;
	m_vTranslationNow		= m_vSourTranslation;
	m_fRotAngleNow			= 0.f;
}

void CUI_Event_Transform::Call_Event(_uint iTag, _float fData)
{
/*	switch (iTag)
	{
	case TAG_SCALE_BY_PERCENT: *//* 일단 HP 바 전용으로 만든다 */
		_float4x4 matLocal = m_pParent->Get_LocalMatrixFloat4x4();

		m_vSourScale = {matLocal._11, matLocal._22, matLocal._33};//m_pParent->Get_OriginalSettingScale();
		m_vDestScale = fData * m_vSourScale;

		m_vDestTranslation = { matLocal._41, matLocal._42, matLocal._43, 1.f };
		_float3 vDestTrans = m_vSourScale *(1 - fData);
		m_vDestTranslation += { vDestTrans.x, vDestTrans.y, vDestTrans.z, 0.f };
	//	break;
	//}

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
	

	m_fScaleSpeed = 0.3f;
	m_fTransSpeed = 0.3f;
	m_fTimeAcc += fTimeDelta;
	if (m_vDestScale.x > m_vScaleNow.x)
	{
		m_vScaleNow.x += 0.1f * m_fScaleSpeed * m_fTimeAcc;
		m_vTranslationNow.x += 0.1f * m_fTransSpeed * m_fTimeAcc;
		_float4x4 matLocal = m_pParent->Get_LocalMatrixFloat4x4();
		matLocal._11 = m_vScaleNow.x;
		matLocal._41 = m_vTranslationNow.x;
		m_pParent->Set_LocalMatrix(matLocal);
	}
	else
	{
		m_bFinished = true;
		_float4x4 matLocal = m_pParent->Get_LocalMatrixFloat4x4();
		matLocal._11 = m_vDestScale.x;
		matLocal._41 = m_vTranslationNow.x;
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

	/* Type */
	//TAG_SCALE_BY_PERCENT, TAG_SCALE_BY_TIME, TAG_TRANS_BY_PERCENT, TAG_TRANS_BY_TIME

	/* 귀찮으니까 TAG_SCALE_BY_PERCENT 기준으로 만듦 나중에 이거 또 써야 하는 일이 있으면 범용성있게 만들자고 */

	//m_vDestScale;
	//m_vDestTranslation;
	//
	//m_vRotAxis;
	//m_fRotAngle;
	//
	//m_fScaleSpeed;
	//m_fTransSpeed;
	//m_fRotSpeed;


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
