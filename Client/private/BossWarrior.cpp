#include "stdafx.h"
#include "..\public\BossWarrior.h"
#include "GameInstance.h"
#include "Bone.h"
#include "BossWarrior_Hat.h"

CBossWarrior::CBossWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CBossWarrior::CBossWarrior(const CBossWarrior& rhs)
	:CMonster(rhs)
{
}

HRESULT CBossWarrior::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBossWarrior::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
	{
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	}
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -10.f;
		m_Desc.WorldMatrix._43 = -10.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pWeaponBone = m_pModelCom->Get_BonePtr("Halberd_Root_Jnt");
	m_pRightLegBone = m_pModelCom->Get_BonePtr("char_rt_ankle_jnt");

	XMStoreFloat4x4(&m_WeaponPivotMatrix, 
		XMMatrixRotationX(m_vWeaPonPivotRot.x) * XMMatrixRotationY(m_vWeaPonPivotRot.y) * XMMatrixRotationZ(m_vWeaPonPivotRot.z)
		* XMMatrixTranslation(m_vWeaPonPivotTrans.x, m_vWeaPonPivotTrans.y, m_vWeaPonPivotTrans.z));
		
	XMStoreFloat4x4(&m_RightLegPivotMatrix, XMMatrixTranslation(m_vRightLegPivotTrans.x, m_vRightLegPivotTrans.y, m_vRightLegPivotTrans.z));
	
	return S_OK;
}

HRESULT CBossWarrior::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);
	// 몸통
	{	
		_float3 vPivotScale = _float3(1.1f, 1.1f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.5f, 0.f);

		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = _float3( 0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.isGravity = true;
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 10.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_MONSTER));

		// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 2.f, 0.f));
	}
	
	{	
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = COL_WEAPON_TEXT;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = 0.18f;
		PxCapsuleDesc.fHalfHeight = 2.4f;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 10.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));
		m_pTransformCom->Add_Collider(PxCapsuleDesc.pActortag, g_IdentityFloat4x4);		
	}	
		
	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = COL_RIGHT_LEG_TEXT;
		PxSphereDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fRadius = 0.4f;		
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.fMass = 10.f;
		PxSphereDesc.fLinearDamping = 10.f;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	return S_OK;
}

void CBossWarrior::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);
	m_pHat->Tick(fTimeDelta);
	
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CBossWarrior::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	m_pHat->Late_Tick(fTimeDelta);
	if (m_pRendererCom /*&& m_bSpawn*/)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CBossWarrior::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if(i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
		}
		else if(i == 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			//m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_OpacityTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
		}
	}
	return S_OK;
}

HRESULT CBossWarrior::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CBossWarrior::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	m_pHat->Imgui_RenderProperty();
	float fEmissiveColor[3] = { m_fEmissiveColor.x, m_fEmissiveColor.y, m_fEmissiveColor.z };
	ImGui::DragFloat3("EmissiveColor", fEmissiveColor, 0.01f, 0.1f, 100.0f);
	m_fEmissiveColor.x = fEmissiveColor[0]; m_fEmissiveColor.y = fEmissiveColor[1]; m_fEmissiveColor.z = fEmissiveColor[2];
	ImGui::DragFloat("HDRIntensity", &m_fHDRIntensity, 0.01f, 0.f, 100.f);

	float fTrans[3] = { m_vWeaPonPivotTrans.x, m_vWeaPonPivotTrans.y, m_vWeaPonPivotTrans.z };
	ImGui::DragFloat3("Weapon Trans", fTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vWeaPonPivotTrans, fTrans, sizeof(_float3));
	 
	float fRot[3] = { m_vWeaPonPivotRot.x, m_vWeaPonPivotRot.y, m_vWeaPonPivotRot.z };
	ImGui::DragFloat3("Weapon Rot", fRot, 0.01f, -100.f, 100.0f);
	memcpy(&m_vWeaPonPivotRot, fRot, sizeof(_float3));

	float fLegTrans[3] = { m_vRightLegPivotTrans.x, m_vRightLegPivotTrans.y, m_vRightLegPivotTrans.z };
	ImGui::DragFloat3("Leg Trans", fLegTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vRightLegPivotTrans, fLegTrans, sizeof(_float3));
}

void CBossWarrior::ImGui_AnimationProperty()
{
	if (ImGui::CollapsingHeader("BossWarrior"))
	{
		ImGui::BeginTabBar("BossWarrior Animation & State");

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
}

void CBossWarrior::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CBossWarrior::ImGui_PhysXValueProperty()
{
	CMonster::ImGui_PhysXValueProperty();

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

HRESULT CBossWarrior::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CBossWarrior::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CBossWarrior::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")				
		.Tick([this](_float fTimeDelta)
	{
		// m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.OnExit([this]()
	{
		
	})
		.AddTransition("SLEEP to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{	
		m_bSpawn = true;
		return false;
		// 대기 종결 조건 수정 필요
		m_fSpawnRange = 10.f;
		return DistanceTrigger(m_fSpawnRange);				
	})

		.AddState("READY_SPAWN")
		.OnStart([this]()
	{
		// 등장 연출 필요
		m_pModelCom->ResetAnimIdx_PlayTime(AWAKE);
		m_pModelCom->Set_AnimIndex(AWAKE);
	})
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);		
		m_bSpawn = true;
	})
		.AddTransition("READY_SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{	
		return AnimFinishChecker(AWAKE);
	})

	
		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(IDLE_LOOP);
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})	
		.AddTransition("IDLE to ENRAGE", "ENRAGE")
		.Predicator([this]()
	{
		return m_bEnRageReady && m_pMonsterStatusCom->Get_PercentHP() < 0.5f;
	})
		.AddTransition("IDLE to BLOCK_INTO", "BLOCK_INTO")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fBlockRange) && m_pKena->Get_State(CKena::STATE_ATTACK);
	})
		.AddTransition("IDLE to JUMP_BACK", "JUMP_BACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fJumpBackRange);
	})
		.AddTransition("IDLE to CHARGE_ATTACK", "CHARGE_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_iCloseAttackIndex == 0;
	})
		.AddTransition("IDLE to COMBO_ATTACK", "UPPER_CUT")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_iCloseAttackIndex == 1;
	})
		.AddTransition("IDLE to COMBO_ATTACK", "COMBO_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_iCloseAttackIndex == 2;
	})
		.AddTransition("IDLE to COMBO_ATTACK", "SWEEP_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_iCloseAttackIndex == 2;
	})		
		.AddTransition("IDLE to JUMP_ATTACK", "JUMP_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fFarAttackRange) && m_iFarAttackIndex == 0;
	})		
		.AddTransition("IDLE to TRIP_UPPERCUT", "TRIP_UPPERCUT")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fFarAttackRange) && m_iFarAttackIndex == 1;
	})



		.AddState("CHARGE_ATTACK")
		.OnStart([this]()
	{	
		Attack_Start(CHARGE_ATTACK);		
	})
		.OnExit([this]()
	{
		Attack_End(&m_iCloseAttackIndex, WARRIR_CLOSE_ATTACK_COUNT, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("CHARGE_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(CHARGE_ATTACK);
	})
		

		.AddState("UPPER_CUT")
		.OnStart([this]()
	{
		Attack_Start(UPPER_CUT);
	})
		.OnExit([this]()
	{
		Attack_End(&m_iCloseAttackIndex, WARRIR_CLOSE_ATTACK_COUNT, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("CHARGE_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(UPPER_CUT);
	})
		

		.AddState("COMBO_ATTACK")
		.OnStart([this]()
	{	
		Attack_Start(COMBO_ATTACK);		
	})
		.OnExit([this]()
	{
		Attack_End(&m_iCloseAttackIndex, WARRIR_CLOSE_ATTACK_COUNT, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("COMBO_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(COMBO_ATTACK);
	})



		.AddState("SWEEP_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(SWEEP_ATTACK);
	})
		.OnExit([this]()
	{
		Attack_End(&m_iCloseAttackIndex, WARRIR_CLOSE_ATTACK_COUNT, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("SWEEP_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(SWEEP_ATTACK);
	})



		.AddState("ENRAGE")
		.OnStart([this]()
	{
		m_bEnRageReady = false;
		m_pMonsterStatusCom->Add_CurrentHP(100);
		Attack_Start(ENRAGE);
		
		// 광역 공격 이펙트(레인지)
		
	})
		.OnExit([this]()
	{
		m_bRealAttack = false;
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("ENRAGE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ENRAGE);
	})

		.AddState("BELL_CALL")
		.OnStart([this]()
	{	
		m_bBellCall = true;
		m_pModelCom->ResetAnimIdx_PlayTime(BELL_CALL);
		m_pModelCom->Set_AnimIndex(BELL_CALL);
		// 몬스터 소환
	})
		.OnExit([this]()
	{
		m_bRealAttack = false;
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("ENRAGE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BELL_CALL);
	})
		


		.AddState("BLOCK_INTO")
		.OnStart([this]()
	{	
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCK_INTO);
		m_pModelCom->Set_AnimIndex(BLOCK_INTO);
	})		
		.OnExit([this]()
	{	
		_uint iAnimIndex = m_bBlockHit ? BLOCK_HIT_2 : IDLE_LOOP;
		m_pModelCom->Set_AnimIndex(iAnimIndex);
	})
		.AddTransition("BLOCK_INTO to BLOCK_HIT", "BLOCK_HIT")
		.Predicator([this]()
	{
		m_bBlockHit = m_pKena->Get_State(CKena::STATE_ATTACK);
		return AnimFinishChecker(BLOCK_INTO) && m_bBlockHit;
	})
		.AddTransition("BLOCK_INTO to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCK_INTO);
	})



		.AddState("BLOCK_HIT")
		.OnStart([this]()
	{
		m_bRealAttack = true;
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCK_HIT_2);
		m_pModelCom->Set_AnimIndex(BLOCK_HIT_2);
	})
		.OnExit([this]()
	{
		m_bRealAttack = true;
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("BLOCK_HIT to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCK_HIT_2);
	})
		


		.AddState("JUMP_BACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(JUMP_BACK);
		m_pModelCom->Set_AnimIndex(JUMP_BACK);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("JUMP_BACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(JUMP_BACK);
	})



		.AddState("JUMP_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(JUMP_ATTACK);		
	})
		.OnExit([this]()
	{
		Attack_End(&m_iFarAttackIndex, WARRIR_FAR_ATTACK_COUNT, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("JUMP_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(JUMP_ATTACK);
	})

		.AddState("TRIP_UPPERCUT")
		.OnStart([this]()
	{
		Attack_Start(TRIP_UPPERCUT);
	})
		.OnExit([this]()
	{
		Attack_End(&m_iFarAttackIndex, WARRIR_FAR_ATTACK_COUNT, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("JUMP_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TRIP_UPPERCUT);
	})


		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(PARRIED);
		m_pModelCom->Set_AnimIndex(PARRIED);
	})
		.OnExit([this]()
	{
		m_bWeaklyHit = m_bStronglyHit = false;
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("PARRIED to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(PARRIED);
	})


		
		.AddState("DYING")
		.OnStart([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(DEATH);
		m_pModelCom->Set_AnimIndex(DEATH);

		m_pKena->Dead_FocusRotIcon(this);
		m_bDying = true;
		// m_pUIHPBar->Set_Active(false);		
	})
		.AddTransition("DYING to DEATH_SCENE", "DEATH_SCENE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})



		.AddState("DEATH_SCENE")
		.OnStart([this]()
	{
		// 죽은 애니메이션 후 죽음 연출 State
	})
		.AddTransition("DEATH_SCENE to DEATH", "DEATH")
		.Predicator([this]()
	{
		return true;
	})
		


		.AddState("DEATH")
		.OnStart([this]()
	{
		m_pTransformCom->Clear_Actor();
		Clear_Death();
	})


		.Build();

	return S_OK;
}

HRESULT CBossWarrior::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Boss_Warrior", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_01_EMISSIVE.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_OPACITY.png")), E_FAIL);

	m_pModelCom->Set_RootBone("VL_Warrior");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_BossWarrior.json");

	CBossWarrior_Hat::MONSTERWEAPONDESC		WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CBossWarrior_Hat::MONSTERWEAPONDESC));

	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("HatJoint");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	WeaponDesc.pOwnerMonster = this;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
		
	m_pHat = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BossWarrior_Hat"), L"BossWarrior_Hat", &WeaponDesc);
	assert(m_pHat && "Boss Warrior Hat is nullptr");
	m_pHat->Late_Initialize(nullptr);
	
	return S_OK;
}

HRESULT CBossWarrior::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_EmissiveColor", &m_fEmissiveColor, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fHDRIntensity", &m_fHDRIntensity, sizeof(_float)), E_FAIL);
	
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &g_bFalse, sizeof(_bool)), E_FAIL);
	// m_bDying && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CBossWarrior::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

void CBossWarrior::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	
	{	
		_matrix SocketMatrix = m_pWeaponBone->Get_OffsetMatrix() * m_pWeaponBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_WeaponPivotMatrix,
			XMMatrixRotationX(m_vWeaPonPivotRot.x) * XMMatrixRotationY(m_vWeaPonPivotRot.y) * XMMatrixRotationZ(m_vWeaPonPivotRot.z)
			* XMMatrixTranslation(m_vWeaPonPivotTrans.x, m_vWeaPonPivotTrans.y, m_vWeaPonPivotTrans.z));

		SocketMatrix = XMLoadFloat4x4(&m_WeaponPivotMatrix) * SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(COL_WEAPON_TEXT, mat);
	}

	{
		_matrix SocketMatrix = m_pRightLegBone->Get_OffsetMatrix() * m_pRightLegBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_RightLegPivotMatrix, XMMatrixTranslation(m_vRightLegPivotTrans.x, m_vRightLegPivotTrans.y, m_vRightLegPivotTrans.z));

		SocketMatrix = XMLoadFloat4x4(&m_RightLegPivotMatrix) * SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(COL_RIGHT_LEG_TEXT, mat);
	}
}

void CBossWarrior::AdditiveAnim(_float fTimeDelta)
{
	CMonster::AdditiveAnim(fTimeDelta);
}

void CBossWarrior::Set_AttackType()
{
}

void CBossWarrior::Reset_Attack()
{
}

void CBossWarrior::Tick_Attack(_float fTimeDelta)
{
}

void CBossWarrior::Set_AFType()
{
}

void CBossWarrior::Reset_AF()
{
}

CBossWarrior* CBossWarrior::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossWarrior*	pInstance = new CBossWarrior(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBossWarrior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossWarrior::Clone(void* pArg)
{
	CBossWarrior*	pInstance = new CBossWarrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBossWarrior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossWarrior::Free()
{
	CMonster::Free();
	Safe_Release(m_pHat);
}


_int CBossWarrior::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget && m_bSpawn)
	{
		if ((iColliderIndex == (_int)COL_PLAYER_WEAPON || iColliderIndex == (_int)COL_PLAYER_ARROW) && m_pKena->Get_State(CKena::STATE_ATTACK))
		{
			m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());

			// m_pUIHPBar->Set_Active(true);
			// m_pUIHPBar->Set_Guage(m_pMonsterStatusCom->Get_PercentHP());

			m_bWeaklyHit = true;
			m_bStronglyHit = true;

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);

			if (m_pKena->Get_State(CKena::STATE_HEAVYATTACK) == false)
			{
				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.15f);
				m_pKena->Add_HitStopTime(0.15f);
				m_fHitStopTime += 0.15f;
				//	dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.003f, 5);
			}
			else
			{
				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.3f);
				m_pKena->Add_HitStopTime(0.25f);
				m_fHitStopTime += 0.25f;
				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.005f, 5);

				vector<_float4>*		vecWeaponPos = m_pKena->Get_WeaponPositions();
				if (vecWeaponPos->size() == 2)
				{
					_vector	vDir = vecWeaponPos->back() - vecWeaponPos->front();
					vDir = XMVectorSetZ(vDir, 0.f);
					//	dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(vDir, XMConvertToRadians/(30.f));
				}
			}
		}
	}

	return 0;
}

void CBossWarrior::Attack_Start(_uint iAnimIndex)
{
	m_bRealAttack = true;

	m_pModelCom->ResetAnimIdx_PlayTime(iAnimIndex);
	m_pModelCom->Set_AnimIndex(iAnimIndex);
}

void CBossWarrior::Attack_End(_uint* pAttackIndex, _uint iMaxAttackIndex, _uint iAnimIndex)
{
	(*pAttackIndex)++;
	(*pAttackIndex) %= iMaxAttackIndex;

	m_pModelCom->ResetAnimIdx_PlayTime(iAnimIndex);
	m_pModelCom->Set_AnimIndex(iAnimIndex);

	m_bRealAttack = false;
}