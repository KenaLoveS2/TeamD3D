#include "stdafx.h"
#include "..\public\UI_Event_Guage.h"
#include "GameInstance.h"
#include "Json/json.hpp"

CUI_Event_Guage::CUI_Event_Guage()
	: m_iRenderPass(7)
	, m_vAcceleration{ 0.f, 0.f }
	, m_vSpeed{ 0.f, 0.f }
	, m_vMinColor{ 0.1f, 0.3f, 0.5f, 1.0f }
	, m_vColor{ 1.0f, 1.0f, 1.0f, 1.0f }
	, m_fGuage{ 1.f }
	, m_fGuageNew{ 1.f }
	, m_fGuageSpeed{ 2.f }
	, m_bReArrangeSettingOn{ false }
{
	m_szEventName = "Guage";
}

_bool CUI_Event_Guage::Is_Zero()
{
	if (m_fGuage <= 0.f)
		return true;
	else
		return false;
}

HRESULT CUI_Event_Guage::Tick(_float fTimeDelta)
{
	/* Verify Immediately Changing Guage */
	if (m_fGuageNew == -1.f)
	{
		m_fGuageNew = 0.f;
		m_fGuage = 0.f;
	}

	return S_OK;
}

HRESULT CUI_Event_Guage::Late_Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > m_fTime)
	{
		m_vSpeed.x += m_vAcceleration.x * fTimeDelta;
		m_vSpeed.y += m_vAcceleration.y * fTimeDelta;
		m_fTimeAcc = 0.f;
	}


	if (m_fGuageNew > m_fGuage)
	{
		m_fGuage += (0.01f * m_fGuageSpeed);
		if (0.0001f > m_fGuageNew - m_fGuage)
			m_fGuage = m_fGuageNew;
	}
	else if (m_fGuageNew < m_fGuage)
	{
		m_fGuage -= (0.01f * m_fGuageSpeed);
		if (0.0001f > m_fGuage - m_fGuageNew)
			m_fGuage = m_fGuageNew;
	}
	return S_OK;
}

HRESULT CUI_Event_Guage::SetUp_ShaderResources(CShader * pShader)
{
	if (FAILED(pShader->Set_RawValue("g_fSpeedX", &m_vSpeed.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_fSpeedY", &m_vSpeed.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vMinColor", &m_vMinColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_fAmount", &m_fGuage, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_fEnd", &m_fGuageNew, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_Event_Guage::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Event: "); ImGui::SameLine();
	ImGui::Text(m_szEventName);

	/* To Fine-Tuning */
	static float fDuration;
	fDuration = m_fTime;
	if (ImGui::SliderFloat("Duration", &fDuration, 0.f, 30.f))
	{
		m_fTime = fDuration;
	}
	static float fUVSpeed[2];
	fUVSpeed[0] = m_vAcceleration.x;
	fUVSpeed[1] = m_vAcceleration.y;
	if (ImGui::SliderFloat("UV Speed", fUVSpeed, -50.f, 50.f))
	{
		m_vAcceleration.x = fUVSpeed[0];
		m_vAcceleration.y = fUVSpeed[1];
	}

	static float fGuageSpeed;
	fGuageSpeed = m_fGuageSpeed;
	if (ImGui::SliderFloat("GuageSpeed", &fGuageSpeed, 0.f, 100.f))
		m_fGuageSpeed = fGuageSpeed;

	static float vColor[4];
	vColor[0] = m_vColor.x;
	vColor[1] = m_vColor.y;
	vColor[2] = m_vColor.z;
	vColor[3] = m_vColor.w;
	if (ImGui::SliderFloat4("Color", vColor, 0.f, 1.f))
	{
		m_vColor.x = vColor[0];
		m_vColor.y = vColor[1];
		m_vColor.z = vColor[2];
		m_vColor.w = vColor[3];
	}

	static float vMinColor[4];
	vMinColor[0] = m_vMinColor.x;
	vMinColor[1] = m_vMinColor.y;
	vMinColor[2] = m_vMinColor.z;
	vMinColor[3] = m_vMinColor.w;
	if (ImGui::SliderFloat4("MinColor", vMinColor, 0.f, 1.f))
	{
		m_vMinColor.x = vMinColor[0];
		m_vMinColor.y = vMinColor[1];
		m_vMinColor.z = vMinColor[2];
		m_vMinColor.w = vMinColor[3];
	}


}

void CUI_Event_Guage::Call_Event(_float fData)
{
	if (m_bReArrangeSettingOn)
	{
		m_fGuageNew = fData;
		m_bReArrangeSettingOn = false;
	}
	else if(m_fGuage == m_fGuageNew)
		m_fGuageNew = fData;

}

void CUI_Event_Guage::Set_InitState(_float fInitialGuage)
{
	m_fGuage = fInitialGuage;
	m_fGuageNew = fInitialGuage;
}

void CUI_Event_Guage::Set_FullState()
{
	m_fGuageNew = 1.0f;
	m_fGuage = m_fGuageNew;
}

HRESULT CUI_Event_Guage::Save_Data(Json* json)
{
	if (json == nullptr)
		return E_FAIL;

	(*json)["Duration"] = m_fTime;
	(*json)["GuageSpeed"] = m_fGuageSpeed;

	_float fValue = 0.f;
	for (int i = 0; i < 2; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&m_vAcceleration + i, sizeof(float));
		(*json)["Acceleration"].push_back(fValue);
	}

	for (int i = 0; i < 2; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&m_vSpeed + i, sizeof(float));
		(*json)["Speed"].push_back(fValue);
	}

	for (int i = 0; i < 4; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&m_vColor + i, sizeof(float));
		(*json)["Color"].push_back(fValue);
	}

	for (int i = 0; i < 4; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&m_vMinColor + i, sizeof(float));
		(*json)["MinColor"].push_back(fValue);
	}

	return S_OK;
}

HRESULT CUI_Event_Guage::Load_Data(wstring fileName)
{
	Json	jLoad;

	wstring name = L"../Bin/Data/UI/";
	name += fileName;
	name += L"_Property.json";
	string filePath = CUtile::wstring_to_utf8(name);
	//filePath.assign(name.begin(), name.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();


	jLoad["Duration"].get_to<_float>(m_fTime);
	jLoad["GuageSpeed"].get_to<_float>(m_fGuageSpeed);

	int i = 0;
	for (float fElement : jLoad["Acceleration"])
		memcpy(((float*)&m_vAcceleration) + (i++), &fElement, sizeof(float));

	i = 0;
	for (float fElement : jLoad["Speed"])
		memcpy(((float*)&m_vSpeed) + (i++), &fElement, sizeof(float));

	i = 0;
	for (float fElement : jLoad["Color"])
		memcpy(((float*)&m_vColor) + (i++), &fElement, sizeof(float));

	i = 0;
	for (float fElement : jLoad["MinColor"])
		memcpy(((float*)&m_vMinColor) + (i++), &fElement, sizeof(float));


	return S_OK;
}

void CUI_Event_Guage::Initialize(_float4 vMinColor, _float4 vColor)
{
	m_vMinColor = vMinColor;
	m_vColor = vColor;
}

CUI_Event_Guage * CUI_Event_Guage::Create()
{
	CUI_Event_Guage* pInstance = new CUI_Event_Guage();
	return pInstance;
}

CUI_Event_Guage * CUI_Event_Guage::Create(wstring fileName)
{
	CUI_Event_Guage* pInstance = new CUI_Event_Guage();
	if (pInstance != nullptr)
		pInstance->Load_Data(fileName);

	return pInstance;
}

CUI_Event_Guage * CUI_Event_Guage::Create(_float4 vMinColor, _float4 vColor)
{
	CUI_Event_Guage* pInstance = new CUI_Event_Guage();
	if (nullptr != pInstance)
		pInstance->Initialize(vMinColor, vColor);

	return pInstance;
}

void CUI_Event_Guage::Free()
{
}
