#include "stdafx.h"
#include "..\public\Kena.h"
#include "GameInstance.h"
#include "AnimationState.h"
#include "Kena_State.h"
#include "Kena_Parts.h"
#include "Kena_Staff.h"
#include "SpiritArrow.h"
#include "RotBomb.h"
#include "Camera_Player.h"
#include "Effect_Base.h"
#include "E_KenaPulse.h"

#include "GroundMark.h"
#include "Terrain.h"
#include "Rope_RotRock.h"
#include "Rot.h"

#include "Kena_Status.h"
#include "Monster.h"

#include "UI_RotIcon.h"
#include "RotForMonster.h"
#include "E_KenaDust.h"
#include "UI_FocusMonster.h"
#include "CPortalPlane.h"

#include "HatCart.h"

#include "E_P_ExplosionGravity.h"
#include "E_KenaDash.h"
#include "Light.h"
#include "Layer.h"
#include "BossWarrior.h"

#include "ControlRoom.h"
#include "Level_Loading.h"
#include "Camera_Photo.h"
#include "E_P_Level_RiseY.h"

float g_QuestIndex = 0.f;
CKena::CKena(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pUI_FocusRot(nullptr)
	, m_bStateLock(false)
	, m_pUI_FocusMonster(nullptr)
	, m_pTargetMonster(nullptr)
	, m_bQuestOn(false)
	, m_iHitTarget(0)
{
}

CKena::CKena(const CKena & rhs)
	: CGameObject(rhs)
	, m_pUI_FocusRot(nullptr)
	, m_bStateLock(false)
	, m_pUI_FocusMonster(nullptr)
	, m_pTargetMonster(nullptr)
	, m_bQuestOn(false)
	, m_iHitTarget(0)
{
}

CKena_Parts * CKena::Get_KenaPart(const _tchar * pCloneObjectTag)
{
	const auto	iter = find_if(m_vecPart.begin(), m_vecPart.end(), [pCloneObjectTag](CKena_Parts* pPart) {
		return !lstrcmp(pCloneObjectTag, pPart->Get_ObjectCloneName());
	});

	if (iter == m_vecPart.end())
		return nullptr;

	return *iter;
}

_double CKena::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

const string & CKena::Get_AnimationState() const
{
	return m_pAnimation->Get_CurrentAnimName();
}

const _uint CKena::Get_AnimationStateIndex() const
{
	return m_pAnimation->Get_CurrentAnimIndex();
}

CEffect_Base* CKena::Get_Effect(const string& strKey)
{
	auto iter = m_mapEffect.find(strKey);
	if (iter == m_mapEffect.end())
		return nullptr;

	return iter->second;
}

const _bool CKena::Get_State(STATERETURN eState) const
{
	/* Used by Camera */
	if (eState == CKena::STATERETURN_END)
		return !m_bDeath && !m_bGrabWarrior && !m_bHeavyAttack && !m_bMask && !m_bAim && !m_bInjectBow && !m_bPulse && !m_bParryLaunch;

	switch (eState)
	{
	case STATE_DEATH:
		return m_bDeath;
		break;

	case STATE_LEVELUP:
		return m_bLevelUp;
		break;

	case STATE_LEVELUP_READY:
		return m_bLevelUp_Ready;
		break;

	case STATE_ATTACK:
		return m_bAttack;
		break;

	case STATE_HEAVYATTACK:
		return m_bHeavyAttack;
		break;

	case STATE_PERFECTATTACK:
		return m_bPerfectAttack;
		break;

	case STATE_COMMONHIT:
		return m_bCommonHit;
		break;

	case STATE_HEAVYHIT:
		return m_bHeavyHit;
		break;

	case STATE_GRAB_WARRIOR:
		return m_bGrabWarrior;
		break;

	case STATE_SPRINT:
		return m_bSprint;
		break;

	case STATE_DASH:
		return m_bDash || m_bDashAttack || m_bDashPortal;
		break;

	case STATE_MASK:
		return m_bMask;
		break;

	case STATE_AIM:
		return m_bAim;
		break;

	case STATE_BOW:
		return m_bBow;
		break;

	case STATE_INJECTBOW:
		return m_bInjectBow;
		break;

	case STATE_BOMB:
		return m_bBomb;
		break;

	case STATE_INJECTBOMB:
		return m_bInjectBomb;
		break;

	case STATE_PULSE:
		return m_bPulse;
		break;

	case STATE_PARRY:
		return m_bParryLaunch;
		break;

	case STATE_JUMP:
		return m_bJump;
		break;

	case STATE_BOSSBATTLE:
		return m_bBossBattle;
		break;

	default:
		return false;
	}
}

const _bool& CKena::Is_PhotoAnimEnd() const
{
	return m_bPhotoReady && m_pAnimation->Get_CurrentAnimIndex() == (_uint)CKena_State::PHOTO_PEACESIGN_LOOP;
}

void CKena::Set_State(STATERETURN eState, _bool bValue)
{
	if (eState == CKena::STATERETURN_END)
		return;

	switch (eState)
	{
	case STATE_DEATH:
		m_bDeath = bValue;
		break;

	case STATE_LEVELUP:
		m_bLevelUp = bValue;
		break;

	case STATE_LEVELUP_READY:
		m_bLevelUp_Ready = bValue;
		break;

	case STATE_ATTACK:
		m_bAttack = bValue;
		break;

	case STATE_HEAVYATTACK:
		m_bHeavyAttack = bValue;
		break;

	case STATE_PERFECTATTACK:
		m_bPerfectAttack = bValue;
		break;

	case STATE_COMMONHIT:
		m_bCommonHit = bValue;
		break;

	case STATE_HEAVYHIT:
		m_bHeavyHit = bValue;
		break;

	case STATE_GRAB_WARRIOR:
		m_bGrabWarrior = bValue;
		break;

	case STATE_SPRINT:
		m_bSprint = bValue;
		break;

	case STATE_DASH:
		m_bDash = bValue;
		break;

	case STATE_MASK:
		m_bMask = bValue;
		break;

	case STATE_AIM:
		m_bAim = bValue;
		break;

	case STATE_BOW:
		m_bBow = bValue;
		break;

	case STATE_INJECTBOW:
		m_bInjectBow = bValue;
		break;

	case STATE_BOMB:
		m_bBomb = bValue;
		break;

	case STATE_INJECTBOMB:
		m_bInjectBomb = bValue;
		break;

	case STATE_PULSE:
		m_bPulse = bValue;
		break;

	case STATE_PARRY:
		m_bParryLaunch = bValue;
		break;

	case STATE_JUMP:
		m_bJump = bValue;
		break;

	case STATE_BOSSBATTLE:
		m_bBossBattle = bValue;
		break;

	default:
		return;
	}
}

HRESULT CKena::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	if (pArg != nullptr)
		//m_pKenaStatus->Load_RunTime((const _tchar*)pArg);
		m_pKenaStatus->Load_RunTime(L"../Bin/Data/Status/Kena_Runtime_Status_BackUp.json");

	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Find_Camera(L"PLAYER_CAM"));
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);
	m_pCamera->Set_Player(this, m_pTransformCom);
	CGameInstance::GetInstance()->Work_Camera(L"PLAYER_CAM");

	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_pKenaState = CKena_State::Create(this, m_pKenaStatus, m_pStateMachine, m_pModelCom, m_pAnimation, m_pTransformCom, m_pCamera);

	FAILED_CHECK_RETURN(Ready_Parts(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Effects(), E_FAIL);

	if(g_LEVEL == (_int)LEVEL_FINAL) 
		FAILED_CHECK_RETURN(Ready_Rots(), E_FAIL);

	Push_EventFunctions();

	m_fSSSAmount = 0.09f;
	m_vSSSColor = _float4(0.2f, 0.18f, 0.16f, 1.f);
	m_vMulAmbientColor = _float4(2.45f, 2.f, 2.f, 1.f);
	m_vEyeAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);

	m_fGravity = 9.81f;
	/* InitJumpSpeed = 2.f * Gravity * JumpHeight */
	//m_fInitJumpSpeed = sqrtf(2.f * m_fGravity * 1.5f);
	m_fInitJumpSpeed = 0.35f;
	m_iObjectProperty = OP_PLAYER;
	m_pKenaStatus->Set_Attack(30);
	
	return S_OK;
}

HRESULT CKena::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(Ready_Arrows(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Bombs(), E_FAIL);

	_float3 vPos = _float3(0.f, 3.f, 0.f);
	_float3 vPivotScale = _float3(0.2f, 0.5f, 1.f); // _float3(0.2f, 3.f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.7f, 0.f);  // _float3(0.f, 3.2f, 0.f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = m_szCloneObjectTag;
	PxCapsuleDesc.vPos = vPos;
	PxCapsuleDesc.fRadius = vPivotScale.x;
	PxCapsuleDesc.fHalfHeight = vPivotScale.y;
	PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fDensity = 1.f;
		
	PxCapsuleDesc.fMass = KENA_MASS;
	PxCapsuleDesc.fAngularDamping = KENA_ANGULAR_DAMING;
	PxCapsuleDesc.fLinearDamping = KENA_LINEAR_DAMING;
	PxCapsuleDesc.bCCD = true;
	PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_PLAYER));
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
	m_pTransformCom->Set_PxPivotScale(vPivotScale);
	m_pTransformCom->Set_PxPivot(vPivotPos);

	/* Bump Collider */
	_tchar*	pTag = nullptr;
	_tchar		wszTag[6] = L"_Bump";
	_uint		iTagLength = lstrlen(m_szCloneObjectTag) + 7;

	pTag = new _tchar[iTagLength];
	lstrcpy(pTag, m_szCloneObjectTag);
	lstrcat(pTag, wszTag);

	CGameInstance::GetInstance()->Add_String(pTag);

	vPivotScale = _float3(0.5f, 0.5f, 1.f);

	_float4x4		matIdentity;
	XMStoreFloat4x4(&matIdentity, XMMatrixTranslation(0.f, 0.7f, 0.f));

	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = pTag;
	PxCapsuleDesc.vPos = vPos;
	PxCapsuleDesc.fRadius = vPivotScale.x;
	PxCapsuleDesc.fHalfHeight = vPivotScale.y;
	PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fDensity = 1.f;
	PxCapsuleDesc.fAngularDamping = 0.5f;
	PxCapsuleDesc.fMass = 200000.f;
	PxCapsuleDesc.fLinearDamping = 10.f;
	PxCapsuleDesc.bCCD = true;
	PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.f;

	//CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_PLAYER_BUMP));
	//m_pTransformCom->Add_Collider(pTag, matIdentity);

	/* Staff Collider */
	CKena_Staff*	pStaff = dynamic_cast<CKena_Staff*>(Get_KenaPart(L"Kena_Staff"));
	CModel*			pStaffModel = dynamic_cast<CModel*>(pStaff->Find_Component(L"Com_Model"));
	CBone*			pStaffRootJnt = pStaffModel->Get_BonePtr("staff_root_jnt");
	_matrix			matSocket = pStaffRootJnt->Get_OffsetMatrix() * pStaffRootJnt->Get_CombindMatrix() * pStaffModel->Get_PivotMatrix();
	
	matSocket.r[0] = XMVector3Normalize(matSocket.r[0]);
	matSocket.r[1] = XMVector3Normalize(matSocket.r[1]);
	matSocket.r[2] = XMVector3Normalize(matSocket.r[2]);

	vPos = _float3(0.f, 0.f, 0.f);
	// vPivotScale = _float3(0.03f, 0.35f, 1.f);
	vPivotScale = _float3(0.1f, 0.5f, 1.f);
	vPivotPos = _float3(0.f, 0.015f, -1.04f);

	_smatrix	matPivot = XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(0.f, 0.015f, -1.04f);
	matPivot = matPivot * matSocket;

	ZeroMemory(&PxCapsuleDesc, sizeof(CPhysX_Manager::PX_CAPSULE_DESC));
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = pStaff->Get_ObjectCloneName();
	PxCapsuleDesc.vPos = vPos;
	PxCapsuleDesc.fRadius = vPivotScale.x;
	PxCapsuleDesc.fHalfHeight = vPivotScale.y;
	PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fDensity = 1.f;
	PxCapsuleDesc.fAngularDamping = 0.5f;
	PxCapsuleDesc.fMass = 59.f;
	PxCapsuleDesc.fLinearDamping = 1.f;
	PxCapsuleDesc.bCCD = true;
	PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::PLAYER_WEAPON;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_PLAYER_WEAPON));
	m_pTransformCom->Add_Collider(pStaff->Get_ObjectCloneName(), matPivot);

	Setup_TerrainPtr();
	
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)

	_tchar szCloneRotTag[32] = { 0, };
	for (_int i = 0; i < 8; i++)
	{
		CGameObject* p_game_object = nullptr;
		swprintf_s(szCloneRotTag, L"RotForMonster_%d", i);
		CRotForMonster::DESC desc;
		ZeroMemory(&desc, sizeof(CRotForMonster::DESC));

		if (i == 0)
			desc.vPivotPos = _float4(-2.f, 0.f, -2.f, 1.f);
		else if (i == 1)
			desc.vPivotPos = _float4(0.f, 0.f, -2.f, 1.f);
		else if (i == 2)
			desc.vPivotPos = _float4(2.f, 0.f, -2.f, 1.f);
		else if (i == 3)
			desc.vPivotPos = _float4(2.f, 0.f, 0.f, 1.f);
		else if (i == 4)
			desc.vPivotPos = _float4(2.f, 0.f, 2.f, 1.f);
		else if (i == 5)
			desc.vPivotPos = _float4(0.f, 0.f, 2.f, 1.f);
		else if (i == 6)
			desc.vPivotPos = _float4(-2.f, 0.f, 2.f, 1.f);
		else if (i == 7)
			desc.vPivotPos = _float4(-2.f, 0.f, 0.f, 1.f);

		if (FAILED(p_game_instance->Clone_GameObject(g_LEVEL, TEXT("Layer_Rot"), TEXT("Prototype_GameObject_RotForMonster"),
			CUtile::Create_StringAuto(szCloneRotTag), &desc, &p_game_object)))
			return E_FAIL;

		m_pRotForMonster[i] = static_cast<CRotForMonster*>(p_game_object);
	}

	RELEASE_INSTANCE(CGameInstance)

	CUI_ClientManager::UI_PRESENT eRot = CUI_ClientManager::HUD_ROT;
	//CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	_float fRotState = (_float)CKena_Status::RS_GOOD;
	m_Delegator.broadcast(eRot, fRotState);
	//m_PlayerDelegator.broadcast(eRot, funcDefault, fRotState);

	if(g_LEVEL == (_int)LEVEL_TESTPLAY)
	{
		const _float4 vPosFloat4 = _float4(13.f, 0.f, 9.f, 1.f);
		m_pTransformCom->Set_Position(vPosFloat4);
	}
	else if(g_LEVEL == (_int)LEVEL_FINAL)
	{
		const _float4 vPosFloat4 = _float4(151.7f, 22.2f, 609.5f, 1.f);
		m_pTransformCom->Set_Position(vPosFloat4);
		m_bQuestOn = true;
	}
	else
	{
		const _float4 vPosFloat4 = _float4(13.f, 0.f, 9.f, 1.f);
		m_pTransformCom->Set_Position(vPosFloat4);
	}

	for (auto& pEffect : m_mapEffect)
	{
		if(pEffect.second != nullptr )
			pEffect.second->Late_Initialize();
	}
	pStaff->Late_Initialize(pArg);

	m_pCamera_Photo = (CCamera_Photo*)CGameInstance::GetInstance()->Find_Camera(CAMERA_PHOTO_TAG);
	if (m_pCamera_Photo)
	{
		m_pCamera_Photo->Set_KenaPtr(this);
		m_pCamera_Photo->Set_FirstRotPtr(m_pFirstRot);
	}
	return S_OK;
}

void CKena::Tick(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_F10))
		m_bStateLock = !m_bStateLock;
		
	if(NanCheck())
	{
		m_pTransformCom->Set_WorldMatrix_float4x4(m_PrevMatrix);
	}
	else
	{
		m_PrevMatrix = m_pTransformCom->Get_WorldMatrixFloat4x4();
	}

#ifdef _DEBUG
	// if (CGameInstance::GetInstance()->IsWorkCamera(TEXT("DEBUG_CAM_1"))) return;	
	m_pKenaStatus->Set_Attack(1000);
	//m_pKenaStatus->Unlock_Skill(CKena_Status::SKILL_BOMB, 0);
	//m_pKenaStatus->Unlock_Skill(CKena_Status::SKILL_BOW, 0);
#endif	
	_float	fTimeRate = Update_TimeRate();

	__super::Tick(fTimeDelta);

	ImGui::Begin("Kena");

	if (ImGui::Button("WarriorPos"))
	{
		const _float4 vPosFloat4 = _float4(22.8f, 15.f, 842.8f, 1.f);
		m_pTransformCom->Set_Position(vPosFloat4);
	}

	if (ImGui::Button("ShamanPos"))
	{
		const _float4 vPosFloat4 = _float4(25.f, 15.f, 1123.8f, 1.f);
		m_pTransformCom->Set_Position(vPosFloat4);
	}

	if (ImGui::Button("HP"))
	{
		m_pKenaStatus->Set_HP(m_pKenaStatus->Get_MaxHP());
	}
	ImGui::End();

	if (m_bQuestOn)
	{
		/* Quest 2 Open */
		//CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;
		//_bool bStart = true;
		//_float fDefaultVal = 2.f;
		//wstring wstr = L"";
		//m_PlayerQuestDelegator.broadcast(eQuest, bStart, fDefaultVal, wstr);

		m_bQuestOn = false;
	}

	LiftRotRockProc();

	Check_Damaged();

	CUI_ClientManager::UI_PRESENT eHP = CUI_ClientManager::HUD_HP;
	//CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	_float fGuage = m_pKenaStatus->Get_PercentHP();
	m_Delegator.broadcast(eHP, fGuage);
	//m_PlayerDelegator.broadcast(eHP, funcDefault, fGuage);

	Update_State_Status(fTimeDelta);

	Update_Collider(fTimeDelta);

	Check_TimeRate_Changed(fTimeDelta, fTimeRate);

	if(m_bPhotoReady)
	{
		_float4 vPos = _float4(-34.7f, 20.4f, 1231.1f, 1.f);
		m_pTransformCom->Set_Position(vPos);
		CRot::Set_RotUseKenaPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_pFirstRot->Execute_PhotoTeleport();
	}

	Play_Animation(fTimeDelta, fTimeRate);

	Push_WeaponPosition();

	Run_Smooth_Targeting(fTimeDelta);

	

	/* Camera Shake Test */
	/*if (CGameInstance::GetInstance()->Key_Down(DIK_O))
		m_pCamera->Camera_Shake(0.003f, 10);
	if (CGameInstance::GetInstance()->Key_Down(DIK_P))
		m_pCamera->Camera_Shake(XMVectorSet(1.f, 1.f, 0.f, 0.f), XMConvertToRadians(10.f));*/


	CRot::Set_RotUseKenaPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	CMonster::Set_MonsterUseKenaPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CKena::Late_Tick(_float fTimeDelta)
{
	/* ���� ����� ���� �ٶ�. */
	for (auto& pPart : m_vecPart)
		pPart->Tick(fTimeDelta);

	for (auto& pArrow : m_vecArrow)
		pArrow->Tick(fTimeDelta);

	for (auto& pBomb : m_vecBomb)
		pBomb->Tick(fTimeDelta);

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	m_pKenaState->Late_Tick(fTimeDelta);

// 	if (m_bParry == false)
// 		m_eDamagedDir = CKena::DAMAGED_FROM_END;

	if (m_iCurParryFrame < m_iParryFrameCount)
	{
		m_iCurParryFrame++;
		m_bParry = true;
	}

	Check_Smooth_Targeting();

	/* UI Control */
	if (CKena_Status::RS_ACTIVE == m_pKenaStatus->Get_RotState())
	{
		//CMonster* pMonster = m_pRotForMonster[0]->Get_Target();
		//if (nullptr != pMonster && false == pMonster->Get_Bind())
		//{
			CUI_ClientManager::UI_PRESENT eRot = CUI_ClientManager::HUD_ROT;
			CKena_Status::ROTSTATE eRotState;
			if (m_pKenaStatus->Get_CurPIPGuage() >= 1.0f)
				eRotState = CKena_Status::RS_GOOD;
			else
				eRotState = CKena_Status::RS_HIDE;

			m_pKenaStatus->Set_RotState(eRotState);
			_float fState = (_float)eRotState;
			m_Delegator.broadcast(eRot, fState);
			//m_PlayerDelegator.broadcast(eRot, funcDefault, fState);
		//}
	}

	//if (CGameInstance::GetInstance()->Key_Down(DIK_1))
	//{       
	//	CUI_ClientManager::UI_PRESENT eQuestOpen = CUI_ClientManager::QUEST_;
	//	_bool bOpen = true;
	//	wstring wstr = L"";
	//	m_PlayerQuestDelegator.broadcast(eQuestOpen, bOpen, g_QuestIndex, wstr);
	//	CGameInstance::GetInstance()->Play_Sound(L"UI_QuestOccur.ogg", 1.f, false, SOUND_UI);
	//}
	//if (CGameInstance::GetInstance()->Key_Down(DIK_2))
	//{
	//	CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_CLEAR_ALL;
	//	_bool bOpen = true;
	//	wstring wstr = L"";
	//	m_PlayerQuestDelegator.broadcast(eQuest, bOpen, g_QuestIndex, wstr);
	//	CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);
	//	g_QuestIndex++;
	//}














	/* ~UI Control */

	/************** Delegator Test *************/
	

	//CUI_ClientManager::UI_PRESENT eHP = CUI_ClientManager::HUD_HP;
	CUI_ClientManager::UI_PRESENT ePip = CUI_ClientManager::HUD_PIP;
	//CUI_ClientManager::UI_PRESENT eType3 = CUI_ClientManager::HUD_SHIELD;
	CUI_ClientManager::UI_PRESENT eType4 = CUI_ClientManager::HUD_ROT;
	//CUI_ClientManager::UI_PRESENT eBomb = CUI_ClientManager::AMMO_BOMB;
	//CUI_ClientManager::UI_PRESENT eArrowGuage = CUI_ClientManager::AMMO_ARROW;
	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;
	//CUI_ClientManager::UI_PRESENT eQuestLine = CUI_ClientManager::QUEST_LINE;
	CUI_ClientManager::UI_PRESENT eInv = CUI_ClientManager::INV_;
	CUI_ClientManager::UI_PRESENT eCart = CUI_ClientManager::HATCART_;

	////CUI_ClientManager::UI_PRESENT eKarma = CUI_ClientManager::INV_KARMA;
	////CUI_ClientManager::UI_PRESENT eNumRots = CUI_ClientManager::INV_NUMROTS;
	////CUI_ClientManager::UI_PRESENT eCrystal = CUI_ClientManager::INV_CRYSTAL;
	//CUI_ClientManager::UI_PRESENT eLetterBox = CUI_ClientManager::LETTERBOX_AIM;

	////CUI_ClientManager::UI_PRESENT eUpgrade = CUI_ClientManager::INV_UPGRADE;

	CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	//CUI_ClientManager::UI_FUNCTION funcLevelup = CUI_ClientManager::FUNC_LEVELUP;
	//CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	//CUI_ClientManager::UI_FUNCTION funcCheck = CUI_ClientManager::FUNC_CHECK;

	if (CGameInstance::GetInstance()->Key_Down(DIK_M))
	{
		CKena* pPlayer = this;
		m_PlayerPtrDelegator.broadcast(eInv, funcDefault, pPlayer);
	}

	if(CGameInstance::GetInstance()->Key_Down(DIK_P))
	{
		/* Test Before Hit Monster */
		_float fGuage = m_pKenaStatus->Get_CurPIPGuage();
		m_pKenaStatus->Plus_CurPIPGuage(0.2f);
		_float fCurGuage = m_pKenaStatus->Get_CurPIPGuage();
		m_Delegator.broadcast(ePip, fCurGuage);
	}

	//if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
	//{
	//	m_pKenaStatus->Add_RotCount();
	//}

	//	//static _float fTag = 0.0f;
	//	//if (fTag < 1.0f)
	//	//	fTag = 1.0f;
	//	//else
	//	//	fTag = 0.0f;
	//	//m_PlayerDelegator.broadcast(eLetterBox, funcDefault, fTag);

	//	/* Inventory */
	//	_float fTag = 0.f;
	//	_float fKarma = (_float)m_pKenaStatus->Get_Karma();
	//	_float fRots = (_float)m_pKenaStatus->Get_RotCount();
	//	_float fCrystal = (_float)m_pKenaStatus->Get_Crystal();

	//	CKena* pPlayer = this;
	//	m_PlayerPtrDelegator.broadcast(eInv, funcDefault, pPlayer);

		//m_PlayerDelegator.broadcast(eInv, funcDefault, fTag);
		//m_PlayerDelegator.broadcast(eKarma, funcDefault, fKarma);
		//m_PlayerDelegator.broadcast(eNumRots, funcDefault, fRots);
		//m_PlayerDelegator.broadcast(eCrystal, funcDefault, fCrystal);

	//}

	//static _float fNum = 3.f;
	//_float fZero = 0.f;
	//if (CGameInstance::GetInstance()->Key_Down(DIK_U))
	//{
	//	static _float fLevel = 0.f;
	//	fLevel += 1.f;
	//	m_PlayerDelegator.broadcast(eArrowGuage, funcLevelup, fLevel);
	//	m_PlayerDelegator.broadcast(eBomb, funcLevelup, fLevel);
	//	m_PlayerDelegator.broadcast(ePip, funcLevelup, fLevel);
	//	m_PlayerDelegator.broadcast(eHP, funcLevelup, fLevel);

	//	m_PlayerDelegator.broadcast(eQuest, funcSwitch, fZero);

	//}
	//if (CGameInstance::GetInstance()->Key_Down(DIK_P))
	//{
	//	/* Pip Guage pop test */
	//	fNum -= 1.f;
	//	m_PlayerDelegator.broadcast(ePip, funcDefault, fZero);




	//	/* Bomb Guage test */
	//	static _float fBomb = 0.f;
	//	m_PlayerDelegator.broadcast(eBomb, funcDefault, fBomb);

	//	/* Arrow Guage test */
	//	static _float fArrow = 1.f;
	//	m_PlayerDelegator.broadcast(eArrowGuage, funcDefault, fArrow);

	//	/* Aim Test */
	//	static _float fAim = 1.f;
	//	m_PlayerDelegator.broadcast(eAim, funcDefault, fAim);

	//	/* Quest Open Test */
	//	static _float fQuestIndex = 0.f;
	//	m_PlayerDelegator.broadcast(eQuestLine, funcSwitch, fQuestIndex);
	//	fQuestIndex += 1.f;

	//}
	//if (CGameInstance::GetInstance()->Key_Down(DIK_I))
	//{
	//	fNum -= 0.1f;
	//	m_PlayerDelegator.broadcast(eHP, funcDefault, fNum);
	//	m_PlayerDelegator.broadcast(ePip, funcDefault, fNum);
	//	m_PlayerDelegator.broadcast(eType3, funcDefault, fNum);

	//	/* Quest Check Test */
	//	static _float fQuestClear = 0.f;
	//	m_PlayerDelegator.broadcast(eQuestLine, funcCheck, fQuestClear);
	//	fQuestClear += 1.f;


	//}
	//if (CGameInstance::GetInstance()->Key_Down(DIK_O))
	//{
	//	fNum += 0.1f;
	//	m_PlayerDelegator.broadcast(eHP, funcDefault, fNum);
	//	m_PlayerDelegator.broadcast(ePip, funcDefault, fNum);
	//	m_PlayerDelegator.broadcast(eType3, funcDefault, fNum);
	//}

	/************** ~Delegator Test *************/


	if (m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	for (auto& pPart : m_vecPart)
		pPart->Late_Tick(fTimeDelta);

	for (auto& pArrow : m_vecArrow)
		pArrow->Late_Tick(fTimeDelta);

	for (auto& pBomb : m_vecBomb)
		pBomb->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Late_Tick(fTimeDelta);
}

HRESULT CKena::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if (i == 1)
		{
			// Arm & Leg
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveMaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SSS_MASK, "g_SSSMaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_HAIR_DEPTH, "g_DetailNormal");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		}
		else if (i == 4)
		{
			// Eye Render
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		}
		else if (i == 5 || i == 6)
		{
			// HEAD
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SSS_MASK, "g_SSSMaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 4);
		}
		else if (i == 0)
		{
			// Render Off
			continue;
		}
		else
		{
			// Eye Lash
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 7);
		}
	}

	return S_OK;
}

HRESULT CKena::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 11);

	return S_OK;
}

HRESULT CKena::RenderCine()
{
	FAILED_CHECK_RETURN(__super::RenderCine(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_CineShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0)
		{
			// Render Off
			continue;
		}
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 11);
	}

	return S_OK;
}

void CKena::Imgui_RenderProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}

	if (ImGui::Button("m_bHeavyHit"))
	{
		m_bHitRim = true;
		m_fHitRimIntensity = 1.f;
	}
	if (ImGui::Button("m_bParry"))
	{
		m_bParryRim = true;
		m_fParryRimIntensity = 1.f;
	}
	if (ImGui::Button("m_bDash"))
	{
		m_bDashRim = true;
		m_fDashRimIntensity = 1.f;
	}
	if (ImGui::Button("m_bLevelUp"))
	{
		m_bLevelUpRim = true;
		m_fLevelUpRimIntensity = 1.f;
	}
	
	if (ImGui::Button("m_bParryLaunch"))
		m_bParryLaunch = !m_bParryLaunch;

	_int	HP[2] = { m_pKenaStatus->Get_HP(), m_pKenaStatus->Get_MaxHP() };
	ImGui::InputInt2("HP", (_int*)&HP, ImGuiInputTextFlags_ReadOnly);

	_int	ArrowCount[2] = { m_pKenaStatus->Get_CurArrowCount(), m_pKenaStatus->Get_MaxArrowCount() };
	ImGui::InputInt2("Arrow Count", (_int*)&ArrowCount, ImGuiInputTextFlags_ReadOnly);

	_float2	ArrowCoolTime{ m_pKenaStatus->Get_CurArrowCoolTime(), m_pKenaStatus->Get_InitArrowCoolTime() };
	ImGui::InputFloat2("Arrow CoolTime", (_float*)&ArrowCoolTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_int	BombCount[2] = { m_pKenaStatus->Get_CurBombCount(), m_pKenaStatus->Get_MaxBombCount() };
	ImGui::InputInt2("Bomb Count", (_int*)&BombCount, ImGuiInputTextFlags_ReadOnly);

	_float3	BombCoolTime{ m_pKenaStatus->Get_CurBombCoolTime(0), m_pKenaStatus->Get_CurBombCoolTime(1), m_pKenaStatus->Get_InitBombCoolTime() };
	ImGui::InputFloat3("Bomb CoolTime", (_float*)&BombCoolTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_float2	Shield{ m_pKenaStatus->Get_Shield(), m_pKenaStatus->Get_MaxShield() };
	ImGui::InputFloat2("Shield", (_float*)&Shield, "%.3f", ImGuiInputTextFlags_ReadOnly);
	
	_float2	ShieldCoolTime{ m_pKenaStatus->Get_CurShieldCoolTime(), m_pKenaStatus->Get_InitShieldCoolTime() };
	ImGui::InputFloat2("Shield CoolTime", (_float*)&ShieldCoolTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_float2	ShieldRecoveryTime{ m_pKenaStatus->Get_CurShieldRecoveryTime(), m_pKenaStatus->Get_InitShieldRecoveryTime() };
	ImGui::InputFloat2("Shield Recovery Time", (_float*)&ShieldRecoveryTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	// TEST	
	ImGui::DragFloat("Linear Damping", &m_fLinearDamping, 0.01f, -100.f, 100.0f);	
	ImGui::DragFloat("Angular Damping", &m_fAngularDamping, 0.01f, -100.f, 100.0f);
	ImGui::DragFloat("Mass", &m_fMass, 10.f, 0.f, FLT_MAX);
	
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		PxRigidDynamic* pDynamic = (PxRigidDynamic*)m_pTransformCom->Get_Actor();
		pDynamic->setLinearDamping(m_fLinearDamping);
		pDynamic->setAngularDamping(m_fAngularDamping);
		pDynamic->setMass(m_fMass);

	}
	
	__super::Imgui_RenderProperty();
}

void CKena::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();

	m_vecArrow[0]->Imgui_RenderProperty();

	ImGui::BeginTabBar("Kena Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("State"))
	{
		ImGui::BulletText("Kena State");
		if (ImGui::Button("Rebuild StateMachine"))
		{
			m_pStateMachine->Rebuild();
			Safe_Release(m_pKenaState);
			m_pKenaState = CKena_State::Create(this, m_pKenaStatus, m_pStateMachine, m_pModelCom, m_pAnimation, m_pTransformCom, m_pCamera);
		}
		m_pStateMachine->Imgui_RenderProperty();
		ImGui::Separator();
		ImGui::BulletText("Animation State");
		m_pAnimation->ImGui_RenderProperty();
		ImGui::Separator();

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CKena::ImGui_ShaderValueProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}

	__super::ImGui_ShaderValueProperty();
	{
		static _float2 AmountMinMax{ -10.f, 10.f };
		ImGui::InputFloat2("SSSAmoutMinMax", (float*)&AmountMinMax);
		ImGui::DragFloat("SSSAmount", &m_fSSSAmount, 0.001f, AmountMinMax.x, AmountMinMax.y);

		_float fColor[3] = { m_vSSSColor.x, m_vSSSColor.y, m_vSSSColor.z };
		static _float2 sssMinMax{ -1.f, 1.f };
		ImGui::InputFloat2("SSSMinMax", (float*)&sssMinMax);
		ImGui::DragFloat3("SSSColor", fColor, 0.001f, sssMinMax.x, sssMinMax.y);
		m_vSSSColor.x = fColor[0];
		m_vSSSColor.y = fColor[1];
		m_vSSSColor.z = fColor[2];
	}

	{
		_float fColor[3] = { m_vMulAmbientColor.x, m_vMulAmbientColor.y, m_vMulAmbientColor.z };
		static _float2 maMinMax{ 0.f, 255.f };
		ImGui::InputFloat2("MAMinMax", (float*)&maMinMax);
		ImGui::DragFloat3("MAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
		m_vMulAmbientColor.x = fColor[0];
		m_vMulAmbientColor.y = fColor[1];
		m_vMulAmbientColor.z = fColor[2];
	}

	{
		_float fColor[3] = { m_vEyeAmbientColor.x, m_vEyeAmbientColor.y, m_vEyeAmbientColor.z };
		static _float2 maMinMax{ 0.f, 255.f };
		ImGui::InputFloat2("EyeAAMinMax", (float*)&maMinMax);
		ImGui::DragFloat3("EyeAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
		m_vEyeAmbientColor.x = fColor[0];
		m_vEyeAmbientColor.y = fColor[1];
		m_vEyeAmbientColor.z = fColor[2];
	}

	{
		static _float2 LashWidthMinMax{ 0.f, 100.f };
		ImGui::InputFloat2("LashWidthMinMax", (float*)&LashWidthMinMax);
		ImGui::DragFloat("LashWidthAmount", &m_fLashWidth, 0.1f, LashWidthMinMax.x, LashWidthMinMax.y);

		static _float2 LashDensityMinMax{ 0.f, 100.f };
		ImGui::InputFloat2("LashDensityAmoutMinMax", (float*)&LashDensityMinMax);
		ImGui::DragFloat("LashDensityAmount", &m_fLashDensity, 0.1f, LashDensityMinMax.x, LashDensityMinMax.y);

		static _float2 LashIntensity{ 0.f, 100.f };
		ImGui::InputFloat2("LashIntensityMinMax", (float*)&LashIntensity);
		ImGui::DragFloat("LashIntensityAmount", &m_fLashIntensity, 0.1f, LashIntensity.x, LashIntensity.y);
	}

	for (auto& pPart : m_vecPart)
	{
		ImGui::NewLine();
		pPart->ImGui_ShaderValueProperty();
	}
}

void CKena::ImGui_PhysXValueProperty()
{
	// __super::ImGui_PhysXValueProperty();
	m_mapEffect["KenaPulse"]->ImGui_PhysXValueProperty();

	//m_vecBomb.front()->ImGui_PhysXValueProperty();
	//m_vecBomb.back()->ImGui_PhysXValueProperty();

	//_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();

	//float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	//ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	//vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	//CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	//m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	//_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();

	//float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	//ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	//vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	//m_pTransformCom->Set_PxPivot(vPxPivot);

	//PxRigidActor* pRigidActor =	CPhysX_Manager::GetInstance()->Find_DynamicActor(m_szCloneObjectTag);
	// 	_float fMass = ((PxRigidDynamic*)pRigidActor)->getMass();
	// 	ImGui::DragFloat("Mass", &fMass, 0.01f, -100.f, 500.f);
	// 	_float fLinearDamping = ((PxRigidDynamic*)pRigidActor)->getLinearDamping();
	// 	ImGui::DragFloat("LinearDamping", &fLinearDamping, 0.01f, -100.f, 500.f);
	// 	_float fAngularDamping = ((PxRigidDynamic*)pRigidActor)->getAngularDamping();
	// 	ImGui::DragFloat("AngularDamping", &fAngularDamping, 0.01f, -100.f, 500.f);
	// 	_float3 vVelocity = CUtile::ConvertPosition_PxToD3D(((PxRigidDynamic*)pRigidActor)->getLinearVelocity());
	// 	float fVelocity[3] = { vVelocity.x, vVelocity.y, vVelocity.z };
	// 	ImGui::DragFloat3("PxVelocity", fVelocity, 0.01f, 0.1f, 100.0f);
	// 	vVelocity.x = fVelocity[0]; vVelocity.y = fVelocity[1]; vVelocity.z = fVelocity[2];
	//CPhysX_Manager::GetInstance()->Set_Velocity(pRigidActor, _float3(0.f, m_fCurJumpSpeed, 0.f));
	//list<CTransform::ActorData>*	pActorList = m_pTransformCom->Get_ActorList();
	//_uint iActorCount = (_uint)pActorList->size();

	//ImGui::BulletText("ColliderLists");
	//{
	//	static _int iSelect = -1;
	//	char** ppObjectTag = new char*[iActorCount];
	//	_uint iTagLength = 0;
	//	_uint i = 0;
	//	for (auto& Pair : *pActorList)
	//		ppObjectTag[i++] = CUtile::WideCharToChar(Pair.pActorTag);
	//	ImGui::ListBox("Collider List", &iSelect, ppObjectTag, iActorCount);

	//	if (iSelect != -1)
	//	{
	//		ImGui::BulletText("Current Collider Object : %s", ppObjectTag[iSelect]);

	//		_tchar*	pActorTag = CUtile::CharToWideChar(ppObjectTag[iSelect]);
	//		CTransform::ActorData*	pActorData = m_pTransformCom->FindActorData(pActorTag);
	//		PxRigidActor*		pActor = pActorData->pActor;

	//		PxShape*			pShape = nullptr;
	//		pActor->getShapes(&pShape, sizeof(PxShape));
	//		PxCapsuleGeometry& Geometry = pShape->getGeometry().capsule();
	//		_float&	fScaleX = Geometry.radius;
	//		_float&	fScaleY = Geometry.halfHeight;
	//		
	//		/* Scale */
	//		ImGui::BulletText("Scale Setting");
	//		ImGui::DragFloat("Scale X", &fScaleX, 0.05f);
	//		ImGui::DragFloat("Scale Y", &fScaleY, 0.05f);

	//		pShape->setGeometry(Geometry);

	//		/* Rotate & Position */
	//		ImGui::Separator();
	//		ImGui::BulletText("Rotate Setting");

	//		CKena_Staff*	pStaff = dynamic_cast<CKena_Staff*>(Get_KenaPart(L"Kena_Staff"));
	//		CModel*			pStaffModel = dynamic_cast<CModel*>(pStaff->Find_Component(L"Com_Model"));
	//		CBone*			pStaffRootJnt = pStaffModel->Get_BonePtr("staff_skin6_jnt");
	//		_matrix			matSocket = pStaffRootJnt->Get_OffsetMatrix() * pStaffRootJnt->Get_CombindMatrix() * pStaffModel->Get_PivotMatrix();

	//		matSocket.r[0] = XMVector3Normalize(matSocket.r[0]);
	//		matSocket.r[1] = XMVector3Normalize(matSocket.r[1]);
	//		matSocket.r[2] = XMVector3Normalize(matSocket.r[2]);

	//		_smatrix		matPivot = pActorData->PivotMatrix * XMMatrixInverse(nullptr, matSocket);
	//		_float4		vScale, vRot, vTrans;
	//		ImGuizmo::DecomposeMatrixToComponents((_float*)&matPivot, (_float*)&vTrans, (_float*)&vRot, (_float*)&vScale);
	//		
	//		ImGui::DragFloat3("Rotate", (_float*)&vRot, 0.01f);
	//		ImGui::DragFloat3("Translation", (_float*)&vTrans, 0.01f);

	//		ImGuizmo::RecomposeMatrixFromComponents((_float*)&vTrans, (_float*)&vRot, (_float*)&vScale, (_float*)&matPivot);

	//		pActorData->PivotMatrix = matPivot * matSocket;

	//		Safe_Delete_Array(pActorTag);
	//	}

	//	for (_uint i = 0; i < iActorCount; ++i)
	//		Safe_Delete_Array(ppObjectTag[i]);
	//	Safe_Delete_Array(ppObjectTag);
	//}
}

void CKena::Update_Child()
{
	//for (auto& pPart : m_vecPart)
	//	pPart->Model_Synchronization(m_pModelCom->Get_PausePlay());
}

HRESULT CKena::Call_EventFunction(const string & strFuncName)
{
	return S_OK;
}

void CKena::Push_EventFunctions()
{
	UnderAttack(true, 0.f);
	Grab_CameraShake(true, 0.f);

	TurnOnFootStep_Left(true, 0.f);
	TurnOnFootStep_Right(true, 0.f);
	TurnOnAttack(true, 0.f);
	TurnOffAttack(true, 0.f);
	TurnOnTrail(true, 0.f);
	TurnOffTrail(true, 0.f);
	TurnOnCharge(true, 0.f);
	TurnOffCharge(true, 0.f);
	TurnOnPulseJump(true, 0.f);
	TurnOnHeavyAttack_Into(true, 0.f);
	TurnOnInteractStaff(true, 0.f);

	TurnOnPulseParry(true, 0.f);
	TurnOnPulseParryHand(true, 0.f);
	TurnOnPulseParryRange(true, 0.f);

	TurnOnDashSt(true, 0.f);
	TurnOnDashLp(true, 0.f);
	TurnOffDashLp(true, 0.f);
	TurnOnDashEd(true, 0.f);

	TurnOnLvUp(true, 0.0f);
	TurnOnLvUp_Part1_Floor(true, 0.f);
	TurnOnLvUp_Part2_RiseY(true, 0.f);
	TurnOffLvUp_Part2_RiseY(true, 0.0f);

	PlaySound_Kena_FootStep(true, 0.f);
	PlaySound_Kena_FootStep_Sprint(true, 0.f);
	PlaySound_Jump(true, 0.f);
	PlaySound_PulseJump(true, 0.f);
	PlaySound_Land(true, 0.f);
	PlaySound_Dodge(true, 0.f);
	PlaySound_Dodge_End(true, 0.f);
	PlaySound_Interact_Staff(true, 0.f);
	PlaySound_Sit(true, 0.f);
	PlaySound_Rot_Action_Command(true, 0.f);
	PlaySound_Rot_Action_Voice(true, 0.f);

	PlaySound_Pulse_Intro(true, 0.f);
	PlaySound_Pulse_Outro(true, 0.f);
	PlaySound_Dash(true, 0.f);

	PlaySound_Attack_Voice(true, 0.f);
	PlaySound_Attack_1(true, 0.f);
	PlaySound_Attack_2(true, 0.f);
	PlaySound_Attack_3(true, 0.f);
	PlaySound_Attack_4(true, 0.f);
	PlaySound_HeavyAttack_Charge(true, 0.f);
	PlaySound_HeavyAttack_Charge_Voice(true, 0.f);
	PlaySound_HeavyAttack_Release_Perfect(true, 0.f);
	PlaySound_HeavyAttack_Spark(true, 0.f);
	PlaySound_HeavyAttack_Staff_Sweep(true, 0.f);
	PlaySound_HeavyAttack_Combo_Charge(true, 0.f);
	PlaySound_HeavyAttack_Combo_Staff_Sweep(true, 0.f);
	PlaySound_AirAttack_Slam_Release(true, 0.f);
	PlaySound_SpinAttack(true, 0.f);
	PlaySound_WarriorGrab(true, 0.f);
}

void CKena::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	if (m_bLocalMoveLock == true)
		return;

	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

HRESULT CKena::Change_Level(LEVEL eNextLevel)
{
	if (eNextLevel == LEVEL_GIMMICK || eNextLevel == LEVEL_FINAL)
		return m_pKenaStatus->Save_RunTime(RUNTIME_STATUS_FILEPATH);

	return E_FAIL;
}

void CKena::Respawn()
{
	if (m_bDeath == false)
		return;

	m_pKenaStatus->Respawn();
}

void CKena::Respawn(_fvector vSpawnPos)
{
	if (m_bDeath == false)
		return;

	m_pKenaStatus->Respawn();

	_float4		vPos = vSpawnPos;
	m_pTransformCom->Set_Position(vPos);
}

void CKena::Smooth_Targeting(CMonster * pMonster)
{
	if (m_pTargetMonster != nullptr)
	{
		if (m_pTransformCom->Calc_Distance_XZ(pMonster->Get_TransformCom()) < m_pTransformCom->Calc_Distance_XZ(m_pTargetMonster->Get_TransformCom()))
			m_pTargetMonster = pMonster;
	}
	else
		m_pTargetMonster = pMonster;
}

//void CKena::Call_FocusIcon(CGameObject * pTarget)
//{
//	Call_FocusRotIcon(pTarget);
//	Call_FocusMonsterIcon(pTarget);
//}

void CKena::Call_FocusRotIcon(CGameObject * pTarget)
{
	if (m_pUI_FocusRot == nullptr)
		return;

	/* This Action needs Pip */
	if (0 == m_pKenaStatus->Get_CurPIPCount())
	{
		m_pUI_FocusRot->Set_Pos(nullptr);
		return;
	}

	if (pTarget != nullptr)
	{
		m_bRotActionPossible = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Down(DIK_R))	
		{
			/* UI Control */
			m_pKenaStatus->Set_CurPIPGuage(m_pKenaStatus->Get_CurPIPGuage() - 1.f);
			m_pKenaStatus->Set_RotState(CKena_Status::RS_ACTIVE);
			CUI_ClientManager::UI_PRESENT ePip = CUI_ClientManager::HUD_PIP;
			CUI_ClientManager::UI_PRESENT eRot = CUI_ClientManager::HUD_ROT;
			//CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
			_float fZero = 0.f;
			//m_PlayerDelegator.broadcast(ePip, funcDefault, fZero);
			m_Delegator.broadcast(ePip, fZero);
			_float fRot = 2;
			//m_PlayerDelegator.broadcast(eRot, funcDefault, fRot);
			m_Delegator.broadcast(eRot, fRot);
			/* ~UI Control */

			static_cast<CMonster*>(pTarget)->Bind(m_pRotForMonster, 8);
			//static_cast<CMonster*>(pTarget)->Setting_Rot();
			m_pRotForMonster[0]->Set_Target(static_cast<CMonster*>(pTarget));

			PlaySound_Rot_Action_Combat_Voice();
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	m_pUI_FocusRot->Set_Pos(pTarget, static_cast<CMonster*>(pTarget)->Get_RotIconPosOffset());
}


void CKena::Call_FocusRotIconFlower(CGameObject* pTarget)
{
	if (m_pUI_FocusRot == nullptr)
		return;

	if (pTarget == nullptr)
		m_pUI_FocusRot->Off_Focus(nullptr);

	/* This Action needs Pip */
	if (0 == m_pKenaStatus->Get_CurPIPCount())
	{
		m_pUI_FocusRot->Set_Pos(nullptr);
		return;
	}

	m_pUI_FocusRot->Set_Pos(pTarget, _float4(0.f, 0.5f, 0.f, 0.f));
}

void CKena::Call_FocusMonsterIcon(CGameObject * pTarget)
{
	if (m_pUI_FocusMonster == nullptr)
		return;

	m_pUI_FocusMonster->Set_Pos(m_pTargetMonster);

	if (m_pTargetMonster != pTarget)
		m_pUI_FocusMonster->Start_Animation();
}

void CKena::Dead_FocusRotIcon(CGameObject* pTarget)
{
	if (m_pUI_FocusRot == nullptr || pTarget == nullptr) return;

	m_pUI_FocusRot->Off_Focus(pTarget);
}

bool CKena::NanCheck()
{
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	if (isnan(XMVectorGetX(matWorld.r[0])) || isnan(XMVectorGetX(matWorld.r[1])) || isnan(XMVectorGetX(matWorld.r[2])))
		return true;

	if (XMVectorGetX(XMVector4Length(matWorld.r[0])) == 0.f ||
		XMVectorGetX(XMVector4Length(matWorld.r[1])) == 0.f ||
		XMVectorGetX(XMVector4Length(matWorld.r[2])) == 0.f)
		return true;

	return false;
}

void CKena::RimColorValue()
{
	// Set rim lighting variables based on attack type
	if (m_bParryLaunch)
	{
		m_bParryRim = true;
		m_fParryRimIntensity = 1.f;
	}

	// Update rim lighting variables and send them to the shader
	{
		if (m_fParryRimIntensity > 0.f)
			m_fParryRimIntensity -= TIMEDELTA;
		else
		{
			m_fParryRimIntensity = 0.f;
			m_bParryRim = false;
		}
			
		m_pShaderCom->Set_RawValue("g_Parry", &m_bParryRim, sizeof(_bool));
		m_pShaderCom->Set_RawValue("g_ParryRimIntensity", &m_fParryRimIntensity, sizeof(_float));
	}
	
	{
		if (m_fHitRimIntensity > 0.f)
			m_fHitRimIntensity -= TIMEDELTA;
		else
		{
			m_fHitRimIntensity = 0.f;
			m_bHitRim = false;
		}
		m_pShaderCom->Set_RawValue("g_Hit", &m_bHitRim, sizeof(_bool));
		m_pShaderCom->Set_RawValue("g_HitRimIntensity", &m_fHitRimIntensity, sizeof(_float));
	}
	// DashAttack Rim
	{
		if (m_fDashRimIntensity > 0.f)
			m_fDashRimIntensity -= TIMEDELTA;
		else
		{
			m_fDashRimIntensity = 0.f;
			m_bDashRim = false;
		}
		m_pShaderCom->Set_RawValue("g_Dash", &m_bDashRim, sizeof(_bool));
		m_pShaderCom->Set_RawValue("g_DashRimIntensity", &m_fDashRimIntensity, sizeof(_float));
	}
	// LevelUp Rim 
	{	
		if (m_fLevelUpRimIntensity > 0.f)
			m_fLevelUpRimIntensity -= TIMEDELTA;
		else
		{
			m_fLevelUpRimIntensity = 0.f;
			m_bLevelUpRim = false;
		}
		m_pShaderCom->Set_RawValue("g_LevelUp", &m_bLevelUpRim, sizeof(_bool));
		m_pShaderCom->Set_RawValue("g_LevelUpRimIntensity", &m_fLevelUpRimIntensity, sizeof(_float));
	}
}

HRESULT CKena::Ready_Parts()
{
	CKena_Parts*	pPart = nullptr; 
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CKena_Parts::KENAPARTS_DESC	PartDesc;

	/* Staff */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_STAFF;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_Staff", L"Kena_Staff", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(g_LEVEL, pPart);
	m_pAnimation->Add_AnimSharingPart(dynamic_cast<CModel*>(pPart->Find_Component(L"Com_Model")), false);

	/* MainOutfit */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_OUTFIT;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_MainOutfit", L"Kena_MainOutfit", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(g_LEVEL, pPart);
	m_pAnimation->Add_AnimSharingPart(dynamic_cast<CModel*>(pPart->Find_Component(L"Com_Model")), true);

	/* Taro Mask */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_MASK;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_Taro_Mask", L"Kena_Mask", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CKena::Ready_Arrows()
{
	//_uint		iArrowCount = m_pKenaStatus->Get_MaxArrowCount();
	_tchar*	pTag = nullptr;
	CSpiritArrow*	pArrow = nullptr;

	for (_uint i = 0; i < 8; ++i)
	{
		pTag = CUtile::Create_DummyString(L"SpiritArrow", i);

		pArrow = dynamic_cast<CSpiritArrow*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_SpiritArrow", pTag, nullptr));
		pArrow->Late_Initialize(nullptr);
		pArrow->Reset();
		m_vecArrow.push_back(pArrow);
	}

	return S_OK;
}

HRESULT CKena::Ready_Bombs()
{
	//_uint		iBombCount = m_pKenaStatus->Get_MaxBombCount();
	_tchar*	pTag = nullptr;
	CRotBomb*	pBomb = nullptr;

	for (_uint i = 0; i < 4; ++i)
	{
		pTag = CUtile::Create_DummyString(L"RotBomb", i);

		pBomb = dynamic_cast<CRotBomb*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Rot_Bomb", pTag, nullptr));
		CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, pBomb);
		pBomb->Late_Initialize(nullptr);
		pBomb->Reset();
		m_vecBomb.push_back(pBomb);
	}

	return S_OK;
}

HRESULT CKena::Ready_Effects() 
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base*	pEffectBase = nullptr;
	_tchar*			pCloneTag = nullptr;

	/* Pulse */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulse", L"KenaPulse"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaPulse", pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaParry", L"KenaPulseParry"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaPulseParry", pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaParryHand", L"KenaPulseParryHand"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaPulseParryHand", pEffectBase);

	/* Damage */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaDamage", L"Damage"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaDamage", pEffectBase);

	/* PulseJump */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaJump", L"PulseJump"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaJump", pEffectBase);

	/* FootStep Dust */
	string		strMapTag = "";
	for (_uint i = 0; i < 6; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"Effect_Kena_FootStep", i);
		strMapTag = "KenaFootStepDust_" + to_string(i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaDust", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		m_mapEffect.emplace(strMapTag, pEffectBase);
	}
	
	/* HeavyAttack_Into */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaHeavyAttackInto", L"HeavyAttackInto"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("HeavyAttackInto", pEffectBase);

	/* InteractStaff */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_InteractStaff", L"InteractStaff"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("InteractStaff", pEffectBase);

	/* Particle  */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"Kena_Particle"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	dynamic_cast<CE_P_ExplosionGravity*>(pEffectBase)->Set_Option(CE_P_ExplosionGravity::TYPE_KENA_ATTACK);
	m_mapEffect.emplace("Kena_Particle", pEffectBase);

	/* KenaDash  */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_DashCloud", L"KenaDash"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaDash", pEffectBase);

	/* Distortion Sphere  */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_DistortionSphere", L"K_D_Sphere"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("K_D_Sphere", pEffectBase);

	/* LevelUp */
	string		strLevelUp = "";
	for (_uint i = 0; i < 2; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"K_LevelUp", i);
		strLevelUp = "K_LevelUp" + to_string(i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_LevelUp", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		m_mapEffect.emplace(strLevelUp, pEffectBase);
	}

	/* KenaLvUp_RiseY  */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaLevel_RiseY", L"KenaLvUp_RiseY"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaLvUp_RiseY", pEffectBase);

	/* KenaLvUp_Floor  */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaLevel_Floor", L"KenaLvUp_Floor"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_mapEffect.emplace("KenaLvUp_Floor", pEffectBase);

	for (auto& pEffects : m_mapEffect)
		pEffects.second->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CKena::Ready_Lights()
{
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	CBone* pBone = m_pModelCom->Get_BonePtr("bow_string_jnt_top");
	NULL_CHECK_RETURN(pBone, E_FAIL);
	_matrix			SocketMatrix =	pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() *	m_pModelCom->Get_PivotMatrix() * 	m_pTransformCom->Get_WorldMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 pivotMatrix;
	XMStoreFloat4x4(&pivotMatrix, SocketMatrix);
	_float4 vPos = _float4(pivotMatrix._41, pivotMatrix._42, pivotMatrix._43, 1.f);
	m_vStaffLightPos = vPos;
	LightDesc.vPosition = m_vStaffLightPos;
	LightDesc.fRange = 3.0f;
	LightDesc.vDiffuse = _float4(0.f, 0.f, 100.f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 100.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 100.f, 0.f);
	LightDesc.szLightName = "Kena_Staff_PointLight";

	if (FAILED(CGameInstance::GetInstance()->Add_Light(m_pDevice, m_pContext, LightDesc, &m_pStaffLight)))
		return E_FAIL;

	return S_OK;
}

HRESULT CKena::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Kena", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	//For.Cloth			
	// AO_R_M
	m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_AO_R_M.png"));
	// EMISSIVE
	m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_EMISSIVE.png"));
	// EMISSIVE_MASK
	m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVEMASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_EMISSIVE_MASK.png"));
	// MASK
	m_pModelCom->SetUp_Material(1, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_MASK.png"));
	// SPRINT_EMISSIVE
	m_pModelCom->SetUp_Material(1, WJTextureType_SPRINT_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_sprint_EMISSIVE.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(1, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_SSS_MASK.png"));
	// Detail_Normal
	m_pModelCom->SetUp_Material(1, WJTextureType_HAIR_DEPTH, TEXT("../Bin/Resources/Anim/Kena/PostProcess/T_FabricDetailNormal.png"));

	// AO_R_M
	m_pModelCom->SetUp_Material(5, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_AO_R_M.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(5, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_SSS_MASK.png"));

	// AO_R_M
	m_pModelCom->SetUp_Material(6, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_AO_R_M.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(6, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_SSS_MASK.png"));

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachine, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_KenaStatus", L"Com_Status", (CComponent**)&m_pKenaStatus, nullptr, this), E_FAIL);
	m_pKenaStatus->Load("../Bin/Data/Status/Kena_Status.json");

	return S_OK;
}

HRESULT CKena::Ready_Rots()
{
	CRot::Clear();
	_int iDevide = 8 - m_pKenaStatus->Get_RotCount();

	for (_uint i = 0; i < 8; ++i)
	{
		_tchar szCloneRotTag[32] = { 0, };
		swprintf_s(szCloneRotTag, L"PlayerRot_%d", i);
		CGameObject* p_game_object = nullptr;
		CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL, L"Layer_Rot", L"Prototype_GameObject_Rot", CUtile::Create_StringAuto(szCloneRotTag), nullptr, &p_game_object);
		dynamic_cast<CRot*>(p_game_object)->AlreadyRot();

		if (i == FIRST_ROT)
		{
			Set_FirstRotPtr((CRot*)p_game_object);
			CRot::Set_RotUseKenaPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			if (m_pCamera_Photo)
				m_pCamera_Photo->Set_KenaPtr(this);
		}
	}

	for (_uint i = 0; i < iDevide; ++i)
		m_pKenaStatus->Add_RotCount();

	return S_OK;
}

HRESULT CKena::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	m_pShaderCom->Set_RawValue("g_fSSSAmount", &m_fSSSAmount, sizeof(float));
	m_pShaderCom->Set_RawValue("g_vSSSColor", &m_vSSSColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vAmbientColor", &m_vMulAmbientColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vAmbientEyeColor", &m_vEyeAmbientColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_fLashDensity", &m_fLashDensity, sizeof(float));
	m_pShaderCom->Set_RawValue("g_fLashWidth", &m_fLashWidth, sizeof(float));
	m_pShaderCom->Set_RawValue("g_fLashIntensity", &m_fLashIntensity, sizeof(float));

	RimColorValue();

	return S_OK;
}

HRESULT CKena::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

HRESULT CKena::SetUp_CineShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_CINEVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

HRESULT CKena::SetUp_State()
{
	m_pModelCom->Set_RootBone("kena_RIG");
	m_pModelCom->Set_BoneIndex(L"../Bin/Data/Animation/Kena BoneInfo.json");
	m_pAnimation = CAnimationState::Create(this, m_pModelCom, "kena_RIG", "../Bin/Data/Animation/Kena.json");
	FAILED_CHECK_RETURN(m_pAnimation->Load_Additional_Animations("../Bin/Data/Animation/Kena Look Animations.json"), E_FAIL);
	m_pAnimation->Connect_AdditiveController(this, &CKena::Update_AdditiveRatio);
	m_pAnimation->Set_RootAnimation("IDLE");

	return S_OK;
}

HRESULT CKena::SetUp_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_UI",
		TEXT("Prototype_GameObject_UI_RotFocuss"),
		L"Clone_RotFocus", nullptr, (CGameObject**)&m_pUI_FocusRot)))
	{
		MSG_BOX("Failed To make UI : Kena");
		return E_FAIL;
	}
	else
		m_pUI_FocusRot->Set_Player(this);

	if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_UI",
		TEXT("Prototype_GameObject_UI_FocusMonster"),
		L"Clone_MonsterFocus", nullptr, (CGameObject**)&m_pUI_FocusMonster)))
	{
		MSG_BOX("Failed To make UI : Kena");
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CKena::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);

	CKena_Staff*	pStaff = dynamic_cast<CKena_Staff*>(Get_KenaPart(L"Kena_Staff"));
	CModel*			pStaffModel = dynamic_cast<CModel*>(pStaff->Find_Component(L"Com_Model"));
	CBone*			pStaffRootJnt = pStaffModel->Get_BonePtr("staff_skin6_jnt");
	_matrix			matSocket = pStaffRootJnt->Get_OffsetMatrix() * pStaffRootJnt->Get_CombindMatrix() * pStaffModel->Get_PivotMatrix();
	
	matSocket.r[0] = XMVector3Normalize(matSocket.r[0]);
	matSocket.r[1] = XMVector3Normalize(matSocket.r[1]);
	matSocket.r[2] = XMVector3Normalize(matSocket.r[2]);

	_smatrix	matPivot =  XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(0.f, 0.015f, -1.04f);
	matPivot = matPivot * matSocket;

	m_pTransformCom->Update_Collider(pStaff->Get_ObjectCloneName(), matPivot);

	matPivot = XMMatrixTranslation(0.f, 0.7f, 0.f);
	m_pTransformCom->Update_Collider(L"Kena_Bump", matPivot);
}

void CKena::Update_LightPos(_float fTimeDelta)
{
	CBone* pBone = m_pModelCom->Get_BonePtr("bow_string_jnt_top");
	NULL_CHECK_RETURN(pBone, );
	_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	_float4x4 pivotMatrix;
	XMStoreFloat4x4(&pivotMatrix, SocketMatrix);
	_float4 vPos = _float4(pivotMatrix._41, pivotMatrix._42, pivotMatrix._43, 1.f);
	_float fPivotPos[3] = { m_vStaffLightPos.x,m_vStaffLightPos.y, m_vStaffLightPos.z };
	ImGui::DragFloat3("StaffLightPivot", fPivotPos,0.01f, -10.f, 10.f);
	m_vStaffLightPos = _float4(fPivotPos[0], fPivotPos[1], fPivotPos[2], 0.f);
	vPos += m_vStaffLightPos;
	m_pStaffLight->Set_Position(vPos);
}

_float CKena::Update_TimeRate()
{
	if (m_bAim && m_bJump)
		CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.3f);
	else
	{
		if (m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::PULSE_PARRY &&
			m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::BOW_INJECT_ADD &&
			m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::BOMB_INJECT_ADD &&
			m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::SHIELD_BREAK_FRONT &&
			m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::SHIELD_BREAK_BACK)
			CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
	}

	return CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
}

void CKena::Check_Damaged()
{
	if (m_bParry == true)
	{
		if (m_iCurParryFrame == m_iParryFrameCount)
		{
			m_bCommonHit = true;
			//m_bHeavyHit = true;
			m_eDamagedDir = Calc_DirToMonster(m_pAttackObject);

			if (m_bPulse == false)
			{
				m_bHitRim = true;
				m_fHitRimIntensity = 1.f;

				if (m_bDeath = m_pKenaStatus->UnderAttack(((CMonster*)m_pAttackObject)->Get_MonsterStatusPtr()))
					m_eDamagedDir = Calc_DirToMonster_2Way(m_pAttackObject);

				/* JH : HP�������� �׳� �������� �������ַ��� Tick���� ����. */
// 				CUI_ClientManager::UI_PRESENT eHP = CUI_ClientManager::HUD_HP;
// 				_float fGuage = m_pKenaStatus->Get_PercentHP();
// 				m_Delegator.broadcast(eHP, fGuage);

				CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::TOP_MOOD_HIT;
				_float fDefault = 1.f;
				m_Delegator.broadcast(tag, fDefault);
			}

			m_bParry = false;
			m_pAttackObject = nullptr;
		}
	}
}

void CKena::Update_State_Status(_float fTimeDelta)
{
	if (m_pAnimation->Get_Preview() == false)
	{
		m_pKenaState->Tick(fTimeDelta);

		if (m_bStateLock == false)
			m_pStateMachine->Tick(fTimeDelta);
		else
			m_pAnimation->State_Animation("IDLE");

		if (m_bAim == true)
			m_pAnimation->Tick(fTimeDelta);
	}

	m_pKenaStatus->Tick(fTimeDelta);

	m_bCommonHit = false;
	m_bHeavyHit = false;
}

void CKena::Update_AdditiveRatio(_float fTimeDelta)
{
	_float	fRatio = LookAnimationController(fTimeDelta);

	if (fRatio < 0.f)
	{
		if (m_bBow == true)
			m_pAnimation->State_AdditionalAnimation("BOW_AIM_DOWN", fRatio * -1.f);
		else
			m_pAnimation->State_AdditionalAnimation("AIM_LOOK_DOWN", fRatio * -2.f);
	}
	else
	{
		if (m_bBow == true)
			m_pAnimation->State_AdditionalAnimation("BOW_AIM_UP", fRatio);
		else
			m_pAnimation->State_AdditionalAnimation("AIM_LOOK_UP", fRatio * 2.f);
	}

	return;
}

void CKena::Check_TimeRate_Changed(_float& fTimeDelta, _float fTimeRate)
{
	if (fTimeRate != CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60"))
	{
		fTimeDelta /= fTimeRate;
		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
		fTimeDelta *= fTimeRate;
	}
}

void CKena::Play_Animation(_float fTimeDelta, _float fTimeRate)
{
	if (m_pModelCom->Get_Preview() == false)
	{
		if (m_fHitStopTime <= 0.f)
		{
			if (m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::PULSE_PARRY &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::BOW_INJECT_ADD &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::BOMB_INJECT_ADD &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::SHIELD_BREAK_FRONT &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::SHIELD_BREAK_BACK &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::WARRIOR_GRAB)
				m_pAnimation->Play_Animation(fTimeDelta / fTimeRate);
			else
				m_pAnimation->Play_Animation(fTimeDelta);
		}
		else
		{
			m_fHitStopTime -= fTimeDelta / fTimeRate;
			CUtile::Saturate<_float>(m_fHitStopTime, 0.f, 3.f);
		}
	}
	else
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CKena::Push_WeaponPosition()
{
	if (m_bAttack)
	{
		CBone* pBone = m_pModelCom->Get_BonePtr("staff_skin9_jnt");
		_matrix	matSocket = pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

		if (m_vecWeaposPos.size() < 2)
			m_vecWeaposPos.push_back(matSocket.r[3]);
		else
		{
			m_vecWeaposPos.front() = m_vecWeaposPos.back();
			m_vecWeaposPos.pop_back();
			m_vecWeaposPos.push_back(matSocket.r[3]);
		}
	}
	else
		m_vecWeaposPos.clear();
}

void CKena::Run_Smooth_Targeting(_float fTimeDelta)
{
	if (m_pTargetMonster && m_bAttack && m_pKenaState->Get_CurrentDirection() == CTransform::DIR_END &&
		m_pTransformCom->Calc_InRange(XMConvertToRadians(120.f), m_pTargetMonster->Get_TransformCom()) == true)
	{
		_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector	vTargetPos = m_pTargetMonster->Get_Position();
		_vector	vDir = vTargetPos - vPos;
		_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

		if (!m_bJump && !m_bSprint)
		{
			if (fDistance > 1.f)
			{
				vTargetPos = vTargetPos - XMVector3Normalize(vDir) * 0.5f;
				vDir = vTargetPos - vPos;

				m_pTransformCom->Set_Speed(7.f);
				m_pTransformCom->LookAt_NoUpDown(vTargetPos);
				m_pTransformCom->Go_DirectionNoY(vDir, fTimeDelta);
				m_bLocalMoveLock = false;
			}
			else
			{
				m_pTransformCom->Set_Speed(5.f);
				m_pTransformCom->LookAt_NoUpDown(vTargetPos);
				m_bLocalMoveLock = true;
			}
		}
	}
	else
		m_bLocalMoveLock = false;
}

void CKena::Check_Smooth_Targeting()
{
	if (m_pTargetMonster != nullptr)
	{
		CTransform* pCameraTransform = CGameInstance::GetInstance()->Get_WorkCameraPtr()->Get_TransformCom();

		if (m_pTargetMonster->Is_Dead() == true ||
			m_pTransformCom->Calc_Distance_XZ(m_pTargetMonster->Get_TransformCom()) > 5.f ||
			pCameraTransform->Calc_InRange(XMConvertToRadians(120.f), m_pTargetMonster->Get_TransformCom()) == false)
			m_pTargetMonster = nullptr;
	}

	Call_FocusMonsterIcon(m_pTargetMonster);
}

_float CKena::LookAnimationController(_float fTimeDelta)
{
	_float	fAdditiveRatio = 0.f;

	_float	fVerticalAngle = XMConvertToDegrees(m_pCamera->Get_VerticalAngle());

	if (fVerticalAngle < 90.f)
		fAdditiveRatio = 1.f - (fVerticalAngle / 90.f);
	else if (fVerticalAngle > 90.f && fVerticalAngle < 180.f)
		fAdditiveRatio = (fVerticalAngle - 90.f) / 90.f * -1.f;

	CUtile::Saturate<_float>(fAdditiveRatio, -1.f, 1.f);

	return fAdditiveRatio;
}

CKena::DAMAGED_FROM CKena::Calc_DirToMonster(CGameObject * pTarget)
{
	DAMAGED_FROM		eDir = DAMAGED_FROM_END;

	if (pTarget == nullptr) return eDir;

	CTransform*	pTargetTransCom = pTarget->Get_TransformCom();
	_float4		vDir = pTargetTransCom->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vDir.Normalize();

	_float			fFrontBackAngle = vDir.Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	_float			fLeftRightAngle = vDir.Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));;

	if (isnan(fFrontBackAngle) || isnan(fLeftRightAngle))
		return CKena::DAMAGED_FROM_END;

	if (fFrontBackAngle <= 0.f)
	{
		if (fLeftRightAngle >= -0.5f && fLeftRightAngle <= 0.5f)
			eDir = DAMAGED_FRONT;
		else if (fLeftRightAngle >= -1.f && fLeftRightAngle <= -0.5f)
			eDir = DAMAGED_RIGHT;
		else if (fLeftRightAngle >= 0.5f && fLeftRightAngle <= 1.f)
			eDir = DAMAGED_LEFT;
	}
	else
	{
		if (fLeftRightAngle >= -0.5f && fLeftRightAngle <= 0.5f)
			eDir = DAMAGED_BACK;
		else	if (fLeftRightAngle >= -1.f && fLeftRightAngle <= -0.5f)
			eDir = DAMAGED_RIGHT;
		else if (fLeftRightAngle >= 0.5f && fLeftRightAngle <= 1.f)
			eDir = DAMAGED_LEFT;
	}

	return eDir;
}

CKena::DAMAGED_FROM CKena::Calc_DirToMonster(const _float3 & vCollisionPos)
{
	DAMAGED_FROM		eDir = DAMAGED_FROM_END;

	_float4		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4		vTargetPos = vCollisionPos;
	vTargetPos.w = 1.f;
	_float4		vDir = vTargetPos - vPos;
	vDir.Normalize();

	_float			fFrontBackAngle = vDir.Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	_float			fLeftRightAngle = vDir.Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));;

	if (isnan(fFrontBackAngle) || isnan(fLeftRightAngle))
		return CKena::DAMAGED_FROM_END;

	if (fFrontBackAngle <= 0.f)
	{
		if (fLeftRightAngle >= -0.5f && fLeftRightAngle <= 0.5f)
			eDir = DAMAGED_FRONT;
		else if (fLeftRightAngle >= -1.f && fLeftRightAngle <= -0.5f)
			eDir = DAMAGED_RIGHT;
		else if (fLeftRightAngle >= 0.5f && fLeftRightAngle <= 1.f)
			eDir = DAMAGED_LEFT;
	}
	else
	{
		if (fLeftRightAngle >= -0.5f && fLeftRightAngle <= 0.5f)
			eDir = DAMAGED_BACK;
		else	if (fLeftRightAngle >= -1.f && fLeftRightAngle <= -0.5f)
			eDir = DAMAGED_RIGHT;
		else if (fLeftRightAngle >= 0.5f && fLeftRightAngle <= 1.f)
			eDir = DAMAGED_LEFT;
	}

	return eDir;
}

CKena:: DAMAGED_FROM CKena::Calc_DirToMonster_2Way(CGameObject* pTarget)
{
	DAMAGED_FROM	eDir = CKena::DAMAGED_FROM_END;

	CTransform* pTargetTransCom = pTarget->Get_TransformCom();
	_float4    vDir = pTargetTransCom->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vDir.Normalize();

	_float	fFrontBackAngle = vDir.Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	if (fFrontBackAngle >= 0.f)
		eDir = CKena::DAMAGED_FRONT;
	else
		eDir = CKena::DAMAGED_BACK;

	return eDir;
}

void CKena::UnderAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::UnderAttack);
		return;
	}

	if (m_pAttackObject == nullptr)
		return;

	m_bCommonHit = true;
	m_bHitRim = true;
	m_fHitRimIntensity = 1.f;

	CMonster_Status*	pStatus = dynamic_cast<CMonster*>(m_pAttackObject)->Get_MonsterStatusPtr();
	if (m_bDeath = m_pKenaStatus->UnderAttack(pStatus))
		m_eDamagedDir = Calc_DirToMonster_2Way(m_pAttackObject);
}

void CKena::Grab_CameraShake(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::Grab_CameraShake);
		return;
	}

	CTransform* pCamTransform = m_pCamera->Get_TransformCom();
	_vector	vNegRight = pCamTransform->Get_State(CTransform::STATE_RIGHT) * -1.f;
	_vector	vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE_LOOK));

	_vector	vDir = XMVector3TransformNormal(vNegRight, XMMatrixRotationAxis(vLook, XMConvertToRadians(-25.f)));

	m_pCamera->Camera_Shake(vDir, XMConvertToRadians(80.f));
	m_pCamera->Camera_Shake(0.008f, 30);

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.2f);
	Add_HitStopTime(0.25f);
	m_pRendererCom->Set_MotionBlur(true);
}

void CKena::TurnOnAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnAttack);
		return;
	}

	m_bAttack = true;
}

void CKena::TurnOffAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffAttack);
		return;
	}

	m_bAttack = false;
}

void CKena::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnTrail);
		return;
	}

	m_bTrailON = true;
}

void CKena::TurnOffTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffTrail);
		return;
	}

	m_bTrailON = false;
}

void CKena::TurnOnFootStep_Left(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnFootStep_Left);
		return;
	}

	for (auto& Pair : m_mapEffect)
	{
		if (dynamic_cast<CE_KenaDust*>(Pair.second))
		{
			if (Pair.second->Get_Active() == false)
			{
				/* ToeDust Update */
				CBone*   pToeBonePtr = m_pModelCom->Get_BonePtr("kena_lf_toe_jnt");
				_matrix SocketMatrix = pToeBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
				_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();
				_matrix matWalk = Pair.second->Get_TransformCom()->Get_WorldMatrix();

				_float4 vFootPos = matWorldSocket.r[3];
				vFootPos.y = vFootPos.y + 0.3f;
				matWalk.r[3] = vFootPos;
				Pair.second->Get_TransformCom()->Set_WorldMatrix(matWalk);
				/* ToeDust Update */

				Pair.second->Set_Active(true);
				break;
			}
		}
	}
}

void CKena::TurnOnFootStep_Right(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnFootStep_Right);
		return;
	}

	for (auto& Pair : m_mapEffect)
	{
		if (dynamic_cast<CE_KenaDust*>(Pair.second))
		{
			if (Pair.second->Get_Active() == false)
			{
				/* ToeDust Update */
				CBone*   pToeBonePtr = m_pModelCom->Get_BonePtr("kena_rt_toe_jnt");
				_matrix SocketMatrix = pToeBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
				_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();
				_matrix matWalk = Pair.second->Get_TransformCom()->Get_WorldMatrix();
				_float4 vFootPos = matWorldSocket.r[3];
				vFootPos.y = vFootPos.y + 0.3f;
				matWalk.r[3] = vFootPos;
				Pair.second->Get_TransformCom()->Set_WorldMatrix(matWalk);
				/* ToeDust Update */

				Pair.second->Set_Active(true);
				break;
			}
		}
	}

}

void CKena::TurnOnCharge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnCharge);
		return;
	}

	m_bChargeLight = true;
}

void CKena::TurnOffCharge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffCharge);
		return;
	}

	m_bChargeLight = false;
}

void CKena::TurnOnPulseJump(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnPulseJump);
		return;
	}
	m_mapEffect["KenaJump"]->Set_Active(true);

	/* JumpPos Update */
	CBone*	pStaffBonePtr = m_pModelCom->Get_BonePtr("kena_lf_toe_jnt");
	_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();
	_matrix matrJump = m_mapEffect["KenaJump"]->Get_TransformCom()->Get_WorldMatrix();
	matrJump.r[3] = matWorldSocket.r[3];
	m_mapEffect["KenaJump"]->Get_TransformCom()->Set_WorldMatrix(matrJump);
	/* JumpPos Update */
}

void CKena::TurnOnHeavyAttack_Into(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnHeavyAttack_Into);
		return;
	}
	/* IntoAttack Update */
	CBone*	pStaffBonePtr = m_pModelCom->Get_BonePtr("staff_skin8_jnt");
	_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	_matrix matIntoAttack = m_mapEffect["HeavyAttackInto"]->Get_TransformCom()->Get_WorldMatrix();
	matIntoAttack.r[3] = matWorldSocket.r[3];
	m_mapEffect["HeavyAttackInto"]->Get_TransformCom()->Set_WorldMatrix(matIntoAttack);
	/* IntoAttack Update */
	m_mapEffect["HeavyAttackInto"]->Set_Active(true);

	CE_P_ExplosionGravity* pParticle = dynamic_cast<CE_P_ExplosionGravity*>(m_mapEffect["Kena_Particle"]);
	pParticle->UpdateParticle(matWorldSocket.r[3]);
}

void CKena::TurnOnInteractStaff(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnInteractStaff);
		return;
	}

	CBone* pStaffBonePtr = m_pModelCom->Get_BonePtr("staff_skin2_jnt");
	_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	_matrix matIntoAttack = m_mapEffect["InteractStaff"]->Get_TransformCom()->Get_WorldMatrix();
	matIntoAttack.r[3] = matWorldSocket.r[3];
	matIntoAttack.r[3] = XMVectorSetY(matIntoAttack.r[3], XMVectorGetY(matIntoAttack.r[3]) + 0.2f);
	m_mapEffect["InteractStaff"]->Get_TransformCom()->Set_WorldMatrix(matIntoAttack);
	m_mapEffect["InteractStaff"]->Set_Active(true);
}

void CKena::TurnOnPulseParry(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnPulseParry);
		return;
	}
	CBone*	pBodyBonePtr = m_pModelCom->Get_BonePtr("winterCape_mid4_jnt");
	_matrix SocketMatrix = pBodyBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	_matrix matIntoAttack = m_mapEffect["KenaPulseParry"]->Get_TransformCom()->Get_WorldMatrix();
	matIntoAttack.r[3] = matWorldSocket.r[3];
	m_mapEffect["KenaPulseParry"]->Get_TransformCom()->Set_WorldMatrix(matIntoAttack);
	m_mapEffect["KenaPulseParry"]->Set_Active(true);
}

void CKena::TurnOnPulseParryHand(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnPulseParryHand);
		return;
	}

	CBone*	pBodyBonePtr = m_pModelCom->Get_BonePtr("lf_hand_socket_jnt");
	_matrix SocketMatrix = pBodyBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	_matrix matIntoAttack = m_mapEffect["KenaPulseParryHand"]->Get_TransformCom()->Get_WorldMatrix();
	matIntoAttack.r[3] = matWorldSocket.r[3];
	m_mapEffect["KenaPulseParryHand"]->Get_TransformCom()->Set_WorldMatrix(matIntoAttack);
	m_mapEffect["KenaPulseParryHand"]->Set_Active(true);

	CE_P_ExplosionGravity* pParticle = dynamic_cast<CE_P_ExplosionGravity*>(m_mapEffect["Kena_Particle"]);
	pParticle->UpdateParticle(matWorldSocket.r[3]);
}

void CKena::TurnOnPulseParryRange(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnPulseParryRange);
		return;
	}
	dynamic_cast<CE_KenaPulse*>(m_mapEffect["KenaPulse"])->Set_Type(CE_KenaPulse::PULSE_PARRY);

	CBone*	pBodyBonePtr = m_pModelCom->Get_BonePtr("lf_hand_socket_jnt");
	_matrix SocketMatrix = pBodyBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	_matrix matIntoAttack = m_mapEffect["KenaPulse"]->Get_TransformCom()->Get_WorldMatrix();
	matIntoAttack.r[3] = matWorldSocket.r[3];
	m_mapEffect["KenaPulse"]->Get_TransformCom()->Set_WorldMatrix(matIntoAttack);
	m_mapEffect["KenaPulse"]->Set_Active(true);
}

void CKena::TurnOnDashSt(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnDashSt);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_Position();
	vPos.y += 1.f;

	m_mapEffect["KenaDamage"]->Set_Effect(vPos, true);
}

void CKena::TurnOnDashLp(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnDashLp);
		return;
	}

	m_mapEffect["K_D_Sphere"]->Set_Scale(_float3(3.f, 3.f, 3.f));
	m_mapEffect["K_D_Sphere"]->Set_Effect(m_pTransformCom->Get_Position(), true);
}

void CKena::TurnOffDashLp(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffDashLp);
		return;
	}

	m_mapEffect["K_D_Sphere"]->Set_Active(false);
}

void CKena::TurnOnDashEd(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnDashEd);
		return;
	}

	_float4 vPos = m_pTransformCom->Get_Position();
	dynamic_cast<CE_KenaDash*>(m_mapEffect["KenaDash"])->Tick_State(vPos);
}

void CKena::TurnOnLvUp(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnLvUp);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_Position();
	vPos.y += 1.f;

	m_mapEffect["K_LevelUp0"]->Set_Effect(vPos, true);
	m_mapEffect["K_LevelUp1"]->Set_Effect(vPos, true);
}

void CKena::TurnOnLvUp_Part1_Floor(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnLvUp_Part1_Floor);
		return;
	}	
	_float4 vPos = m_pTransformCom->Get_Position();
	m_mapEffect["KenaLvUp_Floor"]->Set_Effect(vPos, true);
}

void CKena::TurnOnLvUp_Part2_RiseY(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnLvUp_Part2_RiseY);
		return;
	}
	_float4 vPos = m_pTransformCom->Get_Position();
	m_mapEffect["KenaLvUp_RiseY"]->Set_Effect(vPos, true);
}

void CKena::TurnOffLvUp_Part2_RiseY(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffLvUp_Part2_RiseY);
		return;
	}
	dynamic_cast<CE_P_Level_RiseY*>(m_mapEffect["KenaLvUp_RiseY"])->Set_Reset(true);
}

void CKena::PlaySound_Kena_FootStep(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Kena_FootStep);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 11.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_00.ogg", 0.1f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_01.ogg", 0.1f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_02.ogg", 0.1f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_03.ogg", 0.1f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_04.ogg", 0.1f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_05.ogg", 0.1f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_06.ogg", 0.1f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_07.ogg", 0.1f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_08.ogg", 0.1f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_09.ogg", 0.1f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_10.ogg", 0.1f, false);
	else if (iRand == 11)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_11.ogg", 0.1f, false);
	else if (iRand == 12)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_12.ogg", 0.1f, false);
}

void CKena::PlaySound_Kena_FootStep_Sprint(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Kena_FootStep_Sprint);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 11.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_00.ogg", 0.3f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_01.ogg", 0.3f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_02.ogg", 0.3f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_03.ogg", 0.3f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_04.ogg", 0.3f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_05.ogg", 0.3f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_06.ogg", 0.3f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_07.ogg", 0.3f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_08.ogg", 0.3f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_09.ogg", 0.3f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_10.ogg", 0.3f, false);
	else if (iRand == 11)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_FootStep_Sprint_11.ogg", 0.3f, false);
}

void CKena::PlaySound_Jump(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Jump);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Jump.ogg", 1.f, false, SOUND_PLAYER_VOICE);
}

void CKena::PlaySound_PulseJump(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_PulseJump);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 4.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_0.ogg", 1.f, false, SOUND_PLAYER_SFX);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_1.ogg", 1.f, false, SOUND_PLAYER_SFX);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_2.ogg", 1.f, false, SOUND_PLAYER_SFX);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_3.ogg", 1.f, false, SOUND_PLAYER_SFX);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_4.ogg", 1.f, false, SOUND_PLAYER_SFX);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_Cloth.ogg", 1.f, false, SOUND_PLAYER_SFX);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_Flaps.ogg", 1.f, false, SOUND_PLAYER_SFX);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_PulseJump_Spin.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_Land(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Land);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Land.ogg", 1.f, false, SOUND_PLAYER_VOICE);
}

void CKena::PlaySound_Dodge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Dodge);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Roll.ogg", 1.f, false);

	if (m_pAnimation->Get_CurrentAnimIndex() == (_uint)CKena_State::BACKFLIP)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Backflip.ogg", 1.f, false);
	else if (m_pAnimation->Get_CurrentAnimIndex() == (_uint)CKena_State::ROLL)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Roll.ogg", 1.f, false);
}

void CKena::PlaySound_Dodge_End(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Dodge_End);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Roll_End.ogg", 1.f, false, SOUND_PLAYER_VOICE);
}

void CKena::PlaySound_Interact_Staff(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Interact_Staff);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Interact_Staff.ogg", 1.f, false);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Interect_Staff_Ring.ogg", 1.f, false);
}

void CKena::PlaySound_Sit(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Sit);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Sit_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Sit_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Sit_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Sit_3.ogg", 1.f, false);
}

void CKena::PlaySound_Rot_Action_Command(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Rot_Action_Command);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 10.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_5.ogg", 1.f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_6.ogg", 1.f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_7.ogg", 1.f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_8.ogg", 1.f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_9.ogg", 1.f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Rot_Action_Command_10.ogg", 1.f, false);
}

void CKena::PlaySound_Rot_Action_Voice(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Rot_Action_Voice);
		return;
	}

	if (m_pRotForMonster[0]->Get_Target() != nullptr)
		return;

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 15.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_5.ogg", 1.f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_6.ogg", 1.f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_7.ogg", 1.f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_8.ogg", 1.f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_9.ogg", 1.f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_10.ogg", 1.f, false);
	else if (iRand == 11)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_11.ogg", 1.f, false);
	else if (iRand == 12)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_12.ogg", 1.f, false);
	else if (iRand == 13)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_13.ogg", 1.f, false);
	else if (iRand == 14)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_14.ogg", 1.f, false);
	else if (iRand == 15)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_15.ogg", 1.f, false);
}

void CKena::PlaySound_Pulse_Intro(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Pulse_Intro);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Ring.ogg", 0.2f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge.ogg", 1.f, false);

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 2.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Bamboo_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Bamboo_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Bamboo_2.ogg", 1.f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Bamboo_Tone.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 1.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Init_Build_Synth_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Init_Build_Synth_1.ogg", 1.f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Charge_Init_Suzu.ogg", 1.f, false);
}

void CKena::PlaySound_Pulse_Outro(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Pulse_Outro);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Outro.ogg", 1.f, false);

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 1.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Outro_Wind_Air_0.ogg", 0.1f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Pulse_Outro_Wind_Air_1.ogg", 0.1f, false);
}

void CKena::PlaySound_Dash(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Dash);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Dash.ogg", 1.f, false);
}

void CKena::PlaySound_Attack_Voice(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Attack_Voice);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 24.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_5.ogg", 1.f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_6.ogg", 1.f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_7.ogg", 1.f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_8.ogg", 1.f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_9.ogg", 1.f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_10.ogg", 1.f, false);
	else if (iRand == 11)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_11.ogg", 1.f, false);
	else if (iRand == 12)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_12.ogg", 1.f, false);
	else if (iRand == 13)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_13.ogg", 1.f, false);
	else if (iRand == 14)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_14.ogg", 1.f, false);
	else if (iRand == 15)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_15.ogg", 1.f, false);
	else if (iRand == 16)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_16.ogg", 1.f, false);
	else if (iRand == 17)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_17.ogg", 1.f, false);
	else if (iRand == 18)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_18.ogg", 1.f, false);
	else if (iRand == 19)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_19.ogg", 1.f, false);
	else if (iRand == 20)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_20.ogg", 1.f, false);
	else if (iRand == 21)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_21.ogg", 1.f, false);
	else if (iRand == 22)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_22.ogg", 1.f, false);
	else if (iRand == 23)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_23.ogg", 1.f, false);
	else if (iRand == 24)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_24.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_1.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_2.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_3.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_4.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 1.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_Tone_1.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_Tone_2.ogg", 1.f, false);
}

void CKena::PlaySound_Attack_1(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Attack_1);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_0.ogg", 1.f, false, SOUND_PLAYER_VOICE);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_1.ogg", 1.f, false, SOUND_PLAYER_SFX);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_Tone_1.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_Attack_2(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Attack_2);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_1.ogg", 1.f, false, SOUND_PLAYER_VOICE);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_2.ogg", 1.f, false, SOUND_PLAYER_SFX);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_Tone_2.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_Attack_3(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Attack_3);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_4.ogg", 1.f, false, SOUND_PLAYER_VOICE);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_3.ogg", 1.f, false, SOUND_PLAYER_SFX);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_Tone_3.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_Attack_4(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_Attack_4);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_16.ogg", 1.f, false, SOUND_PLAYER_VOICE);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_4.ogg", 1.f, false, SOUND_PLAYER_SFX);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Attack_Tone_4.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_HeavyAttack_Charge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Charge);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_3.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 2.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Crickets_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Crickets_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Crickets_2.ogg", 1.f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Weapon.ogg", 0.5f, false);
}

void CKena::PlaySound_HeavyAttack_Charge_Voice(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Charge_Voice);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_3.ogg", 1.f, false);
	
	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_0.ogg", 0.5f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_1.ogg", 0.5f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_2.ogg", 0.5f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_3.ogg", 0.5f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 2.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Crickets_0.ogg", 0.5f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Crickets_1.ogg", 0.5f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Crickets_2.ogg", 0.5f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Charge_Weapon.ogg", 0.5f, false);
}

void CKena::PlaySound_HeavyAttack_Spark(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Spark);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 2.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Spark_0.ogg", 1.f, false, SOUND_PLAYER_SFX);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Spark_1.ogg", 1.f, false, SOUND_PLAYER_SFX);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Spark_2.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_HeavyAttack_Release_Perfect(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Release_Perfect);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Release_Perfect.ogg", 1.f, false, SOUND_PLAYER_SFX);
}

void CKena::PlaySound_HeavyAttack_Staff_Sweep(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Staff_Sweep);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 24.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_5.ogg", 1.f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_6.ogg", 1.f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_7.ogg", 1.f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_8.ogg", 1.f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_9.ogg", 1.f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_10.ogg", 1.f, false);
	else if (iRand == 11)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_11.ogg", 1.f, false);
	else if (iRand == 12)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_12.ogg", 1.f, false);
	else if (iRand == 13)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_13.ogg", 1.f, false);
	else if (iRand == 14)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_14.ogg", 1.f, false);
	else if (iRand == 15)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_15.ogg", 1.f, false);
	else if (iRand == 16)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_16.ogg", 1.f, false);
	else if (iRand == 17)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_17.ogg", 1.f, false);
	else if (iRand == 18)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_18.ogg", 1.f, false);
	else if (iRand == 19)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_19.ogg", 1.f, false);
	else if (iRand == 20)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_20.ogg", 1.f, false);
	else if (iRand == 21)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_21.ogg", 1.f, false);
	else if (iRand == 22)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_22.ogg", 1.f, false);
	else if (iRand == 23)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_23.ogg", 1.f, false);
	else if (iRand == 24)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_24.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 1.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Staff_Sweep_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Staff_Sweep_1.ogg", 1.f, false);
}

void CKena::PlaySound_HeavyAttack_Combo_Charge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Combo_Charge);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Charge_3.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Combo_Charge_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Combo_Charge_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Combo_Charge_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Combo_Charge_3.ogg", 1.f, false);
}

void CKena::PlaySound_HeavyAttack_Combo_Staff_Sweep(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_HeavyAttack_Combo_Staff_Sweep);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 24.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_5.ogg", 1.f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_6.ogg", 1.f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_7.ogg", 1.f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_8.ogg", 1.f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_9.ogg", 1.f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_10.ogg", 1.f, false);
	else if (iRand == 11)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_11.ogg", 1.f, false);
	else if (iRand == 12)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_12.ogg", 1.f, false);
	else if (iRand == 13)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_13.ogg", 1.f, false);
	else if (iRand == 14)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_14.ogg", 1.f, false);
	else if (iRand == 15)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_15.ogg", 1.f, false);
	else if (iRand == 16)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_16.ogg", 1.f, false);
	else if (iRand == 17)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_17.ogg", 1.f, false);
	else if (iRand == 18)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_18.ogg", 1.f, false);
	else if (iRand == 19)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_19.ogg", 1.f, false);
	else if (iRand == 20)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_20.ogg", 1.f, false);
	else if (iRand == 21)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_21.ogg", 1.f, false);
	else if (iRand == 22)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_22.ogg", 1.f, false);
	else if (iRand == 23)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_23.ogg", 1.f, false);
	else if (iRand == 24)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Attack_24.ogg", 1.f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_HeavyAttack_Combo_Sweep.ogg", 1.f, false);
}

void CKena::PlaySound_AirAttack_Slam_Release(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_AirAttack_Slam_Release);
		return;
	}

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 1.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Release_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_HeavyAttack_Release_1.ogg", 1.f, false);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_Base.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 1.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_1.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_Debris_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_Debris_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_Debris_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Slam_Impact_Debris_3.ogg", 1.f, false);
}

void CKena::PlaySound_SpinAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_SpinAttack);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_ParryAttack_Sweep.ogg", 1.f, false);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_ParryAttack_Woong.ogg", 1.f, false);
}

void CKena::PlaySound_WarriorGrab(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::PlaySound_WarriorGrab);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Warrior_Grab.ogg", 1.f, false);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Sit_1.ogg", 0.5f, false);
}

void CKena::PlaySound_Rot_Action_Combat_Voice()
{
	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 10.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_5.ogg", 1.f, false);
	else if (iRand == 6)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_6.ogg", 1.f, false);
	else if (iRand == 7)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_7.ogg", 1.f, false);
	else if (iRand == 8)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_8.ogg", 1.f, false);
	else if (iRand == 9)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_9.ogg", 1.f, false);
	else if (iRand == 10)
		CGameInstance::GetInstance()->Play_Sound(L"VOX_Kena_Rot_Action_Combat_10.ogg", 1.f, false);
}

void CKena::PlaySound_Hit()
{
	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Hit_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Hit_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Hit_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Hit_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Hit_4.ogg", 1.f, false);
}

void CKena::PlaySound_Damage()
{
	CGameInstance::GetInstance()->Play_Sound(L"Voice_Kena_Damage.ogg", 1.f, false);

	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_A1_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_A1_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_A1_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_A1_3.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_M1_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_M1_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_M1_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_M1_3.ogg", 1.f, false);

	iRand = _int(CUtile::Get_RandomFloat(0.f, 3.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_P1_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_P1_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_P1_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_P1_3.ogg", 1.f, false);
}

void CKena::PlaySound_Damage_Pulse()
{
	_int	iRand = _int(CUtile::Get_RandomFloat(0.f, 5.9f));
	if (iRand == 0)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_Pulse_0.ogg", 1.f, false);
	else if (iRand == 1)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_Pulse_1.ogg", 1.f, false);
	else if (iRand == 2)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_Pulse_2.ogg", 1.f, false);
	else if (iRand == 3)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_Pulse_3.ogg", 1.f, false);
	else if (iRand == 4)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_Pulse_4.ogg", 1.f, false);
	else if (iRand == 5)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Damage_Pulse_5.ogg", 1.f, false);
}

CKena * CKena::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKena*	pInstance = new CKena(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CKena::Clone(void * pArg)
{
	CKena*	pInstance = new CKena(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena::Free()
{
	__super::Free();

	for (auto& pPart : m_vecPart)
		Safe_Release(pPart);
	m_vecPart.clear();

	for (auto pArrow : m_vecArrow)
		Safe_Release(pArrow);
	m_vecArrow.clear();

	for (auto pBomb : m_vecBomb)
		Safe_Release(pBomb);
	m_vecBomb.clear();

	for (auto& Pair : m_mapEffect)
		Safe_Release(Pair.second);
	m_mapEffect.clear();

	Safe_Release(m_pAnimation);
	Safe_Release(m_pStateMachine);
	Safe_Release(m_pKenaState);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pKenaStatus);
}

_int CKena::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	/* Terrain */
	if (m_bJump)
	{
		if (pTarget == nullptr || iColliderIndex == (_int)COLLISON_DUMMY || iColliderIndex == (_int)COL_GROUND || iColliderIndex == (_int)COL_ENVIROMENT)
		{
			m_bOnGround = true;
			m_bJump = false;
			m_bPulseJump = false;
			m_fCurJumpSpeed = 0.f;
		}

		if (iColliderIndex == (_int)COL_WATER)
		{

		}
	}
	else
	{
		if (pTarget == nullptr || iColliderIndex == (_int)COLLISON_DUMMY || m_bGrabWarrior == true) return 0;

		CGameObject* pGameObject = nullptr;

		if (m_bPulse == false && m_bDodge == false && m_bDeath == false)
		{
			_bool bRealAttack = false;
			if ((iColliderIndex == (_int)COL_MONSTER_WEAPON) && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()))
			{
				for (auto& Effect : m_mapEffect)
				{
					if (Effect.first == "KenaDamage")
					{
						Effect.second->Set_Active(true);
						Effect.second->Set_Position(vCollisionPos);
					}
				}

				m_bParry = true;
				m_iCurParryFrame = 0;
				m_pAttackObject = pTarget;
			}
			if (iColliderIndex == (_int)COL_MONSTER_ARROW)
			{
				for (auto& Effect : m_mapEffect)
				{
					if (Effect.first == "KenaDamage")
					{
						Effect.second->Set_Active(true);
						Effect.second->Set_Position(vCollisionPos);
					}
				}

				m_bParry = true;
				m_iCurParryFrame = 0;
				m_pAttackObject = pTarget;
			}
			if (iColliderIndex == (_int)COL_BOSS_SWIPECHARGE)
			{
				for (auto& Effect : m_mapEffect)
				{
					if (Effect.first == "KenaDamage")
					{
						Effect.second->Set_Active(true);
						Effect.second->Set_Position(vCollisionPos);
					}
				}

				m_bParry = true;
				m_iCurParryFrame = 0;
				m_pAttackObject = pTarget;
			}
		}

		if (iColliderIndex == (_int)COL_MONSTER && m_bDash == true)
		{
			m_bDashAttack = true;
			m_pDashTarget = pTarget;
		}

		if (iColliderIndex == (_int)COL_PORTAL && m_bDash == true)
		{
			CPortalPlane* pPortal = dynamic_cast<CPortalPlane*>(pTarget);
			CPortalPlane* pLinkedPortal = nullptr;

			if (pPortal != nullptr)
				pLinkedPortal = pPortal->Get_LinkedPortal();

			if (pLinkedPortal != nullptr)
			{
				m_bDashPortal = true;
				m_pDashTarget = pTarget;

				_smatrix	matOutPortal = pLinkedPortal->Get_WorldMatrix();

				_float4		vOutRight;
				_float4		vOutLook = matOutPortal.Up();
				vOutLook.Normalize();
				vOutRight = XMVector3Normalize(XMVector3Cross(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)), vOutLook));

				_float3		vScale = m_pTransformCom->Get_Scaled();

				m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vOutRight * vScale.x);
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, vOutLook * vScale.z);

				_float4		vPos = matOutPortal.Translation() + vOutLook * 0.5f;
				vPos.y -= 4.f;
				m_pTransformCom->Set_Position(vPos);

				/* Quest 1 - 3 Clear */
				if(g_LEVEL == LEVEL_TESTPLAY)
				{
					CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
					_bool bStart = true;
					_float fIdx = 7.f;
					wstring wstr = L"";
					m_PlayerQuestDelegator.broadcast(tag, bStart, fIdx, wstr);
					//CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);
				}
	
			}
		}

		if (iColliderIndex == (_int)COL_SCENECHANGE_PORTAL && m_bDash == true)
		{
			m_bSceneChange = true;
		}
	}

	return 0;
}

_int CKena::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == TRIGGER_DUMMY || pTarget == nullptr) return 0;

	_bool bRealAttack = false;
	if (iColliderIndex == (_int)COL_MONSTER_WEAPON && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()) && m_bPulse == false)
	{
		m_bParry = true;
		m_iCurParryFrame = 0;
		m_pAttackObject = pTarget;
	}

	if (iColliderIndex == (_int)COL_BOSS_SWIPECHARGE)
	{
		m_bParry = true;
		m_iCurParryFrame = 0;
		m_pAttackObject = pTarget;
	}
	
	if (iColliderIndex == (_int)COL_WARRIOR_GRAB_HAND)
	{
		CBossWarrior* pWarrior = dynamic_cast<CBossWarrior*>(pTarget);
		if (pWarrior->Is_WarriorGrabAnimation())
		{
			m_bGrabWarrior = true;
			m_pAttackObject = pWarrior;
		}
	}

	if (iColliderIndex == (_int)COL_WATER && g_LEVEL == (_int)LEVEL_GIMMICK)
	{
		m_bWater = m_bDeath = m_pKenaStatus->UnderAttack(m_pKenaStatus->Get_MaxHP());
	}

	return 0;
}

_int CKena::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

void CKena::LiftRotRockProc()
{	
	CTerrain* pCurTerrain = m_pTerrain[CGameInstance::GetInstance()->Get_CurrentPlayerRoomIndex()];
	if (pCurTerrain == nullptr)
		return;



	if (m_bRotRockChoiceFlag == false && GetKeyState('R') & 0x8000)
	{
		if (m_pRopeRotRock)
		{
			m_bRotRockChoiceFlag = true;
			m_pRopeRotRock->Set_ChoiceFlag(true);
		}
		else
			m_bRotRockChoiceFlag = false;
	}

	if (m_bRotRockChoiceFlag && m_bAim && m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::AIM_INTO)
	{	
		CGameInstance* pGameInst = CGameInstance::GetInstance();
		_vector vCamPos = pGameInst->Get_CamPosition();
		_vector vCamLook = pGameInst->Get_CamLook_Float4();
		_float3 vOut;

		if (CPhysX_Manager::GetInstance()->Raycast_Collision(vCamPos, vCamLook, 10.f, &vOut))
		{
			pCurTerrain->Set_BrushPosition(vOut);

			if (GetKeyState('R') & 0x8000)
			{
				m_bRotRockMoveFlag = true;
				if (m_pRopeRotRock && m_pRopeRotRock->Get_MoveFlag() == false)
				{
					m_pRopeRotRock->Set_MoveFlag(true);
					m_pRopeRotRock->Set_MoveTargetPosition(vOut);
				}
			}
		}
	}
	else
	{
		if (pCurTerrain && m_bRotRockMoveFlag == false)
		{
			pCurTerrain->Set_BrushPosition(_float3(-1000.f, 0.f, 0.f));
		}	
	}
}

void CKena::Set_RopeRotRockPtr(CRope_RotRock* pObject)
{
	_float fNoMeaning = 1.f;
	if (m_pRopeRotRock == nullptr && pObject != nullptr)
	{
		CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::BOT_KEY_MOVEROT;
		m_Delegator.broadcast(tag, fNoMeaning);
	}
	else if (m_pRopeRotRock != nullptr && pObject == nullptr)
	{
		CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::BOT_KEY_OFF;
		m_Delegator.broadcast(tag, fNoMeaning);
	}

	m_pRopeRotRock = pObject;
}

void CKena::Setup_TerrainPtr()
{
	CGameInstance* pGameInst = CGameInstance::GetInstance();
	
	_tchar szCloneTag[64] = TEXT("");
	for (_uint i = 0; i < TERRAIN_COUNT; i++)
	{
		swprintf_s(szCloneTag, TEXT("Terrain%d"), i);
		m_pTerrain[i] = (CTerrain*)pGameInst->Get_GameObjectPtr(g_LEVEL, L"Layer_BackGround", szCloneTag);
	}
}

void CKena::End_LiftRotRock()
{	
	m_bRotRockChoiceFlag = m_bRotRockMoveFlag = false;
	CTerrain* pCurTerrain = m_pTerrain[CGameInstance::GetInstance()->Get_CurrentPlayerRoomIndex()];
	if (pCurTerrain)		
		pCurTerrain->Set_BrushPosition(_float3(-1000.f, 0.f, 0.f));
}

void CKena::Buy_RotHat(_uint iHatIndex)
{
	if (m_pFirstRot == nullptr) return;

	m_pFirstRot->Buy_Hat(iHatIndex);
	m_pHatCart ? m_pHatCart->Change_MannequinHat(iHatIndex) : 0;
}

_bool CKena::IsBuyPossible_RotHat()
{
	if (m_pFirstRot == nullptr) return false;

	return m_pFirstRot->IsBuyPossible();
}
