#include "stdafx.h"
#include "..\public\FireBullet.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Effect_Base.h"
#include "Monster.h"
#include "E_RectTrail.h"
#include "E_P_ExplosionGravity.h"
#include "E_FireBulletExplosion.h"

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
		GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	m_iTotalDTextureComCnt = 1;	m_iTotalMTextureComCnt = 0;
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);		
	FAILED_CHECK_RETURN(Set_ChildEffects(), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	// Push_EventFunctions();
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	assert(m_pKena != nullptr && "CFireBullet::Initialize()");

	m_pTransformCom->Set_Scaled(_float3(0.15f, 0.15f, 0.15f));
	m_pTransformCom->Set_Position(m_vInvisiblePos);

	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.fFrame[0] = 99.f;
	return S_OK;
}

HRESULT CFireBullet::Late_Initialize(void * pArg)
{
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
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COLLISON_DUMMY));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	}

	{
		_float3 vPos = _float3(0.f, 0.f, 0.f);
		_float3 vPivotScale = _float3(0.05f, 0.5f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.f, 0.35f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = CUtile::Create_DummyString();
		PxSphereDesc.vPos = vPos;
		PxSphereDesc.fRadius = vPivotScale.x;
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.fMass = 59.f;
		PxSphereDesc.fLinearDamping = 1.f;
		PxSphereDesc.bCCD = true;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(m_pOnwerMonster, false, COL_MONSTER_WEAPON));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	}
		
	return S_OK;
}

void CFireBullet::Tick(_float fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_bDissolve)
	{
		_bool bResult = TurnOffSystem(m_fDissolveTime, 4.f, fTimeDelta);
		if (bResult == true) m_bDissolve = false;
	}

	FireBullet_Proc(fTimeDelta);
	m_pTransformCom->Tick(fTimeDelta);
}

void CFireBullet::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
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

		m_pModelCom->Render(m_pShaderCom, i, nullptr, 8);
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

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Sphere", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/MageBullet/T_Deadzone_REAM.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Textures/Effect/MageBullet/t_fur_noise_02.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/MageBullet/T_GR_Cloud_Noise_A.png")), E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
	if (m_bDissolve)
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::Set_ChildEffects()
{	
	m_vecChild.reserve(CHILD_END);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base* pEffectBase = nullptr;

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

	/* RectTrail */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", L"BulletRectTrail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	dynamic_cast<CE_RectTrail*>(pEffectBase)->SetUp_Option(CE_RectTrail::OBJ_TRAIL);
	m_vecChild.push_back(pEffectBase);

	/* DeadParticle */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"BulletDeadParticle"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	dynamic_cast<CE_P_ExplosionGravity*>(pEffectBase)->Set_Option(CE_P_ExplosionGravity::TYPE_DEAD_MONSTER);
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

	for (auto& iter : m_vecChild)
		Safe_Release(iter);
}

void CFireBullet::FireBullet_Proc(_float fTimeDelta)
{
	m_vTargetPos = CMonster::Get_MonsterUseKenaPos();
	m_vTargetPos.y += 1.f;

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
		// m_vecChild[CHILD_EXPLOSION]->Set_Active(false);
		// 
		//_vector vPos = m_pTransformCom->Get_Position();
		//dynamic_cast<CE_RectTrail*>(m_vecChild[CHILD_P_TRAIL])->Trail_InputPos(vPos);

		m_eState = STATE_CHASE;

		break;
	}
	case STATE_CHASE:
	{	
		m_pTransformCom->Chase(m_vTargetPos, fTimeDelta, 0.2f, true);
		m_vecChild[CHILD_P_TRAIL]->Set_Active(true);

		_vector vPos = m_pTransformCom->Get_Position();
		dynamic_cast<CE_RectTrail*>(m_vecChild[CHILD_P_TRAIL])->Trail_InputPos(vPos);

		// if (m_pTransformCom->IsClosed_XZ(m_vTargetPos, 0.5f)) m_bCollision = true;

		m_eState = m_bCollision ? STATE_EXPLOSION_START : m_eState;

		break;
	}
	case STATE_EXPLOSION_START:
	{	
		// Turn Off => Object + RectTrail
		m_vecChild[CHILD_COVER]->Set_Active(false);
		m_vecChild[CHILD_BACK]->Set_Active(false);
		m_vecChild[CHILD_P_TRAIL]->Set_Active(false);

		// Turn On => Particle + Explosion Effect
		m_bDissolve = true;
		dynamic_cast<CE_FireBulletExplosion*>(m_vecChild[CHILD_EXPLOSION])->SetUp_State(m_vTargetPos);
		dynamic_cast<CE_P_ExplosionGravity*>(m_vecChild[CHILD_P_DEAD])->UpdateParticle(m_vTargetPos);
		m_eState = STATE_EXPLOSION;
		break;
	}
	case STATE_EXPLOSION:
	{		
		_bool bActive = m_vecChild[CHILD_EXPLOSION]->Get_Active();
		
		if (bActive == false)
		{
			m_eState = STATE_RESET;
		}	

		break;
	}
	case STATE_RESET:
	{
		m_eEFfectDesc.bActive = false;
		m_bDissolve = false;
		m_bCollision = false;
		m_pTransformCom->Set_Position(m_vInvisiblePos);
		m_eState = STATE_WAIT;
		break;
	}
	}
}

void CFireBullet::Execute_Create(_float4 vCreatePos)
{
	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_Position(vCreatePos);
	m_pTransformCom->LookAt(CMonster::Get_MonsterUseKenaPos());

	m_vecChild[CHILD_COVER]->Set_Active(true);
	m_vecChild[CHILD_BACK]->Set_Active(true);
	m_vecChild[CHILD_EXPLOSION]->Set_Active(false);
}

_int CFireBullet::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PLAYER && m_eEFfectDesc.bActive)
	{
		m_bCollision = true;
		m_pOnwerMonster->Set_RealAttack(true);
	}

	return 0;
}

