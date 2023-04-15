#include "stdafx.h"
#include "..\public\BossShaman.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Sticks01.h"
#include "ShamanTrapHex.h"
#include "E_RectTrail.h"
#include "SpiritArrow.h"
#include "E_ShamanSmoke.h"
#include "E_ShamanTrail.h"
#include "E_ShamanThunderCylinder.h"
#include "E_ShamanBossHandPlane.h"
#include "E_ShamanBossPlate.h"
#include "E_ShamanSummons.h"
#include "E_ShamanIceDagger.h"
#include "E_ShamanLazer.h"
#include "BossShaman_Mask.h"
#include "Camera_Shaman.h"

// #define EFFECTDEBUG

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
	FAILED_CHECK_RETURN(Ready_Effects(), E_FAIL);

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
		m_Desc.WorldMatrix._41 = -10.f;
		m_Desc.WorldMatrix._43 = -10.f;
	}

	m_pModelCom->Init_AnimIndex(IDLE_LOOP);
	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pWeaponBone = m_pModelCom->Get_BonePtr("sword_root_jnt");
	m_pWeaponTrailBone = m_pModelCom->Get_BonePtr("sword_jnt_6");

	/* For.DashAttackTrail */
	m_pLeftHandBone = m_pModelCom->Get_BonePtr("char_lf_middle_a_jnt");
	m_pHeadBone = m_pModelCom->Get_BonePtr("char_mask_jnt");

	Create_Minions();	
	Create_ShamanMask();

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
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, m_vPivotPos);
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

	for (auto& Pair : m_mapEffect)
		Pair.second->Late_Initialize(nullptr);
	
	m_pCamera_Shaman = (CCamera_Shaman*)m_pGameInstance->Find_Camera(CAMERA_SHAMAN_TAG);
	assert(m_pCamera_Shaman && "CCamera_Shaman is NULL!!");

	return S_OK;
}

void CBossShaman::Tick(_float fTimeDelta)
{	
	/*m_pModelCom->Play_Animation(fTimeDelta);
	Update_Collider(fTimeDelta);
	return;*/

#ifdef EFFECTDEBUG
	// m_bDashAttackTrail = true;
	//m_bSpawn = true;
#endif // EFFECTDEBUG
	
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	SwordRenderProc(fTimeDelta);
	Update_Collider(fTimeDelta);
	Tick_Effects(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	for (auto& Pair : m_mapEffect)
		Pair.second->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	// AdditiveAnim(fTimeDelta);

	m_pShamanMask->Tick(fTimeDelta);
}

void CBossShaman::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	
	for (auto& Pair : m_mapEffect)
		Pair.second->Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bStartRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	m_pShamanMask->Late_Tick(fTimeDelta);
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
#ifdef EFFECTDEBUG
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
#else
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
				m_bDying&& Bind_Dissolove(m_pShaderCom);
			}
			else if (m_eSwordRenderState == RENDER) {
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 1, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
			}
#endif // EFFECTDEBUG

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

	ImGui::DragFloat("m_fTrapHeightY", &m_fTrapHeightY, 0.01f, -100.f, 100.0f);
	ImGui::DragFloat("m_fTrapColliserHeightY", &m_fTrapColliserHeightY, 0.01f, -100.f, 100.0f);

	ImGui::InputInt("ShamanLookIndex", &m_iFakeShamanLookIndex);

	float fTemp[3] = { m_vPivotPos.x, m_vPivotPos.y, m_vPivotPos.z };
	ImGui::DragFloat3("Body Collider", fTemp, 0.01f, -100.f, 100.0f);
	memcpy(&m_vPivotPos, fTemp, sizeof(_float3));	

	ImGui::DragFloat("m_fMeleeAttackTeleportDist", &m_fMeleeAttackTeleportDist, 0.01f, -100.f, 100.0f);
			
	_float4 vCamOffset = m_pCamera_Shaman->Get_TargetPositionOffset();
	ImGui::DragFloat("vCamOffset.y", &vCamOffset.y, 0.01f, -100.f, 100.0f);
	m_pCamera_Shaman->Set_TargetPositionOffset(vCamOffset);
	
	_float fCamDist = m_pCamera_Shaman->Get_TargetDistance();		
	ImGui::DragFloat("fCamDist", &fCamDist, 0.01f, -100.f, 100.0f);
	m_pCamera_Shaman->Set_TargetDistance(fCamDist);

	ImGui::DragFloat("m_fCamDistRate", &m_fCamDistRate, 0.01f, -100.f, 100.0f);

	if (ImGui::Button("Start Render"))
		m_bStartRender = !m_bStartRender;
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
	TurnOnWeaponCloudTrail(true, 0.0f);
	TurnOffWeaponCloudTrail(true, 0.0f);

	TurnOnHandSummons(true, 0.0f);
	TurnOffHandSummons(true, 0.0f);
	TurnOnSwipeChareged(true, 0.0f);
	TurnOnTeleport(true, 0.0f);

	TurnOnSummons(true, 0.0f);
	TurnOffSummons(true, 0.0f);

	TurnOnDashAttackTrail(true, 0.0f);
	TurnOffDashAttackTrail(true, 0.0f);
	TurnOnSwipeChareged(true, 0.0f);
	TurnOnIceDagger(true, 0.0f);
	TurnOffIceDagger(true, 0.0f);
	TurnOnLazer(true, 0.0f);

	TurnOnCameraShake(true, 0.0f);

	// Sound CallBack
	Play_Attack1Sound(true, 0.0f);
	Play_Attack2Sound(true, 0.0f);
	Play_Attack3Sound(true, 0.0f);
	Play_Attack4Sound(true, 0.0f);
	Play_Attack5Sound(true, 0.0f);
	Play_Attack6Sound(true, 0.0f);
	Play_Attack7Sound(true, 0.0f);
	Play_Attack8Sound(true, 0.0f);

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

	Play_Move1Sound(true, 0.0f);
	Play_Move2Sound(true, 0.0f);
	Play_Move3Sound(true, 0.0f);
	Play_Move4Sound(true, 0.0f);
	Play_Move5Sound(true, 0.0f);
	Play_Move6Sound(true, 0.0f);

	Play_Tense1Sound(true, 0.0f);
	Play_Tense2Sound(true, 0.0f);
	Play_Tense3Sound(true, 0.0f);
	Play_Tense4Sound(true, 0.0f);

	Play_Impact1Sound(true, 0.0f);
	Play_Impact2Sound(true, 0.0f);
	Play_Impact3Sound(true, 0.0f);
	Play_Impact4Sound(true, 0.0f);
	Play_Impact5Sound(true, 0.0f);

	Play_Knife1Sound(true, 0.0f);
	Play_Knife2Sound(true, 0.0f);
	Play_Knife3Sound(true, 0.0f);

	Play_Sword1Sound(true, 0.0f);
	Play_Sword2Sound(true, 0.0f);

	Play_Swing1Sound(true, 0.0f);
	Play_Swing2Sound(true, 0.0f);

	Play_HandclapSound(true, 0.0f);
	Play_TeleportSound(true, 0.0f);	
	Play_DingSound(true, 0.0f);
	Play_WalkSound(true, 0.0f);

	Teleport_MeleeAttack(true, 0.0f);
}

HRESULT CBossShaman::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")		
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_bReadySpawn = true;		
	})
		.AddTransition("SLEEP to CAMERA_SCENE", "CAMERA_SCENE_START")
		.Predicator([this]()
	{	
		m_fSpawnRange = 20.f;
		return DistanceTrigger(m_fSpawnRange);
	})

		.AddState("CAMERA_SCENE_START")
		.OnStart([this]()
	{	
		m_pKena->Set_StateLock(true);
		BossFight_Start();
				
		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 18.f);
		vPosition.y += 2.f;

		m_pCamera_Shaman->Start_Action(this, vPosition);
		m_pGameInstance->Work_Camera(CAMERA_SHAMAN_TAG);
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DING], 0.5f);
	})			
		.AddTransition("CAMERA_SCENE_START to CAMERA_SCENE_ACTION", "CAMERA_SCENE_ACTION")
		.Predicator([this]()
	{
		return m_pCamera_Shaman->Is_WaitShamanAction();
	})


		.AddState("CAMERA_SCENE_ACTION")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DING], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(AWAKE_LOOP);
		m_pModelCom->Set_AnimIndex(AWAKE_LOOP);
		
		m_pShamanMask->Start_Dissolve();
		
		m_bStartRender = true;
		m_bTeleportDissolve = true;
		m_fTeleportDissolveTime = 1.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		if (m_pShamanMask->Is_DissolveEnd())
		{
			if(m_fTeleportDissolveTime == 1.f)
				m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DING], 0.5f);

			m_fTeleportDissolveTime -= fTimeDelta * 0.3f;
			if (m_fTeleportDissolveTime <= 0.f)
				m_fTeleportDissolveTime = 0.f;
					
			_float fNewDist = m_pCamera_Shaman->Get_TargetDistance() + m_fCamDistRate * fTimeDelta;
			m_pCamera_Shaman->Set_TargetDistance(fNewDist);
		}		
	})
		.OnExit([this]()
	{
		m_bTeleportDissolve = false;
		m_fTeleportDissolveTime = 0.f;
	})
		.AddTransition("CAMERA_SCENE_ACTION to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{
		return m_fTeleportDissolveTime == 0.f;
	})


		.AddState("READY_SPAWN")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENT], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(AWAKE);
		m_pModelCom->Set_AnimIndex(AWAKE);

		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = 20.f; /* == BossWarrior Name */
		m_BossShamanDelegator.broadcast(eBossHP, fValue);
		m_fAwakeLoopTimeCheck = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fAwakeLoopTimeCheck += fTimeDelta;
	})
		.OnExit([this]()
	{
		m_pKena->Set_StateLock(false);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_bSpawn = true;
		m_pGameInstance->Work_Camera(TEXT("PLAYER_CAM"));
		m_pGameInstance->Get_WorkCameraPtr()->LookAt_NoUpDown(m_pTransformCom->Get_Position());
		
		m_pCamera_Shaman->Clear();
		// 임시
		// m_pShamanMask->Clear(); 
		// m_bStartRender = false;
	})
		.AddTransition("READY_SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		// return m_fAwakeLoopTimeCheck >= 2.f; // 임시
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
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
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
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
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
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENT], 0.5f);
		// m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE4], 0.5f);
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
		/*.AddTransition("TELEPORT_EXIT To FREEZE_BLAST", "FREEZE_BLAST")
		.Predicator([this]()
	{
		return m_eAttackType == AT_FREEZE_BLAST;
	})*/
		.AddTransition("TELEPORT_EXIT To ICE_DAGGER", "ICE_DAGGER")
		.Predicator([this]()
	{
		return m_eAttackType == AT_ICE_DAGGER;
	})


		.AddState("MELEE_ATTACK")
		.OnStart([this]()
	{
		Attack_Start(true, MELEE_ATTACK);
		m_bMeleeAttackTeleport = false;
	})	
		.Tick([this](_float fTimeDelta)
	{
		if (m_bMeleeAttackTeleport)
		{
			if (!DistanceTrigger(4.f)) {
				m_bTeleportDissolve = true;
				m_fTeleportDissolveTime = 0.f;
				m_bMeleeAttackDissolve = true;
			}

			m_bMeleeAttackTeleport = false;
		}

		if (m_bTeleportDissolve)
		{
			if (m_bMeleeAttackDissolve)
			{
				m_fTeleportDissolveTime += fTimeDelta;
				if (m_fTeleportDissolveTime >= 1.f)
				{
					m_fTeleportDissolveTime = 1.f;
					m_bMeleeAttackDissolve = false;
					
					_float3 vCamLook = CGameInstance::GetInstance()->Get_CamLook_Float3();
					vCamLook.y = 0.f;
					_float4 vTeleportPos = XMLoadFloat4(&m_vKenaPos) + XMVector3Normalize(vCamLook) * m_fMeleeAttackTeleportDist;
					
					m_pTransformCom->Set_Position(vTeleportPos);
					m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
					
					m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENT], 0.5f);
				}
			}
			else
			{
				m_fTeleportDissolveTime -= fTimeDelta * 1.1f;
				if (m_fTeleportDissolveTime <= 0.f)
				{
					m_fTeleportDissolveTime = 0.f;
					m_bMeleeAttackDissolve = false;
					m_bTeleportDissolve = false;
				}
			}
		}
	})
		.OnExit([this]()
	{	
		Attack_End(true, IDLE_LOOP);

		m_bMeleeAttackTeleport = false;
		m_bMeleeAttackDissolve = false;
		m_bTeleportDissolve = false;
		m_fTeleportDissolveTime = 0.f;
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
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
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
		
		_float4 vTrapPos = m_vKenaPos;
		vTrapPos.y = m_pTransformCom->Get_PositionY();
		m_pShamanTapHex->Execute_Trap(vTrapPos);

		m_fTeleportDissolveTime = 0.f;
		m_bTeleportDissolve = false;
	})
		.Tick([this](_float fTimeDelta)
	{	
		_float4 vTrapPos = m_pShamanTapHex->Get_JointBonePos();
		vTrapPos.y = m_pShamanTapHex->Get_TransformCom()->Get_PositionY() + m_fTrapHeightY;
		m_pTransformCom->Set_Position(vTrapPos);

		_float4 vLookPos = m_pShamanTapHex->Get_FakeShamanPos(m_iFakeShamanLookIndex);
		m_pTransformCom->LookAt_NoUpDown(vLookPos);
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
		m_bTrap = false;
		m_bLaserFire = false;
		Attack_End(false, IDLE_LOOP);
	})		
		.AddTransition("LASER_FIRE to TRAP_END", "TRAP_END") // 레이저 발사가 끝나면 아이들로
		.Predicator([this]()
	{
		if (m_bTrap == false)
		{
			if (m_bLaserFire == false)
			{
				_matrix Head_SocketMatrix = m_pHeadBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
				dynamic_cast<CE_ShamanLazer*>(m_mapEffect["S_Lazer"])->Set_SpawnPos(Head_SocketMatrix.r[3]);
				m_bLaserFire = true;
			}
			
			if (dynamic_cast<CE_ShamanLazer*>(m_mapEffect["S_Lazer"])->Get_FinalState() == true)
			{
				_float4 vPos = m_pShamanTapHex->Get_TransformCom()->Get_Position();				
				m_mapEffect["Shaman_Charged"]->Set_Effect(vPos, true);
				m_bTrap = true;
			}
		}
		
		_bool bTemp = m_mapEffect["S_Lazer"]->Get_Active() == false;
		if(bTemp) 
			m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_LASER_IMPACT], 0.5f);
		
		return bTemp;
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

		_float4 vPos;
		
		CTransform* pKenaTransform = m_pKena->Get_TransformCom();		
		XMStoreFloat4(&vPos, pKenaTransform->Get_State(CTransform::STATE_TRANSLATION) + pKenaTransform->Get_State(CTransform::STATE_LOOK) * 2.5f);

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
		m_pModelCom->ResetAnimIdx_PlayTime(STUN_TAKE_DAMAGE);
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
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
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
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("FREEZE_BLAST to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(FREEZE_BLAST);
	})


		.AddState("ICE_DAGGER")
		.OnStart([this]()
	{	
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ICE], 0.5f);
		Attack_Start(false, ICE_DAGGER_INTO);
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
		.AddTransition("ICE_DAGGER to ICE_DAGGER_EXIT", "ICE_DAGGER_EXIT")
		.Predicator([this]()
	{
		return AnimFinishChecker(ICE_DAGGER_INTO);
	})
		
		.AddState("ICE_DAGGER_EXIT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ICE_DAGGER_EXIT);
		m_pModelCom->Set_AnimIndex(ICE_DAGGER_EXIT);
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
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("ICE_DAGGER to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ICE_DAGGER_EXIT);
	})
		
		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER);
		m_pModelCom->Set_AnimIndex(STAGGER);
	})
		.OnExit([this]()
	{
		m_bWeaklyHit = m_bStronglyHit = false;
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("PARRIED to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(STAGGER);
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
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENT], 0.5f);
		m_bTeleportDissolve = true;
		m_fTeleportDissolveTime = 0.f;
		BossFight_End();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTeleportDissolveTime += fTimeDelta * 0.2f;
		if (m_fTeleportDissolveTime >= 1.f)
			m_fTeleportDissolveTime = 1.f;
	})
		.AddTransition("DEATH_SCENE to DEATH", "DEATH")
		.Predicator([this]()
	{
		return m_fTeleportDissolveTime == 1.f;
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

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Boss_Shaman", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

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
	
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bTeleportDissolve, sizeof(_bool)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fTeleportDissolveTime, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"), E_FAIL);

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
	m_mapEffect.emplace("S_Trail", pEffectBase);
	
	///* 칼에 달린 연기 트레일임 */  
	//pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", L"S_KnifeTrail"));
	//NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	//dynamic_cast<CE_RectTrail*>(pEffectBase)->SetUp_Option(CE_RectTrail::OBJ_W_SHAMAN); // ShamanWeapon
	//m_mapEffect.emplace("S_KnifeTrail", pEffectBase);

	/* 몸에 달린 트레일임 => 텔레포트 할때 */
	_tchar* pDummyString = CUtile::Create_DummyString();
	m_pMovementTrail = dynamic_cast<CE_RectTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", pDummyString));
	NULL_CHECK_RETURN(m_pMovementTrail, E_FAIL);
	m_pMovementTrail->SetUp_Option(CE_RectTrail::OBJ_BODY_SHAMAN);
	/* Trail */

	/* Plate */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanBossPlate", L"S_Plate"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("S_Plate", pEffectBase);

	/* Hand */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanBossHandPlane", L"S_Hand"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("S_Hand", pEffectBase);

	/* Summons */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanSummons", L"S_Summons"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("S_Summons", pEffectBase);

	/* Shaman_Charged */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Swipes_Charged", L"Shaman_Charged"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("Shaman_Charged", pEffectBase);

	/* Shaman_Smoke */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanSmoke", L"Shaman_Smoke"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("Shaman_Smoke", pEffectBase);
	
	/* S_Lazer */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShammanLazer", L"S_Lazer"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("S_Lazer", pEffectBase);

	// S_P_Teleport
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanTeleport", L"S_P_Teleport"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("S_P_Teleport", pEffectBase);

	/* Minion 소환될 때 나옴 */
	for (_int i = 0; i < MINION_COUNT; ++i)
	{
		m_pShamanThunder[i] = dynamic_cast<CE_ShamanThunderCylinder*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShamanThunderCylinder",
			CUtile::Create_DummyString(L"ShamanThunder", i)));
		NULL_CHECK_RETURN(m_pShamanThunder[i], E_FAIL);
		m_pShamanThunder[i]->Set_Parent(this);
	}

	/* IceDagger */
	_tchar* pCloneTag = nullptr;
	string strMapTag = "";
	CBone* pBonePtr = m_pModelCom->Get_BonePtr("char_neck_jnt");
	for (_int i = 0; i < ICEDAGGER_COUNT; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"S_IceDagger", i);
		strMapTag = "S_IceDagger_" + to_string(i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ShammanIceDagger", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		dynamic_cast<CE_ShamanIceDagger*>(pEffectBase)->Set_CenterBone(pBonePtr, XMConvertToRadians(i * 45.f), i);		
		m_mapEffect.emplace(strMapTag, pEffectBase);
	}

	/* For.DashAttackTrail */
	for (_int i = 0; i < 3; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"S_DashAttackTrail", i);
		strMapTag = "S_DashAttackTrail_" + to_string(i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		dynamic_cast<CE_RectTrail*>(pEffectBase)->SetUp_Option(CE_RectTrail::OBJ_W_SHAMAN);
		m_mapEffect.emplace(strMapTag, pEffectBase);
	}

	for (auto& Pair : m_mapEffect)
		Pair.second->Set_Parent(this);

	CGameObject* p_game_object = nullptr;
	m_pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_Monster", TEXT("Prototype_GameObject_ShamanTrapHex"), L"ShamanTrapHex_0", nullptr, &p_game_object);
	m_pShamanTapHex = (CShamanTrapHex*)p_game_object;
	assert(m_pShamanTapHex && "FAILED!! -> CBossShaman::Late_Initialize()");

	return S_OK;
}

void CBossShaman::Tick_Effects(_float fTimeDelta)
{
	if (m_bTrail == true) 
		Update_Trail(nullptr);

	if (m_pMovementTrail->Get_Active() == true)	
		Update_MovementTrail("char_spine_low_jnt");

	if (m_bDashAttackTrail == true)
		Update_DashAttackTrail();

	if (m_bIceDagger == true) 
		Update_IceDagger();
}

void CBossShaman::Update_Trail(const char* pBoneTag)
{
	_matrix SocketMatrix = m_pWeaponTrailBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	m_mapEffect["S_Trail"]->Get_TransformCom()->Set_WorldMatrix(SocketMatrix);

	if (m_mapEffect["S_Trail"]->Get_Active() == true) /* Normal Trail Setting */
		dynamic_cast<CEffect_Trail*>(m_mapEffect["S_Trail"])->Trail_InputPos(SocketMatrix.r[3]);
}

void CBossShaman::Update_DashAttackTrail()
{
	// RightHand = m_pWeaponBone // LeftHand = m_pLeftHandBone // Head = m_pHeadBone // 
	_matrix Right_SocketMatrix = m_pWeaponBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	_matrix Left_SocketMatrix = m_pLeftHandBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	_matrix Head_SocketMatrix = m_pHeadBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

	// S_DashAttackTrail_0 / S_DashAttackTrail_1 / S_DashAttackTrail_2
	dynamic_cast<CEffect_Trail*>(m_mapEffect["S_DashAttackTrail_0"])->Trail_InputPos(Right_SocketMatrix.r[3]);
	dynamic_cast<CEffect_Trail*>(m_mapEffect["S_DashAttackTrail_1"])->Trail_InputPos(Left_SocketMatrix.r[3]);
	dynamic_cast<CEffect_Trail*>(m_mapEffect["S_DashAttackTrail_2"])->Trail_InputPos(Head_SocketMatrix.r[3]);
}

void CBossShaman::Update_IceDagger()
{
	//S_IceDagger
	for (auto& Pair : m_mapEffect)
	{
		if (dynamic_cast<CE_ShamanIceDagger*>(Pair.second))
		{
			CE_ShamanIceDagger* pObject = dynamic_cast<CE_ShamanIceDagger*>(Pair.second);
			if (pObject->Get_Active() == false/* && pObject->Get_Index() == m_iIceDaggerIdx*/)
			{
				CBone*  pBonePtr = m_pModelCom->Get_BonePtr("char_neck_jnt");
				_matrix SocketMatrix = m_pWeaponBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
				pObject->Set_Effect(SocketMatrix.r[3], true);
			}
		}
	}
}

void CBossShaman::Update_LazerPos()
{
	_matrix SocketMatrix = m_pHeadBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	_float4 vPos = SocketMatrix.r[3];
	vPos.x += 2.f;
	vPos.z += 2.f;
	m_mapEffect["S_Lazer"]->Set_Position(vPos);
	m_mapEffect["S_Lazer"]->Set_Active(true);
}

void CBossShaman::Update_Collider(_float fTimeDelta)
{
	{	
		_float fTemp[2] = { m_vPivotPos.y, m_fTrapColliserHeightY };
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


	m_pTransformCom->Set_PxPivot(m_vPivotPos);

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

void CBossShaman::BossFight_Start()
{
	g_bDayOrNight = false;
	m_pRendererCom->Set_Fog(true);
	const _float4 vColor = _float4(130.f / 255.f, 144.f / 255.f, 196.f / 255.f, 1.f);
	m_pRendererCom->Set_FogValue(vColor, 60.f);
}

void CBossShaman::BossFight_End()
{
	g_bDayOrNight = true;
	m_pRendererCom->Set_Fog(false);
}

void CBossShaman::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnTrail);
		return;
	}
	m_bTrail = true;
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
	m_bTrail = false;
	dynamic_cast<CE_ShamanTrail*>(m_mapEffect["S_Trail"])->Reset();
}

void CBossShaman::TurnOffMoveMentTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffMoveMentTrail);
		return;
	}
	m_bTrail = false;
	m_pMovementTrail->Set_Active(false);
}

void CBossShaman::TurnOnWeaponCloudTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnWeaponCloudTrail);
		return;
	}
	m_bDashAttackTrail = true;
}

void CBossShaman::TurnOffWeaponCloudTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffWeaponCloudTrail);
		return;
	}
	m_bDashAttackTrail = false;
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
	_matrix Socketmatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	_float4 vHandPos = Socketmatrix.r[3];

	dynamic_cast<CE_ShamanBossHandPlane*>(m_mapEffect["S_Hand"])->Set_Dissolve(true);
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
	dynamic_cast<CE_ShamanBossHandPlane*>(m_mapEffect["S_Hand"])->Set_Dissolve(true);
}

void CBossShaman::TurnOnDashAttackTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnDashAttackTrail);
		return;
	}
	m_bDashAttackTrail = true;
}

void CBossShaman::TurnOffDashAttackTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffDashAttackTrail);
		return;
	}
	m_bDashAttackTrail = false;
}

void CBossShaman::TurnOnSwipeChareged(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnSwipeChareged);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_Position();	_float fRange = 3.f;
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos += vLook * fRange;
	vPos.y = -1.5f;

	m_mapEffect["Shaman_Charged"]->Set_Position(vPos);
	m_mapEffect["Shaman_Charged"]->Set_Active(true);
}

void CBossShaman::TurnOnIceDagger(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnIceDagger);
		return;
	}
	m_bIceDagger = true;
}

void CBossShaman::TurnOffIceDagger(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffIceDagger);
		return;
	}
	m_bIceDagger = false;
}

void CBossShaman::TurnOnLazer(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnLazer);
		return;
	}
	// S_Lazer
}

void CBossShaman::TurnOnTeleport(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnTeleport);
		return;
	}
	/* 연기 출력 */
	_float4 vPos = m_pTransformCom->Get_Position();

	m_mapEffect["S_P_Teleport"]->Set_Position(vPos);
	dynamic_cast<CE_ShamanBossPlate*>(m_mapEffect["S_Plate"])->Set_Teleport(vPos);

	vPos.y += 1.5f;
	CE_ShamanSmoke* pShamanSmoke = dynamic_cast<CE_ShamanSmoke*>(m_mapEffect["Shaman_Smoke"]);
	{
		if (m_pModelCom->Get_AnimIndex() == (_uint)AWAKE || m_pModelCom->Get_AnimIndex() == (_uint)IDLE_LOOP)
		{
			_matrix Left_SocketMatrix = m_pLeftHandBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			pShamanSmoke->Set_State(CE_ShamanSmoke::STATE::STATE_IDLE, vPos, Left_SocketMatrix.r[3]);
		}
		else
		{
			pShamanSmoke->Set_State(CE_ShamanSmoke::STATE::STATE_TELEPORT, vPos);

			m_mapEffect["S_P_Teleport"]->Set_Active(true);
			m_pMovementTrail->Set_Active(true);
		}
	}
}

void CBossShaman::TurnOnCameraShake(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOnCameraShake);
		return;
	}
	CCamera_Player* pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
	if (pCamera != nullptr)
		pCamera->Camera_Shake(0.005f, 30);
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

	CE_ShamanBossPlate* pPlate = dynamic_cast<CE_ShamanBossPlate*>(m_mapEffect["S_Plate"]);
	pPlate->Set_Dissolve(true);
	pPlate->Set_Position(vPos);
	pPlate->Set_Active(true);

	CE_ShamanSummons* pSummons = dynamic_cast<CE_ShamanSummons*>(m_mapEffect["S_Summons"]);
	pSummons->Set_Dissolve(true);
	pSummons->Set_Position(vPos);
	pSummons->Set_Active(true);
}

void CBossShaman::TurnOffSummons(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::TurnOffSummons);
		return;
	}
	dynamic_cast<CE_ShamanBossPlate*>(m_mapEffect["S_Plate"])->Set_Dissolve(true);
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

	for (_uint i = 0; i < MINION_COUNT; i++)
	{
		Safe_Release(m_pMinions[i]);
		Safe_Release(m_pShamanThunder[i]);
	}

	Safe_Release(m_pShamanTapHex);
	Safe_Release(m_pShamanMask);

	for (auto& Pair : m_mapEffect)
		Safe_Release(Pair.second);
	m_mapEffect.clear();
}

void CBossShaman::SwordRenderProc(_float fTimeDelta)
{
	// m_eSwordRenderState = RENDER;
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
				CCamera_Player* pCamara = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
				if (pCamara != nullptr)
					pCamara->Camera_Shake(0.003f, 5);
			}
			else
			{
				m_bWeaklyHit = false;
				m_bStronglyHit = true;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.3f);
				m_pKena->Add_HitStopTime(0.25f);
				m_fHitStopTime += 0.25f;

				CCamera_Player* pCamara = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
				if (pCamara != nullptr)
					pCamara->Camera_Shake(0.005f, 5);

				vector<_float4>* vecWeaponPos = m_pKena->Get_WeaponPositions();
				if (vecWeaponPos->size() == 2)
				{
					_vector	vDir = vecWeaponPos->back() - vecWeaponPos->front();
					vDir = XMVectorSetZ(vDir, 0.f);

					if (pCamara != nullptr)
						pCamara->Camera_Shake(vDir, XMConvertToRadians(30.f));
				}
			}
		}

		if (iColliderIndex == (_int)COL_PLAYER_ARROW)
		{
			CSpiritArrow* pArrow = dynamic_cast<CSpiritArrow*>(pTarget);

			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			//m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());
			m_pMonsterStatusCom->UnderAttack(pArrow->Get_Damage());

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
		_float4(1.f, 0.f, 2.f, 0.f),
		_float4(1.f, 0.f, -2.f, 0.f),
		_float4(-1.5f, 0.f, 0.f, 0.f),
	};

	for (_uint i = 0; i < MINION_COUNT; i++)
	{
		m_pMinions[i]->Spawn_ByMaster(this, vPos + vOffset[i]);
		m_pShamanThunder[i]->Set_Position(vPos + vOffset[i]);
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

void CBossShaman::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_BossShaman_Attack1.ogg"), TEXT("Mon_BossShaman_Attack2.ogg"), TEXT("Mon_BossShaman_Attack3.ogg"), TEXT("Mon_BossShaman_Attack4.ogg"),
		TEXT("Mon_BossShaman_Attack5.ogg"), TEXT("Mon_BossShaman_Attack6.ogg"), TEXT("Mon_BossShaman_Attack7.ogg"), TEXT("Mon_BossShaman_Attack8.ogg"),
		
		TEXT("Mon_BossShaman_Hit1.ogg"), TEXT("Mon_BossShaman_Hit2.ogg"), TEXT("Mon_BossShaman_Hit3.ogg"), TEXT("Mon_BossShaman_Hit4.ogg"),
		
		TEXT("Mon_BossShaman_Hurt1.ogg"), TEXT("Mon_BossShaman_Hurt2.ogg"), TEXT("Mon_BossShaman_Hurt3.ogg"), TEXT("Mon_BossShaman_Hurt4.ogg"), TEXT("Mon_BossShaman_Hurt5.ogg"),
		TEXT("Mon_BossShaman_Hurt6.ogg"), TEXT("Mon_BossShaman_Hurt7.ogg"), 
		
		TEXT("Mon_BossShaman_Move1.ogg"), TEXT("Mon_BossShaman_Move2.ogg"), TEXT("Mon_BossShaman_Move3.ogg"),
		TEXT("Mon_BossShaman_Move4.ogg"), TEXT("Mon_BossShaman_Move5.ogg"), TEXT("Mon_BossShaman_Move6.ogg"),

		TEXT("Mon_BossShaman_Tense1.ogg"), TEXT("Mon_BossShaman_Tense2.ogg"), TEXT("Mon_BossShaman_Tense3.ogg"), TEXT("Mon_BossShaman_Tense4.ogg"),

		TEXT("Mon_Attack_Impact1.ogg"), TEXT("Mon_Attack_Impact2.ogg"), TEXT("Mon_Attack_Impact3.ogg"), TEXT("Mon_Attack_Impact4.ogg"), TEXT("Mon_Attack_Impact5.ogg"),	

		TEXT("Mon_Knife1.ogg"), TEXT("Mon_Knife2.ogg"), TEXT("Mon_Knife3.ogg"), TEXT("Mon_Sword1.ogg"), TEXT("Mon_Sword2.ogg"), TEXT("Mon_Swing.ogg"), TEXT("Mon_Swing2.ogg"),
		TEXT("Mon_BossShaman_Handclap.ogg"), TEXT("Mon_BossShaman_Teleport.ogg"), TEXT("Mon_BossSound_Ding.ogg"), TEXT("Mon_Walk_M.ogg"),
		TEXT("Mon_BossShaman_Ice.ogg"),
		TEXT("Mon_BossShaman_LaserImpact.ogg"),
		TEXT("Mon_BossHunter_Elemental7.ogg"),		
	};

	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}


void CBossShaman::Play_Attack1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK1], 0.5f);
}

void CBossShaman::Play_Attack2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK2], 0.5f);
}

void CBossShaman::Play_Attack3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK3], 0.5f);
}

void CBossShaman::Play_Attack4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK4], 0.5f);
}

void CBossShaman::Play_Attack5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK5], 0.5f);
}

void CBossShaman::Play_Attack6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK6], 0.5f);
}

void CBossShaman::Play_Attack7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK7], 0.5f);
}

void CBossShaman::Play_Attack8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Attack8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK8], 0.5f);
}

void CBossShaman::Play_Hit1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hit1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT1], 0.5f);
}

void CBossShaman::Play_Hit2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hit2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT2], 0.5f);
}

void CBossShaman::Play_Hit3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hit3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT3], 0.5f);
}

void CBossShaman::Play_Hit4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hit4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIT4], 0.5f);
}

void CBossShaman::Play_Hurt1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT1], 0.5f);
}

void CBossShaman::Play_Hurt2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT2], 0.5f);
}

void CBossShaman::Play_Hurt3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT3], 0.5f);
}

void CBossShaman::Play_Hurt4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT4], 0.5f);
}

void CBossShaman::Play_Hurt5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT5], 0.5f);
}

void CBossShaman::Play_Hurt6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT6], 0.5f);
}

void CBossShaman::Play_Hurt7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Hurt7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT7], 0.5f);
}

void CBossShaman::Play_Move1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Move1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE1], 0.5f);
}

void CBossShaman::Play_Move2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Move2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE2], 0.5f);
}

void CBossShaman::Play_Move3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Move3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE3], 0.5f);
}

void CBossShaman::Play_Move4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Move4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE4], 0.5f);
}

void CBossShaman::Play_Move5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Move5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE5], 0.5f);
}

void CBossShaman::Play_Move6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Move6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_MOVE6], 0.5f);
}

void CBossShaman::Play_Tense1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Tense1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.5f);
}

void CBossShaman::Play_Tense2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Tense2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE2], 0.5f);
}

void CBossShaman::Play_Tense3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Tense3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE3], 0.5f);
}

void CBossShaman::Play_Tense4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Tense4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE4], 0.5f);
}

void CBossShaman::Play_Impact1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Impact1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT1], 0.7f);
}

void CBossShaman::Play_Impact2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Impact2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT2], 0.7f);
}

void CBossShaman::Play_Impact3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Impact3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT3], 0.7f);
}

void CBossShaman::Play_Impact4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Impact4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT4], 0.7f);
}

void CBossShaman::Play_Impact5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Impact5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT5], 0.7f);
}


void CBossShaman::Play_Knife1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Knife1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_KNIFE1], 0.5f);
}

void CBossShaman::Play_Knife2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Knife2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_KNIFE2], 0.5f);
}

void CBossShaman::Play_Knife3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Knife3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_KNIFE3], 0.5f);
}

void CBossShaman::Play_Sword1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Sword1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SWORD1], 0.5f);
}

void CBossShaman::Play_Sword2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Sword2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SWORD2], 0.5f);
}

void CBossShaman::Play_Swing1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Swing1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SWING1], 0.5f);
}

void CBossShaman::Play_Swing2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_Swing2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SWING2], 0.5f);
}

void CBossShaman::Play_HandclapSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_HandclapSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HANDCLAP], 0.5f);
}

void CBossShaman::Play_TeleportSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_TeleportSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TELEPORT], 0.5f);
}

void CBossShaman::Play_DingSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_DingSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DING], 0.5f);
}

void CBossShaman::Play_WalkSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Play_WalkSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WALK], 0.5f);
}

void CBossShaman::Teleport_MeleeAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossShaman::Teleport_MeleeAttack);
		return;
	}

	m_bMeleeAttackTeleport = true;
}

void CBossShaman::Create_ShamanMask()
{
	CMonsterWeapon::MONSTERWEAPONDESC WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CMonsterWeapon::MONSTERWEAPONDESC));

	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("char_mask_jnt");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	WeaponDesc.pOwnerMonster = this;

	m_pShamanMask = (CBossShaman_Mask*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ShamanMask"), TEXT("BossShamanMask"), &WeaponDesc);
	assert(m_pShamanMask && "Boss Shaman Mask is nullptr");
	m_pShamanMask->Late_Initialize(nullptr);
}