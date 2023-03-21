#include "stdafx.h"
#include "..\public\FireBullet.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Effect_Trail.h"
#include "Kena.h"

CFireBullet::CFireBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CFireBullet::CFireBullet(const CFireBullet & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CFireBullet::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::Initialize(void * pArg)
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
	FAILED_CHECK_RETURN(Set_ChildEffects(), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	// Push_EventFunctions();
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	assert(m_pKena != nullptr && "CFireBullet::Initialize()");

	m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 0.2f));
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CFireBullet::Late_Initialize(void * pArg)
{
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(0.05f, 0.5f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.35f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.fMass = 59.f;
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PLAYER_WEAPON;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER_ARROW));

	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	m_pRendererCom->Set_PhysXRender(true);

	return S_OK;
}

void CFireBullet::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	//FireBullet_Proc(fTimeDelta);

	//m_pTransformCom->Tick(fTimeDelta);

	 __super::Tick(fTimeDelta);

	 m_vecChild[CHILD_COVER]->Tick(fTimeDelta);
	 m_vecChild[CHILD_BACK]->Tick(fTimeDelta);
}

void CFireBullet::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	m_vecChild[CHILD_COVER]->Late_Tick(fTimeDelta);
	m_vecChild[CHILD_BACK]->Late_Tick(fTimeDelta);
}

HRESULT CFireBullet::Render()
{	
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		 m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		 m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_ReamTexture");
		 m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 8);
	}

	return S_OK;
}

_bool CFireBullet::IsActiveState()
{
	return m_eEFfectDesc.bActive;
}

void CFireBullet::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
}

void CFireBullet::Imgui_RenderProperty()
{

}

HRESULT CFireBullet::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sphere", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	 FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/MageBullet/T_Deadzone_REAM.png")), E_FAIL);
	 FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Textures/Effect/MageBullet/t_fur_noise_02.png")), E_FAIL);
	 FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/MageBullet/T_GR_Cloud_Noise_A.png")), E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::Set_ChildEffects()
{	
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* cover */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_FireBulletCover", L"BulletCover"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);	
	m_vecChild.push_back(pEffectBase);

	/* Back */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_FireBulletCloud", L"BulletBack"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	/* Explosion */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_FireBulletExplosion", L"BulletExplosion"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CFireBullet * CFireBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFireBullet * pInstance = new CFireBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CFireBullet Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFireBullet::Clone(void * pArg)
{
	CFireBullet * pInstance = new CFireBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CFireBullet Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireBullet::Free()
{
	__super::Free();
}

void CFireBullet::FireBullet_Proc(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE_WAIT:
	{
		if (m_eEFfectDesc.bActive)
			m_eState = STATE_CREATE;

		break;
	}
	case STATE_CREATE:
	{
		// 생성 시뮬레이션

		for (auto& pChild : m_vecChild)
			pChild->Set_Active(true);
		m_vecChild[CHILD_EXPLOSION]->Set_Active(false);

		m_eState = STATE_CHASE;

		break;
	}
	case STATE_CHASE:
	{
		m_pTransformCom->Chase(m_vTargetPos, fTimeDelta, 0.2f);
		
		if (m_pTransformCom->IsClosed_XZ(m_vTargetPos, 1.f))
			m_bCollision = true;

		if (m_bCollision)
			m_eState = STATE_EXPLOSION;

		break;
	}
	case STATE_EXPLOSION:
	{
		m_vecChild[CHILD_EXPLOSION]->Set_Active(true);
		m_eState = STATE_RESET;
		break;
	}
	case STATE_RESET:
	{
		m_eEFfectDesc.bActive = false;
		m_eState = STATE_WAIT;
		break;
	}
	}
}

void CFireBullet::Execute_Create(_float4 vCreatePos)
{
	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_Position(vCreatePos);

	m_vTargetPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
}

_int CFireBullet::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget && iColliderIndex == COL_PLAYER)
	{
		m_bCollision = true;
	}

	return 0;
}