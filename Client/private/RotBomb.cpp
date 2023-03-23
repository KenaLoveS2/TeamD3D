#include "stdafx.h"
#include "..\public\RotBomb.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Kena_State.h"
#include "Bone.h"
#include "E_KenaPulse.h"
#include "E_P_Explosion.h"

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

	Set_Child();
	for (auto& pchild : m_vecChild)
		pchild->Set_Parent(this);

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

	if (m_bBomb && m_eEFfectDesc.bActive == true)
	{
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		for (auto& pChild : m_vecChild)
		{
			pChild->Set_Active(true);
			pChild->Set_Position(vPos);
		}
		m_eEFfectDesc.bActive = false;
	}
}

void CRotBomb::Late_Tick(_float fTimeDelta)
{
	//if (m_eEFfectDesc.bActive == false)
	//	return;

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

void CRotBomb::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_vecChild.reserve(CHILD_END);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RotBombExplosion", L"RotBombExplosion"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);
	
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Explosion_p", L"RotBombExplosion_P"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	for (auto& pChild : m_vecChild)
		pChild->Set_Position(vPos);

	RELEASE_INSTANCE(CGameInstance);
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
