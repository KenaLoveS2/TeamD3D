#include "stdafx.h"
#include "..\public\E_FireBulletCover.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Effect_Trail.h"
#include "Kena.h"
#include "FireBullet.h"

CE_FireBulletCover::CE_FireBulletCover(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_FireBulletCover::CE_FireBulletCover(const CE_FireBulletCover & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_FireBulletCover::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_FireBulletCover::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	/* 모델을 별도로 설정해 주기 때문에 Desc 일부 변경 해줌 */
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; // Mesh 생성 안함
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);		

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_eEFfectDesc.vScale = XMVectorSet(0.16f, 0.16f, 0.16f, 1.f);
	m_fHDRValue = 3.0f;
	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.fFrame[0] = 99.f;
	return S_OK;
}

void CE_FireBulletCover::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	if (dynamic_cast<CFireBullet*>(m_pParent)->Get_Dissolve() == true)
		m_fDissolveTime += fTimeDelta;
	else
		m_fDissolveTime = 0.0f;
}

void CE_FireBulletCover::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		m_fDissolveTime = 0.0f;
		return;
	}

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_FireBulletCover::Render()
{	
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_MaskTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 9);
	}

	return S_OK;
}

void CE_FireBulletCover::Imgui_RenderProperty()
{
	if (ImGui::Button("active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;

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
}

HRESULT CE_FireBulletCover::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sphere", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	 FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/MageBullet/T_GR_Cloud_Noise_A.png")), E_FAIL);

	return S_OK;
}

HRESULT CE_FireBulletCover::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	if (dynamic_cast<CFireBullet*>(m_pParent)->Get_Dissolve() == true)
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);

	return S_OK;
}

CE_FireBulletCover * CE_FireBulletCover::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_FireBulletCover * pInstance = new CE_FireBulletCover(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CE_FireBulletCover Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_FireBulletCover::Clone(void * pArg)
{
	CE_FireBulletCover * pInstance = new CE_FireBulletCover(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_FireBulletCover Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_FireBulletCover::Free()
{
	__super::Free();
}

