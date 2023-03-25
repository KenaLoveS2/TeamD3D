#include "stdafx.h"
#include "..\public\Effect_Base_S2.h"
#include "GameInstance.h"

CEffect_Base_S2::CEffect_Base_S2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
	, m_pTarget(nullptr)
	, m_iRenderPass(0)
	, m_iTextureIndex(0)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_fHDRIntensity(1.f)
	, m_bFire(false)
{
}

CEffect_Base_S2::CEffect_Base_S2(const CEffect_Base_S2 & rhs)
	: CGameObject(rhs)
	, m_pTarget(nullptr)
	, m_iRenderPass(0)
	, m_iTextureIndex(0)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_fHDRIntensity(1.f)
	, m_bFire(false)
{
}

HRESULT CEffect_Base_S2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Base_S2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Base_S2::Late_Initialize(void * pArg)
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

void CEffect_Base_S2::Free()
{
	__super::Free();
}
