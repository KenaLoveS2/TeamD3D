#include "stdafx.h"
#include "..\public\E_KenaPulseDot.h"
#include "GameInstance.h"

CE_KenaPulseDot::CE_KenaPulseDot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_KenaPulseDot::CE_KenaPulseDot(const CE_KenaPulseDot & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_KenaPulseDot::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulseDot::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	// m_szVIBufferProtoTag = L"Prototype_Component_KenaPulseDot";

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_KenaPulseDot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("dot");
	if (ImGui::Button("DotConfirm"))
		m_pShaderCom->ReCompile();

	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_eEFfectDesc.vColor;

	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	ImGui::End();
}

void CE_KenaPulseDot::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_KenaPulseDot::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaPulseDot * CE_KenaPulseDot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaPulseDot * pInstance = new CE_KenaPulseDot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulseDot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulseDot::Clone(void * pArg)
{
	CE_KenaPulseDot * pInstance = new CE_KenaPulseDot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulseDot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulseDot::Free()
{
	__super::Free();
}
