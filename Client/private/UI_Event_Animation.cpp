#include "stdafx.h"
#include "..\public\UI_Event_Animation.h"
#include "GameInstance.h"
#include "UI.h"
#include "Json/json.hpp"

CUI_Event_Animation::CUI_Event_Animation(CUI* pUI)
{
	m_szEventName = "Animation";
	m_iRenderPass = 4;
	m_pParent = pUI;


	m_bStart = false;
	m_bFinished = false;
	m_bLoop = true;
	m_iTextureIndex = 0;

	//m_iFramesX		= 1;
	//m_iFramesY		= 1;
	m_iFrameXNow = 0;
	m_iFrameYNow = 0;

	m_iTextureNum = m_pParent->Get_DiffuseTexture()->Get_TextureIdx();
	for (_uint i = 0; i < m_iTextureNum; ++i)
	{
		m_vecFramesX.push_back(1);
		m_vecFramesY.push_back(1);
	}
}

void CUI_Event_Animation::Call_Event(_uint iTexIdx)
{
	m_bStart = true;
	m_iFrameXNow = 0;
	m_iFrameYNow = 0;
	m_bFinished = false;
	m_iTextureIndex = iTexIdx;
}

HRESULT CUI_Event_Animation::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Event_Animation::Late_Tick(_float fTimeDelta)
{
	//m_bStart = true;

	/* Need Start Call */
	if (!m_bStart)
		return S_OK;

	/* For. non-Loop animation */
	if (m_bFinished)
	{
		m_bStart = false;
		return S_OK;
	}


	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > m_fTime)
	{
		++m_iFrameXNow;

		if (m_iFrameXNow == m_vecFramesX[m_iTextureIndex])
		{
			if (m_iFrameYNow == m_vecFramesY[m_iTextureIndex] - 1) /* Last Sprite */
			{
				if (!m_bLoop)
				{
					/* Fix to Last Sprite */
					m_iFrameXNow = m_vecFramesX[m_iTextureIndex] - 1;
					m_bFinished = true;
				}
				else
				{
					/* Go back To First Sprite */
					m_iFrameXNow = 0;
					m_iFrameYNow = 0;
				}
			}
			else /* next line exists */
			{
				m_iFrameXNow = 0;
				++m_iFrameYNow;
			}
		}
		m_fTimeAcc = 0.f;
	}

	return S_OK;
}

HRESULT CUI_Event_Animation::SetUp_ShaderResources(CShader * pShader)
{
	/* Sprite Animation */
	if (FAILED(pShader->Set_RawValue("g_XFrames", &m_vecFramesX[m_iTextureIndex], sizeof(_int))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_YFrames", &m_vecFramesY[m_iTextureIndex], sizeof(_int))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_XFrameNow", &m_iFrameXNow, sizeof(_int))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_YFrameNow", &m_iFrameYNow, sizeof(_int))))
		return E_FAIL;

	return S_OK;
}

void CUI_Event_Animation::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Event: "); ImGui::SameLine();
	ImGui::Text(m_szEventName);

	/* RenderPass */
	ImGui::Text("Recommended RenderPass:"); ImGui::SameLine();
	_int iPass = (_int)m_iRenderPass;
	ImGui::InputInt("RenderPass", &iPass);

	/* Total Cnt of x,y frames */

	static int totalTextures = m_iTextureNum;
	ImGui::InputInt("TotalTextures", &totalTextures, 0, 100, ImGuiInputTextFlags_ReadOnly);

	static int selectedIdx;
	selectedIdx = m_iTextureIndex;
	if (ImGui::SliderInt("Texture Index", &selectedIdx, 0, totalTextures - 1))
	{
		m_iTextureIndex = selectedIdx;

		if (m_pParent != nullptr)
			m_pParent->Set_TextureIndex(m_iTextureIndex);
	}

	static int framesX;
	static int framesY;
	framesX = m_vecFramesX[m_iTextureIndex];
	framesY = m_vecFramesY[m_iTextureIndex];
	if (ImGui::SliderInt("Cnt of FramesX", &framesX, 1, 20))
		m_vecFramesX[m_iTextureIndex] = framesX;
	if (ImGui::SliderInt("Cnt of FramesY", &framesY, 1, 20))
		m_vecFramesY[m_iTextureIndex] = framesY;

	/* Animation Speed */
	static float fDuration;
	fDuration = m_fTime;
	if (ImGui::SliderFloat("Animation Duration", &fDuration, 0.f, 30.f))
		m_fTime = fDuration;

	/* Is_Loop */
	static bool bLoop;
	bLoop = m_bLoop;
	if (ImGui::Checkbox("IsLoop", &bLoop))
	{
		m_bStart = true;
		m_bLoop = bLoop;
		m_bFinished = false;
		m_fTimeAcc = 0.f;
		m_iFrameXNow = 0;
		m_iFrameYNow = 0;
	}
}

HRESULT CUI_Event_Animation::Save_Data(Json * json)
{
	if (json == nullptr)
		return E_FAIL;

	(*json)["Duration"] = m_fTime;
	(*json)["Loop"] = m_bLoop;

	for (_uint i = 0; i < m_iTextureNum; ++i)
	{
		(*json)["FramesX"].push_back(m_vecFramesX[i]);
		(*json)["FramesY"].push_back(m_vecFramesY[i]);
	}

	return S_OK;
}

HRESULT CUI_Event_Animation::Load_Data(wstring fileName)
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

	jLoad["Duration"].get_to<_float>(m_fTime);
	jLoad["Loop"].get_to<_bool>(m_bLoop);

	_uint i = 0;
	for (_int iFrame : jLoad["FramesX"])
	{
		m_vecFramesX[i] = iFrame;
		++i;
	}

	i = 0;
	for (_int iFrame : jLoad["FramesY"])
	{
		m_vecFramesY[i] = iFrame;
		++i;
	}

	return S_OK;
}

CUI_Event_Animation * CUI_Event_Animation::Create(CUI* pUI)
{
	CUI_Event_Animation* pInstance = new CUI_Event_Animation(pUI);
	return pInstance;
}

CUI_Event_Animation * CUI_Event_Animation::Create(wstring fileName, CUI* pUI)
{
	CUI_Event_Animation* pInstance = new CUI_Event_Animation(pUI);
	if (pInstance != nullptr)
		pInstance->Load_Data(fileName);
	return pInstance;
}

void CUI_Event_Animation::Free()
{
}
