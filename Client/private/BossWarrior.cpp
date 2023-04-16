#include "stdafx.h"
#include "..\public\BossWarrior.h"
#include "GameInstance.h"
#include "Bone.h"
#include "BossWarrior_Hat.h"
#include "CameraForNpc.h"
#include "E_WarriorTrail.h"
#include "E_RectTrail.h"
#include "E_Hieroglyph.h"
#include "ControlRoom.h"
#include "E_Warrior_FireSwipe.h"
#include "SpiritArrow.h"
#include "CinematicCamera.h"
#include "BossRock_Pool.h"
#include "E_WarriorEyeTrail.h"
#include "BGM_Manager.h"

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
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);
	// FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	// FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
	{
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	}
	else
	{
		m_Desc.pGroupName = L"";
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -15.f;
		m_Desc.WorldMatrix._43 = -15.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_pModelCom->Init_AnimIndex(IDLE_LOOP);
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pWeaponBone = m_pModelCom->Get_BonePtr("Halberd_Root_Jnt");
	assert(m_pWeaponBone && "CBossWarrior::Initialize()");
	m_pRightLegBone = m_pModelCom->Get_BonePtr("char_rt_ankle_jnt");
	assert(m_pRightLegBone && "CBossWarrior::Initialize()");
	m_pGrabHandBone = m_pModelCom->Get_BonePtr("char_lf_hand_jnt");
	assert(m_pGrabHandBone && "CBossWarrior::Initialize()");
	m_pGrabJointBone = m_pModelCom->Get_BonePtr("GrabJoint");
	assert(m_pGrabJointBone && "CBossWarrior::Initialize()");

	XMStoreFloat4x4(&m_WeaponPivotMatrix, 
		XMMatrixRotationX(m_vWeaPonPivotRot.x) * XMMatrixRotationY(m_vWeaPonPivotRot.y) * XMMatrixRotationZ(m_vWeaPonPivotRot.z)
		* XMMatrixTranslation(m_vWeaPonPivotTrans.x, m_vWeaPonPivotTrans.y, m_vWeaPonPivotTrans.z));
		
	XMStoreFloat4x4(&m_RightLegPivotMatrix, XMMatrixTranslation(m_vRightLegPivotTrans.x, m_vRightLegPivotTrans.y, m_vRightLegPivotTrans.z));
	XMStoreFloat4x4(&m_GrabHandPivotMatrix, XMMatrixTranslation(m_vGrabHandPivotTrans.x, m_vGrabHandPivotTrans.y, m_vGrabHandPivotTrans.z));

	CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	CBossRock_Pool::DESC BossRockPoolDesc;
	BossRockPoolDesc.iRockCount = 30;
	BossRockPoolDesc.vCenterPos = _float4(60.449f, 14.639f, 869.108f, 1.f);
	//BossRockPoolDesc.vCenterPos = _float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f);
	m_pBossRockPool = (CBossRock_Pool*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BossRockPool"), TEXT("Warrior_BossRockPool"), &BossRockPoolDesc);
	assert(m_pBossRockPool && "CBossWarrior::Initialize()");
	m_pBossRockPool->Late_Initialize(nullptr);

	m_bNoUseFocusIcon = true;

	return S_OK;
}

HRESULT CBossWarrior::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);
	
	{	
		_float3 vPivotScale = _float3(0.85f, 0.8f, 1.f); // _float3(0.8f, 3.f, 1.f); 
		_float3 vPivotPos = _float3(0.f, 1.6f, 0.f);    // _float3(0.f, 3.8f, 0.f)

		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = _float3( 0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.isGravity = true;
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fLinearDamping = MONSTER_LINEAR_DAMING;
		PxCapsuleDesc.fAngularDamping = MONSTER_ANGULAR_DAMING;
		PxCapsuleDesc.fMass = MONSTER_MASS;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_MONSTER));
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
	}
	
	{	
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = COL_WEAPON_TEXT;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = 0.18f;
		PxCapsuleDesc.fHalfHeight = 2.0f;
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

	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = COL_GRAB_HAND_TEXT;
		PxSphereDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fRadius = 0.6f;
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.fMass = 10.f;
		PxSphereDesc.fLinearDamping = 10.f;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
		PxSphereDesc.isTrigger = true;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_WARRIOR_GRAB_HAND));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	}

	m_pCineCam[0] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("BOSSWARRIOR_START")));
	m_pCineCam[1] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("BOSSWARRIOR_END")));

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);

	for (auto& Pair : m_mapEffect)
		Pair.second->Late_Initialize(nullptr);

	return S_OK;
}

void CBossWarrior::Tick(_float fTimeDelta)
{		
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);
	Update_Trail("Halberd_Jnt6");

	m_pHat->Tick(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	//m_pTransformCom->Set_Position(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	AdditiveAnim(fTimeDelta);

	m_pBossRockPool->Tick(fTimeDelta);
}

void CBossWarrior::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	m_pHat->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Late_Tick(fTimeDelta);

	if (m_pRendererCom /*&& m_bSpawn*/)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
		
	m_pBossRockPool->Late_Tick(fTimeDelta);
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
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E);
		}
		else if(i == 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			//m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_OpacityTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M);
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

	float fHandTrans[3] = { m_vGrabHandPivotTrans.x, m_vGrabHandPivotTrans.y, m_vGrabHandPivotTrans.z };
	ImGui::DragFloat3("GrabHand Trans", fHandTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vGrabHandPivotTrans, fHandTrans, sizeof(_float3));
}

void CBossWarrior::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();

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
	// CMonster::Push_EventFunctions();

	TurnOnTrail(true, 0.0f);
	TurnOffTrail(true, 0.0f);
	TurnOnSwipesCharged(true, 0.0f);
	TurnOnHieroglyph(true, 0.0f);
	TurnOnShockFrontExtended(true, 0.0f);

	TurnOnFireSwipe(true, 0.0f);
	TurnOnFireSwipe_End(true, 0.0f);
	TurnOnRoot(true, 0.0f);
	TurnOnPlaneRoot(true, 0.0f);

	TurnOnEnrage_Into(true, 0.0f);
	TurnOnEnrage_Attck(true, 0.0f);

	TurnOnCamShake(true, 0.0f);
	TurnOnMotionBlur(true, 0.0f);
	TurnOffMotionBlur(true, 0.0f);

	Grab_Turn(true, 0.f);

	// Sound CallBack
	Play_Attack1Sound(true, 0.0f);
	Play_Attack2Sound(true, 0.0f);
	Play_Attack3Sound(true, 0.0f);
	Play_Attack4Sound(true, 0.0f);
	Play_Attack5Sound(true, 0.0f);
	Play_Attack6Sound(true, 0.0f);
	Play_Attack7Sound(true, 0.0f);
	Play_Attack8Sound(true, 0.0f);
	Play_Attack9Sound(true, 0.0f);

	Play_Hit1Sound(true, 0.0f);
	Play_Hit2Sound(true, 0.0f);
	Play_Hit3Sound(true, 0.0f);
	Play_Hit4Sound(true, 0.0f);

	Play_Hurt1Sound(true, 0.0f);
	Play_Hurt2Sound(true, 0.0f);
	Play_Hurt3Sound(true, 0.0f);
	Play_Hurt4Sound(true, 0.0f);
	Play_Hurt5Sound(true, 0.0f);
	Play_Hurt6Sound(true, 0.0f);
	Play_Hurt7Sound(true, 0.0f);

	Play_Tense1Sound(true, 0.0f);
	Play_Tense2Sound(true, 0.0f);
	Play_Tense3Sound(true, 0.0f);

	Play_BackSound(true, 0.0f);
	
	Play_ImpactSound(true, 0.0f);
	Play_Impact3Sound(true, 0.0f);
	Play_Impact4Sound(true, 0.0f);
	Play_Impact5Sound(true, 0.0f);

	Play_WalkSound(true, 0.0f);
	Play_SwingSound(true, 0.0f);
	Play_SlashSound(true, 0.0f);

	Play_BossBaseSound(true, 0.0f);
	Play_BossDingSound(true, 0.0f);

	Play_Elemental1Sound(true, 0.0f);
	Play_Elemental2Sound(true, 0.0f);
	Play_Elemental11Sound(true, 0.0f);

	Start_RealAttack(true, 0.0f);
	End_RealAttack(true, 0.0f);

	Execute_UpRocksPool(true, 0.0f);
}

HRESULT CBossWarrior::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")				
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);
		m_pModelCom->Set_AnimationBlendDuration((_uint)IDLE_LOOP, 0.2f);
	})
		.OnExit([this]()
	{
		m_bReadySpawn = true;
	})
		.AddTransition("SLEEP to CINEMA", "IDLE") // "IDLE" "CINEMA"
		.Predicator([this]()
	{			
		m_fSpawnRange = 20.f;
		return DistanceTrigger(m_fSpawnRange);				
	})

		.AddState("CINEMA")
		.OnStart([this]()
	{
		BossFight_Start();
		CBGM_Manager::GetInstance()->Change_FieldState(CBGM_Manager::FIELD_BOSS_BATTLE_WARRIOR_PHASE_1);
	})
		.AddTransition("CINEMA to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{
		return m_pCineCam[0]->CameraFinishedChecker(0.3f);
	})

		.AddState("READY_SPAWN")
		.OnStart([this]()
	{	
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_BOSS_BASE], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(AWAKE);
		m_pModelCom->Set_AnimIndex(AWAKE);

		/* HP Bar Active */
		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = 10.f; /* == BossWarrior Name */
		m_BossWarriorDelegator.broadcast(eBossHP, fValue);
		/* ~HP Bar Active */
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fDissolveTime -= fTimeDelta * 0.5f;
		if (m_fDissolveTime < -0.5f)
			m_bDissolve = false;

		m_fFogRange -= fTimeDelta * 50.f;
		if (m_fFogRange < 60.f)
			m_fFogRange = 60.f;
		const _float4 vColor = _float4(140.f / 255.f, 70.f / 255.f, 70.f / 255.f, 1.f);
		m_pRendererCom->Set_FogValue(vColor, m_fFogRange);

		if (AnimIntervalChecker(AWAKE, 0.9f, 1.f))
			m_pModelCom->FixedAnimIdx_PlayTime(AWAKE, 0.95f);
	})
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);		
		m_bSpawn = true;
		m_pKena->Set_State(CKena::STATE_BOSSBATTLE, true);
		CGameInstance::GetInstance()->Work_Camera(L"PLAYER_CAM");
		m_pCineCam[0]->CinemaUIOff();
		m_pKena->Set_StateLock(false);
	})
		.AddTransition("READY_SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{	
		return m_pCineCam[0]->CameraFinishedChecker() && m_bDissolve == false;
	})

	
		.AddState("IDLE")
		.OnStart([this]()
	{	
		// m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.5f);
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
		return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && DistanceTrigger(m_fJumpBackRange);
	})
		.AddTransition("IDLE to CHASE_RUN", "CHASE_RUN")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && (m_eAttackType == AT_CHARGE || m_eAttackType == AT_UPPER_CUT || m_eAttackType == AT_COMBO || m_eAttackType == AT_SWEEP || m_eAttackType == AT_GRAB);
	})		
		.AddTransition("IDLE to CHASE_WALK", "CHASE_WALK")
		.Predicator([this]()
	{				
		return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && (m_eAttackType == AT_JUMP || m_eAttackType == AT_TRIP_UPPERCUT);
	})		
		


		.AddState("CHASE_RUN")
		.OnStart([this]()
	{	
		m_pModelCom->ResetAnimIdx_PlayTime(RUN);
		m_pTransformCom->Speed_Boost(true, 1.2f);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(RUN);
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta);
	})
		.OnExit([this]()
	{
		m_pTransformCom->Speed_Boost(true, 1.f);
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
		.AddTransition("CHASE_RUN to CHARGE_ATTACK", "CHARGE_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_eAttackType == AT_CHARGE;
	})
		.AddTransition("CHASE_RUN to UPPER_CUT", "UPPER_CUT")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_eAttackType == AT_UPPER_CUT;
	})
		.AddTransition("CHASE_RUN to COMBO_ATTACK", "COMBO_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_eAttackType == AT_COMBO;
	})
		.AddTransition("CHASE_RUN to SWEEP_ATTACK", "SWEEP_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_eAttackType == AT_SWEEP;
	})
		.AddTransition("CHASE_RUN to GRAB", "GRAB")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fCloseAttackRange) && m_eAttackType == AT_GRAB;
	})
		

		.AddState("CHASE_WALK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(WALK);		
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(WALK);
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta);
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
		.AddTransition("CHASE_WALK to JUMP_ATTACK", "JUMP_ATTACK")
		.Predicator([this]()
	{			
		return DistanceTrigger(m_fFarAttackRange) && m_eAttackType == AT_JUMP;
	})
		.AddTransition("CHASE_WALK to TRIP_UPPERCUT", "TRIP_UPPERCUT")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fFarAttackRange) && m_eAttackType == AT_TRIP_UPPERCUT;
	})



		.AddState("CHARGE_ATTACK")
		.OnStart([this]()
	{	
		Attack_Start(CHARGE_ATTACK);		
	})
		.OnExit([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		Attack_End();
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
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
		Attack_End();
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
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
		m_mapEffect["W_Trail"]->Set_Active(false);
		return AnimFinishChecker(UPPER_CUT);
	})
		

		.AddState("COMBO_ATTACK")
		.OnStart([this]()
	{	
		Attack_Start(COMBO_ATTACK);		
	})
		.OnExit([this]()
	{
		Attack_End();
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return IsParried();
	})
		.AddTransition("COMBO_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return AnimFinishChecker(COMBO_ATTACK);
	})

		.AddState("SWEEP_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(SWEEP_ATTACK);
	})
		.OnExit([this]()
	{
		Attack_End();
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return IsParried();
	})
		.AddTransition("SWEEP_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return AnimFinishChecker(SWEEP_ATTACK);
	})

		.AddState("ENRAGE")
		.OnStart([this]()
	{
		m_bEnRageReady = false;
		m_pMonsterStatusCom->Add_CurrentHP(100);
		Attack_Start(ENRAGE);
	})
		.OnExit([this]()
	{
		m_bRealAttack = false;
		m_pModelCom->Set_AnimIndex(IDLE_LOOP);

		CBGM_Manager::GetInstance()->Change_FieldState(CBGM_Manager::FIELD_BOSS_BATTLE_WARRIOR_PHASE_2);
	})
		.AddTransition("ENRAGE to BELL_CALL", "BELL_CALL")
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
	})
		.OnExit([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
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
		m_bBlock = true;
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCK_INTO);
		m_pModelCom->Set_AnimIndex(BLOCK_INTO);
	})		
		.OnExit([this]()
	{	
		m_bBlock = false;
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
		m_bRealAttack = false;
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


		.AddState("GRAB")
		.OnStart([this]()
	{
		m_bKenaGrab = true;
		Attack_Start(GRAB);
	})		
		.OnExit([this]()
	{
		m_bKenaGrab = false;
		Attack_End();
	})
		.AddTransition("GRAB to GRAB_ATTACK", "GRAB_ATTACK")
		.Predicator([this]()
	{
		_bool	bPass = m_pKena->Get_State(CKena::STATE_GRAB_WARRIOR);

		return bPass;
	})
		.AddTransition("GRAB to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(GRAB);
	})
		
		.AddState("GRAB_ATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(GRAB_ATTACK);
		m_pModelCom->Set_AnimIndex(GRAB_ATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		_float	fProgress = m_pModelCom->Get_AnimationProgress();
		if (fProgress > 0.493f && fProgress < 0.496f)
			m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta * 2.f);
	})
		.AddTransition("GRAB_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		_bool IsEnd = AnimFinishChecker(GRAB_ATTACK);
		if (IsEnd)
			m_pModelCom->Set_AnimationBlendDuration((_uint)IDLE_LOOP, 0.f);

		return IsEnd;
	})
		
		.AddState("JUMP_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(JUMP_ATTACK);		
	})
		.OnExit([this]()
	{
		Attack_End();
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
		Attack_End();
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return IsParried();
	})
		.AddTransition("JUMP_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		m_mapEffect["W_Trail"]->Set_Active(false);
		return AnimFinishChecker(TRIP_UPPERCUT);
	})



		.AddState("PARRIED")
		.OnStart([this]()
	{
		Update_ParticleType(CE_P_ExplosionGravity::TYPE::TYPE_BOSS_PARRY, m_pTransformCom->Get_Position(),false);

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
		m_pKena->Get_Status()->Add_Karma(480);

		/* HP Bar DeActivate */
		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = -1.f;
		m_BossWarriorDelegator.broadcast(eBossHP, fValue);
		/* ~HP Bar DeActivate */

		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(DEATH);
		m_pModelCom->Set_AnimIndex(DEATH);

		m_pKena->Dead_FocusRotIcon(this);
		m_bDying = true;
		BossFight_End();
	})
		.AddTransition("DYING to DEATH_SCENE", "DEATH_SCENE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})
		
		.AddState("DEATH_SCENE")
		.OnStart([this]()
	{
		m_bDissolve = true;
		m_fEndTime = 0.f;

		CBGM_Manager::GetInstance()->Change_FieldState(CBGM_Manager::FIELD_IDLE);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fEndTime += fTimeDelta;
		m_fDissolveTime = m_fEndTime * 0.3f;
		if (m_fDissolveTime >= 1.f)
			m_bDissolve = false;

		m_fFogRange += fTimeDelta * 50.f;
		if (m_fFogRange >= 100.f)
			m_pRendererCom->Set_Fog(false);
		const _float4 vColor = _float4(140.f / 255.f, 70.f / 255.f, 70.f / 255.f, 1.f);
		m_pRendererCom->Set_FogValue(vColor, m_fFogRange);
	})
		.OnExit([this]()
	{
		CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		pCtrlRoom->Boss_WarriorDeadGimmick();

		CGameInstance::GetInstance()->Work_Camera(m_pCineCam[1]->Get_ObjectCloneName());
		m_pCineCam[1]->Play();
	})
		.AddTransition("DEATH_SCENE to DEATH", "DEATH")
		.Predicator([this]()
	{
		return m_fEndTime >= 2.5f && !m_bDissolve;
	})

		.AddState("DEATH")
		.OnStart([this]()
	{
		g_bDayOrNight = true;
		m_pTransformCom->Clear_Actor();
		Clear_Death();
		m_pKena->Set_State(CKena::STATE_BOSSBATTLE, false);
	})


		.Build();

	return S_OK;
}

HRESULT CBossWarrior::SetUp_Effects()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Base* pEffectBase = nullptr;

	/* Trail */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_WarriorTrail", L"W_Trail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_Trail", pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", L"W_MovementParticle"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	dynamic_cast<CE_RectTrail*>(pEffectBase)->SetUp_Option(CE_RectTrail::OBJ_BOSS);
	m_mapEffect.emplace("W_MovementParticle", pEffectBase);
	/* Trail */

	/* Swipe */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_FireSwipe", L"W_FireSwipe"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_FireSwipe", pEffectBase);

	/* Root */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_Root", L"W_Root"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_Root", pEffectBase);

	/* PlaneRoot */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_PlaneRoot", L"W_PlaneRoot"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_PlaneRoot", pEffectBase);

	/* Warrior_Charged */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Swipes_Charged", L"Warrior_Charged"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("Warrior_Charged", pEffectBase);

	/* W_ShockFront */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_ShockFrontExtended", L"W_ShockFront"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_ShockFront", pEffectBase);

	/* Hieroglyph */
	_tchar*		pCloneTag = nullptr;
	string		strMapTag = "";
	for (_uint i = 0; i < 4; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"W_Hieroglyph", i);
		strMapTag = "W_Hieroglyph" + to_string(i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_Hieroglyph", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		m_mapEffect.emplace(strMapTag, pEffectBase);
	}

	/* W_DistortionPlane */
	{
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Distortion_Plane", L"W_DistortionPlane"));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		pEffectBase->Set_Parent(this);
		m_mapEffect.emplace("W_DistortionPlane", pEffectBase);
	}

	/* W_Enrageinto */
	{
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnrageInto", L"W_Enrageinto"));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		pEffectBase->Set_Parent(this);
		m_mapEffect.emplace("W_Enrageinto", pEffectBase);
	}

	//Prototype_GameObject_WarriorWeaponAcc
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_WarriorWeaponAcc", L"W_WeaponAcc"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_WeaponAcc", pEffectBase);

	// Prototype_GameObject_WarriorBodyAcc
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_WarriorBodyAcc", L"W_Body_P"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("W_Body_P", pEffectBase);

	// Prototype_GameObject_WarriorEyeTrail
	for (_uint i = 0; i < 2; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"W_WarriorEye", i);
		strMapTag = "W_WarriorEye" + to_string(i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_WarriorEyeTrail", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		pEffectBase->Set_Parent(this);
		dynamic_cast<CE_WarriorEyeTrail*>(pEffectBase)->Set_Boneprt(i, m_pModelCom->Get_BonePtr("char_mask_jnt"));
		m_mapEffect.emplace(strMapTag, pEffectBase);
	}

	return S_OK;
}

void CBossWarrior::Update_Trail(const char * pBoneTag)
{
	CBone*	pBonePtr = m_pModelCom->Get_BonePtr(pBoneTag);
	_matrix SocketMatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	m_mapEffect["W_Trail"]->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	m_mapEffect["W_MovementParticle"]->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	m_mapEffect["W_WeaponAcc"]->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);

	if (m_mapEffect["W_Trail"]->Get_Active() == true)
	{
		dynamic_cast<CEffect_Trail*>(m_mapEffect["W_Trail"])->Trail_InputPos(matWorldSocket.r[3]);
		dynamic_cast<CEffect_Trail*>(m_mapEffect["W_MovementParticle"])->Trail_InputRandomPos(matWorldSocket.r[3]);
	}
}

void CBossWarrior::Update_ParticleType(CE_P_ExplosionGravity::TYPE eType, _float4 vCreatePos,_bool bSetDir, _fvector vDir)
{	
	if(bSetDir)
		m_pExplsionGravity->Set_Option(eType, vDir);
	else
		m_pExplsionGravity->Set_Option(eType);

	m_pExplsionGravity->UpdateParticle(vCreatePos);
}

HRESULT CBossWarrior::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Boss_Warrior", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_01_EMISSIVE.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_OPACITY.png")), E_FAIL);

	m_pModelCom->Set_RootBone("VL_Warrior");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_BossWarrior.json");

	CMonsterWeapon::MONSTERWEAPONDESC		WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CMonsterWeapon::MONSTERWEAPONDESC));

	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("HatJoint");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	WeaponDesc.pOwnerMonster = this;
	//Safe_AddRef(WeaponDesc.pSocket);
	//Safe_AddRef(m_pTransformCom);
		
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;
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
				
		/*XMStoreFloat4x4(&m_WeaponPivotMatrix,
			XMMatrixRotationX(m_vWeaPonPivotRot.x) * XMMatrixRotationY(m_vWeaPonPivotRot.y) * XMMatrixRotationZ(m_vWeaPonPivotRot.z)
			* XMMatrixTranslation(m_vWeaPonPivotTrans.x, m_vWeaPonPivotTrans.y, m_vWeaPonPivotTrans.z));*/
		
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
				
		// XMStoreFloat4x4(&m_RightLegPivotMatrix, XMMatrixTranslation(m_vRightLegPivotTrans.x, m_vRightLegPivotTrans.y, m_vRightLegPivotTrans.z));

		SocketMatrix = XMLoadFloat4x4(&m_RightLegPivotMatrix) * SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(COL_RIGHT_LEG_TEXT, mat);
	}

	{
		_matrix SocketMatrix = m_pGrabHandBone->Get_OffsetMatrix() * m_pGrabHandBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		// XMStoreFloat4x4(&m_GrabHandPivotMatrix, XMMatrixTranslation(m_vGrabHandPivotTrans.x, m_vGrabHandPivotTrans.y, m_vGrabHandPivotTrans.z));

		SocketMatrix = XMLoadFloat4x4(&m_GrabHandPivotMatrix) * SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(COL_GRAB_HAND_TEXT, mat);
	}
}

void CBossWarrior::AdditiveAnim(_float fTimeDelta)
{
	CMonster::AdditiveAnim(fTimeDelta);
}

void CBossWarrior::BossFight_Start()
{
	g_bDayOrNight = false;
	if (m_pCineCam[0] != nullptr)
	{
		CGameInstance::GetInstance()->Work_Camera(m_pCineCam[0]->Get_ObjectCloneName());
		m_pCineCam[0]->Play();
	}
	m_bDissolve = true;
	m_fDissolveTime = 1.f;
	m_pRendererCom->Set_Fog(true);
	const _float4 vColor = _float4(130.f / 255.f, 70.f / 255.f, 70.f / 255.f, 1.f);
	m_pRendererCom->Set_FogValue(vColor, 100.f);

}

void CBossWarrior::BossFight_End()
{
	CGameInstance::GetInstance()->Work_Camera(TEXT("NPC_CAM"));

	const _float3 vOffset = _float3(0.f, 1.5f, 0.f);
	const _float3 vLookOffset = _float3(0.f, 0.3f, 0.f);

	dynamic_cast<CCameraForNpc*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CAM")))->Set_Target(this, CCameraForNpc::OFFSET_FRONT_LERP, vOffset, vLookOffset, 0.9f, 3.f);
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

void CBossWarrior::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnTrail);
		return;
	}
	m_mapEffect["W_Trail"]->Set_Active(true);
	Update_ParticleType(CE_P_ExplosionGravity::TYPE::TYPE_BOSS_WEAPON, m_pTransformCom->Get_Position(),false);
}

void CBossWarrior::TurnOffTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOffTrail);
		return;
	}
	m_mapEffect["W_Trail"]->Set_Active(false);
}

void CBossWarrior::TurnOnSwipesCharged(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnSwipesCharged);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_mapEffect["Warrior_Charged"]->Set_Position(vPos);
	m_mapEffect["Warrior_Charged"]->Set_Active(true);

	Update_ParticleType(CE_P_ExplosionGravity::TYPE::TYPE_BOSS_WEAPON, m_pTransformCom->Get_Position(),false);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL11], 0.5f);
}

void CBossWarrior::TurnOnHieroglyph(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnHieroglyph);
		return;
	}

	for (auto& Pair : m_mapEffect)
	{
		if (dynamic_cast<CE_Hieroglyph*>(Pair.second))
		{
			if (Pair.second->Get_Active() == false)
			{
				_float4 WarriorPos = m_pTransformCom->Get_WorldMatrix().r[3];
				_float4 vPos;
				_float	fRange = 1.0f;
				if (Pair.first == "W_Hieroglyph0") 
				{
					vPos = _float4(WarriorPos.x + 0.7f, WarriorPos.y + fRange, WarriorPos.z + fRange, 1.f);
					dynamic_cast<CE_Hieroglyph*>(Pair.second)->Set_TexRandomPrint(0);
				}
				if (Pair.first == "W_Hieroglyph1") 
				{
					vPos = _float4(WarriorPos.x + fRange, WarriorPos.y + fRange * 2.f, WarriorPos.z + fRange, 1.f);
					dynamic_cast<CE_Hieroglyph*>(Pair.second)->Set_TexRandomPrint(1);
				}
				if (Pair.first == "W_Hieroglyph2") 
				{
					vPos = _float4(WarriorPos.x - 0.4f, WarriorPos.y + fRange + 0.2f, WarriorPos.z + fRange, 1.f);
					dynamic_cast<CE_Hieroglyph*>(Pair.second)->Set_TexRandomPrint(2);
				}
				if (Pair.first == "W_Hieroglyph3") 
				{
					vPos = _float4(WarriorPos.x - 0.7f, WarriorPos.y + fRange * 2.5f, WarriorPos.z + fRange, 1.f);
					dynamic_cast<CE_Hieroglyph*>(Pair.second)->Set_TexRandomPrint(3);
				}

				Pair.second->Set_Position(vPos);
				Pair.second->Set_Active(true);
			}
		}
	}
}

void CBossWarrior::TurnOnShockFrontExtended(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnShockFrontExtended);
		return;
	}

	CBone*  pBonePtr = m_pModelCom->Get_BonePtr("Halberd_Jnt8");
	_matrix matrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matSocket = matrix * m_pTransformCom->Get_WorldMatrix();
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	_float4 vPosition = matSocket.r[3];
	m_mapEffect["W_ShockFront"]->Set_Position(vPosition);
	m_mapEffect["W_ShockFront"]->Set_Active(true);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL11], 0.5f);

	m_pBossRockPool->Execute_UpRocks();
}

void CBossWarrior::TurnOnFireSwipe(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnFireSwipe);
		return;
	}

	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);

	CBone*  pUpBonePtr = m_pModelCom->Get_BonePtr("Halberd_Jnt6"); // UP
	CBone*  pCenterBonePtr = m_pModelCom->Get_BonePtr("Halberd_Jnt7"); // DOWN == center

	_matrix UpMatrix = pUpBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix CenterMatrix = pCenterBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	_matrix matUpSocket = UpMatrix * matWorld;
	_matrix matCenterSocket = CenterMatrix * matWorld;

	_vector vLook = XMVector3Normalize(matWorld.r[2] * -1.f);
	_vector vPosition = matCenterSocket.r[3];
	_vector vRight = XMVector3Normalize(matUpSocket.r[3] - vPosition);
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_smatrix worldmatrix(vRight, vUp, vLook, vPosition);
	m_mapEffect["W_FireSwipe"]->Get_TransformCom()->Set_WorldMatrix(worldmatrix);
	m_mapEffect["W_FireSwipe"]->Set_Active(true);

	Update_ParticleType(CE_P_ExplosionGravity::TYPE::TYPE_BOSS_ATTACK, m_pTransformCom->Get_Position(), true, matWorld.r[2]);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL2], 0.5f);
}

void CBossWarrior::TurnOnFireSwipe_End(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnFireSwipe_End);
		return;
	}

	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);

	CBone*  pBonePtr = m_pModelCom->Get_BonePtr("Halberd_Jnt8");
	_matrix matrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matSocket = matrix * m_pTransformCom->Get_WorldMatrix();
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	_vector vLook = XMVector3Normalize(matWorld.r[2] * -1.f);
	_vector vPosition = matSocket.r[3];
	_vector vRight = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_smatrix worldmatrix(vRight, vUp, vLook, vPosition);
	m_mapEffect["W_FireSwipe"]->Get_TransformCom()->Set_WorldMatrix(worldmatrix);
	m_mapEffect["W_FireSwipe"]->Set_Active(true);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_mapEffect["Warrior_Charged"]->Set_Position(vPos);
	m_mapEffect["Warrior_Charged"]->Set_Active(true);

	Update_ParticleType(CE_P_ExplosionGravity::TYPE::TYPE_BOSS_WEAPON, m_pTransformCom->Get_Position(), false);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL11], 0.5f);

	m_pBossRockPool->Execute_UpRocks();
}

void CBossWarrior::TurnOnRoot(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnRoot);
		return;
	}
	// W_Root

	CBone*  pBonePtr = m_pModelCom->Get_BonePtr("Halberd_EndJnt"); // end
	_matrix EndMatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matEndSocket = EndMatrix * matWorld;

	_vector vPosition = matEndSocket.r[3];
	_vector vRight = matWorld.r[2];
	_vector vLook = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vRight));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

	_smatrix worldmatrix(vRight, vUp, vLook, vPosition);
	m_mapEffect["W_Root"]->Get_TransformCom()->Set_WorldMatrix(worldmatrix);
	m_mapEffect["W_Root"]->Set_Active(true);
}

void CBossWarrior::TurnOnPlaneRoot(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnPlaneRoot);
		return;
	}
	CBone*  pBonePtr = m_pModelCom->Get_BonePtr("Halberd_Jnt8"); // end
	_matrix EndMatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matEndSocket = EndMatrix * matWorld;

	_vector vPosition = matEndSocket.r[3];
	_vector vLook = XMVector3Normalize(vPosition - matWorld.r[3]);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_smatrix worldmatrix(vLook,  vUp, -vRight, vPosition);
	m_mapEffect["W_PlaneRoot"]->Get_TransformCom()->Set_WorldMatrix(worldmatrix);
	m_mapEffect["W_PlaneRoot"]->Set_Active(true);
}

void CBossWarrior::TurnOnEnrage_Into(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnEnrage_Into);
		return;
	}
	
	_float3 vEffectScale = m_mapEffect["W_Enrageinto"]->Get_TransformCom()->Get_Scaled();
	_float4 vWarriorPos = m_pTransformCom->Get_Position();
	vWarriorPos.y = vWarriorPos.y + (vEffectScale.y * 0.5f);

	m_mapEffect["W_Enrageinto"]->Set_Position(vWarriorPos);
	m_mapEffect["W_Enrageinto"]->Set_Active(true);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL1], 0.3f);
}

void CBossWarrior::TurnOnEnrage_Attck(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnEnrage_Attck);
		return;
	}
	_matrix WarriorMatrix = m_pTransformCom->Get_WorldMatrix();

	_float4 vPosition = WarriorMatrix.r[3];
	vPosition.y += 1.f;

	m_mapEffect["W_DistortionPlane"]->Set_Position(vPosition);
	m_mapEffect["W_DistortionPlane"]->Set_Active(true);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL2], 0.5f);

	m_pBossRockPool->Execute_UpRocks();
}

void CBossWarrior::TurnOnCamShake(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnCamShake);
		return;
	}
	CCamera_Player* pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
	if (pCamera != nullptr)
		pCamera->Camera_Shake(0.005f, 30);
}

void CBossWarrior::TurnOnMotionBlur(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOnMotionBlur);
		return;
	}
	m_pRendererCom->Set_MotionBlur(true);
}

void CBossWarrior::TurnOffMotionBlur(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::TurnOffMotionBlur);
		return;
	}
	m_pRendererCom->Set_MotionBlur(false);
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
	Safe_Release(m_pBossRockPool);

	for (auto& Pair : m_mapEffect)
		Safe_Release(Pair.second);
	m_mapEffect.clear();
}

_int CBossWarrior::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{	
	if (pTarget && m_bSpawn)
	{
		if ((iColliderIndex == (_int)COL_PLAYER_WEAPON && m_pKena->Get_State(CKena::STATE_ATTACK)))
		{
			if (m_bBlock == false)
				m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());

			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);

			CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
			_float fGauge = m_pMonsterStatusCom->Get_PercentHP();
			m_BossWarriorDelegator.broadcast(eBossHP, fGauge);

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);

			CCamera_Player* pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());

			if (m_pKena->Get_State(CKena::STATE_HEAVYATTACK) == false)
			{
				m_bWeaklyHit = true;
				m_bStronglyHit = false;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.15f);
				m_pKena->Add_HitStopTime(0.15f);
				m_fHitStopTime += 0.15f;
				
				if (pCamera != nullptr)
					pCamera->Camera_Shake(0.003f, 5);
			}
			else
			{
				m_bWeaklyHit = false;
				m_bStronglyHit = true;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.3f);
				m_pKena->Add_HitStopTime(0.25f);
				m_fHitStopTime += 0.25f;
				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.005f, 5);

				vector<_float4>* vecWeaponPos = m_pKena->Get_WeaponPositions();
				if (vecWeaponPos->size() == 2)
				{
					_vector	vDir = vecWeaponPos->back() - vecWeaponPos->front();
					vDir = XMVectorSetZ(vDir, 0.f);
					
					if (pCamera != nullptr)
						pCamera->Camera_Shake(vDir, XMConvertToRadians(30.f));
				}
			}
		}

		if (iColliderIndex == (_int)COL_PLAYER_ARROW)
		{
			CSpiritArrow* pArrow = dynamic_cast<CSpiritArrow*>(pTarget);

			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			//m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());
			m_pMonsterStatusCom->UnderAttack(pArrow->Get_Damage());

			CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
			_float fGauge = m_pMonsterStatusCom->Get_PercentHP();
			m_BossWarriorDelegator.broadcast(eBossHP, fGauge);

			//m_bStronglyHit = m_pKena->Get_State(CKena::STATE_INJECTBOW);
			//m_bWeaklyHit = !m_bStronglyHit;

			if (pArrow->Get_CurrentState() == CSpiritArrow::ARROW_INJECT_FIRE)
			{
				m_bStronglyHit = true;
				m_bWeaklyHit = false;
			}
			else
			{
				m_bWeaklyHit = true;
				m_bStronglyHit = false;
			}

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);
		}
	}

	return 0;
}

_int CBossWarrior::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (m_bKenaGrab && pTarget && iTriggerIndex == (_uint)ON_TRIGGER_PARAM_TRIGGER && iColliderIndex == (_int)COL_PLAYER)
	{
	}

	return 0; 
}

void CBossWarrior::Attack_End()
{
	m_bRealAttack = false;

	_uint iAttack = m_eAttackType + 1;
	iAttack %= ATTACKTYPE_END;
	m_eAttackType = (ATTACKTYPE)iAttack;	
}

void CBossWarrior::Grab_Turn(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Grab_Turn);
		return;
	}

	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);
	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
}

void CBossWarrior::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_BossWarrior_Attack1.ogg"),
		TEXT("Mon_BossWarrior_Attack2.ogg"),
		TEXT("Mon_BossWarrior_Attack3.ogg"),
		TEXT("Mon_BossWarrior_Attack4.ogg"),
		TEXT("Mon_BossWarrior_Attack5.ogg"),
		TEXT("Mon_BossWarrior_Attack6.ogg"),
		TEXT("Mon_BossWarrior_Attack7.ogg"),
		TEXT("Mon_BossWarrior_Attack8.ogg"),
		TEXT("Mon_BossWarrior_Attack9.ogg"),
		TEXT("Mon_BossWarrior_Hit1.ogg"),
		TEXT("Mon_BossWarrior_Hit2.ogg"),
		TEXT("Mon_BossWarrior_Hit3.ogg"),
		TEXT("Mon_BossWarrior_Hit4.ogg"),
		TEXT("Mon_BossWarrior_Hurt1.ogg"),
		TEXT("Mon_BossWarrior_Hurt2.ogg"),
		TEXT("Mon_BossWarrior_Hurt3.ogg"),
		TEXT("Mon_BossWarrior_Hurt4.ogg"),
		TEXT("Mon_BossWarrior_Hurt5.ogg"),
		TEXT("Mon_BossWarrior_Hurt6.ogg"),
		TEXT("Mon_BossWarrior_Hurt7.ogg"),
		TEXT("Mon_BossWarrior_Tense1.ogg"),
		TEXT("Mon_BossWarrior_Tense2.ogg"),
		TEXT("Mon_BossWarrior_Tense3.ogg"),
		TEXT("Mon_BossWarrior_Back.ogg"),
		TEXT("Mon_Attack_Impact2.ogg"),
		TEXT("Mon_Attack_Impact3.ogg"),
		TEXT("Mon_Attack_Impact4.ogg"),
		TEXT("Mon_Attack_Impact5.ogg"),
		TEXT("Mon_Walk_L.ogg"),
		TEXT("Mon_Swing2.ogg"),
		TEXT("Mon_Knife2.ogg"),
		TEXT("Mon_BossSound_Ding.ogg"),
		TEXT("Mon_BossSound_Base.ogg"),

		TEXT("Mon_BossHunter_Elemental1.ogg"),
		TEXT("Mon_BossHunter_Elemental2.ogg"),
		TEXT("Mon_BossHunter_Elemental11.ogg"),
	};

	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}

void CBossWarrior::Play_Attack1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK1], 0.5f);
}

void CBossWarrior::Play_Attack2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK2], 0.5f);
}

void CBossWarrior::Play_Attack3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK3], 0.5f);
}

void CBossWarrior::Play_Attack4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK4], 0.5f);
}

void CBossWarrior::Play_Attack5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK5], 0.5f);
}

void CBossWarrior::Play_Attack6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK6], 0.5f);
}

void CBossWarrior::Play_Attack7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK7], 0.5f);
}

void CBossWarrior::Play_Attack8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK8], 0.5f);
}

void CBossWarrior::Play_Attack9Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Attack9Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK9], 0.5f);
}

void CBossWarrior::Play_Hit1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hit1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT1], 0.5f);
}

void CBossWarrior::Play_Hit2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hit2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT2], 0.5f);
}

void CBossWarrior::Play_Hit3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hit3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT3], 0.5f);
}

void CBossWarrior::Play_Hit4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hit4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT4], 0.5f);
}

void CBossWarrior::Play_Hurt1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT1], 0.5f);
}

void CBossWarrior::Play_Hurt2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT2], 0.5f);
}

void CBossWarrior::Play_Hurt3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT3], 0.5f);
}

void CBossWarrior::Play_Hurt4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT4], 0.5f);
}

void CBossWarrior::Play_Hurt5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT5], 0.5f);
}

void CBossWarrior::Play_Hurt6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT6], 0.5f);
}

void CBossWarrior::Play_Hurt7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Hurt7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT7], 0.5f);
}

void CBossWarrior::Play_Tense1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Tense1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.7f);
}

void CBossWarrior::Play_Tense2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Tense2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE2], 0.7f);
}

void CBossWarrior::Play_Tense3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Tense3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE3], 0.7f);
}

void CBossWarrior::Play_BackSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_BackSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_BACK], 0.5f);
}

void CBossWarrior::Play_ImpactSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_ImpactSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT2], 0.5f);
}

void CBossWarrior::Play_Impact3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Impact3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT3], 0.5f);
}

void CBossWarrior::Play_Impact4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Impact4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT4], 0.5f);
}

void CBossWarrior::Play_Impact5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Impact5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT5], 0.5f);
}

void CBossWarrior::Play_WalkSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_WalkSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WALK], 0.1f);
}

void CBossWarrior::Play_SwingSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_SwingSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SWING], 0.5f);
}

void CBossWarrior::Play_SlashSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_SlashSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SLASH], 0.7f);
}

void CBossWarrior::Play_BossDingSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_BossDingSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_BOSS_DING], 0.5f);
}

void CBossWarrior::Play_BossBaseSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_BossBaseSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_BOSS_BASE], 0.5f);
}

void CBossWarrior::Play_Elemental1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Elemental1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL1], 0.5f);
}

void CBossWarrior::Play_Elemental2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Elemental2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL2], 0.5f);
}

void CBossWarrior::Play_Elemental11Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Play_Elemental11Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL11], 0.5f);
}

void CBossWarrior::Execute_UpRocksPool(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossWarrior::Execute_UpRocksPool);
		return;
	}

	m_pBossRockPool->Execute_UpRocks();
}