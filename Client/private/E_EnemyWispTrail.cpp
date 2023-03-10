#include "stdafx.h"
#include "..\public\E_EnemyWispTrail.h"
#include "GameInstance.h"

CE_EnemyWispTrail::CE_EnemyWispTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_EnemyWispTrail::CE_EnemyWispTrail(const CE_EnemyWispTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_EnemyWispTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_EnemyWispTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iTotalDTextureComCnt = 0;
	m_iTotalMTextureComCnt = 0;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Trail Texture */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this)))
		return E_FAIL;
	/* Trail Texture */

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.08f;
	m_eEFfectDesc.fLife = 0.5f; 
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.001f; 
	m_eEFfectDesc.vColor = XMVectorSet(160.f, 231.f, 255.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_iTrailFlowTexture = 0;
	m_eEFfectDesc.bActive = true;
	return S_OK;
}

void CE_EnemyWispTrail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
	
	ImGui::Begin("Trail Option");
	ImGui::Checkbox("IsTrail", &m_eEFfectDesc.IsTrail);
	ImGui::Checkbox("bActive", &m_eEFfectDesc.bActive);
	ImGui::Checkbox("bAlpha", &m_eEFfectDesc.bAlpha);
	ImGui::DragFloat("Life", (_float*)&m_eEFfectDesc.fLife);
	ImGui::DragFloat("Width", (_float*)&m_eEFfectDesc.fWidth);
	ImGui::DragFloat("Alpha", (_float*)&m_eEFfectDesc.fAlpha);
	ImGui::DragFloat("SegmentSize", (_float*)&m_eEFfectDesc.fSegmentSize, 1.f,0.f,0.f, "%.6f");
	ImGui::DragFloat4("Pos", (_float*)&m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	ImGui::DragFloat4("vColor", (_float*)&m_eEFfectDesc.vColor);

	ImGui::InputInt("FlowTexture", (_int*)&m_iTrailFlowTexture);

	if (ImGui::Button("ReCompile"))
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

	ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	ImGui::End();

}

void CE_EnemyWispTrail::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_EnemyWispTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

HRESULT CE_EnemyWispTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/* Flow */
	if (FAILED(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_TrailflowTexture", m_iTrailFlowTexture)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UV", &m_fUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_EnemyWispTrail * CE_EnemyWispTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_EnemyWispTrail * pInstance = new CE_EnemyWispTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_EnemyWispTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_EnemyWispTrail::Clone(void * pArg)
{
	CE_EnemyWispTrail * pInstance = new CE_EnemyWispTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_EnemyWispTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_EnemyWispTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
}
