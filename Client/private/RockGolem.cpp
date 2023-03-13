#include "stdafx.h"
#include "..\public\RockGolem.h"
#include "GameInstance.h"

CRockGolem::CRockGolem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CRockGolem::CRockGolem(const CRockGolem & rhs)
	: CMonster(rhs)
{
}

HRESULT CRockGolem::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRockGolem::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	// SetUp_Component(); Monster가 불러줌
	//	Push_EventFunctions();

	m_pModelCom->Set_AllAnimCommonType();
	
	return S_OK;
}

HRESULT CRockGolem::Late_Initialize(void * pArg)
{
	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(1.8f, 0.1f, 1.f);
		_float3 vPivotPos = _float3(0.f, 1.95f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = vPos;
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 20.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_MONSTER));

		// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
		m_pRendererCom->Set_PhysXRender(true);
		m_pTransformCom->Set_PxPivotScale(vPivotScale);
		m_pTransformCom->Set_PxPivot(vPivotPos);
	}

	m_pTransformCom->Set_Position(_float4(26.f, 0.3f, 15.f, 1.f));

	return S_OK;
}

void CRockGolem::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	if (m_pFSM)
		m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CRockGolem::Late_Tick(_float fTimeDelta)
{
	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CRockGolem::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
	}

	return S_OK;
}

HRESULT CRockGolem::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CRockGolem::Imgui_RenderProperty()
{
	if (ImGui::Button("SPAWN"))
		m_bSpawn = true;

	ImGui::Text("Possible EXPLODEATTACK");
	if(ImGui::Button("TAKEDAMAGE"))
	{
		if(m_pFSM->IsCompareState("EXPLODEATTACK"))
			m_bHit = true;
	}
}

void CRockGolem::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("RockGolem Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("State"))
	{
		m_pFSM->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CRockGolem::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CRockGolem::ImGui_PhysXValueProperty()
{
	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();
	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();
	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	m_pTransformCom->Set_PxPivot(vPxPivot);
}

HRESULT CRockGolem::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CRockGolem::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

void CRockGolem::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = (vPos + vDisplacement) * 0.5f;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

HRESULT CRockGolem::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEPIDLE")
		.AddState("SLEEPIDLE")
		.Tick([this](_float fTimeDelta) 
		{
			m_pModelCom->Set_AnimIndex(SLEEPIDLE);
		})
			.AddTransition("SLEEPIDLE to WISPIN", "WISPIN")
			.Predicator([this]()
		{
			return m_bSpawn && DistanceTrigger(20.f);
		})

			.AddState("INTOSLEEP")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(INTOSLEEP);
			m_pModelCom->Set_AnimIndex(INTOSLEEP);
		})
			.AddTransition("INTOSLEEP to SLEEPIDLE" , "SLEEPIDLE")
			.Predicator([this]()
		{
			return AnimFinishChecker(INTOSLEEP);
		})

			.AddState("IDLE")
			.OnStart([this]()
		{
			m_fIdletoAttackTime = 0.f;
		})
			.Tick([this](_float fTimeDelta)
		{
			m_fIdletoAttackTime += fTimeDelta;
			m_pModelCom->Set_AnimIndex(IDLE);
		})
			.AddTransition("IDLE to WALK", "WALK")
			.Predicator([this]()
		{
			return TimeTrigger(m_fIdletoAttackTime, 5.f);
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("WALK")
			.OnStart([this]()
		{
			Set_AttackType();
		})
			.Tick([this](_float fTimeDelta)
		{
			m_pModelCom->Set_AnimIndex(WALK);
			m_pTransformCom->Chase(m_vKenaPos, fTimeDelta);
			Tick_Attack(fTimeDelta);
		})
			.OnExit([this]()
		{
			Reset_Attack();
		})
			.AddTransition("WALK to WISPOUT", "WISPOUT")
			.Predicator([this]()
		{
			return !DistanceTrigger(20.f);
		})
			.AddTransition("WALK to CHARGEATTACK", "CHARGEATTACK")
			.Predicator([this]()
		{
			return m_bRealAttack && m_bChargeAttack;
		})
			.AddTransition("WALK to SLAMATTACK", "SLAMATTACK")
			.Predicator([this]()
		{
			return m_bRealAttack && m_bSlamAttack; // 가까이 있을때 가까이 가서 공격함
		})
			.AddTransition("WALK to EXPLODEATTACK", "EXPLODEATTACK")
			.Predicator([this]()
		{
			return m_bRealAttack && m_bExplodeAttack; // 어디든 상관없을듯 
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("CHARGEATTACK")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(CHARGEATTACK);
			m_pModelCom->Set_AnimIndex(CHARGEATTACK);
		})
			.AddTransition("CHARGEATTACK to IDLE", "IDLE")
			.Predicator([this]()
		{
			return AnimFinishChecker(CHARGEATTACK);
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("SLAMATTACK")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(CHARGESLAM);
			m_pModelCom->Set_AnimIndex(CHARGESLAM);
		})
			.AddTransition("SLAMATTACK to IDLE", "IDLE")
			.Predicator([this]()
		{
			return AnimFinishChecker(CHARGESLAM);
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("EXPLODEATTACK")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(EXPLODE);
			m_pModelCom->Set_AnimIndex(EXPLODE);
		})
			.AddTransition("EXPLODEATTACK to IDLE", "IDLE")
			.Predicator([this]()
		{
			return AnimFinishChecker(EXPLODE);
		})
			.AddTransition("EXPLODEATTACK to TAKEDAMAGE" , "TAKEDAMAGE")
			.Predicator([this]()
		{
			return m_bHit;
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("TAKEDAMAGE")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGE);
			m_pModelCom->Set_AnimIndex(TAKEDAMAGE);
		})
			.AddTransition("TAKEDAMAGE to IDLE", "IDLE")
			.Predicator([this]()
		{
			return AnimFinishChecker(TAKEDAMAGE);
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("WISPIN")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(WISPIN);
			m_pModelCom->Set_AnimIndex(WISPIN);
		})
			.AddTransition("WISPIN to IDLE", "IDLE")
			.Predicator([this]()
		{
			return AnimFinishChecker(WISPIN);
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})

			.AddState("WISPOUT")
			.OnStart([this]()
		{
			m_pModelCom->ResetAnimIdx_PlayTime(WISPOUT);
			m_pModelCom->Set_AnimIndex(WISPOUT);
		})
			.AddTransition("WISPOUT to INTOSLEEP ", "INTOSLEEP")
			.Predicator([this]()
		{
			return AnimFinishChecker(WISPOUT);
		})
			.AddTransition("To DYING", "DYING")
			.Predicator([this]()
		{
			return m_pMonsterStatusCom->IsDead();
		})


			.AddState("DYING")
			.OnStart([this]()
		{
			m_pModelCom->Set_AnimIndex(ANIMATION_END);
			m_bDying = true;
		})
			.AddTransition("DYING to DEATH", "DEATH")
			.Predicator([this]()
		{
			return m_pModelCom->Get_AnimationFinish();
		})
			.AddState("DEATH")
			.OnStart([this]()
		{
			m_bDeath = true;
		})
			.Tick([this](_float fTimeDelta)
		{

		})
			.OnExit([this]()
		{

		})
			.Build();

	return S_OK;
}

HRESULT CRockGolem::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_RockGolem", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_RockGolem.json");

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/RockGolem/RockGolem_UV01_AO_R_M.png")), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/RockGolem/RockGolem_UV01_EMISSIVE.png")), E_FAIL);
	}

	m_pModelCom->Set_RootBone("RockGolem");
	
	return S_OK;
}

HRESULT CRockGolem::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CRockGolem::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CRockGolem::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

void CRockGolem::AdditiveAnim(_float fTimeDelta)
{
	_float fRatio = Calc_PlayerLookAtDirection();
	if (fRatio >= 0.f)
	{
		fRatio *= 1.5f;
		m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOK_LEFT);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_RockGolem.ao");
	}
	else
	{
		fRatio *= -1.5f;
		m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOK_RIGHT);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_RockGolem.ao");
	}
}

void CRockGolem::Set_AttackType()
{
	m_bRealAttack = false;
	m_bChargeAttack = false;
	m_bSlamAttack = false;
	m_bExplodeAttack = false;

	m_iAttackType = rand() % 3;

	switch(m_iAttackType)
	{
	case AT_CHARGEATTACK:
		m_bChargeAttack = true;
		break;
	case AT_CHARGESLAM:
		m_bSlamAttack = true;
		break;
	case AT_EXPLODE:
		m_bExplodeAttack = true;
		break;
	default:
		break;
	}
}

void CRockGolem::Reset_Attack()
{
	m_bRealAttack = false;
	m_bChargeAttack = false;
	m_bSlamAttack = false;
	m_bExplodeAttack = false;
	m_iAttackType = ATTACKTYPE_END;
}

void CRockGolem::Tick_Attack(_float fTimeDelta)
{
	switch (m_iAttackType)
	{
	case AT_CHARGESLAM:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 2.f);
		if (DistanceTrigger(2.f))
			m_bRealAttack = true;
		break;
	case AT_CHARGEATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 10.f);
		if (DistanceTrigger(10.f))
			m_bRealAttack = true;
		break;
	case AT_EXPLODE:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 10.f);
		if (DistanceTrigger(10.f))
			m_bRealAttack = true;
		break;
	default:
		break;
	}
}

CRockGolem* CRockGolem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRockGolem*	pInstance = new CRockGolem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRockGolem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRockGolem::Clone(void* pArg)
{
	CRockGolem*	pInstance = new CRockGolem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRockGolem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRockGolem::Free()
{
	CMonster::Free();
}
