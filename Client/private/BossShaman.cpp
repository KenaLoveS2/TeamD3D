#include "stdafx.h"
#include "..\public\BossShaman.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Sticks01.h"
#include "ShamanTrapHex.h"
#include "E_RectTrail.h"

CBossShaman::CBossShaman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CBossShaman::CBossShaman(const CBossShaman& rhs)
	: CMonster(rhs)
{
}

HRESULT CBossShaman::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBossShaman::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	// FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	// FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

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
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pWeaponBone = m_pModelCom->Get_BonePtr("sword_root_jnt");

	Create_Minions();
	
	return S_OK;
}

HRESULT CBossShaman::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		//_float3 vPivotScale = _float3(0.25f, 0.25f, 1.f);
		_float3 vPivotScale = _float3(0.5f, 0.5f, 1.f);
		_float3 vPivotPos = _float3(0.f, 1.f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		
		PxCapsuleDesc.fLinearDamping = MONSTER_LINEAR_DAMING;
		PxCapsuleDesc.fAngularDamping = MONSTER_ANGULAR_DAMING;
		PxCapsuleDesc.fMass = MONSTER_MASS;
		
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_MONSTER));

		// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
		m_pTransformCom->Set_PxPivotScale(vPivotScale);
		m_pTransformCom->Set_PxPivot(vPivotPos);
	}
	
	{
		_float3 vPivotScale = _float3(0.6f, 0.7f, 1.f);
		
		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = TEXT_COL_SHAMAN_BODY;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 10.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER));
		m_pTransformCom->Add_Collider(TEXT_COL_SHAMAN_BODY, g_IdentityFloat4x4);
	}

	{			
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = TEXT_COL_SHAMAN_WEAPON;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = 0.12f;
		PxCapsuleDesc.fHalfHeight = 1.f;
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
		m_pTransformCom->Add_Collider(TEXT_COL_SHAMAN_WEAPON, g_IdentityFloat4x4);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);

	m_pShamanTapHex = (CShamanTrapHex*)m_pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Monster"), TEXT("ShamanTrapHex_0"));
	assert(m_pShamanTapHex && "FAILED!! -> CBossShaman::Late_Initialize()");

	return S_OK;
}

void CBossShaman::Tick(_float fTimeDelta)
{	
	/*m_pModelCom->Play_Animation(fTimeDelta);
	Update_Collider(fTimeDelta);
	return;*/

	if (m_bDeath) return;

	__super::Tick(fTimeDelta);
	
	SwordRenderProc(fTimeDelta);
	Update_Collider(fTimeDelta);
	Update_Trail("sword_jnt_6");
	// Update_MovementTrail("char_spine_low_jnt");

	// if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	for (auto& Pair : m_mapEffect)
		Pair.second->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CBossShaman::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	
	for (auto& Pair : m_mapEffect)
		Pair.second->Late_Tick(fTimeDelta);

	if (m_pRendererCom /*&& m_bSpawn*/)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CBossShaman::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		// Cloth UV2
		if(i==0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_OpacityTexture"),E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M), E_FAIL);
		}
		else if(i == 3) // 검 렌더
		{	
			if (m_eSwordRenderState == NO_RENDER) continue;
			else if (m_eSwordRenderState == CREATE || m_eSwordRenderState == DISSOLVE) {
				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &g_bTrue, sizeof(_bool)), E_FAIL);
				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fSwordDissolveTime, sizeof(_float)), E_FAIL);
				FAILED_CHECK_RETURN(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"), E_FAIL);

				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);

				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDying, sizeof(_bool)), E_FAIL);
				m_bDying && Bind_Dissolove(m_pShaderCom);
			}
			else if (m_eSwordRenderState == RENDER) {
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
			}
		}
		else 
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
		}
	}
	return S_OK;
}

HRESULT CBossShaman::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CBossShaman::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	float fTrans[3] = { m_vWeaPonPivotTrans.x, m_vWeaPonPivotTrans.y, m_vWeaPonPivotTrans.z };
	ImGui::DragFloat3("Weapon Trans", fTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vWeaPonPivotTrans, fTrans, sizeof(_float3));

	float fRot[3] = { m_vWeaPonPivotRot.x, m_vWeaPonPivotRot.y, m_vWeaPonPivotRot.z };
	ImGui::DragFloat3("Weapon Rot", fRot, 0.01f, -100.f, 100.0f);
	memcpy(&m_vWeaPonPivotRot, fRot, sizeof(_float3));
}

void CBossShaman::ImGui_AnimationProperty()
{
	if (ImGui::CollapsingHeader("BossShaman"))
	{
		ImGui::BeginTabBar("BossShaman Animation & State");

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

void CBossShaman::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CBossShaman::ImGui_PhysXValueProperty()
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

HRESULT CBossShaman::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CBossShaman::Push_EventFunctions()
{
	// CMonster::Push_EventFunctions();
	
	LookAt_Kena(true, 0.0f);

	TurnOnTrail(true, 0.0f);
	TurnOffTrail(true, 0.0f);
	TurnOffMoveMentTrail(true, 0.0f);
	TurnOnHandSummons(true, 0.0f);
	TurnOffHandSummons(true, 0.0f);
	TurnOnSwipeChareged(true, 0.0f);
	TurnOnTeleport(true, 0.0f);

	TurnOnSummons(true, 0.0f);
	TurnOffSummons(true, 0.0f);
}

HRESULT CBossShaman::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")		
		.OnExit([this]()
	{

	})
		.AddTransition("SLEEP to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{
		// 대기 종결 조건 수정 필요
		m_fSpawnRange = 5.f;
		return DistanceTrigger(m_fSpawnRange);
	})


		.AddState("READY_SPAWN")
		.OnStart([this]()
	{
		// 등장 연출 필요
		m_pModelCom->ResetAnimIdx_PlayTime(AWAKE);
		m_pModelCom->Set_AnimIndex(AWAKE);

		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = 20.f; /* == BossWarrior Name */
		m_BossShamanDelegator.broadcast(eBossHP, fValue);
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
		m_fIdleTimeCheck = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdleTimeCheck += fTimeDelta;
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("IDLE To BACK_FLIP", "BACK_FLIP")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fBackFlipRange) && m_pKena->Get_State(CKena::STATE_ATTACK);
	})
		.AddTransition("IDLE To TELEPORT_INTO", "TELEPORT_INTO")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdleTimeCheck, 2.f);
	})
				

		.AddState("BACK_FLIP")
		.OnStart([this]()
	{		
		m_pModelCom->ResetAnimIdx_PlayTime(BACK_FLIP);
		m_pModelCom->Set_AnimIndex(BACK_FLIP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("BACK_FLIP to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BACK_FLIP);
	})


		.AddState("SUMMON_INTO")
		.OnStart([this]()
	{
		m_bNoDamage = true;
		m_pModelCom->ResetAnimIdx_PlayTime(SUMMON_INTO);
		m_pModelCom->Set_AnimIndex(SUMMON_INTO);
		Summon();
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(SUMMON_LOOP);
	})
		.AddTransition("SUMMON_INTO to SUMMON_LOOP", "SUMMON_LOOP")
		.Predicator([this]()
	{
		return AnimFinishChecker(SUMMON_INTO);
	})

		.AddState("SUMMON_LOOP")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SUMMON_LOOP);
		m_pModelCom->Set_AnimIndex(SUMMON_LOOP);
	})
		.OnExit([this]()
	{	
		m_pModelCom->Set_AnimIndex(SUMMON_EXIT);
	})
		.AddTransition("SUMMON_LOOP to SUMMON_EXIT", "SUMMON_EXIT")
		.Predicator([this]()
	{	
		return AnimFinishChecker(SUMMON_LOOP);
	})

		.AddState("SUMMON_EXIT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SUMMON_EXIT);
		m_pModelCom->Set_AnimIndex(SUMMON_EXIT);
	})
		.OnExit([this]()
	{
		m_bNoDamage = false;
		m_bSummonEnd = true;		
		m_pModelCom->ResetAnimIdx_PlayTime(IDLE_LOOP);
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);		
	})
		.AddTransition("SUMMON_EXIT to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(SUMMON_EXIT);
	})

		.AddState("FOG_SNAP")
		.OnStart([this]()
	{
		// 애니메이션 끝날 때쯤 맵 전체 안개 생성
		Attack_Start(false, FOG_SNAP);
	})
		.OnExit([this]()
	{
		m_bFogSnapEnd = true;
		Attack_End(false, IDLE_LOOP);
	})
		.AddTransition("FOG_SNAP to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(FOG_SNAP);
	})
		
		
		
		.AddState("TELEPORT_INTO")
		.OnStart([this]()
	{
		m_bNoDamage = true;
		m_pModelCom->ResetAnimIdx_PlayTime(TELEPORT_INTO);
		m_pModelCom->Set_AnimIndex(TELEPORT_INTO);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(TELEPORT_LOOP);
	})		
		.AddTransition("TELEPORT_INTO to TELEPORT_LOOP", "TELEPORT_LOOP_DISSOLVE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TELEPORT_INTO);
	})


		.AddState("TELEPORT_LOOP_DISSOLVE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(TELEPORT_LOOP);
		m_pModelCom->Set_AnimIndex(TELEPORT_LOOP);

		m_bTeleportDissolve = true;
		m_fTeleportDissolveTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime += m_fDissolveRate;
	})
		.OnExit([this]()
	{		
		m_fTeleportDissolveTime = 1.f;
	})		
		.AddTransition("TELEPORT_LOOP to TELEPORT_LOOP_CREATE", "TELEPORT_LOOP_CREATE")
		.Predicator([this]()
	{
		return m_fTeleportDissolveTime >= 1.f;
	})


		.AddState("TELEPORT_LOOP_CREATE")
		.OnStart([this]()
	{
		_vector vRandDir = XMVector3Normalize(XMVectorSet(CUtile::Get_RandomFloat(-1.f, 1.f), 0.f, CUtile::Get_RandomFloat(-1.f, 1.f), 0.f));
		_float fRange = m_fTeleportRange + m_fTeleportRangeTable[m_eAttackType];
		_float4 vRandPos = XMLoadFloat4(&m_vKenaPos) + vRandDir * fRange;

		m_pTransformCom->Set_Position(vRandPos);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);

		m_pModelCom->ResetAnimIdx_PlayTime(TELEPORT_LOOP);
		m_pModelCom->Set_AnimIndex(TELEPORT_LOOP);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime -= m_fDissolveRate;
	})
		.OnExit([this]()
	{
		m_bTeleportDissolve = false;
		m_fTeleportDissolveTime = 0.f;
	})
		.AddTransition("TELEPORT_LOOP_CREATE to TELEPORT_EXIT", "TELEPORT_EXIT")
		.Predicator([this]()
	{
		return m_fTeleportDissolveTime <= 0.f;
	})


		.AddState("TELEPORT_EXIT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(TELEPORT_EXIT);
		m_pModelCom->Set_AnimIndex(TELEPORT_EXIT);
	})
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
		m_bNoDamage = false;
	})
		.AddTransition("IDLE To SUMMON_INTO", "SUMMON_INTO")
		.Predicator([this]()
	{
		return m_bSummonEnd == false && m_pMonsterStatusCom->Get_PercentHP() < 0.7f;
	})
		.AddTransition("IDLE To FOG_SNAP", "FOG_SNAP")
		.Predicator([this]()
	{
		return m_bFogSnapEnd == false && m_pMonsterStatusCom->Get_PercentHP() < 0.3f;
	})
		.AddTransition("TELEPORT_EXIT To MELEE_ATTACK", "MELEE_ATTACK")
		.Predicator([this]()
	{
		return m_eAttackType == AT_MELEE_ATTACK;
	})
		.AddTransition("TELEPORT_EXIT To TRIPLE_ATTACK", "TRIPLE_ATTACK")
		.Predicator([this]()
	{
		return m_eAttackType == AT_TRIPLE_ATTACK;
	})
		.AddTransition("TELEPORT_EXIT To TRAP", "TRAP")
		.Predicator([this]()
	{		
		return m_eAttackType == AT_TRAP;
	})		
		.AddTransition("TELEPORT_EXIT To DASH_ATTACK", "DASH_ATTACK")
		.Predicator([this]()
	{
		return m_eAttackType == AT_DASH_ATTACK;
	})
		.AddTransition("TELEPORT_EXIT To FREEZE_BLAST", "FREEZE_BLAST")
		.Predicator([this]()
	{
		return m_eAttackType == AT_FREEZE_BLAST;
	})



		.AddState("MELEE_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(true, MELEE_ATTACK);
	})	
		.OnExit([this]()
	{	
		Attack_End(true, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("MELEE_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(MELEE_ATTACK);
	})


		.AddState("TRIPLE_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(true, TRIPLE_ATTACK);		
	})
		.OnExit([this]()
	{	
		Attack_End(true, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("TRIPLE_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TRIPLE_ATTACK);
	})
		

		.AddState("TRAP")
		.OnStart([this]()
	{	
		m_pModelCom->ResetAnimIdx_PlayTime(TRAP);
		m_pModelCom->Set_AnimIndex(TRAP);
	})
		.OnExit([this]()
	{	
		m_pModelCom->ResetAnimIdx_PlayTime(TRAP_LOOP);
		m_pModelCom->Set_AnimIndex(TRAP_LOOP);
	})
		.AddTransition("TRAP to TRAP_TELEPORT", "TRAP_TELEPORT")
		.Predicator([this]()
	{	
		return AnimFinishChecker(TRAP);	
	})

		.AddState("TRAP_TELEPORT")
		.OnStart([this]()
	{		
		m_pModelCom->Set_AnimIndex(TRAP_LOOP);
		m_bTeleportDissolve = true;
		m_fTeleportDissolveTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime += m_fDissolveRate;
	})
		.OnExit([this]()
	{	
		m_fTeleportDissolveTime = 1.f;		
	})
		.AddTransition("TRAP_TELEPORT to TRAP_LOOP", "TRAP_LOOP")
		.Predicator([this]()
	{
		return m_fTeleportDissolveTime > 1.f;
	})

		.AddState("TRAP_LOOP")
		.OnStart([this]()
	{	
		m_bTraptBreak = false;
		m_bTraptLoop = true;
		m_bTrapOffset = true;
		m_bNoDamage = false;
		m_pModelCom->Set_AnimIndex(TRAP_LOOP);

		m_vTrapLookPos = m_vKenaPos;
		m_pShamanTapHex->Execute_Trap(m_vKenaPos);
		m_fTeleportDissolveTime = 0.f;
		m_bTeleportDissolve = false;
	})
		.Tick([this](_float fTimeDelta)
	{		
		m_pTransformCom->LookAt_NoUpDown(m_vTrapLookPos);
		_float4 vTrapPos = m_pShamanTapHex->Get_JointBonePos();
		vTrapPos.y = 0.f;
		vTrapPos.y -= m_fTrapOffstY;
		
		m_pTransformCom->Set_Position(vTrapPos);
	})
		.OnExit([this]()
	{	
		m_bTrapOffset = false;
		m_bTraptLoop = false;		
		m_pModelCom->Set_AnimIndex(TRAP_LOOP);
	})
		.AddTransition("TRAP_LOOP to LASER_FIRE", "LASER_FIRE")
		.Predicator([this]()
	{	
		// return false;
		return m_pShamanTapHex->IsTrapSuccess();
	})
		.AddTransition("TRAP_LOOP to TRAP_BREAK", "TRAP_BREAK")
		.Predicator([this]()
	{
		return m_bTraptBreak;
	})


		.AddState("LASER_FIRE")
		.OnStart([this]()
	{
		Attack_Start(false, TRAP_LOOP);
	})
		.OnExit([this]()
	{
		Attack_End(false, IDLE_LOOP);
	})		
		.AddTransition("LASER_FIRE to TRAP_END", "TRAP_END")
		.Predicator([this]()
	{
		// 레이저 발사가 끝나면 아이들로
		return true;
	})

		.AddState("TRAP_END")
		.OnStart([this]()
	{
		m_bTeleportDissolve = true;
		m_fTeleportDissolveTime = 0.f;

		m_pModelCom->ResetAnimIdx_PlayTime(TRAP_ESCAPE);
		m_pModelCom->Set_AnimIndex(TRAP_ESCAPE);
		m_pShamanTapHex->Execute_End();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime += m_fDissolveRate * 0.5f;
		m_pModelCom->Set_AnimIndex(TRAP_ESCAPE);
	})
		.OnExit([this]()
	{	
		m_fTeleportDissolveTime = 1.f;
		m_pModelCom->Set_AnimIndex(TELEPORT_LOOP);
	})
		.AddTransition("TRAP_END to TELEPORT_LOOP_CREATE", "TELEPORT_LOOP_CREATE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TRAP_ESCAPE);
	})

		.AddState("TRAP_BREAK")
		.OnStart([this]()
	{
		m_bTeleportDissolve = true;
		m_fTeleportDissolveTime = 0.f;

		m_pModelCom->ResetAnimIdx_PlayTime(TRAP_ESCAPE);
		m_pModelCom->Set_AnimIndex(TRAP_ESCAPE);
		m_pShamanTapHex->Execute_Break();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime += m_fDissolveRate * 0.5f;
	})
		.OnExit([this]()
	{	
		m_fTeleportDissolveTime = 1.f;

		_float4 vPos = m_vKenaPos + _float4(2.f, 0.f, 2.f, 0.f);
		m_pTransformCom->Set_Position(vPos);

		Attack_End(false, STUN_TAKE_DAMAGE);
	})
		.AddTransition("TRAP_BREAK to STUN", "STUN")
		.Predicator([this]()
	{	
		return AnimFinishChecker(TRAP_ESCAPE);
	})


		.AddState("STUN")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(STUN_TAKE_DAMAGE);		
		m_fStunTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime -= m_fDissolveRate;
		if (m_fTeleportDissolveTime < 0.f)
		{
			m_fTeleportDissolveTime = 0.f;
			m_bTeleportDissolve = false;
		}

		m_fStunTime += fTimeDelta;
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("STUN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return TimeTrigger(m_fStunTime, 5.f);
	})


		.AddState("DASH_ATTACK")
		.OnStart([this]()
	{		
		// 애니메이션 끝날 때쯤 펄스 같은 광역 이펙트
		Attack_Start(false, DASH_ATTACK);
	})
		.OnExit([this]()
	{
		Attack_End(false, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("DASH_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(DASH_ATTACK);
	})


		.AddState("FREEZE_BLAST")
		.OnStart([this]()
	{
		// 애니메이션 끝날 때쯤 얼음 빔 발사
		Attack_Start(false, FREEZE_BLAST);
	})
		.OnExit([this]()
	{
		Attack_End(false, IDLE_LOOP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("FREEZE_BLAST to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(FREEZE_BLAST);
	})

		

		.AddState("DYING")
		.OnStart([this]()
	{	
		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = -1.f;
		m_BossShamanDelegator.broadcast(eBossHP, fValue);
		
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(DEATH);
		m_pModelCom->Set_AnimIndex(DEATH);

		m_pKena->Dead_FocusRotIcon(this);
		m_bDying = true;		

		for (_uint i = 0; i < MINION_COUNT; i++)
			m_pMinions[i]->Execute_Dying();
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

HRESULT CBossShaman::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Boss_Shaman", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv02_ALPHA.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Shaman_Uv01_EMISSIVE.png")), E_FAIL);

	m_pModelCom->Set_RootBone("Shaman");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_BossShaman.json");

	//SK_VL_Shaman.ao (additive)

	return S_OK;
}

HRESULT CBossShaman::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_EmissiveColor", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4)), E_FAIL);
	float fHDRIntensity = 0.f;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fHDRIntensity", &fHDRIntensity, sizeof(_float)), E_FAIL);
	


	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bTeleportDissolve, sizeof(_bool)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fTeleportDissolveTime, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;

	return S_OK;
}

HRESULT CBossShaman::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CBossShaman::Ready_Effects()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Base* pEffectBase = nullptr;

	/* Trail */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanTrail", L"S_Trail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("S_Trail", pEffectBase);
	
	_tchar* pDummyString = CUtile::Create_DummyString();
	m_pMovementTrail = dynamic_cast<CE_RectTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", pDummyString));
	NULL_CHECK_RETURN(m_pMovementTrail, E_FAIL);
	m_pMovementTrail->SetUp_Option(CE_RectTrail::OBJ_BODY_SHAMAN);
	m_pMovementTrail->Set_Parent(this);
	/* Trail */

	/* Plate */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanBossPlate", L"S_Plate"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("S_Plate", pEffectBase);

	/* Hand */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanBossHandPlane", L"S_Hand"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("S_Hand", pEffectBase);

	/* Summons */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanSummons", L"S_Summons"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("S_Summons", pEffectBase);

	/* Shaman_Charged */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Swipes_Charged", L"Shaman_Charged"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("Shaman_Charged", pEffectBase);

	/* Shaman_Smoke */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanSmoke", L"Shaman_Smoke"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("Shaman_Smoke", pEffectBase);
	
	return S_OK;
}

void CBossShaman::Update_Trail(const char* pBoneTag)
{
	CBone* pBonePtr = m_pModelCom->Get_BonePtr(pBoneTag);
	_matrix SocketMatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	m_mapEffect["S_Trail"]->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	m_pMovementTrail->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	dynamic_cast<CEffect_Trail*>(m_mapEffect["S_Trail"])->Trail_InputPos(matWorldSocket.r[3]);
	
	if (m_mapEffect["S_Trail"]->Get_Active() == true)
	{
		_float4 vPos = m_pTransformCom->Get_Position();
		vPos.y = vPos.y + 1.f;
		m_pMovementTrail->Trail_InputRandomPos(vPos);
	}
}

void CBossShaman::Update_Collider(_float fTimeDelta)
{
	{	
		m_fTrapOffstY = 5.f;
		_float fTemp[2] = { 1.f , 3.2f };
		_float4x4 PivotMatrix;
		XMStoreFloat4x4(&PivotMatrix, XMMatrixTranslation(0.f, fTemp[m_bTrapOffset], 0.f));
		m_pTransformCom->Update_Collider(TEXT_COL_SHAMAN_BODY, PivotMatrix);
	}
	

	if (m_eSwordRenderState == RENDER)
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
		m_pTransformCom->Update_Collider(TEXT_COL_SHAMAN_WEAPON, mat);
	}

	m_pTransformCom->Tick(fTimeDelta);
}

void CBossShaman::AdditiveAnim(_float fTimeDelta)
{
	CMonster::AdditiveAnim(fTimeDelta);
}

void CBossShaman::Set_AttackType()
{
}

void CBossShaman::Reset_Attack()
{
}

void CBossShaman::Tick_Attack(_float fTimeDelta)
{
}

void CBossShaman::Set_AFType()
{
}

void CBossShaman::Reset_AF()
{
}

void CBossShaman::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnTrail);
		return;
	}
	m_mapEffect["S_Trail"]->Set_Active(true);
}

void CBossShaman::TurnOffTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffTrail);
		return;
	}
	m_mapEffect["S_Trail"]->Set_Active(false);
	dynamic_cast<CEffect_Trail*>(m_mapEffect["S_Trail"])->ResetInfo();
}

void CBossShaman::TurnOffMoveMentTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffMoveMentTrail);
		return;
	}
	m_pMovementTrail->Set_Active(false);
}

void CBossShaman::TurnOnHandSummons(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnHandSummons);
		return;
	}

	CBone* pBonePtr = m_pModelCom->Get_BonePtr("char_lf_ring_d_jnt");
	_matrix matrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix Shamanmatrix = m_pTransformCom->Get_WorldMatrix();

	_matrix matSocket = matrix * Shamanmatrix;
	_float4 vHandPos = matSocket.r[3];

	m_mapEffect["S_Hand"]->Set_Position(vHandPos);
	m_mapEffect["S_Hand"]->Set_Active(true);
}

void CBossShaman::TurnOffHandSummons(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffHandSummons);
		return;
	}
	m_mapEffect["S_Hand"]->Set_Active(false);
}

void CBossShaman::TurnOnSwipeChareged(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnSwipeChareged);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_Position();
	m_mapEffect["Shaman_Charged"]->Set_Position(vPos);
	m_mapEffect["Shaman_Charged"]->Set_Active(true);
}

void CBossShaman::TurnOnTeleport(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnTeleport);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_Position();
	m_mapEffect["Shaman_Smoke"]->Set_Position(vPos);
	m_mapEffect["Shaman_Smoke"]->Set_Active(true);

	Update_MovementTrail("char_spine_low_jnt");
	m_pMovementTrail->Set_Active(true);
}

void CBossShaman::TurnOnSummons(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnSummons);
		return;
	}

	_float4 vPos = m_pTransformCom->Get_Position();
	vPos.y = vPos.y + 0.1f;

	m_mapEffect["S_Plate"]->Set_Position(vPos);
	m_mapEffect["S_Plate"]->Set_Active(true);

	m_mapEffect["S_Summons"]->Set_Position(vPos);
	m_mapEffect["S_Summons"]->Set_Active(true);

}

void CBossShaman::TurnOffSummons(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffSummons);
		return;
	}

	m_mapEffect["S_Plate"]->Set_Active(false);
	m_mapEffect["S_Summons"]->Set_Active(false);
}

CBossShaman* CBossShaman::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossShaman*	pInstance = new CBossShaman(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBossShaman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossShaman::Clone(void* pArg)
{
	CBossShaman*	pInstance = new CBossShaman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBossShaman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossShaman::Free()
{
	CMonster::Free();
}

void CBossShaman::SwordRenderProc(_float fTimeDelta)
{
	switch (m_eSwordRenderState)
	{
	case NO_RENDER:
		m_fSwordDissolveTime = 1.f;
		break;

	case CREATE:
		m_fSwordDissolveTime -= m_fDissolveRate;
		if (m_fSwordDissolveTime < 0.f)
		{
			m_fSwordDissolveTime = 0.f;
			m_eSwordRenderState = RENDER;
		}
		break;

	case RENDER:
		m_fSwordDissolveTime = 0.f;
		break;

	case DISSOLVE:
		m_fSwordDissolveTime += m_fDissolveRate;
		if (m_fSwordDissolveTime > 1.f)
		{
			m_fSwordDissolveTime = 1.f;
			m_eSwordRenderState = NO_RENDER;
		}
		break;
	}
}

void CBossShaman::Attack_Start(_bool bSwordRender, _uint iAnimIndex)
{
	m_eSwordRenderState = bSwordRender ? CREATE : m_eSwordRenderState;

	CMonster::Attack_Start(iAnimIndex);
}

void CBossShaman::Attack_End(_bool bSwordRender, _uint iAnimIndex)
{
	m_eSwordRenderState = bSwordRender ? DISSOLVE : m_eSwordRenderState;

	_uint iTemp = m_eAttackType + 1;
	iTemp %= ATTACKTYPE_END;
	m_eAttackType = (ATTACKTYPE)iTemp;

	CMonster::Attack_End(iAnimIndex);
}

_int CBossShaman::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget && m_bSpawn)
	{
		if (m_bTraptLoop && iColliderIndex == (_int)COL_PLAYER_ARROW)
		{
			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			m_bTraptBreak = true;
			return 0;
		}

		if ((iColliderIndex == (_int)COL_PLAYER_WEAPON && m_pKena->Get_State(CKena::STATE_ATTACK)))
		{
			if (m_bNoDamage == false)
				m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());

			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
			_float fGauge = m_pMonsterStatusCom->Get_PercentHP();
			m_BossShamanDelegator.broadcast(eBossHP, fGauge);

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);

			if (m_pKena->Get_State(CKena::STATE_HEAVYATTACK) == false)
			{
				m_bWeaklyHit = true;
				m_bStronglyHit = false;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.15f);
				m_pKena->Add_HitStopTime(0.15f);
				m_fHitStopTime += 0.15f;
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.003f, 5);
			}
			else
			{
				m_bWeaklyHit = false;
				m_bStronglyHit = true;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.3f);
				m_pKena->Add_HitStopTime(0.25f);
				m_fHitStopTime += 0.25f;
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.005f, 5);

				vector<_float4>* vecWeaponPos = m_pKena->Get_WeaponPositions();
				if (vecWeaponPos->size() == 2)
				{
					_vector	vDir = vecWeaponPos->back() - vecWeaponPos->front();
					vDir = XMVectorSetZ(vDir, 0.f);
					dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(vDir, XMConvertToRadians(30.f));
				}
			}
		}

		if (iColliderIndex == (_int)COL_PLAYER_ARROW)
		{
			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());

			//m_bStronglyHit = m_pKena->Get_State(CKena::STATE_INJECTBOW);
			//m_bWeaklyHit = !m_bStronglyHit;

			m_bStronglyHit = true;
			m_bWeaklyHit = false;

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);
		}
	}

	return 0;
}


void CBossShaman::Create_Minions()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < MINION_COUNT; i++)
	{
		pGameInstance->Clone_AnimObject(
			g_LEVEL,
			TEXT("Layer_Monster"),
			TEXT("Prototype_GameObject_Sticks01"),
			CUtile::Create_DummyString(TEXT("Summon_Stick"), i),
			nullptr,
			(CGameObject**)&m_pMinions[i]);

		m_pMinions[i]->Set_DeathFlag(true);
	}

	RELEASE_INSTANCE(CGameInstance)
}

void CBossShaman::Summon()
{
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vOffset[MINION_COUNT] = {
		_float4(0.f, 0.f, 3.f, 0.f),
		_float4(3.f, 0.f, 0.f, 0.f),
		_float4(-3.f, 0.f, 0.f, 0.f),
	};

	for (_uint i = 0; i < MINION_COUNT; i++)
	{
		m_pMinions[i]->Spawn_ByMaster(this, vPos + vOffset[i]);		
	}
}

void CBossShaman::LookAt_Kena(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::LookAt_Kena);
		return;
	}

	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
}