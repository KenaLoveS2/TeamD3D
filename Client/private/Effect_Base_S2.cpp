#include "stdafx.h"
#include "..\public\Effect_Base_S2.h"
#include "GameInstance.h"

CEffect_Base_S2::CEffect_Base_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
	, m_pTarget(nullptr)
	, m_iRenderPass(0)
	, m_iTextureIndex(0)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_fHDRIntensity(1.f)
	, m_bActive(false)
	, m_fDissolveAlpha(0.f)
	, m_fDissolveSpeed(0.f)
	, m_fFrameSpeed(0.f)
	, m_fFrameNow(0.f)
	, m_vScaleSpeed(0.0f, 0.0f)
	, m_ParentPosition(0.0f, 0.0f, 0.0f, 1.0f)
	, m_bSelfStop(false)
	, m_fSelfStopTime(0.0f)
	, m_fSelfStopTimeAcc(0.0f)
	, m_iDissolvePass(0)
	, m_bActiveSlowly(false)
	, m_bDeActiveSlowly(false)
{
	XMStoreFloat4x4(&m_WorldOriginal, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LocalMatrixOriginal, XMMatrixIdentity());


	for (_uint i = 0; i < 2; ++i)
	{
		m_iFrames[i] = 1;
		m_iFrameNow[i] = 0;
		m_fUVSpeeds[i] = 0.f;
		m_fUVMove[i] = 0.f;
	}

	for (_uint i = 0; i < OPTION_END; ++i)
		m_bOptions[i] = false;

}

CEffect_Base_S2::CEffect_Base_S2(const CEffect_Base_S2& rhs)
	: CGameObject(rhs)
	, m_pTarget(nullptr)
	, m_iRenderPass(0)
	, m_iTextureIndex(0)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_fHDRIntensity(1.f)
	, m_bActive(false)
	, m_fDissolveAlpha(0.f)
	, m_fDissolveSpeed(0.f)
	, m_fFrameSpeed(0.f)
	, m_fFrameNow(0.f)
	, m_vScaleSpeed(0.0f, 0.0f)
	, m_ParentPosition(0.0f, 0.0f, 0.0f, 1.0f)
	, m_bSelfStop(false)
	, m_fSelfStopTime(0.0f)
	, m_fSelfStopTimeAcc(0.0f)
	, m_iDissolvePass(0)
	, m_bActiveSlowly(false)
	, m_bDeActiveSlowly(false)
{
	XMStoreFloat4x4(&m_WorldOriginal, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LocalMatrixOriginal, XMMatrixIdentity());

	for (_uint i = 0; i < 2; ++i)
	{
		m_iFrames[i] = 1;		/* -1 means the effect is not sprite animation */
		m_iFrameNow[i] = 0;
		m_fUVSpeeds[i] = 0.f;
		m_fUVMove[i] = 0.f;
	}

	for (_uint i = 0; i < OPTION_END; ++i)
		m_bOptions[i] = false;
}

HRESULT CEffect_Base_S2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Base_S2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Base_S2::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Base_S2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pTarget != nullptr)
	{
		_float4 vTargetPos = m_pTarget->Get_TransformCom()->Get_Position();
		m_pTransformCom->Set_Position(vTargetPos);
	}

	/* UV Animation */
	if (true == m_bOptions[OPTION_UV])
	{
		for (_uint i = 0; i < 2; ++i)
		{
			m_fUVMove[i] += m_fUVSpeeds[i] * fTimeDelta;
			m_fUVMove[i] = fmodf(m_fUVMove[i], 1);

		}
	}

	/* Sprite Animation */
	if (true == m_bOptions[OPTION_SPRITE])
	{
		m_fFrameNow += m_fFrameSpeed * fTimeDelta;
		_float fFrameLast = _float(m_iFrames[0] * m_iFrames[1] - 1);
		if (m_fFrameNow > fFrameLast)
		{
			if (true == m_bOptions[OPTION_SPRITE_REPEAT])
				m_fFrameNow = 0.f;
			else
				m_fFrameNow = fFrameLast;
		}

		m_iFrameNow[0] = (_int)m_fFrameNow % m_iFrames[0];
		m_iFrameNow[1] = (_int)m_fFrameNow / m_iFrames[0];

		//if (m_iFrameNow[0] == m_iFrames[0] - 1 && m_iFrameNow[1] == m_iFrames[1] -1 )
		//	MSG_BOX("test");
	}

	if (true == m_bSelfStop)
	{
		m_fSelfStopTimeAcc += fTimeDelta;
		if (m_fSelfStopTimeAcc > m_fSelfStopTime)
			DeActivate();
	}

}

void CEffect_Base_S2::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CEffect_Base_S2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Base_S2::DeActivate()
{
	m_bActive = false;

	m_fDissolveAlpha = 0.0f;
	m_iFrameNow[0] = 0;
	m_iFrameNow[1] = 0;
	m_fFrameNow = 0.0f;

	m_fUVMove[0] = 0.0f;
	m_fUVMove[1] = 0.0f;

	m_fSelfStopTimeAcc = 0.0f;

	m_iRenderPass = m_iRenderPassOriginal;
}

void CEffect_Base_S2::DeActivate_Slowly()
{
	m_bDeActiveSlowly = true;
	//m_fDissolveAlpha = 0.0f;
	//m_iRenderPass = m_iDissolvePass;
}

_float4 CEffect_Base_S2::ColorCode()
{
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_vColor;

	if (ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL))
		m_vColor = vSelectColor;
	if (ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags))
		m_vColor = vSelectColor;

	static _float fAlpha = 1.f;
	fAlpha = m_fHDRIntensity;
	if (ImGui::DragFloat("HDR Intensity", &fAlpha, 0.1f, 0.f, 50.f))
		m_fHDRIntensity = fAlpha;

	return vSelectColor;
}

void CEffect_Base_S2::Options()
{
	/* Options 1: UV */
	static _bool bUV;
	bUV = m_bOptions[OPTION_UV];
	if (ImGui::Checkbox("IsUVAnim", &bUV))
		m_bOptions[OPTION_UV] = bUV;
	ImGui::SameLine();

	/* Options 2 : Sprite */
	static _bool bSprite;
	bSprite = m_bOptions[OPTION_SPRITE];
	if (ImGui::Checkbox("IsSpriteAnim", &bSprite))
		m_bOptions[OPTION_SPRITE] = bSprite;
	ImGui::SameLine();

	/* Options 3 : Sprite_Repeat */
	static _bool bRepeat;
	bRepeat = m_bOptions[OPTION_SPRITE_REPEAT];
	if (ImGui::Checkbox("IsSpriteRepeat", &bRepeat))
		m_bOptions[OPTION_SPRITE_REPEAT] = bRepeat;

	/* UV speed */
	static _float fUVSpeed[2];
	fUVSpeed[0] = m_fUVSpeeds[0];
	fUVSpeed[1] = m_fUVSpeeds[1];
	if (ImGui::DragFloat2("UVSpeed", fUVSpeed, 0.001f, -10.f, 10.f))
	{
		m_fUVSpeeds[0] = fUVSpeed[0];
		m_fUVSpeeds[1] = fUVSpeed[1];
	}

	/* Sprite Animation */
	static _int iFrames[2];
	iFrames[0] = m_iFrames[0];
	iFrames[1] = m_iFrames[1];
	if (ImGui::DragInt2("Frames", iFrames, 1, 1, 10))
	{
		m_iFrames[0] = iFrames[0];
		m_iFrames[1] = iFrames[1];
	}

	static _float fFrameSpeed;
	fFrameSpeed = m_fFrameSpeed;
	if (ImGui::DragFloat("FrameSpeed", &fFrameSpeed, 0.001f, 0.f, 100.f))
		m_fFrameSpeed = fFrameSpeed;
	/* ~ Sprite Animation */

	if (ImGui::Button("Replay"))
		m_fFrameNow = 0.f;
	ImGui::SameLine();

	if (ImGui::Button("No UV, Sprite Anim"))
	{
		for (_uint i = 0; i < 2; ++i)
		{
			m_fUVSpeeds[i] = 0.0f;
			m_fUVMove[i] = 0.0f;
			m_fFrameSpeed = 0.f;
			m_iFrameNow[i] = 0;
			m_iFrames[i] = 1;
			m_fFrameNow = 0.f;
		}
	}
}

void CEffect_Base_S2::Free()
{
	__super::Free();
}
