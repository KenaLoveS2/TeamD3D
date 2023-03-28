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

CKena::CKena(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pUI_FocusRot(nullptr)
	, m_bStateLock(false)
	, m_pUI_FocusMonster(nullptr)
	, m_pTargetMonster(nullptr)
{
}

CKena::CKena(const CKena & rhs)
	: CGameObject(rhs)
	, m_pUI_FocusRot(nullptr)
	, m_bStateLock(false)
	, m_pUI_FocusMonster(nullptr)
	, m_pTargetMonster(nullptr)
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

const _bool CKena::Get_State(STATERETURN eState) const
{
	/* Used by Camera */
	if (eState == CKena::STATERETURN_END)
		return !m_bHeavyAttack && !m_bMask && !m_bAim && !m_bInjectBow && !m_bPulse && !m_bParryLaunch;

	switch (eState)
	{
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

	case STATE_SPRINT:
		return m_bSprint;
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

	default:
		return false;
	}
}

void CKena::Set_State(STATERETURN eState, _bool bValue)
{
	if (eState == CKena::STATERETURN_END)
		return;

	switch (eState)
	{
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

	case STATE_SPRINT:
		m_bSprint = bValue;
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

	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Find_Camera(L"PLAYER_CAM"));
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);
	m_pCamera->Set_Player(this, m_pTransformCom);
	CGameInstance::GetInstance()->Work_Camera(L"PLAYER_CAM");

	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_pKenaState = CKena_State::Create(this, m_pKenaStatus, m_pStateMachine, m_pModelCom, m_pAnimation, m_pTransformCom, m_pCamera);

	FAILED_CHECK_RETURN(Ready_Parts(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Effects(), E_FAIL);

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

	return S_OK;
}

HRESULT CKena::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(Ready_Arrows(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Bombs(), E_FAIL);

	_float3 vPos = _float3(0.f, 3.f, 0.f);
	_float3 vPivotScale = _float3(0.2f, 0.5f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.7f, 0.f);

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
	PxCapsuleDesc.fMass = 59.f;
	PxCapsuleDesc.fLinearDamping = 10.f;
	PxCapsuleDesc.bCCD = true;
	PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_PLAYER));

	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
	m_pRendererCom->Set_PhysXRender(true);
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
	PxCapsuleDesc.fMass = 20.f;
	PxCapsuleDesc.fLinearDamping = 10.f;
	PxCapsuleDesc.bCCD = true;
	PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;

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

	CGameInstance* pGameInst = CGameInstance::GetInstance();
	m_pTerrain = (CTerrain*)pGameInst->Get_GameObjectPtr(g_LEVEL, L"Layer_BackGround", L"Terrain");

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

		if (FAILED(p_game_instance->Clone_AnimObject(g_LEVEL, TEXT("Layer_Rot"), TEXT("Prototype_GameObject_RotForMonster"), 
			CUtile::Create_StringAuto(szCloneRotTag), &desc, &p_game_object)))
			return E_FAIL;

		m_pRotForMonster[i] = static_cast<CRotForMonster*>(p_game_object);
	}

	RELEASE_INSTANCE(CGameInstance)

	CUI_ClientManager::UI_PRESENT eRot = CUI_ClientManager::HUD_ROT;
	CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	_float fRotState = (_float)CKena_Status::RS_GOOD;
	m_PlayerDelegator.broadcast(eRot, funcDefault, fRotState);

	m_pTransformCom->Set_Position(_float4(13.f, 0.f, 9.f, 1.f));

	for (auto& pEffect : m_mapEffect)
	{
		if(pEffect.second != nullptr )
			pEffect.second->Late_Initialize();
	}
	pStaff->Late_Initialize(pArg);

	return S_OK;
}

void CKena::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	// if (CGameInstance::GetInstance()->IsWorkCamera(TEXT("DEBUG_CAM_1"))) return;	
	//m_pKenaStatus->Set_Attack(20);
#endif	
	
	if (m_bCommonHit || m_bHeavyHit || m_bParryLaunch)
		m_fChangeColorTime += fTimeDelta;

	if (m_fChangeColorTime > 1.f)
	{
		m_bCommonHit = false;
		m_bHeavyHit = false;
		m_bParryLaunch = false;
		m_fChangeColorTime = 0.0f;
	}
	
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

	_float	fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	__super::Tick(fTimeDelta);

	Test_Raycast();

 	if (m_bParry == true)
 	{
 		if (m_iCurParryFrame == m_iParryFrameCount)
 		{
 			m_bCommonHit = true;
 			//m_bHeavyHit = true;
 			m_eDamagedDir = Calc_DirToMonster(m_pAttackObject);

			if (m_bPulse == false)
 				m_pKenaStatus->UnderAttack(((CMonster*)m_pAttackObject)->Get_MonsterStatusPtr());
 
 			m_bParry = false;
 			m_pAttackObject = nullptr;
 		}
 	}

	if (m_pAnimation->Get_Preview() == false)
	{
		m_pKenaState->Tick(fTimeDelta);

		if(!m_bStateLock)
			m_pStateMachine->Tick(fTimeDelta);
	}

	m_pKenaStatus->Tick(fTimeDelta);

	m_bCommonHit = false;
	m_bHeavyHit = false;

	Update_Collider(fTimeDelta);

	if (fTimeRate != CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60"))
	{
		fTimeDelta /= fTimeRate;
		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
		fTimeDelta *= fTimeRate;
	}

	if (m_pModelCom->Get_Preview() == false)
	{
		if (m_fHitStopTime <= 0.f)
		{
			if (m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::PULSE_PARRY &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::BOW_INJECT_ADD &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::BOMB_INJECT_ADD && 
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::SHIELD_BREAK_FRONT &&
				m_pAnimation->Get_CurrentAnimIndex() != (_uint)CKena_State::SHIELD_BREAK_BACK)
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

	if (m_bAttack)
	{
		CBone*	pBone = m_pModelCom->Get_BonePtr("staff_skin9_jnt");
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

	if (m_pTargetMonster && m_bAttack)
	{
		_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector	vTargetPos = m_pTargetMonster->Get_Position();
		_vector	vDir = vTargetPos - vPos;
		_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

		if (!m_bJump && !m_bSprint)
		{
			if (fDistance > 0.5f)
			{
				vTargetPos = vTargetPos + XMVector3Normalize(vDir) * 0.5f;
				vDir = vTargetPos - vPos;

				m_pTransformCom->Set_Speed(7.f);
				m_pTransformCom->LookAt_NoUpDown(vTargetPos);
				m_pTransformCom->Go_DirectionNoY(vDir, fTimeDelta);
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

	for (auto& pPart : m_vecPart)
		pPart->Tick(fTimeDelta);

	for (auto& pArrow : m_vecArrow)
		pArrow->Tick(fTimeDelta);

	for (auto& pBomb : m_vecBomb)
		pBomb->Tick(fTimeDelta);
	
	for (auto& pEffect : m_mapEffect)
		pEffect.second->Tick(fTimeDelta);

	if (CGameInstance::GetInstance()->Key_Down(DIK_O))
		m_pCamera->Camera_Shake(0.003f, 10);
	if (CGameInstance::GetInstance()->Key_Down(DIK_P))
		m_pCamera->Camera_Shake(XMVectorSet(1.f, 1.f, 0.f, 0.f), XMConvertToRadians(10.f));

	/* Delegator Arrow */
	// CKena_Status::m_iCurArrowCount, m_iMaxArrowCount, m_fCurArrowCoolTime, m_fInitArrowCount
	//CUI_ClientManager::UI_PRESENT eArrow = CUI_ClientManager::AMMO_ARROW;
	//_float fCurArrowCount = (_float)m_pKenaStatus->Get_CurArrowCount();
	//
	//
	//_float fMaxArrowCount = (_float)m_pKenaStatus->Get_MaxArrowCount();
	//_float fCurArrowCoolTime = (_float)m_pKenaStatus->Get_CurArrowCoolTime();
	//_float fInitArrowCoolTime = (_float)m_pKenaStatus->Get_InitArrowCoolTime();
	//
	//m_PlayerAmmoDelegator.broadcast(eArrow, fCurArrowCount, fMaxArrowCount, fCurArrowCoolTime, fInitArrowCoolTime);

	/* ~Delegator */

	CRot::Set_RotUseKenaPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	CMonster::Set_MonsterUseKenaPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CKena::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pKenaState->Late_Tick(fTimeDelta);

	if (m_iCurParryFrame < m_iParryFrameCount)
	{
		m_iCurParryFrame++;
		m_bParry = true;
	}

	if (m_pTargetMonster != nullptr)
	{
		if (m_pTargetMonster->Is_Dead() == true ||
			m_pTransformCom->Calc_Distance_XZ(m_pTargetMonster->Get_TransformCom()) > 5.f ||
			m_pTransformCom->Calc_InRange(XMConvertToRadians(240.f), m_pTargetMonster->Get_TransformCom()) == false)
			m_pTargetMonster = nullptr;
	}

	Call_FocusMonsterIcon(m_pTargetMonster);

	/* UI Control */
	if (CKena_Status::RS_ACTIVE == m_pKenaStatus->Get_RotState())
	{
		CMonster* pMonster = m_pRotForMonster[0]->Get_Target();
		if (nullptr != pMonster && false == pMonster->Get_Bind())
		{
			CUI_ClientManager::UI_PRESENT eRot = CUI_ClientManager::HUD_ROT;
			CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
			CKena_Status::ROTSTATE eRotState;
			if (m_pKenaStatus->Get_CurPIPGuage() >= 1.0f)
				eRotState = CKena_Status::RS_GOOD;
			else
				eRotState = CKena_Status::RS_HIDE;

			m_pKenaStatus->Set_RotState(eRotState);
			_float fState = (_float)eRotState;
			m_PlayerDelegator.broadcast(eRot, funcDefault, fState);
		}
	}


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
		m_PlayerDelegator.broadcast(ePip, funcDefault, fCurGuage);
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
	{
		CKena* pPlayer = this;
		m_PlayerPtrDelegator.broadcast(eCart, funcDefault, pPlayer);
	}

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
		m_bHeavyHit = true;
	if (ImGui::Button("m_bCommonHit"))
		m_bCommonHit = true;
	if (ImGui::Button("m_bParryLaunch"))
		m_bParryLaunch = !m_bParryLaunch;

	_int	ArrowCount[2] = { m_pKenaStatus->Get_CurArrowCount(), m_pKenaStatus->Get_MaxArrowCount() };
	ImGui::InputInt2("Arrow Count", (_int*)&ArrowCount, ImGuiInputTextFlags_ReadOnly);

	_float2	ArrowCoolTime{ m_pKenaStatus->Get_CurArrowCoolTime(), m_pKenaStatus->Get_InitArrowCoolTime() };
	ImGui::InputFloat2("Arrow CoolTime", (_float*)&ArrowCoolTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_int	BombCount[2] = { m_pKenaStatus->Get_CurBombCount(), m_pKenaStatus->Get_MaxBombCount() };
	ImGui::InputInt2("Bomb Count", (_int*)&BombCount, ImGuiInputTextFlags_ReadOnly);

	_float2	BombCoolTime{ m_pKenaStatus->Get_CurBombCoolTime(), m_pKenaStatus->Get_InitBombCoolTime() };
	ImGui::InputFloat2("Bomb CoolTime", (_float*)&BombCoolTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_float2	Shield{ m_pKenaStatus->Get_Shield(), m_pKenaStatus->Get_MaxShield() };
	ImGui::InputFloat2("Shield", (_float*)&Shield, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_float2	ShieldCoolTime{ m_pKenaStatus->Get_CurShieldCoolTime(), m_pKenaStatus->Get_InitShieldCoolTime() };
	ImGui::InputFloat2("Shield CoolTime", (_float*)&ShieldCoolTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	_float2	ShieldRecoveryTime{ m_pKenaStatus->Get_CurShieldRecoveryTime(), m_pKenaStatus->Get_InitShieldRecoveryTime() };
	ImGui::InputFloat2("Shield Recovery Time", (_float*)&ShieldRecoveryTime, "%.3f", ImGuiInputTextFlags_ReadOnly);

	__super::Imgui_RenderProperty();
}

void CKena::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();

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

}

void CKena::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	if (m_bLocalMoveLock == true)
		return;

	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
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
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Down(DIK_R))
		{
			/* UI Control */
			m_pKenaStatus->Set_CurPIPGuage(m_pKenaStatus->Get_CurPIPGuage() - 1.f);
			m_pKenaStatus->Set_RotState(CKena_Status::RS_ACTIVE);
			CUI_ClientManager::UI_PRESENT ePip = CUI_ClientManager::HUD_PIP;
			CUI_ClientManager::UI_PRESENT eRot = CUI_ClientManager::HUD_ROT;
			CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
			_float fZero = 0.f;
			m_PlayerDelegator.broadcast(ePip, funcDefault, fZero);
			_float fRot = 2;
			m_PlayerDelegator.broadcast(eRot, funcDefault, fRot);
			/* ~UI Control */

			static_cast<CMonster*>(pTarget)->Bind(m_pRotForMonster, 8);
			//static_cast<CMonster*>(pTarget)->Setting_Rot();
			m_pRotForMonster[0]->Set_Target(static_cast<CMonster*>(pTarget));
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	m_pUI_FocusRot->Set_Pos(pTarget);
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
	pGameInstance->Add_AnimObject(LEVEL_GAMEPLAY, pPart);
	m_pAnimation->Add_AnimSharingPart(dynamic_cast<CModel*>(pPart->Find_Component(L"Com_Model")), false);

	/* MainOutfit */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_OUTFIT;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_MainOutfit", L"Kena_MainOutfit", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(LEVEL_GAMEPLAY, pPart);
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
	_uint		iArrowCount = m_pKenaStatus->Get_MaxArrowCount();
	_tchar*	pTag = nullptr;
	CSpiritArrow*	pArrow = nullptr;

	for (_uint i = 0; i < iArrowCount; ++i)
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
	_uint		iBombCount = m_pKenaStatus->Get_MaxBombCount();
	_tchar*	pTag = nullptr;
	CRotBomb*	pBomb = nullptr;

	for (_uint i = 0; i < iBombCount; ++i)
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
	CEffect_Base*	pEffectBase = nullptr;
	_tchar*			pCloneTag = nullptr;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

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

	for (auto& pEffects : m_mapEffect)
		pEffects.second->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CKena::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Kena", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

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

	/* Kena Damage Parry Value */
	//_bool bHit = false;
	//if (m_bHeavyHit == true || m_bCommonHit == true)
	//	bHit = true;
	//m_pShaderCom->Set_RawValue("g_Hit", &bHit, sizeof(_bool));
	//m_pShaderCom->Set_RawValue("g_Parry", &m_bParryLaunch, sizeof(_bool));
	//m_pShaderCom->Set_RawValue("g_Time", &m_fChangeColorTime, sizeof(_float));

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

CKena::DAMAGED_FROM CKena::Calc_DirToMonster(CGameObject * pTarget)
{
	DAMAGED_FROM		eDir = DAMAGED_FROM_END;

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
				matWalk.r[3] = matWorldSocket.r[3];
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
				matWalk.r[3] = matWorldSocket.r[3];
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
}

void CKena::TurnOnInteractStaff(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnInteractStaff);
		return;
	}

	CBone*	pStaffBonePtr = m_pModelCom->Get_BonePtr("staff_skin2_jnt");
	_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	_matrix matIntoAttack = m_mapEffect["InteractStaff"]->Get_TransformCom()->Get_WorldMatrix();
	matIntoAttack.r[3] = matWorldSocket.r[3];
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
	}
	else
	{
		if (pTarget == nullptr || iColliderIndex == (_int)COLLISON_DUMMY) return 0;

		CGameObject* pGameObject = nullptr;

		_bool bRealAttack = false;
		if (iColliderIndex == (_int)COL_MONSTER_WEAPON && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()) && m_bPulse == false)
		{
			CUI_ClientManager::UI_PRESENT eHP = CUI_ClientManager::HUD_HP;
			CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
			_float fGuage = m_pKenaStatus->Get_PercentHP();
			m_PlayerDelegator.broadcast(eHP, funcDefault, fGuage);

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

		if (iColliderIndex == COL_PLAYER_WEAPON)
		{
			/* Increase Pip Guage */
			m_pKenaStatus->Plus_CurPIPGuage(0.2f);
				//for (auto& Effect : m_mapEffect)
				//{
				//	if (Effect.first == "KenaHit")
				//	{
				//		Effect.second->Set_Active(true);
				//		Effect.second->Set_Position(vCollisionPos);
				//	}
				//}
			
			pGameObject->Set_Position(vCollisionPos);

			// m_bCommonHit = true;
			// m_bHeavyHit = true;
		}

		int temp = 0;
	}

	return 0;
}

_int CKena::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

_int CKena::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

void CKena::Test_Raycast()
{
	//if (GetKeyState(VK_LCONTROL) & 0x8000 && GetKeyState('S') & 0x8000)
	//	m_pKenaStatus->Save();

	if (m_pTerrain == nullptr)
		return;

	if (GetKeyState('T') & 0x8000)
	{
		if (m_pRopeRotRock)
		{
			m_pRopeRotRock->Set_ChoiceFlag(true);
		}
	}

	if (GetKeyState(VK_LSHIFT) & 0x0800)
	{
		CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
		CGameInstance* pGameInst = CGameInstance::GetInstance();

		_vector vCamPos = pGameInst->Get_CamPosition();
		_vector vCamLook = pGameInst->Get_CamLook_Float4();
		_float3 vOut;

		if (pPhysX->Raycast_Collision(vCamPos, vCamLook, 10.f, &vOut))
		{
			m_pTerrain->Set_BrushPosition(vOut);

			if (GetKeyState('R') & 0x8000)
			{
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
		m_pTerrain->Set_BrushPosition(_float3(-1000.f, 0.f, 0.f));
	}
}
