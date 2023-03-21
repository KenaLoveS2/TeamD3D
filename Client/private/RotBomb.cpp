#include "stdafx.h"
#include "..\public\RotBomb.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Kena_State.h"
#include "Bone.h"

CRotBomb::CRotBomb(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CRotBomb::CRotBomb(const CRotBomb & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CRotBomb::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CRotBomb::Initialize(void * pArg)
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

	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; 
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	assert(m_pKena != nullptr && "CFireBullet::Initialize()");

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CRotBomb::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CRotBomb::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CRotBomb::Late_Tick(_float fTimeDelta)
{
	//if (m_eEFfectDesc.bActive == false)
	//	return;
	int a = 0;

	__super::Late_Tick(fTimeDelta);

	if(m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}
}

HRESULT CRotBomb::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0 || i == 2) //  0 || 2 == Sphere
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_NoiseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_CloudTexture");

			if (i == 0)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		}

		if (i == 1) //  1== inrot
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");

			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		}
	}
	return S_OK;
}

void CRotBomb::Imgui_RenderProperty()
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
	vSelectColor = m_eEFfectDesc.vColor;

	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;
}

void CRotBomb::ImGui_ShaderValueProperty()
{
}

void CRotBomb::ImGui_PhysXValueProperty()
{
}

void CRotBomb::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
}

HRESULT CRotBomb::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Rot_Bomb", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/* Texture */
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 1) // Rot
		{
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Rot Bomb/T_RotStreakWithEyes_D.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_NORMALS, TEXT("../Bin/Resources/Anim/Rot Bomb/T_RotStreak_N.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Rot Bomb/T_RotStreak_AO_R_M.png")), E_FAIL);
		}

		if (i == 0 || i == 2)
		{
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Rot Bomb/Noise_cloudsmed.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Rot Bomb/T_NoiseClassic01.png")), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CRotBomb::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

HRESULT CRotBomb::SetUp_ShadowShaderResources()
{
	return S_OK;
}

_int CRotBomb::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

CRotBomb * CRotBomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRotBomb*	pInstance = new CRotBomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRotBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotBomb::Clone(void * pArg)
{
	CRotBomb*	pInstance = new CRotBomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRotBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotBomb::Free()
{
	__super::Free();
}
