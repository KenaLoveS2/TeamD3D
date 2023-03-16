#include "stdafx.h"
#include "..\public\UI_Event_Fade.h"
#include "GameInstance.h"
#include "Json/json.hpp"

CUI_Event_Fade::CUI_Event_Fade()
	: m_bStart(false)
	, m_fAlpha(0.0f)
	, m_fSpeed(0.0f)
	, m_bInOut(false)
{
	m_szEventName = "Fade";
	m_iRenderPass = 13;
}

void CUI_Event_Fade::Call_Event(_bool bStart)
{
	m_bStart = true;
	m_fTimeAcc = 0.0f;
	m_fAlpha = 0.0f;
	m_bInOut = true;
}

void CUI_Event_Fade::FadeOut()
{
	m_bStart = true;
	m_fAlpha = 1.0f;
	m_bInOut = false;
	m_fTimeAcc = 0.0f;
}

HRESULT CUI_Event_Fade::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_Fade::Late_Tick(_float fTimeDelta)
{
	if (!m_bStart)
		return S_OK;

	if (m_bInOut)
	{
		m_fAlpha += m_fSpeed;
		if (m_fSpeed > 0.0f)
		{
			if (m_fAlpha >= 1.0f) /* Appear Ended */
			{
				m_fAlpha = 1.0f;
				m_bInOut = false;
				m_fTimeAcc = 0.0f;
			}
		}
		else
		{
			if (m_fAlpha <= 0.0f) /* Disappear Ended */
			{
				m_fAlpha = 0.0f;
				m_bStart = false;
				m_fTimeAcc = 0.0f;
				m_fSpeed = -m_fSpeed;
			}
		}

	}
	else /* Appear Done And Delay Time */
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc > m_fTime)
		{
			m_bInOut = true;
			m_fSpeed = -m_fSpeed;
		}
	}

	return S_OK;
}

HRESULT CUI_Event_Fade::SetUp_ShaderResources(CShader * pShader)
{
	if (FAILED(pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_Event_Fade::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Event: "); ImGui::SameLine();
	ImGui::Text(m_szEventName);

	static float fDuration;
	fDuration = m_fTime;
	if (ImGui::SliderFloat("Delay Duration", &fDuration, 0.f, 30.f))
	{
		m_fTime = fDuration;
	}

	static float fSpeed;
	fSpeed = m_fSpeed;
	if (ImGui::SliderFloat("Alpha Speed", &fSpeed, 0.f, 30.f))
	{
		m_fSpeed = fSpeed;
	}
}

HRESULT CUI_Event_Fade::Save_Data(Json * json)
{
	return S_OK;
}

HRESULT CUI_Event_Fade::Load_Data(wstring fileName)
{
	return S_OK;
}

void CUI_Event_Fade::Initialize(_float fSpeed, _float fDurTime)
{
	m_fSpeed = fSpeed;
	m_fTime = fDurTime;
}

CUI_Event_Fade * CUI_Event_Fade::Create()
{
	CUI_Event_Fade* pInstance = new CUI_Event_Fade();
	return pInstance;
}

CUI_Event_Fade * CUI_Event_Fade::Create(wstring fileName)
{
	CUI_Event_Fade* pInstance = new CUI_Event_Fade();
	if (pInstance != nullptr)
		pInstance->Load_Data(fileName);

	return pInstance;
}

CUI_Event_Fade * CUI_Event_Fade::Create(_float fSpeed, _float fDurTime)
{
	CUI_Event_Fade* pInstance = new CUI_Event_Fade();
	if (nullptr != pInstance)
		pInstance->Initialize(fSpeed, fDurTime);

	return pInstance;
}

void CUI_Event_Fade::Free()
{
}
