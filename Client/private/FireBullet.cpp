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
	m_eEFfectDesc.bActive = false;
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	assert(m_pKena != nullptr && "CFireBullet::Initialize()");

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

	FireBullet_Proc(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CFireBullet::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CFireBullet::Render()
{	
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		// m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		// m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_ReamTexture");
		// m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT, "g_LineTexture");
		// m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_ShapeTexture");
		// m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_SmoothTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 0);
	}

	return S_OK;
}

_bool CFireBullet::IsActiveState()
{
	return m_eEFfectDesc.bActive;
}

void CFireBullet::Push_EventFunctions()
{
	TurnOnTrail(true, 0.f);
	TurnOnBack(true, 0.f);
	TurnOnGround(true, 0.f);
	TurnOnParticle(true, 0.f);

	TurnOffTrail(true, 0.f);
	TurnOffBack(true, 0.f);
	TurnOffGround(true, 0.f);
	TurnOffParticle(true, 0.f);
}

void CFireBullet::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
}

HRESULT CFireBullet::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sphere", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	// FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/Noise_cloudsmed.png")), E_FAIL);
	// FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Deadzone_REAM.png")), E_FAIL);
	// FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Black_Linear.png")), E_FAIL);
	// FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/E_Effect_0.png")), E_FAIL);
	// FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_GR_Noise_Smooth_A.png")), E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

HRESULT CFireBullet::Set_ChildEffects()
{	
	return S_OK; // 임시

	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// child trail 추가 // CHILD_TRAIL
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispTrail", L"EnemyWispTrail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);	
	m_vecChild.push_back(pEffectBase);

	// child backgorund 추가 // CHILD_BACK
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispBackground", L"EnemyWispBack"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	// child backgorund 추가 // CHILD_GROUND
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispGround", L"EnemyWispGround"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	// child particle 추가 // CHILD_PARTICLE 
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispParticle", L"EnemyWispParticle"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CFireBullet::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOnTrail);
		return;
	}
	m_vecChild[CHILD_TRAIL]->Set_Active(true);
}

void CFireBullet::TurnOnBack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOnBack);
		return;
	}
	m_vecChild[CHILD_BACK]->Set_Active(true);

	if(m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		m_vecChild[CHILD_BACK]->Set_Position(vPos);
	}
}

void CFireBullet::TurnOnGround(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOnGround);
		return;
	}
	m_vecChild[CHILD_GROUND]->Set_Active(true);
	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		m_vecChild[CHILD_GROUND]->Set_Position(vPos);
	}
}

void CFireBullet::TurnOnParticle(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOnParticle);
		return;
	}
	m_vecChild[CHILD_PARTICLE]->Set_Active(true);
	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		m_vecChild[CHILD_PARTICLE]->Set_Position(vPos);
	}
}

void CFireBullet::TurnOffTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOffTrail);
		return;
	}
	m_vecChild[CHILD_TRAIL]->Set_Active(false);
}

void CFireBullet::TurnOffBack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOffBack);
		return;
	}
	m_vecChild[CHILD_BACK]->Set_Active(false);
}

void CFireBullet::TurnOffGround(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOffGround);
		return;
	}
	m_vecChild[CHILD_GROUND]->Set_Active(false);
}

void CFireBullet::TurnOffParticle(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CFireBullet::TurnOffParticle);
		return;
	}
	m_vecChild[CHILD_PARTICLE]->Set_Active(false);
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
		// 터지는 시물레이션
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