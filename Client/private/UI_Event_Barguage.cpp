#include "stdafx.h"
#include "..\public\UI_Event_Barguage.h"
#include "GameInstance.h"
#include "Json/json.hpp"

CUI_Event_Barguage::CUI_Event_Barguage()
{
	m_szEventName = "BarGuage";
	m_iRenderPass = 7;

	m_vAcceleration = {0.f, 0.f};
	m_vSpeed		= {0.f, 0.f};
	m_vMinColor		= {0.1f, 0.3f, 0.5f, 1.0f};
	m_vColor		= {1.0f, 1.0f, 1.0f, 1.0f};
	m_fGuage		= 1.f;
}

HRESULT CUI_Event_Barguage::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > m_fTime)
	{
		m_vSpeed.x += m_vAcceleration.x * fTimeDelta;
		m_vSpeed.y += m_vAcceleration.y * fTimeDelta;
		m_fTimeAcc = 0.f;
	}

	return S_OK;
}

HRESULT CUI_Event_Barguage::Late_Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_Barguage::SetUp_ShaderResources(CShader * pShader)
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

	return S_OK;
}

void CUI_Event_Barguage::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Event: "); ImGui::SameLine(); 
	ImGui::Text(m_szEventName);

	/* RenderPass */
	ImGui::Text("Recommended RenderPass:"); ImGui::SameLine();
	_int iPass = (_int)m_iRenderPass;
	ImGui::InputInt("RenderPass", &iPass);

	/* To Fine-Tuning */
	static float fDuration;
	fDuration = m_fTime;
	if (ImGui::SliderFloat("Duration", &fDuration, 0.f, 30.f))
	{
		m_fTime = fDuration;
	}

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

	static float fUVSpeed[2];
	fUVSpeed[0] = m_vAcceleration.x;
	fUVSpeed[1] = m_vAcceleration.y;
	if (ImGui::SliderFloat("UV Speed", fUVSpeed, -50.f, 50.f))
	{
		m_vAcceleration.x = fUVSpeed[0];
		m_vAcceleration.y = fUVSpeed[1];
	}
}

void CUI_Event_Barguage::Call_Event(_float fData)
{
	Set_Guage(fData);
}

HRESULT CUI_Event_Barguage::Save_Data(Json* json)
{
	if (json == nullptr)
		return E_FAIL;

	(*json)["Duration"] = m_fTime;

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

HRESULT CUI_Event_Barguage::Load_Data(wstring fileName)
{
	Json	jLoad;

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


	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);
	jLoad["Duration"].get_to<_float>(m_fTime);

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

CUI_Event_Barguage * CUI_Event_Barguage::Create(wstring fileName)
{
	CUI_Event_Barguage* pInstance = new CUI_Event_Barguage();
	if (pInstance != nullptr)
		pInstance->Load_Data(fileName);

	//if (pInstance->Load_Data(fileName))
	//{
	//	MSG_BOX("Failed To Create : CUI_Event_Barguage");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

void CUI_Event_Barguage::Free()
{
}
