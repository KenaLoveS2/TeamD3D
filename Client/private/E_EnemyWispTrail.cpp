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
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture[0], this)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_OutflowTexture", (CComponent**)&m_pTrailflowTexture[1], this)))
		return E_FAIL;
	/* Trail Texture */

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.4f;
	m_eEFfectDesc.fLife = 1.0f; 
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.001f; 
	m_eEFfectDesc.vColor = XMVectorSet(255.f, 97.f, 0.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_eEFfectDesc.bActive = true;
	return S_OK;
}

void CE_EnemyWispTrail::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	ImGui::Begin("test");
	if (ImGui::Button("rebuild"))
		m_pShaderCom->ReCompile();

	ImGui::InputInt("flow", (_int*)&m_iTrailFlowTexture[0]);
	ImGui::InputInt("texture", (_int*)&m_iTrailFlowTexture[1]);

	ImGui::Checkbox("Trail", &m_eEFfectDesc.IsTrail);
	ImGui::Checkbox("bAlpha", &m_eEFfectDesc.bAlpha);

	ImGui::InputFloat("Width", &m_eEFfectDesc.fWidth);
	ImGui::InputFloat("fLife", &m_eEFfectDesc.fLife);
	ImGui::InputFloat("fAlpha", &m_eEFfectDesc.fAlpha);
	ImGui::InputFloat("fSegmentSize", &m_eEFfectDesc.fSegmentSize);

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

void CE_EnemyWispTrail::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

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

	/* Instance Buffer */
	if (FAILED(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_WispInfoMatrix")))
		return E_FAIL;
	/* Instance Buffer */

	/* Flow */
	if (FAILED(m_pTrailflowTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_WispflowTexture", m_iTrailFlowTexture[1])))
		return E_FAIL;
	if (FAILED(m_pTrailflowTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_WispOutTexture", m_iTrailFlowTexture[1])))
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

	for (auto& pTextureCom : m_pTrailflowTexture)
		Safe_Release(pTextureCom);
}
