#include "stdafx.h"
#include <cstddef>
#include "..\public\Kena_State.h"
#include "GameInstance.h"
#include "Utile.h"
#include "UI_ClientManager.h"
#include "AnimationState.h"
#include "Effect_Base.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "SpiritArrow.h"
#include "RotBomb.h"
#include "Camera_Player.h"
#include "E_KenaPulse.h"
#include "Kena_Parts.h"

#define NULLFUNC	(_bool(CKena_State::*)())nullptr
CKena_State::CKena_State()
{
}

HRESULT CKena_State::Initialize(CKena * pKena, CKena_Status * pStatus, CStateMachine * pStateMachine, CModel * pModel, CAnimationState * pAnimation, CTransform * pTransform, CCamera_Player * pCamera)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pKena = pKena;
	m_pStatus = pStatus;
	m_pStateMachine = pStateMachine;
	m_pModel = pModel;
	m_pAnimationState = pAnimation;
	m_pTransform = pTransform;
	m_pCamera = pCamera;

	NULL_CHECK_RETURN(m_pKena, E_FAIL);
	NULL_CHECK_RETURN(m_pStateMachine, E_FAIL);
	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Run(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Aim(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Aim_Air(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Air_Attack(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack1(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack2(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack3(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack4(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bomb(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bomb_Air(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bow(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bow_Inject(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bow_Air(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Combat(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Crouch(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged_Common(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged_Heavy(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Dodge(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Fall(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack1(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack2(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack3(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack_Combo(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Interact(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Jump(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Land(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Mask(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Meditate(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Pulse(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Shield(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Spin_Attack(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Sprint(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_TeleportFlower(), E_FAIL);

	return S_OK;
}

void CKena_State::Tick(_double dTimeDelta)
{
	m_eDir = DetectDirectionInput();

	if (m_pKena->m_bSprint == true)
		m_pTransform->Set_Speed(7.f);
	else if (m_pKena->m_bAim == true)
	{
		if (m_pKena->m_bInjectBow == true)
			m_pTransform->Set_Speed(1.5f);
		else
			m_pTransform->Set_Speed(3.5f);
	}
	else if (m_pKena->m_bPulse == true)
		m_pTransform->Set_Speed(2.f);
	else
		m_pTransform->Set_Speed(5.f);
}

void CKena_State::Late_Tick(_double dTimeDelta)
{
	m_ePreDir = m_eDir;

	/* Arrow */
	if (m_pKena->m_bBow == true && m_pKena->m_pCurArrow == nullptr && m_pStatus->Get_CurArrowCount() > 0)
	{
		for (auto pArrow : m_pKena->m_vecArrow)
		{
			if (pArrow->IsActive() == false)
			{
				m_pKena->m_pCurArrow = pArrow;
				m_pKena->m_pCurArrow->Reset();
				m_pKena->m_pCurArrow->Set_Active(true);
				break;
			}
		}
	}

	if (m_pKena->m_bBow == false && m_pKena->m_pCurArrow != nullptr)
	{
		if (m_pKena->m_pCurArrow->Get_CurrentState() >= CSpiritArrow::ARROW_FIRE)
		{
			m_pStatus->Set_CurArrowCount(m_pStatus->Get_CurArrowCount() - 1);
			m_pKena->m_pCurArrow = nullptr;
		}
		else
		{
			m_pKena->m_pCurArrow->Reset();
			m_pKena->m_pCurArrow = nullptr;
		}
	}

	/* Bomb */
	if (m_pKena->m_bBomb == true && m_pKena->m_pCurBomb == nullptr && m_pStatus->Get_CurBombCount() > 0)
	{
		for (auto pBomb : m_pKena->m_vecBomb)
		{
			if (pBomb->Get_Active() == false)
			{
				m_pKena->m_pCurBomb = pBomb;
				m_pKena->m_pCurBomb->Reset();
				m_pKena->m_pCurBomb->Set_Active(true);
				break;
			}
		}
	}

	if (m_pKena->m_bBomb == false && m_pKena->m_pCurBomb != nullptr)
	{
		if (m_pKena->m_pCurBomb->Get_CurrentState() >= CRotBomb::BOMB_RELEASE)
		{
			m_pStatus->Set_CurBombCount(m_pStatus->Get_CurBombCount() - 1);
			m_pKena->m_pCurBomb = nullptr;
		}
		else
		{
			m_pKena->m_pCurBomb->Reset();
			m_pKena->m_pCurBomb = nullptr;
		}
	}

// 	m_pKena->m_bCommonHit = false;
// 	m_pKena->m_bHeavyHit = false;
}

void CKena_State::ImGui_RenderProperty()
{
}

CKena_State * CKena_State::Create(CKena * pKena, CKena_Status * pStatus, CStateMachine * pStateMachine, CModel * pModel, CAnimationState * pAnimation, CTransform * pTransform, CCamera_Player * pCamera)
{
	CKena_State*	pInstance = new CKena_State;

	if (FAILED(pInstance->Initialize(pKena, pStatus, pStateMachine, pModel, pAnimation, pTransform, pCamera)))
	{
		MSG_BOX("Failed to Create : CKena_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena_State::Free()
{
}

HRESULT CKena_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE")
		.Add_State(L"IDLE")
		.Init_Start(this, &CKena_State::Start_Idle)
		.Init_Tick(this, &CKena_State::Tick_Idle)
		.Init_End(this, &CKena_State::End_Idle)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"MEDITATE_INTO", this, &CKena_State::KeyDown_Q)
		.Init_Changer(L"MASK_ON", this, &CKena_State::KeyDown_T)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
 		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Run()
{
	m_pStateMachine->Add_State(L"RUN")
		.Init_Start(this, &CKena_State::Start_Run)
		.Init_Tick(this, &CKena_State::Tick_Run)
		.Init_End(this, &CKena_State::End_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"RUN_STOP")
		.Init_Start(this, &CKena_State::Start_Run_Stop)
		.Init_Tick(this, &CKena_State::Tick_Run_Stop)
		.Init_End(this, &CKena_State::End_Run_Stop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Aim()
{
	m_pStateMachine->Add_State(L"AIM_INTO")
		.Init_Start(this, &CKena_State::Start_Aim_Into)
		.Init_Tick(this, &CKena_State::Tick_Aim_Into)
		.Init_End(this, &CKena_State::End_Aim_Into)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_LOOP")
		.Init_Start(this, &CKena_State::Start_Aim_Loop)
		.Init_Tick(this, &CKena_State::Tick_Aim_Loop)
		.Init_End(this, &CKena_State::End_Aim_Loop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)

		.Add_State(L"AIM_RETURN")
		.Init_Start(this, &CKena_State::Start_Aim_Return)
		.Init_Tick(this, &CKena_State::Tick_Aim_Return)
		.Init_End(this, &CKena_State::End_Aim_Return)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_RUN")
		.Init_Start(this, &CKena_State::Start_Aim_Run)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"AIM_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"AIM_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"AIM_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"AIM_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"AIM_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"AIM_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"AIM_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward_Left)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward_Right)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward_Left)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward_Right)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Left)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Right)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Aim_Air()
{
	m_pStateMachine->Add_State(L"AIM_AIR_INTO")
		.Init_Start(this, &CKena_State::Start_Aim_Air_Into)
		.Init_Tick(this, &CKena_State::Tick_Aim_Air_Into)
		.Init_End(this, &CKena_State::End_Aim_Air_Into)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_AIR_CHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_AIR_INTO", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)
		.Init_Changer(L"AIM_AIR_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_AIR_LOOP")
		.Init_Start(this, &CKena_State::Start_Aim_Air_Loop)
		.Init_Tick(this, &CKena_State::Tick_Aim_Air_Loop)
		.Init_End(this, &CKena_State::End_Aim_Air_Loop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"FALL", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"BOW_AIR_CHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOMB_AIR_INTO", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Air_Attack()
{
	m_pStateMachine->Add_State(L"AIR_ATTACK_1")
		.Init_Start(this, &CKena_State::Start_Air_Attack_1)
		.Init_Tick(this, &CKena_State::Tick_Air_Attack_1)
		.Init_End(this, &CKena_State::End_Air_Attack_1)
		.Init_Changer(L"AIR_ATTACK_2", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)
		.Init_Changer(L"FALL", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIR_ATTACK_2")
		.Init_Start(this, &CKena_State::Start_Air_Attack_2)
		.Init_Tick(this, &CKena_State::Tick_Air_Attack_2)
		.Init_End(this, &CKena_State::End_Air_Attack_2)
		.Init_Changer(L"AIR_ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)
		.Init_Changer(L"FALL", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIR_ATTACK_SLAM_INTO")
		.Init_Start(this, &CKena_State::Start_Air_Attack_Slam_Into)
		.Init_Tick(this, &CKena_State::Tick_Air_Attack_Slam_Into)
		.Init_End(this, &CKena_State::End_Air_Attack_Slam_Into)
		.Init_Changer(L"AIR_ATTACK_SLAM_LOOP", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"AIR_ATTACK_SLAM_FINISH", this, &CKena_State::OnGround)

		.Add_State(L"AIR_ATTACK_SLAM_LOOP")
		.Init_Start(this, &CKena_State::Start_Air_Attack_Slam_Loop)
		.Init_Tick(this, &CKena_State::Tick_Air_Attack_Slam_Loop)
		.Init_End(this, &CKena_State::End_Air_Attack_Slam_Loop)
		.Init_Changer(L"AIR_ATTACK_SLAM_FINISH", this, &CKena_State::OnGround)

		.Add_State(L"AIR_ATTACK_SLAM_FINISH")
		.Init_Start(this, &CKena_State::Start_Air_Attack_Slam_Finish)
		.Init_Tick(this, &CKena_State::Tick_Air_Attack_Slam_Finish)
		.Init_End(this, &CKena_State::End_Air_Attack_Slam_Finish)
		.Init_Changer(L"HEAVY_ATTACK_COMBO_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_COMBO_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack1()
{
	m_pStateMachine->Add_State(L"ATTACK_1")
		.Init_Start(this, &CKena_State::Start_Attack_1)
		.Init_Tick(this, &CKena_State::Tick_Attack_1)
		.Init_End(this, &CKena_State::End_Attack_1)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"TAKE_DAMAGE", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)

		.Add_State(L"ATTACK_1_FROM_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_1_From_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_From_Run)
		.Init_End(this, &CKena_State::End_Attack_1_From_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)

		.Add_State(L"ATTACK_1_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_1_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_1_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ATTACK_1_INTO_WALK")
		.Init_Start(this, &CKena_State::Start_Attack_1_Into_Walk)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Into_Walk)
		.Init_End(this, &CKena_State::End_Attack_1_Into_Walk)

		.Add_State(L"ATTACK_1_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_1_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Return)
		.Init_End(this, &CKena_State::End_Attack_1_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack2()
{
	m_pStateMachine->Add_State(L"ATTACK_2")
		.Init_Start(this, &CKena_State::Start_Attack_2)
		.Init_Tick(this, &CKena_State::Tick_Attack_2)
		.Init_End(this, &CKena_State::End_Attack_2)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"ATTACK_3", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"HEAVY_ATTACK_3_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_2_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.45f)

		.Add_State(L"ATTACK_2_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_2_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_2_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_2_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ATTACK_2_INTO_WALK")
		.Init_Start(this, &CKena_State::Start_Attack_2_Into_Walk)
		.Init_Tick(this, &CKena_State::Tick_Attack_2_Into_Walk)
		.Init_End(this, &CKena_State::End_Attack_2_Into_Walk)

		.Add_State(L"ATTACK_2_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_2_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_2_Return)
		.Init_End(this, &CKena_State::End_Attack_2_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack3()
{
	m_pStateMachine->Add_State(L"ATTACK_3")
		.Init_Start(this, &CKena_State::Start_Attack_3)
		.Init_Tick(this, &CKena_State::Tick_Attack_3)
		.Init_End(this, &CKena_State::End_Attack_3)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"ATTACK_4", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"ATTACK_3_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.3f)
		.Init_Changer(L"ATTACK_3_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)

		.Add_State(L"ATTACK_3_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_3_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_3_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_3_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ATTACK_3_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_3_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_3_Return)
		.Init_End(this, &CKena_State::End_Attack_3_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack4()
{
	m_pStateMachine->Add_State(L"ATTACK_4")
		.Init_Start(this, &CKena_State::Start_Attack_4)
		.Init_Tick(this, &CKena_State::Tick_Attack_4)
		.Init_End(this, &CKena_State::End_Attack_4)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"ATTACK_4_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"ATTACK_4_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)

		.Add_State(L"ATTACK_4_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_4_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_4_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_4_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ATTACK_4_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_4_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_4_Return)
		.Init_End(this, &CKena_State::End_Attack_4_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bomb()
{
	m_pStateMachine->Add_State(L"BOMB_INTO")
		.Init_Start(this, &CKena_State::Start_Bomb_Into)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into)
		.Init_End(this, &CKena_State::End_Bomb_Into)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_CANCEL", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Bomb_Run)
		.Init_Changer(L"BOMB_INTO_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOMB_INTO_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOMB_INTO_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOMB_INTO_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOMB_INTO_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOMB_INTO_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOMB_INTO_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOMB_INTO_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOMB_INTO_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INTO_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Into_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Into_Run)
		.Init_End(this, &CKena_State::End_Bomb_Into_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_INTO_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INTO", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_LOOP")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop)
		.Init_End(this, &CKena_State::End_Bomb_Loop)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOMB_CANCEL", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN")
		.Init_Start(this, &CKena_State::Start_Bomb_Run)
		.Init_Changer(L"BOMB_LOOP_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOMB_LOOP_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOMB_LOOP_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOMB_LOOP_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOMB_LOOP_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOMB_LOOP_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOMB_LOOP_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOMB_LOOP_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOMB_LOOP_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_LOOP_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Loop_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Loop_Run)
		.Init_End(this, &CKena_State::End_Bomb_Loop_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyDown_R, &CKena_State::Check_PipCount)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bomb_Release)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release)
		.Init_End(this, &CKena_State::End_Bomb_Release)
		.Init_Changer(L"IDLE", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN")
		.Init_Start(this, &CKena_State::Start_Bomb_Run)
		.Init_Changer(L"BOMB_RELEASE_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOMB_RELEASE_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOMB_RELEASE_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOMB_RELEASE_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOMB_RELEASE_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOMB_RELEASE_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOMB_RELEASE_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOMB_RELEASE_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOMB_RELEASE_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_RELEASE_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Release_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Release_Run)
		.Init_End(this, &CKena_State::End_Bomb_Release_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel)
		.Init_End(this, &CKena_State::End_Bomb_Cancel)
		.Init_Changer(L"IDLE", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN")
		.Init_Start(this, &CKena_State::Start_Bomb_Run)
		.Init_Changer(L"BOMB_CANCEL_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOMB_CANCEL_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOMB_CANCEL_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOMB_CANCEL_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOMB_CANCEL_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOMB_CANCEL_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOMB_CANCEL_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOMB_CANCEL_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOMB_CANCEL", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOMB_CANCEL_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)		
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_CANCEL_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Cancel_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Cancel_Run)
		.Init_End(this, &CKena_State::End_Bomb_Cancel_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject)
		.Init_End(this, &CKena_State::End_Bomb_Inject)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_CANCEL", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN")
		.Init_Start(this, &CKena_State::Start_Bomb_Run)
		.Init_Changer(L"BOMB_INJECT_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOMB_INJECT_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOMB_INJECT_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOMB_INJECT_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOMB_INJECT_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOMB_INJECT_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOMB_INJECT_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOMB_INJECT_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOMB_INJECT_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_INJECT_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bomb_Inject_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Inject_Run)
		.Init_End(this, &CKena_State::End_Bomb_Inject_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOMB_INJECT_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOMB_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bomb_Air()
{
	m_pStateMachine->Add_State(L"BOMB_AIR_INTO")
		.Init_Start(this, &CKena_State::Start_Bomb_Air_Into)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Air_Into)
		.Init_End(this, &CKena_State::End_Bomb_Air_Into)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOMB_AIR_CANCEL", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_AIR_RELEASE", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"BOMB_AIR_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_AIR_LOOP")
		.Init_Start(this, &CKena_State::Start_Bomb_Air_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Air_Loop)
		.Init_End(this, &CKena_State::End_Bomb_Air_Loop)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOMB_LOOP", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_LOOP_RUN", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOMB_AIR_CANCEL", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_AIR_RELEASE", this, &CKena_State::MouseUp_Right)

		.Add_State(L"BOMB_AIR_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bomb_Air_Release)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Air_Release)
		.Init_End(this, &CKena_State::End_Bomb_Air_Release)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOMB_RELEASE", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_RELEASE_RUN", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_AIR_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOMB_AIR_CANCEL")
		.Init_Start(this, &CKena_State::Start_Bomb_Air_Cancel)
		.Init_Tick(this, &CKena_State::Tick_Bomb_Air_Cancel)
		.Init_End(this, &CKena_State::End_Bomb_Air_Cancel)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOMB_CANCEL", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOMB_CANCEL_RUN", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOMB_AIR_CANCEL", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"BOMB_AIR_RELEASE", this, &CKena_State::MouseUp_Right)
		.Init_Changer(L"AIM_AIR_LOOP", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bow()
{
	m_pStateMachine->Add_State(L"BOW_CHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Charge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge)
		.Init_End(this, &CKena_State::End_Bow_Charge)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN")
		.Init_Start(this, &CKena_State::Start_Bow_Run)
		.Init_Changer(L"BOW_CHARGE_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_CHARGE_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_CHARGE_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_CHARGE_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_CHARGE_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_CHARGE_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_CHARGE_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_CHARGE_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_CHARGE_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN")
		.Init_Start(this, &CKena_State::Start_Bow_Run)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_CHARGE_FULL_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full_Run)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full_Run)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_LOOP")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN")
		.Init_Start(this, &CKena_State::Start_Bow_Run)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop_Run)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bow_Release)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release)
		.Init_End(this, &CKena_State::End_Bow_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RELEASE_RUN")
		.Init_Start(this, &CKena_State::Start_Bow_Run)
		.Init_Changer(L"BOW_RELEASE_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_RELEASE_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_RELEASE_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_RELEASE_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_RELEASE_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_RELEASE_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_RELEASE_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_RELEASE_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_RELEASE_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RELEASE_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Release_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release_Run)
		.Init_End(this, &CKena_State::End_Bow_Release_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RECHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge)
		.Init_End(this, &CKena_State::End_Bow_Recharge)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RECHARGE_RUN")
		.Init_Start(this, &CKena_State::Start_Bow_Run)
		.Init_Changer(L"BOW_RECHARGE_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_RECHARGE_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_RECHARGE_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_RECHARGE_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_RECHARGE_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_RECHARGE_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_RECHARGE_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_RECHARGE_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		
		.Add_State(L"BOW_RECHARGE_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RECHARGE_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge_Run)
		.Init_End(this, &CKena_State::End_Bow_Recharge_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Check_PipCount, &CKena_State::KeyDown_R)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RELEASE_RUN", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RETURN")
		.Init_Start(this, &CKena_State::Start_Bow_Return)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return)
		.Init_End(this, &CKena_State::End_Bow_Return)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RETURN_RUN")
		.Init_Start(this, &CKena_State::Start_Bow_Run)
		.Init_Changer(L"BOW_RETURN_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_RETURN_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_RETURN_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_RETURN_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_RETURN_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_RETURN_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_RETURN_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_RETURN_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		
		.Add_State(L"BOW_RETURN_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_RETURN_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Return_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return_Run)
		.Init_End(this, &CKena_State::End_Bow_Return_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bow_Inject()
{
	m_pStateMachine->Add_State(L"BOW_INJECT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject)
		.Init_End(this, &CKena_State::End_Bow_Inject)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk)
		.Init_Changer(L"BOW_INJECT_WALK_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_INJECT_WALK_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_INJECT_WALK_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_INJECT_WALK_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_INJECT_WALK_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_INJECT_WALK_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_INJECT_WALK_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_INJECT_WALK_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		
		.Add_State(L"BOW_INJECT_WALK_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)
		
		.Add_State(L"BOW_INJECT_WALK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Walk_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_LOOP")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Loop)
		.Init_End(this, &CKena_State::End_Bow_Inject_Loop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_INJECT_LOOP_WALK_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_LOOP_WALK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Walk)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_LOOP_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_LOOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_INJECT_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Release)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Loop_Walk)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Backward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_LEFT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Left)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_INJECT_RELEASE_WALK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Bow_Inject_Release_Walk_Right)
		.Init_Tick(this, &CKena_State::Tick_Bow_Inject_Walk)
		.Init_End(this, &CKena_State::End_Bow_Inject_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_INJECT_RELEASE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"BOW_INJECT_RELEASE", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"BOW_RECHARGE_RUN", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_RETURN_RUN", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bow_Air()
{
	m_pStateMachine->Add_State(L"BOW_AIR_CHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Air_Charge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Air_Charge)
		.Init_End(this, &CKena_State::End_Bow_Air_Charge)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)
		.Init_Changer(L"BOW_AIR_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_AIR_CHARGE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_AIR_CHARGE_LOOP")
		.Init_Start(this, &CKena_State::Start_Bow_Air_Charge_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bow_Air_Charge_Loop)
		.Init_End(this, &CKena_State::End_Bow_Air_Charge_Loop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_AIR_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)

		.Add_State(L"BOW_AIR_RECHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Air_Recharge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Air_Recharge)
		.Init_End(this, &CKena_State::End_Bow_Air_Recharge)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_AIR_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)
		.Init_Changer(L"BOW_AIR_CHARGE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_AIR_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bow_Air_Release)
		.Init_Tick(this, &CKena_State::Tick_Bow_Air_Release)
		.Init_End(this, &CKena_State::End_Bow_Air_Release)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_AIR_RECHARGE", this, &CKena_State::MouseInput_Left, &CKena_State::Check_ArrowCount)
		.Init_Changer(L"BOW_AIR_RETURN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)

		.Add_State(L"BOW_AIR_RETURN")
		.Init_Start(this, &CKena_State::Start_Bow_Air_Return)
		.Init_Tick(this, &CKena_State::Tick_Bow_Air_Return)
		.Init_End(this, &CKena_State::End_Bow_Air_Return)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_LAND", this, &CKena_State::OnGround)
		.Init_Changer(L"AIM_AIR_LOOP", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Combat()
{
	m_pStateMachine->Add_State(L"IDLE_INTO_LOCK_ON")
		.Init_Start(this, &CKena_State::Start_Idle_Into_Lock_On)
		.Init_Tick(this, &CKena_State::Tick_Idle_Into_Lock_On)
		.Init_End(this, &CKena_State::End_Idle_Into_Lock_On)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"LOCK_ON_IDLE")
		.Init_Start(this, &CKena_State::Start_Lock_On_Idle)
		.Init_Tick(this, &CKena_State::Tick_Lock_On_Idle)
		.Init_End(this, &CKena_State::End_Lock_On_Idle)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_TO_IDLE", this, &CKena_State::CombatTimeToIdle)

		.Add_State(L"LOCK_ON_TO_IDLE")
		.Init_Start(this, &CKena_State::Start_Lock_On_To_Idle)
		.Init_Tick(this, &CKena_State::Tick_Lock_On_To_Idle)
		.Init_End(this, &CKena_State::End_Lock_On_To_Idle)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"COMBAT_IDLE_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Combat_Idle_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Combat_Idle_Into_Run)
		.Init_End(this, &CKena_State::End_Combat_Idle_Into_Run)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"COMBAT_RUN")
		.Init_Start(this, &CKena_State::Start_Combat_Run)
		.Init_Tick(this, &CKena_State::Tick_Combat_Run)
		.Init_End(this, &CKena_State::End_Combat_Run)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Crouch()
{
	m_pStateMachine->Add_State(L"CROUCH_TO_IDLE")
		.Init_Start(this, &CKena_State::Start_Crouch_To_Idle)
		.Init_Tick(this, &CKena_State::Tick_Crouch_To_Idle)
		.Init_End(this, &CKena_State::End_Crouch_To_Idle)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"CROUCH_TO_RUN")
		.Init_Start(this, &CKena_State::Start_Crouch_To_Run)
		.Init_Tick(this, &CKena_State::Tick_Crouch_To_Run)
		.Init_End(this, &CKena_State::End_Crouch_To_Run)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Damaged_Common()
{
	m_pStateMachine->Add_State(L"TAKE_DAMAGE")
		.Init_Changer(L"TAKE_DAMAGE_FRONT", this, &CKena_State::Damaged_Dir_Front)
		.Init_Changer(L"TAKE_DAMAGE_BACK", this, &CKena_State::Damaged_Dir_Back)
		.Init_Changer(L"TAKE_DAMAGE_LEFT", this, &CKena_State::Damaged_Dir_Left)
		.Init_Changer(L"TAKE_DAMAGE_RIGHT", this, &CKena_State::Damaged_Dir_Right)

		.Add_State(L"TAKE_DAMAGE_FRONT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Front)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Front)
		.Init_End(this, &CKena_State::End_Take_Damage_Front)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_BACK")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Back)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Back)
		.Init_End(this, &CKena_State::End_Take_Damage_Back)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_LEFT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Left)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Left)
		.Init_End(this, &CKena_State::End_Take_Damage_Left)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_RIGHT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Right)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Right)
		.Init_End(this, &CKena_State::End_Take_Damage_Right)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Damaged_Heavy()
{
	m_pStateMachine->Add_State(L"TAKE_DAMAGE_HEAVY_FRONT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Heavy_Front)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Heavy_Front)
		.Init_End(this, &CKena_State::End_Take_Damage_Heavy_Front)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_HEAVY_BACK")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Heavy_Back)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Heavy_Back)
		.Init_End(this, &CKena_State::End_Take_Damage_Heavy_Back)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_HEAVY_AIR")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Heavy_Air)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Heavy_Air)
		.Init_End(this, &CKena_State::End_Take_Damage_Heavy_Air)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Pulse()
{
	m_pStateMachine->Add_State(L"INTO_PULSE")
		.Init_Start(this, &CKena_State::Start_Into_Pulse)
		.Init_Tick(this, &CKena_State::Tick_Into_Pulse)
		.Init_End(this, &CKena_State::End_Into_Pulse)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"INTO_PULSE_FROM_RUN")
		.Init_Start(this, &CKena_State::Start_Into_Pulse_From_Run)
		.Init_Tick(this, &CKena_State::Tick_Into_Pulse_From_Run)
		.Init_End(this, &CKena_State::End_Into_Pulse_From_Run)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE")
		.Init_Start(this, &CKena_State::Start_Pulse)
		.Init_Tick(this, &CKena_State::Tick_Pulse)
		.Init_End(this, &CKena_State::End_Pulse)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE_INTO_RUN", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_INTO_IDLE", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_LOOP")
		.Init_Start(this, &CKena_State::Start_Pulse_Loop)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Loop)
		.Init_End(this, &CKena_State::End_Pulse_Loop)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::KeyInput_Direction)

		.Add_State(L"PULSE_INTO_COMBAT_END")
		.Init_Start(this, &CKena_State::Start_Pulse_Into_Combat_End)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Into_Combat_End)
		.Init_End(this, &CKena_State::End_Pulse_Into_Combat_End)

		.Add_State(L"PULSE_INTO_IDLE")
		.Init_Start(this, &CKena_State::Start_Pulse_Into_Idle)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Into_Idle)
		.Init_End(this, &CKena_State::End_Pulse_Into_Idle)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Pulse_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Into_Run)
		.Init_End(this, &CKena_State::End_Pulse_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_PARRY")
		.Init_Start(this, &CKena_State::Start_Pulse_Parry)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Parry)
		.Init_End(this, &CKena_State::End_Pulse_Parry)
		.Init_Changer(L"SPIN_ATTACK", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.35f)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE_WALK")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_WALK_FRONT_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"PULSE_WALK_FRONT_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"PULSE_WALK_BACK_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"PULSE_WALK_BACK_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"PULSE_WALK_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"PULSE_WALK_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"PULSE_WALK_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"PULSE_WALK_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_FORWARD")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Forward)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_FRONT_LEFT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Front_Left)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_FRONT_RIGHT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Front_Right)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Backward)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_BACK_LEFT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Back_Left)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_BACK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Back_Right)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_LEFT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Left)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Right)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"SHIELD_BREAK_FRONT", this, &CKena_State::Shield_Break_Front)
		.Init_Changer(L"SHIELD_BREAK_BACK", this, &CKena_State::Shield_Break_Back)
		.Init_Changer(L"SHIELD_IMPACT_MEDIUM", this, &CKena_State::Shield_Medium)
		.Init_Changer(L"SHIELD_IMPACT_BIG", this, &CKena_State::Shield_Big)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"PULSE_SQUAT_SPRINT", this, &CKena_State::KeyUp_E, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_SQUAT_SPRINT")
		.Init_Start(this, &CKena_State::Start_Pulse_Squat_Sprint)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Squat_Sprint)
		.Init_End(this, &CKena_State::End_Pulse_Squat_Sprint)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_COMBO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"SPRINT_ATTACK", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"SPRINT_STOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"SPRINT", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Shield()
{
	m_pStateMachine->Add_State(L"SHIELD_IMPACT")
		.Init_Start(this, &CKena_State::Start_Shield_Impact)
		.Init_Tick(this, &CKena_State::Tick_Shield_Impact)
		.Init_End(this, &CKena_State::End_Shield_Impact)

		.Add_State(L"SHIELD_IMPACT_MEDIUM")
		.Init_Start(this, &CKena_State::Start_Shield_Impact_Medium)
		.Init_Tick(this, &CKena_State::Tick_Shield_Impact_Medium)
		.Init_End(this, &CKena_State::End_Shield_Impact_Medium)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"SHIELD_IMPACT_BIG")
		.Init_Start(this, &CKena_State::Start_Shield_Impact_Big)
		.Init_Tick(this, &CKena_State::Tick_Shield_Impact_Big)
		.Init_End(this, &CKena_State::End_Shield_Impact_Big)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"SHIELD_BREAK_FRONT")
		.Init_Start(this, &CKena_State::Start_Shield_Break_Front)
		.Init_Tick(this, &CKena_State::Tick_Shield_Break_Front)
		.Init_End(this, &CKena_State::End_Shield_Break_Front)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"SHIELD_BREAK_BACK")
		.Init_Start(this, &CKena_State::Start_Shield_Break_Back)
		.Init_Tick(this, &CKena_State::Tick_Shield_Break_Back)
		.Init_End(this, &CKena_State::End_Shield_Break_Back)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Spin_Attack()
{
	m_pStateMachine->Add_State(L"SPIN_ATTACK")
		.Init_Start(this, &CKena_State::Start_Spin_Attack)
		.Init_Tick(this, &CKena_State::Tick_Spin_Attack)
		.Init_End(this, &CKena_State::End_Spin_Attack)
		.Init_Changer(L"SPIN_ATTACK_RETURN", this, &CKena_State::TruePass, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)

		.Add_State(L"SPIN_ATTACK_RETURN")
		.Init_Start(this, &CKena_State::Start_Spin_Attack_Return)
		.Init_Tick(this, &CKena_State::Tick_Spin_Attack_Return)
		.Init_End(this, &CKena_State::End_Spin_Attack_Return)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Sprint()
{
	m_pStateMachine->Add_State(L"INTO_SPRINT")
		.Init_Start(this, &CKena_State::Start_Into_Sprint)
		.Init_Tick(this, &CKena_State::Tick_Into_Sprint)
		.Init_End(this, &CKena_State::End_Into_Sprint)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"SPRINT_ATTACK", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_COMBO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"SPRINT_STOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"SPRINT", this, &CKena_State::Animation_Finish)

		.Add_State(L"SPRINT")
		.Init_Start(this, &CKena_State::Start_Sprint)
		.Init_Tick(this, &CKena_State::Tick_Sprint)
		.Init_End(this, &CKena_State::End_Sprint)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"HEAVY_ATTACK_COMBO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"SPRINT_ATTACK", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"SPRINT_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"SPRINT_STOP")
		.Init_Start(this, &CKena_State::Start_Sprint_Stop)
		.Init_Tick(this, &CKena_State::Tick_Sprint_Stop)
		.Init_End(this, &CKena_State::End_Sprint_Stop)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"SPRINT_LEAN_LEFT")
		.Init_Start(this, &CKena_State::Start_Sprint_Lean_Left)
		.Init_Tick(this, &CKena_State::Tick_Sprint_Lean_Left)
		.Init_End(this, &CKena_State::End_Sprint_Lean_Left)

		.Add_State(L"SPRINT_LEAN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Sprint_Lean_Right)
		.Init_Tick(this, &CKena_State::Tick_Sprint_Lean_Right)
		.Init_End(this, &CKena_State::End_Sprint_Lean_Right)

		.Add_State(L"SPRINT_TURN_180")
		.Init_Start(this, &CKena_State::Start_Sprint_Turn_180)
		.Init_Tick(this, &CKena_State::Tick_Sprint_Turn_180)
		.Init_End(this, &CKena_State::End_Sprint_Turn_180)

		.Add_State(L"SPRINT_ATTACK")
		.Init_Start(this, &CKena_State::Start_Sprint_Attack)
		.Init_Tick(this, &CKena_State::Tick_Sprint_Attack)
		.Init_End(this, &CKena_State::End_Sprint_Attack)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"ATTACK_4_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_4_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_TeleportFlower()
{
	m_pStateMachine->Add_State(L"TELEPORT_FLOWER")
		.Init_Start(this, &CKena_State::Start_Teleport_Flower)
		.Init_Tick(this, &CKena_State::Tick_Teleport_Flower)
		.Init_End(this, &CKena_State::End_Teleport_Flower)
		.Init_Changer(L"FALL", this, &CKena_State::TruePass, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.34f)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Dodge()
{
	m_pStateMachine->Add_State(L"BACKFLIP")
		.Init_Start(this, &CKena_State::Start_Backflip)
		.Init_Tick(this, &CKena_State::Tick_Backflip)
		.Init_End(this, &CKena_State::End_Backflip)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.42f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.42f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.42f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.42f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.42f)
		.Init_Changer(L"ROLL_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.42f)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"ROLL")
		.Init_Start(this, &CKena_State::Start_Roll)
		.Init_Tick(this, &CKena_State::Tick_Roll)
		.Init_End(this, &CKena_State::End_Roll)
		.Init_Changer(L"ROLL_INTO_IDLE", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_None)
		.Init_Changer(L"ROLL_INTO_RUN", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_Direction)

		.Add_State(L"ROLL_LEFT")
		.Init_Start(this, &CKena_State::Start_Roll_Left)
		.Init_Tick(this, &CKena_State::Tick_Roll_Left)
		.Init_End(this, &CKena_State::End_Roll_Left)

		.Add_State(L"ROLL_RIGHT")
		.Init_Start(this, &CKena_State::Start_Roll_Right)
		.Init_Tick(this, &CKena_State::Tick_Roll_Right)
		.Init_End(this, &CKena_State::End_Roll_Right)

		.Add_State(L"ROLL_INTO_IDLE")
		.Init_Start(this, &CKena_State::Start_Roll_Into_Idle)
		.Init_Tick(this, &CKena_State::Tick_Roll_Into_Idle)
		.Init_End(this, &CKena_State::End_Roll_Into_Idle)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"ROLL_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"ROLL_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Roll_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Roll_Into_Run)
		.Init_End(this, &CKena_State::End_Roll_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ROLL_INTO_WALK")
		.Init_Start(this, &CKena_State::Start_Roll_Into_Walk)
		.Init_Tick(this, &CKena_State::Tick_Roll_Into_Walk)
		.Init_End(this, &CKena_State::End_Roll_Into_Walk)

		.Add_State(L"ROLL_INTO_FALL")
		.Init_Start(this, &CKena_State::Start_Roll_Into_Fall)
		.Init_Tick(this, &CKena_State::Tick_Roll_Into_Fall)
		.Init_End(this, &CKena_State::End_Roll_Into_Fall)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Fall()
{
	m_pStateMachine->Add_State(L"FALL")
		.Init_Start(this, &CKena_State::Start_Fall)
		.Init_Tick(this, &CKena_State::Tick_Fall)
		.Init_End(this, &CKena_State::End_Fall)
		.Init_Changer(L"AIM_AIR_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"AIR_ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIR_ATTACK_SLAM_INTO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"PULSE_JUMP", this, &CKena_State::KeyDown_Space, &CKena_State::Pulse_Jump)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)

		.Add_State(L"FALL_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Fall_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Fall_Into_Run)
		.Init_End(this, &CKena_State::End_Fall_Into_Run)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Heavy_Attack1()
{
	m_pStateMachine->Add_State(L"HEAVY_ATTACK_1_CHARGE")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Charge)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Charge)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Charge)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"HEAVY_ATTACK_1_RELEASE_PERFECT", this, &CKena_State::MouseUp_Right, &CKena_State::HeavyAttack1_Perfect)
		.Init_Changer(L"HEAVY_ATTACK_1_RELEASE", this, &CKena_State::NoMouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"HEAVY_ATTACK_1_RELEASE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_1_RELEASE")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Release)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Release)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Release)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.35f)
		.Init_Changer(L"HEAVY_ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)

		.Add_State(L"HEAVY_ATTACK_1_RELEASE_PERFECT")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Release_Perfect)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Release_Perfect)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Release_Perfect)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.35f)
		.Init_Changer(L"HEAVY_ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)

		.Add_State(L"HEAVY_ATTACK_1_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_1_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Into_Run)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Heavy_Attack2()
{
	m_pStateMachine->Add_State(L"HEAVY_ATTACK_2_CHARGE")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Charge)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Charge)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Charge)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.235f)
		.Init_Changer(L"HEAVY_ATTACK_2_RELEASE_PERFECT", this, &CKena_State::MouseUp_Right, &CKena_State::HeavyAttack2_Perfect)
		.Init_Changer(L"HEAVY_ATTACK_2_RELEASE", this, &CKena_State::NoMouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.235f)
		.Init_Changer(L"HEAVY_ATTACK_2_RELEASE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_2_RELEASE")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Release)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Release)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Release)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_3", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_3_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_2_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_2_RELEASE_PERFECT")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Release_Perfect)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Release_Perfect)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Release_Perfect)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_3", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_3_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_2_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_2_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_2_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Into_Run)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Heavy_Attack3()
{
	m_pStateMachine->Add_State(L"HEAVY_ATTACK_3_CHARGE")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_3_Charge)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_3_Charge)
		.Init_End(this, &CKena_State::End_Heavy_Attack_3_Charge)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.36f)
		.Init_Changer(L"HEAVY_ATTACK_3_RELEASE_PERFECT", this, &CKena_State::MouseUp_Right, &CKena_State::HeavyAttack3_Perfect)
		.Init_Changer(L"HEAVY_ATTACK_3_RELEASE", this, &CKena_State::NoMouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.36f)
		.Init_Changer(L"HEAVY_ATTACK_3_RELEASE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_3_RELEASE")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_3_Release)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_3_Release)
		.Init_End(this, &CKena_State::End_Heavy_Attack_3_Release)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_3_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_3_RELEASE_PERFECT")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_3_Release_Perfect)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_3_Release_Perfect)
		.Init_End(this, &CKena_State::End_Heavy_Attack_3_Release_Perfect)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_3_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_3_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_3_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_3_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_3_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Heavy_Attack_Combo()
{
	m_pStateMachine->Add_State(L"HEAVY_ATTACK_COMBO")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_Combo)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_Combo)
		.Init_End(this, &CKena_State::End_Heavy_Attack_Combo)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"HEAVY_ATTACK_COMBO_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"HEAVY_ATTACK_COMBO_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)
		
		.Add_State(L"HEAVY_ATTACK_COMBO_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_Combo_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_Combo_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_Combo_Return)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_COMBO_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_Combo_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_Combo_Into_Run)
		.Init_End(this, &CKena_State::End_Heavy_Attack_Combo_Into_Run)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Interact()
{
	m_pStateMachine->Add_State(L"INTERACT")
		.Init_Start(this, &CKena_State::Start_Interact)
		.Init_Tick(this, &CKena_State::Tick_Interact)
		.Init_End(this, &CKena_State::End_Interact)

		.Add_State(L"INTERACT_STAFF")
		.Init_Start(this, &CKena_State::Start_Interact_Staff)
		.Init_Tick(this, &CKena_State::Tick_Interact_Staff)
		.Init_End(this, &CKena_State::End_Interact_Staff)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Jump()
{
	m_pStateMachine->Add_State(L"JUMP_SQUAT")
		.Init_Start(this, &CKena_State::Start_Jump_Squat)
		.Init_Tick(this, &CKena_State::Tick_Jump_Squat)
		.Init_End(this, &CKena_State::End_Jump_Squat)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"JUMP", this, &CKena_State::Animation_Finish)

		.Add_State(L"JUMP")
		.Init_Start(this, &CKena_State::Start_Jump)
		.Init_Tick(this, &CKena_State::Tick_Jump)
		.Init_End(this, &CKena_State::End_Jump)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_AIR_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"AIR_ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIR_ATTACK_SLAM_INTO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"PULSE_JUMP", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"FALL", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)

		.Add_State(L"RUNNING_JUMP_SQUAT")
		.Init_Start(this, &CKena_State::Start_Running_Jump_Squat)
		.Init_Tick(this, &CKena_State::Tick_Running_Jump_Squat)
		.Init_End(this, &CKena_State::End_Running_Jump_Squat)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"RUNNING_JUMP", this, &CKena_State::Animation_Finish)

		.Add_State(L"RUNNING_JUMP")
		.Init_Start(this, &CKena_State::Start_Running_Jump)
		.Init_Tick(this, &CKena_State::Tick_Running_Jump)
		.Init_End(this, &CKena_State::End_Running_Jump)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_AIR_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"AIR_ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIR_ATTACK_SLAM_INTO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"PULSE_JUMP", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"FALL", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_JUMP")
		.Init_Start(this, &CKena_State::Start_Pulse_Jump)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Jump)
		.Init_End(this, &CKena_State::End_Pulse_Jump)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_AIR_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"AIR_ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIR_ATTACK_SLAM_INTO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"LAND_RUNNING", this, &CKena_State::OnGround, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Land()
{
	m_pStateMachine->Add_State(L"LAND")
		.Init_Start(this, &CKena_State::Start_Land)
		.Init_Tick(this, &CKena_State::Tick_Land)
		.Init_End(this, &CKena_State::End_Land)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"LAND_HEAVY")
		.Init_Start(this, &CKena_State::Start_Land_Heavy)
		.Init_Tick(this, &CKena_State::Tick_Land_Heavy)
		.Init_End(this, &CKena_State::End_Land_Heavy)

		.Add_State(L"LAND_WALKING")
		.Init_Start(this, &CKena_State::Start_Land_Walking)
		.Init_Tick(this, &CKena_State::Tick_Land_Walking)
		.Init_End(this, &CKena_State::End_Land_Walking)

		.Add_State(L"LAND_RUNNING")
		.Init_Start(this, &CKena_State::Start_Land_Running)
		.Init_Tick(this, &CKena_State::Tick_Land_Running)
		.Init_End(this, &CKena_State::End_Land_Running)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"PULSE_PARRY", this, &CKena_State::Parry, &CKena_State::KeyDown_E)
		.Init_Changer(L"INTERACT_STAFF", this, &CKena_State::KeyDown_Q, &CKena_State::Interactable)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E, &CKena_State::Check_Shield)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_LAND")
		.Init_Start(this, &CKena_State::Start_Bow_Land)
		.Init_Tick(this, &CKena_State::Tick_Bow_Land)
		.Init_End(this, &CKena_State::End_Bow_Land)
		.Init_Changer(L"TELEPORT_FLOWER", this, &CKena_State::Teleport_Flower)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_None)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.2f)
		//.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_None)
		//.Init_Changer(L"BOW_CHARGE_FULL_RUN", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_Direction)

		.Add_State(L"LEDGE_LAND")
		.Init_Start(this, &CKena_State::Start_Ledge_Land)
		.Init_Tick(this, &CKena_State::Tick_Ledge_Land)
		.Init_End(this, &CKena_State::End_Ledge_Land)

		.Add_State(L"SLIDE_LAND")
		.Init_Start(this, &CKena_State::Start_Slide_Land)
		.Init_Tick(this, &CKena_State::Tick_Slide_Land)
		.Init_End(this, &CKena_State::End_Slide_Land)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_LevelUp()
{
	m_pStateMachine->Add_State(L"LEVEL_UP")
		.Init_Start(this, &CKena_State::Start_Level_Up)
		.Init_Tick(this, &CKena_State::Tick_Level_Up)
		.Init_End(this, &CKena_State::End_Level_Up)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Mask()
{
	m_pStateMachine->Add_State(L"MASK_ON")
		.Init_Start(this, &CKena_State::Start_Mask_On)
		.Init_Tick(this, &CKena_State::Tick_Mask_On)
		.Init_End(this, &CKena_State::End_Mask_On)
		.Init_Changer(L"MASK_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"MASK_LOOP")
		.Init_Start(this, &CKena_State::Start_Mask_Loop)
		.Init_Tick(this, &CKena_State::Tick_Mask_Loop)
		.Init_End(this, &CKena_State::End_Mask_Loop)
		.Init_Changer(L"MASK_OFF", this, &CKena_State::KeyDown_T)

		.Add_State(L"MASK_OFF")
		.Init_Start(this, &CKena_State::Start_Mask_Off)
		.Init_Tick(this, &CKena_State::Tick_Mask_Off)
		.Init_End(this, &CKena_State::End_Mask_Off)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"MASK_OFF_MEMORY_COLLECT")
		.Init_Start(this, &CKena_State::Start_Mask_Off_Memory_Collect)
		.Init_Tick(this, &CKena_State::Tick_Mask_Off_Memory_Collect)
		.Init_End(this, &CKena_State::End_Mask_Off_Memory_Collect)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Meditate()
{
	m_pStateMachine->Add_State(L"MEDITATE_INTO")
		.Init_Start(this, &CKena_State::Start_Meditate_Into)
		.Init_Tick(this, &CKena_State::Tick_Meditate_Into)
		.Init_End(this, &CKena_State::End_Meditate_Into)
		.Init_Changer(L"MEDITATE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"MEDITATE_LOOP")
		.Init_Start(this, &CKena_State::Start_Meditate_Loop)
		.Init_Tick(this, &CKena_State::Tick_Meditate_Loop)
		.Init_End(this, &CKena_State::End_Meditate_Loop)
		.Init_Changer(L"MEDITATE_EXIT", this, &CKena_State::KeyDown_BackSpace)

		.Add_State(L"MEDITATE_EXIT")
		.Init_Start(this, &CKena_State::Start_Meditate_Exit)
		.Init_Tick(this, &CKena_State::Tick_Meditate_Exit)
		.Init_End(this, &CKena_State::End_Meditate_Exit)
		.Init_Changer(L"CROUCH_TO_RUN", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"CROUCH_TO_IDLE", this, &CKena_State::Animation_Finish, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

void CKena_State::Start_Idle(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)RUN_STOP ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ROLL_INTO_IDLE ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)LOCK_ON_TO_IDLE ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)INTERACT ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)INTERACT_STAFF)
		m_pAnimationState->State_Animation("IDLE", 0.f);
	else
		m_pAnimationState->State_Animation("IDLE");
}

void CKena_State::Start_Run(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ATTACK_1_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ATTACK_2_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ATTACK_3_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ATTACK_4_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)HEAVY_ATTACK_1_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)HEAVY_ATTACK_2_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)HEAVY_ATTACK_COMBO_INTO_RUN ||
		//m_pAnimationState->Get_CurrentAnimIndex() == (_uint)PULSE_INTO_RUN ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ROLL_INTO_RUN)
		m_pAnimationState->State_Animation("RUN", 0.f);
	else
		m_pAnimationState->State_Animation("RUN");
}

void CKena_State::Start_Run_Stop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("RUN_STOP");
}

void CKena_State::Start_Aim_Into(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_INTO");

	m_pKena->m_bAim = true;

	/* Turn On Canvas Aim */
	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	//_float fTag = 1.f;
	//m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);
}

void CKena_State::Start_Aim_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_LOOP");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RETURN");

	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;

	/* Turn Off Canvas Aim */
	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	//_float fTag = 0.f;
	//m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);
}

void CKena_State::Start_Aim_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_FORWARD");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_BACKWARD");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_LEFT");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_RIGHT");

	m_pKena->m_bAim = true;
}

void CKena_State::Start_Aim_Air_Into(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_AIR_INTO");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Aim_Air_Loop(_float fTimeDetla)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)AIM_AIR_INTO)
		m_pAnimationState->State_Animation("AIM_AIR_LOOP", 0.f);
	else
		m_pAnimationState->State_Animation("AIM_AIR_LOOP");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Air_Attack_1(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIR_ATTACK_1");
}

void CKena_State::Start_Air_Attack_2(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIR_ATTACK_2");
}

void CKena_State::Start_Air_Attack_Slam_Into(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIR_ATTACK_SLAM_INTO");

	m_pKena->m_fCurJumpSpeed = m_pKena->m_fInitJumpSpeed;
}

void CKena_State::Start_Air_Attack_Slam_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIR_ATTACK_SLAM_LOOP");

	m_pKena->m_fCurJumpSpeed = 0.f;
}

void CKena_State::Start_Air_Attack_Slam_Finish(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIR_ATTACK_SLAM_FINISH");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Attack_1(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1");

	Move(0.f, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Start_Attack_1_From_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1_FROM_RUN");

	Move(0.f, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Start_Attack_1_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1_INTO_RUN");
}

void CKena_State::Start_Attack_1_Into_Walk(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1_INTO_WALK");
}

void CKena_State::Start_Attack_1_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1_RETURN");
}

void CKena_State::Start_Attack_2(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_2");
	Move(0.f, m_eDir);
}

void CKena_State::Start_Attack_2_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_2_INTO_RUN");
}

void CKena_State::Start_Attack_2_Into_Walk(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_2_INTO_WALK");
}

void CKena_State::Start_Attack_2_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_2_RETURN");
}

void CKena_State::Start_Attack_3(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_3");
	Move(0.f, m_eDir);
}

void CKena_State::Start_Attack_3_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_3_INTO_RUN");
}

void CKena_State::Start_Attack_3_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_3_RETURN");
}

void CKena_State::Start_Attack_4(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_4");
	Move(0.f, m_eDir);
}

void CKena_State::Start_Attack_4_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_4_INTO_RUN");
}

void CKena_State::Start_Attack_4_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_4_RETURN");
}

void CKena_State::Start_Bomb_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bJump = false;
}

void CKena_State::Start_Bomb_Into(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Into_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INTO_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)BOMB_INTO_ADD)
		m_pAnimationState->State_Animation("BOMB_LOOP", 0.f);
	else
		m_pAnimationState->State_Animation("BOMB_LOOP");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bJump = false;
}

void CKena_State::Start_Bomb_Loop_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Loop_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_LOOP_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Release(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)BOMB_LOOP_ADD)
		m_pAnimationState->State_Animation("BOMB_RELEASE", 0.f);
	else
		m_pAnimationState->State_Animation("BOMB_RELEASE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
	m_pKena->m_bJump = false;
}

void CKena_State::Start_Bomb_Release_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Release_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_RELEASE_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
	m_pKena->m_bJump = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Cancel_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_CANCEL_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Inject(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;
	m_pKena->m_bJump = false;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Inject_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_INJECT_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
	m_pKena->m_bInjectBomb = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bomb_Air_Into(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_AIR_INTO");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Air_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_AIR_LOOP");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = true;
}

void CKena_State::Start_Bomb_Air_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_AIR_RELEASE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bomb_Air_Cancel(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOMB_AIR_CANCEL");

	m_pKena->m_bAim = true;
	m_pKena->m_bBomb = false;
}

void CKena_State::Start_Bow_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Full(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimName() == "BOW_CHARGE")
		m_pAnimationState->State_Animation("BOW_CHARGE_FULL");
	else
		m_pAnimationState->State_Animation("BOW_CHARGE_FULL", 0.2f);

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Full_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Charge_Loop_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Release_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Return_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN_RUN_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Inject(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk(_float fTimeDelta)
{
	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Walk_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_WALK_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Bow_Inject_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk(_float fTimeDelta)
{
	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Loop_Walk_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_LOOP_WALK_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_FORWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_FORWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_FORWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_BACKWARD");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_BACKWARD_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_BACKWARD_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_LEFT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Inject_Release_Walk_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_INJECT_RELEASE_WALK_RIGHT");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = true;
}

void CKena_State::Start_Bow_Air_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_AIR_CHARGE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Air_Charge_Loop(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)BOW_AIR_CHARGE_ADD ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)BOW_AIR_RECHARGE_ADD)
		m_pAnimationState->State_Animation("BOW_AIR_CHARGE_LOOP", 0.f);
	else
		m_pAnimationState->State_Animation("BOW_AIR_CHARGE_LOOP");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Air_Recharge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_AIR_RECHARGE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = true;
}

void CKena_State::Start_Bow_Air_Release(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)BOW_AIR_CHARGE_LOOP_ADD)
		m_pAnimationState->State_Animation("BOW_AIR_RELEASE", 0.f);
	else
		m_pAnimationState->State_Animation("BOW_AIR_RELEASE");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Bow_Air_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_AIR_RETURN");

	m_pKena->m_bAim = true;
	m_pKena->m_bBow = false;
}

void CKena_State::Start_Idle_Into_Lock_On(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("IDLE_INTO_LOCK_ON");
}

void CKena_State::Start_Lock_On_Idle(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)IDLE_INTO_LOCK_ON ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)BACKFLIP ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ROLL_INTO_IDLE)
		m_pAnimationState->State_Animation("LOCK_ON_IDLE", 0.f);
	else
		m_pAnimationState->State_Animation("LOCK_ON_IDLE");

	m_fCombatIdleTime = 0.f;
}

void CKena_State::Start_Lock_On_To_Idle(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LOCK_ON_TO_IDLE");
}

void CKena_State::Start_Combat_Idle_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("COMBAT_IDLE_INTO_RUN");
}

void CKena_State::Start_Combat_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("COMBAT_RUN_ADDITIVE_POSE");
}

void CKena_State::Start_Crouch_To_Idle(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("CROUCH_TO_IDLE");
}

void CKena_State::Start_Crouch_To_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("CROUCH_TO_RUN");
}

void CKena_State::Start_Take_Damage_Front(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_FRONT");
}

void CKena_State::Start_Take_Damage_Back(_float fTImeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_BACK");
}

void CKena_State::Start_Take_Damage_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_LEFT");
}

void CKena_State::Start_Take_Damage_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_RIGHT");
}

void CKena_State::Start_Take_Damage_Heavy_Front(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_HEAVY_FRONT");
}

void CKena_State::Start_Take_Damage_Heavy_Back(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_HEAVY_BACK");
}

void CKena_State::Start_Take_Damage_Heavy_Air(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TAKE_DAMAGE_HEAVY_AIR");
}

void CKena_State::Start_Backflip(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BACKFLIP");
}

void CKena_State::Start_Roll(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL");
	Move(0.f, m_eDir);
}

void CKena_State::Start_Roll_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL_LEFT");
}

void CKena_State::Start_Roll_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL_RIGHT");
}

void CKena_State::Start_Roll_Into_Idle(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL_INTO_IDLE");
	Move(0.f, m_eDir);
}

void CKena_State::Start_Roll_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL_INTO_RUN");
}

void CKena_State::Start_Roll_Into_Walk(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL_INTO_WALK");
}

void CKena_State::Start_Roll_Into_Fall(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ROLL_INTO_FALL");
}

void CKena_State::Start_Fall(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("FALL");
}

void CKena_State::Start_Fall_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("FALL_INTO_RUN");
}

void CKena_State::Start_Heavy_Attack_1_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_CHARGE");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Heavy_Attack_1_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_RELEASE");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Heavy_Attack_1_Release_Perfect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_RELEASE_PERFECT");

	m_pKena->m_bHeavyAttack = true;
	m_pKena->m_bPerfectAttack = true;
}

void CKena_State::Start_Heavy_Attack_1_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_RETURN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Heavy_Attack_1_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_INTO_RUN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Heavy_Attack_2_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_CHARGE");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Heavy_Attack_2_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_RELEASE");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Heavy_Attack_2_Release_Perfect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_RELEASE_PERFECT");

	m_pKena->m_bHeavyAttack = true;
	m_pKena->m_bPerfectAttack = true;
}

void CKena_State::Start_Heavy_Attack_2_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_RETURN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Heavy_Attack_2_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_INTO_RUN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Heavy_Attack_3_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_CHARGE");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Heavy_Attack_3_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_RELEASE");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Heavy_Attack_3_Release_Perfect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_RELEASE_PERFECT");

	m_pKena->m_bHeavyAttack = true;
	m_pKena->m_bPerfectAttack = true;
}

void CKena_State::Start_Heavy_Attack_3_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_RETURN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Heavy_Attack_Combo(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_COMBO");

	m_pKena->m_bHeavyAttack = true;
	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Heavy_Attack_Combo_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_COMBO_RETURN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Heavy_Attack_Combo_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_COMBO_INTO_RUN");

	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::Start_Interact(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTERACT");
}

void CKena_State::Start_Interact_Staff(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTERACT_STAFF");
}

void CKena_State::Start_Jump_Squat(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("JUMP_SQUAT");
}

void CKena_State::Start_Jump(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("JUMP");

	m_pKena->m_bJump = true;
	m_pKena->m_fCurJumpSpeed = m_pKena->m_fInitJumpSpeed;
}

void CKena_State::Start_Running_Jump_Squat(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("RUNNING_JUMP_SQUAT");
}

void CKena_State::Start_Running_Jump(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("RUNNING_JUMP");

	m_pKena->m_bJump = true;
	m_pKena->m_fCurJumpSpeed = m_pKena->m_fInitJumpSpeed;
}

void CKena_State::Start_Pulse_Jump(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_JUMP");

	m_pKena->m_bJump = true;
	m_pKena->m_bPulseJump = true;

	//if (m_pKena->m_fCurJumpSpeed < 0.f)
		m_pKena->m_fCurJumpSpeed = m_pKena->m_fInitJumpSpeed;
	//else
	//	m_pKena->m_fCurJumpSpeed += m_pKena->m_fInitJumpSpeed;
}

void CKena_State::Start_Land(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LAND");
}

void CKena_State::Start_Land_Heavy(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LAND_HEAVY");
}

void CKena_State::Start_Land_Walking(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LAND_WALKING");
}

void CKena_State::Start_Land_Running(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LAND_RUNNING");
}

void CKena_State::Start_Bow_Land(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_LAND");
}

void CKena_State::Start_Ledge_Land(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LEDGE_LAND");
}

void CKena_State::Start_Slide_Land(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SLIDE_LAND");
}

void CKena_State::Start_Level_Up(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("LEVEL_UP");
}

void CKena_State::Start_Mask_On(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MASK_ON");

	for (auto pPart : m_pKena->m_vecPart)
	{
		if (pPart->Get_PartsType() == CKena_Parts::KENAPARTS_MASK)
			pPart->Set_Active(true);
	}
}

void CKena_State::Start_Mask_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MASK_LOOP");

	m_pKena->m_bMask = true;
}

void CKena_State::Start_Mask_Off(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MASK_OFF");
}

void CKena_State::Start_Mask_Off_Memory_Collect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MASK_OFF_MEMORY_COLLECT");
}

void CKena_State::Start_Meditate_Into(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MEDITATE_INTO");
}

void CKena_State::Start_Meditate_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MEDITATE_LOOP");
}

void CKena_State::Start_Meditate_Exit(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("MEDITATE_EXIT");
}

void CKena_State::Start_Into_Pulse(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTO_PULSE");
	
	m_pKena->m_bPulse = true;

	CE_KenaPulse*	pPulse = dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"]);
	pPulse->Set_Type(CE_KenaPulse::PULSE_DEFAULT);
	pPulse->Set_Active(true);
	pPulse->Reset();
}

void CKena_State::Start_Into_Pulse_From_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTO_PULSE_FROM_RUN");

	m_pKena->m_bPulse = true;

	CE_KenaPulse*	pPulse = dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"]);
	pPulse->Set_Type(CE_KenaPulse::PULSE_DEFAULT);
	pPulse->Set_Active(true);
	pPulse->Reset();
}

void CKena_State::Start_Pulse(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE");

	m_pKena->m_bPulse = false;

	dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"])->Set_NoActive(true);
}

void CKena_State::Start_Pulse_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_LOOP");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Into_Combat_End(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_INTO_COMBAT_END");

	m_pKena->m_bPulse = false;

	dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"])->Set_NoActive(true);
}

void CKena_State::Start_Pulse_Into_Idle(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_INTO_IDLE");

	m_pKena->m_bPulse = false;

	dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"])->Set_NoActive(true);
}

void CKena_State::Start_Pulse_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_INTO_RUN");

	m_pKena->m_bPulse = false;

	dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"])->Set_NoActive(true);
}

void CKena_State::Start_Pulse_Parry(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_PARRY");

	m_pKena->m_bPulse = false;

	m_pTransform->LookAt_NoUpDown(m_pKena->m_pAttackObject->Get_WorldMatrix().r[3]);

	m_pKena->m_bParry = false;
	m_pKena->m_bParryLaunch = true;
	m_pKena->m_iCurParryFrame = m_pKena->m_iParryFrameCount;
	m_pKena->m_pAttackObject = nullptr;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.5f);
}

void CKena_State::Start_Pulse_Walk(_float fTimeDelta)
{
	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_FORWARD");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Front_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_FRONT_LEFT");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Front_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_FRONT_RIGHT");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_BACKWARD");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Back_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_BACK_LEFT");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Back_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_BACK_RIGHT");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_LEFT");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Walk_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_RIGHT");

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Pulse_Squat_Sprint(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_SQUAT_SPRINT");

	m_pKena->m_bPulse = false;
	m_pKena->m_bSprint = true;

	dynamic_cast<CE_KenaPulse*>(m_pKena->m_mapEffect["KenaPulse"])->Set_NoActive(true);
}

void CKena_State::Start_Shield_Impact(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SHIELD_IMPACT");

	m_pTransform->LookAt_NoUpDown(m_pKena->m_pAttackObject->Get_TransformCom()->Get_Position());

	m_pKena->m_bCommonHit = false;
	m_pKena->m_bHeavyHit = false;

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Shield_Impact_Medium(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SHIELD_IMPACT_MEDIUM");

	m_pTransform->LookAt_NoUpDown(m_pKena->m_pAttackObject->Get_TransformCom()->Get_Position());

	m_pKena->m_bCommonHit = false;
	m_pKena->m_bHeavyHit = false;

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Shield_Impact_Big(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SHIELD_IMPACT_BIG");

	m_pTransform->LookAt_NoUpDown(m_pKena->m_pAttackObject->Get_TransformCom()->Get_Position());

	m_pKena->m_bCommonHit = false;
	m_pKena->m_bHeavyHit = false;

	m_pKena->m_bPulse = true;
}

void CKena_State::Start_Shield_Break_Front(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SHIELD_BREAK_FRONT");

	m_pKena->m_bCommonHit = false;
	m_pKena->m_bHeavyHit = false;
	m_pKena->m_eDamagedDir = CKena::DAMAGED_FROM_END;

	m_pKena->m_bPulse = false;
	
	m_pCamera->Camera_Shake(0.005f, 5);
	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.3f);
}

void CKena_State::Start_Shield_Break_Back(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SHIELD_BREAK_BACK");

	m_pKena->m_bCommonHit = false;
	m_pKena->m_bHeavyHit = false;
	m_pKena->m_eDamagedDir = CKena::DAMAGED_FROM_END;

	m_pKena->m_bPulse = false;

	m_pCamera->Camera_Shake(0.005f, 5);
	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 0.3f);
}

void CKena_State::Start_Spin_Attack(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPIN_ATTACK");

	m_pKena->m_bHeavyAttack = true;
}

void CKena_State::Start_Spin_Attack_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPIN_ATTACK_RETURN");
}

void CKena_State::Start_Into_Sprint(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTO_SPRINT");

	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Sprint(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)INTO_SPRINT ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)PULSE_SQUAT_SPRINT)
		m_pAnimationState->State_Animation("SPRINT", 0.05f);
	else
		m_pAnimationState->State_Animation("SPRINT");

	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Sprint_Stop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPRINT_STOP");

	m_pKena->m_bSprint = false;
}

void CKena_State::Start_Sprint_Lean_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPRINT_LEAN_LEFT");

	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Sprint_Lean_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPRINT_LEAN_RIGHT");

	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Sprint_Turn_180(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPRINT_TURN_180");

	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Sprint_Attack(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("SPRINT_ATTACK");

	m_pKena->m_bSprint = true;
}

void CKena_State::Start_Teleport_Flower(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("TELEPORT_FLOWER");

	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
	m_pKena->m_bTeleportFlower = false;
	m_pKena->m_bJump = true;
}

void CKena_State::Tick_Idle(_float fTimeDelta)
{
}

void CKena_State::Tick_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Run_Stop(_float fTimeDelta)
{
}

void CKena_State::Tick_Aim_Into(_float fTimeDelta)
{

}

void CKena_State::Tick_Aim_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Aim_Return(_float fTimeDelta)
{
	/* Turn Off Canvas Aim */
	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	//_float fTag = 0.f;
	//m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);
}

void CKena_State::Tick_Aim_Run_Forward(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Forward_Left(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Forward_Right(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Backward(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Backward_Left(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Backward_Right(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Left(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Run_Right(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Air_Into(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	//m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;
	m_pKena->m_fCurJumpSpeed -= fTimeDelta;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Aim_Air_Loop(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	//m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;
	m_pKena->m_fCurJumpSpeed -= fTimeDelta;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Air_Attack_1(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Air_Attack_2(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Air_Attack_Slam_Into(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Air_Attack_Slam_Loop(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	
	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Air_Attack_Slam_Finish(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_From_Run(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Attack_1_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_2(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_2_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Attack_2_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_2_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_3(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_3_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Attack_3_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_4(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_4_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Attack_4_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Bomb_Into(_float fTimeDelta)
{
}

void CKena_State::Tick_Bomb_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Bomb_Loop_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Bomb_Release_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Cancel(_float fTimeDelta)
{
}

void CKena_State::Tick_Bomb_Cancel_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Inject(_float fTimeDelta)
{
}

void CKena_State::Tick_Bomb_Inject_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Air_Into(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Air_Loop(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Air_Release(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bomb_Air_Cancel(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Charge(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Charge_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Charge_Full(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Charge_Full_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Charge_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Charge_Loop_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Release_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Recharge(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Recharge_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Return_Run(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Inject(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Inject_Walk(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Inject_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Inject_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Air_Charge(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Air_Charge_Loop(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Air_Recharge(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Air_Release(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Bow_Air_Return(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");

	if (m_pKena->m_fCurJumpSpeed > 0.f)
		CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta / fTimeRate;

	Move(fTimeDelta / fTimeRate, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Idle_Into_Lock_On(_float fTimeDelta)
{
}

void CKena_State::Tick_Lock_On_Idle(_float fTimeDelta)
{
	m_fCombatIdleTime += fTimeDelta;
}

void CKena_State::Tick_Lock_On_To_Idle(_float fTimeDelta)
{
}

void CKena_State::Tick_Combat_Idle_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Combat_Run(_float fTimeDelta)
{
}

void CKena_State::Tick_Crouch_To_Idle(_float fTimeDelta)
{
}

void CKena_State::Tick_Crouch_To_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Take_Damage_Front(_float fTimeDelta)
{
}

void CKena_State::Tick_Take_Damage_Back(_float fTImeDelta)
{
}

void CKena_State::Tick_Take_Damage_Left(_float fTimeDelta)
{
}

void CKena_State::Tick_Take_Damage_Right(_float fTimeDelta)
{
}

void CKena_State::Tick_Take_Damage_Heavy_Front(_float fTimeDelta)
{
}

void CKena_State::Tick_Take_Damage_Heavy_Back(_float fTimeDelta)
{
}

void CKena_State::Tick_Take_Damage_Heavy_Air(_float fTimeDelta)
{
}

void CKena_State::Tick_Backflip(_float fTimeDelta)
{
}

void CKena_State::Tick_Roll(_float fTimeDelta)
{
}

void CKena_State::Tick_Roll_Left(_float fTimeDelta)
{
}

void CKena_State::Tick_Roll_Right(_float fTimeDelta)
{
}

void CKena_State::Tick_Roll_Into_Idle(_float fTimeDelta)
{
}

void CKena_State::Tick_Roll_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Roll_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::Tick_Roll_Into_Fall(_float fTimeDelta)
{
}

void CKena_State::Tick_Fall(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Fall_Into_Run(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);

	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Heavy_Attack_1_Charge(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_1_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_1_Release_Perfect(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_1_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_1_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Heavy_Attack_2_Charge(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_2_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_2_Release_Perfect(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_2_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_2_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Heavy_Attack_3_Charge(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_3_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_3_Release_Perfect(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_3_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_Combo(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_Combo_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Heavy_Attack_Combo_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Interact(_float fTimeDelta)
{
}

void CKena_State::Tick_Interact_Staff(_float fTimeDelta)
{
}

void CKena_State::Tick_Jump_Squat(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Jump(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);
 	//if (m_pKena->m_fCurJumpSpeed >= 0.f)
 		m_pKena->m_fCurJumpSpeed -= fTimeDelta;
 	//else
 	//	m_pKena->m_fCurJumpSpeed = 0.f;
}

void CKena_State::Tick_Running_Jump_Squat(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Running_Jump(_float fTimeDelta)
{
	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0.f, 1.f, 0.f) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);

	//m_pKena->m_fCurJumpSpeed -= 9.81f * fTimeDelta * 0.7f;
	m_pKena->m_fCurJumpSpeed -= fTimeDelta;
}

void CKena_State::Tick_Pulse_Jump(_float fTimeDelta)
{
 	CPhysX_Manager::GetInstance()->Add_Force(m_pKena->m_szCloneObjectTag, _float3(0, 1.f, 0) * m_pKena->m_fCurJumpSpeed);
	Move(fTimeDelta, m_eDir);
 	
	//m_pKena->m_fCurJumpSpeed -= 9.81f * fTimeDelta * 0.7f;
	m_pKena->m_fCurJumpSpeed -= fTimeDelta * 0.8f;
// 	if (m_pKena->m_fCurJumpSpeed > 0.f)
// 		m_pKena->m_fCurJumpSpeed -= fTimeDelta * 14.5f;
// 	else
// 		m_pKena->m_fCurJumpSpeed = 0.f;
}

void CKena_State::Tick_Land(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Land_Heavy(_float fTimeDelta)
{
}

void CKena_State::Tick_Land_Walking(_float fTimeDelta)
{
}

void CKena_State::Tick_Land_Running(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Bow_Land(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Ledge_Land(_float fTimeDelta)
{
}

void CKena_State::Tick_Slide_Land(_float fTimeDelta)
{
}

void CKena_State::Tick_Level_Up(_float fTimeDelta)
{
}

void CKena_State::Tick_Mask_On(_float fTimeDelta)
{
}

void CKena_State::Tick_Mask_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Mask_Off(_float fTimeDelta)
{
}

void CKena_State::Tick_Mask_Off_Memory_Collect(_float fTimeDelta)
{
}

void CKena_State::Tick_Meditate_Into(_float fTimeDelta)
{
}

void CKena_State::Tick_Meditate_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Meditate_Exit(_float fTimeDelta)
{
}

void CKena_State::Tick_Into_Pulse(_float fTimeDelta)
{
}

void CKena_State::Tick_Into_Pulse_From_Run(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Pulse_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse_Into_Combat_End(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse_Into_Idle(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse_Into_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Pulse_Parry(_float fTimeDelta)
{
	CAnimation*	pAnimation = m_pAnimationState->Get_CurrentAnim()->m_pMainAnim;

	if (pAnimation->Get_AnimationProgress() < 0.065f)
		pAnimation->Get_AnimationTickPerSecond() = 2.0;
	else
	{
		pAnimation->Get_AnimationTickPerSecond() = 24.0;
		CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
	}
}

void CKena_State::Tick_Pulse_Walk(_float fTimeDelta)
{
	Move(fTimeDelta, CTransform::DIR_LOOK);
}

void CKena_State::Tick_Pulse_Squat_Sprint(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Shield_Impact(_float fTimeDelta)
{
}

void CKena_State::Tick_Shield_Impact_Medium(_float fTimeDelta)
{
}

void CKena_State::Tick_Shield_Impact_Big(_float fTimeDelta)
{
}

void CKena_State::Tick_Shield_Break_Front(_float fTimeDelta)
{
	if (m_pAnimationState->Get_AnimationProgress() > 0.06f)
		CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::Tick_Shield_Break_Back(_float fTimeDelta)
{
	if (m_pAnimationState->Get_AnimationProgress() > 0.06f)
		CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::Tick_Spin_Attack(_float fTimeDelta)
{
}

void CKena_State::Tick_Spin_Attack_Return(_float fTimeDelta)
{
}

void CKena_State::Tick_Into_Sprint(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
}

void CKena_State::Tick_Sprint(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
}

void CKena_State::Tick_Sprint_Stop(_float fTimeDelta)
{
}

void CKena_State::Tick_Sprint_Lean_Left(_float fTimeDelta)
{
}

void CKena_State::Tick_Sprint_Lean_Right(_float fTimeDelta)
{
}

void CKena_State::Tick_Sprint_Turn_180(_float fTimeDelta)
{
}

void CKena_State::Tick_Sprint_Attack(_float fTimeDelta)
{
}

void CKena_State::Tick_Teleport_Flower(_float fTimeDelta)
{
}

void CKena_State::End_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Run(_float fTimeDelta)
{
}

void CKena_State::End_Run_Stop(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Into(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Loop(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Return(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;

	/* Turn Off Canvas Aim */
	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	//_float fTag = 0.f;
	//m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);
}

void CKena_State::End_Aim_Run_Forward(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Forward_Left(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Forward_Right(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Backward(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Backward_Left(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Backward_Right(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Left(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Run_Right(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Aim_Air_Into(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Aim_Air_Loop(_float fTimeDetla)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Air_Attack_1(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Air_Attack_2(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Air_Attack_Slam_Into(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_Slam_Loop(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_Slam_Finish(_float fTimeDelta)
{
	m_pKena->m_bAttack= false;
	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::End_Attack_1(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Attack_1_From_Run(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Attack_1_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Attack_1_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::End_Attack_1_Return(_float fTimeDelta)
{
}

void CKena_State::End_Attack_2(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Attack_2_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Attack_2_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::End_Attack_2_Return(_float fTimeDelta)
{
}

void CKena_State::End_Attack_3(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Attack_3_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Attack_3_Return(_float fTimeDelta)
{
}

void CKena_State::End_Attack_4(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Attack_4_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Attack_4_Return(_float fTimeDelta)
{
}

void CKena_State::End_Bomb_Into(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Into_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Loop(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Loop_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Release(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bomb_Release_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bomb_Cancel(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bomb_Cancel_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bomb_Inject(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
	m_pKena->m_bInjectBomb = false;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::End_Bomb_Inject_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
	m_pKena->m_bInjectBomb = false;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::End_Bomb_Air_Into(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Air_Loop(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Air_Release(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bomb_Air_Cancel(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBomb = false;
}

void CKena_State::End_Bow_Charge(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Charge_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Charge_Full(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Charge_Full_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Charge_Loop(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Charge_Loop_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Release(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;

	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	//_float fTag = 0.0f;
	//m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::End_Bow_Release_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Recharge(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Recharge_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
}

void CKena_State::End_Bow_Return(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Return_Run(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Inject(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = false;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::End_Bow_Inject_Walk(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = false;

	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::End_Bow_Inject_Loop(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = false;
}

void CKena_State::End_Bow_Inject_Release(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
	m_pKena->m_bInjectBow = false;
}

void CKena_State::End_Bow_Air_Charge(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Air_Charge_Loop(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Air_Recharge(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Air_Release(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Bow_Air_Return(_float fTimeDelta)
{
	m_pKena->m_bAim = false;
	m_pKena->m_bBow = false;
}

void CKena_State::End_Idle_Into_Lock_On(_float fTimeDelta)
{
}

void CKena_State::End_Lock_On_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Lock_On_To_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Combat_Idle_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Combat_Run(_float fTimeDelta)
{
}

void CKena_State::End_Crouch_To_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Crouch_To_Run(_float fTimeDelta)
{
}

void CKena_State::End_Take_Damage_Front(_float fTimeDelta)
{
}

void CKena_State::End_Take_Damage_Back(_float fTImeDelta)
{
}

void CKena_State::End_Take_Damage_Left(_float fTimeDelta)
{
}

void CKena_State::End_Take_Damage_Right(_float fTimeDelta)
{
}

void CKena_State::End_Take_Damage_Heavy_Front(_float fTimeDelta)
{
}

void CKena_State::End_Take_Damage_Heavy_Back(_float fTimeDelta)
{
}

void CKena_State::End_Take_Damage_Heavy_Air(_float fTimeDelta)
{
}

void CKena_State::End_Backflip(_float fTimeDelta)
{
}

void CKena_State::End_Roll(_float fTimeDelta)
{
}

void CKena_State::End_Roll_Left(_float fTimeDelta)
{
}

void CKena_State::End_Roll_Right(_float fTimeDelta)
{
}

void CKena_State::End_Roll_Into_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Roll_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Roll_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::End_Roll_Into_Fall(_float fTimeDelta)
{
}

void CKena_State::End_Fall(_float fTimeDelta)
{
}

void CKena_State::End_Fall_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_1_Charge(_float fTimeDelta)
{
	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::End_Heavy_Attack_1_Release(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Heavy_Attack_1_Release_Perfect(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bPerfectAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Heavy_Attack_1_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_1_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Charge(_float fTimeDelta)
{
	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::End_Heavy_Attack_2_Release(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Heavy_Attack_2_Release_Perfect(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bPerfectAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Heavy_Attack_2_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_3_Charge(_float fTimeDelta)
{
	m_pKena->m_bHeavyAttack = false;
}

void CKena_State::End_Heavy_Attack_3_Release(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Heavy_Attack_3_Release_Perfect(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bPerfectAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Heavy_Attack_3_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_Combo(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Heavy_Attack_Combo_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_Combo_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Interact(_float fTimeDelta)
{
}

void CKena_State::End_Interact_Staff(_float fTimeDelta)
{
}

void CKena_State::End_Jump_Squat(_float fTimeDelta)
{
}

void CKena_State::End_Jump(_float fTimeDelta)
{
}

void CKena_State::End_Running_Jump_Squat(_float fTimeDelta)
{
}

void CKena_State::End_Running_Jump(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Jump(_float fTimeDelta)
{
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Land(_float fTimeDelta)
{
}

void CKena_State::End_Land_Heavy(_float fTimeDelta)
{
}

void CKena_State::End_Land_Walking(_float fTimeDelta)
{
}

void CKena_State::End_Land_Running(_float fTimeDelta)
{
}

void CKena_State::End_Bow_Land(_float fTimeDelta)
{
}

void CKena_State::End_Ledge_Land(_float fTimeDelta)
{
}

void CKena_State::End_Slide_Land(_float fTimeDelta)
{
}

void CKena_State::End_Level_Up(_float fTimeDelta)
{
}

void CKena_State::End_Mask_On(_float fTimeDelta)
{
}

void CKena_State::End_Mask_Loop(_float fTimeDelta)
{
	m_pKena->m_bMask = false;
}

void CKena_State::End_Mask_Off(_float fTimeDelta)
{
	for (auto pPart : m_pKena->m_vecPart)
	{
		if (pPart->Get_PartsType() == CKena_Parts::KENAPARTS_MASK)
			pPart->Set_Active(false);
	}
}

void CKena_State::End_Mask_Off_Memory_Collect(_float fTimeDelta)
{
	for (auto pPart : m_pKena->m_vecPart)
	{
		if (pPart->Get_PartsType() == CKena_Parts::KENAPARTS_MASK)
			pPart->Set_Active(false);
	}
}

void CKena_State::End_Meditate_Into(_float fTimeDelta)
{
}

void CKena_State::End_Meditate_Loop(_float fTimeDelta)
{
}

void CKena_State::End_Meditate_Exit(_float fTimeDelta)
{
}

void CKena_State::End_Into_Pulse(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Into_Pulse_From_Run(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Pulse(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Loop(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Pulse_Into_Combat_End(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Into_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Parry(_float fTimeDelta)
{
	m_pKena->m_bParryLaunch = false;
	
	CGameInstance::GetInstance()->Set_TimeRate(L"Timer_60", 1.f);
}

void CKena_State::End_Pulse_Walk(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Pulse_Squat_Sprint(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Shield_Impact(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Shield_Impact_Medium(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Shield_Impact_Big(_float fTimeDelta)
{
	m_pKena->m_bPulse = false;
}

void CKena_State::End_Shield_Break_Front(_float fTimeDelta)
{
}

void CKena_State::End_Shield_Break_Back(_float fTimeDelta)
{
}

void CKena_State::End_Spin_Attack(_float fTimeDelta)
{
	m_pKena->m_bAttack = false;
	m_pKena->m_bHeavyAttack = false;
	m_pKena->m_bTrailON = false;
}

void CKena_State::End_Spin_Attack_Return(_float fTimeDelta)
{
}

void CKena_State::End_Into_Sprint(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Sprint(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Sprint_Stop(_float fTimeDelta)
{
}

void CKena_State::End_Sprint_Lean_Left(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Sprint_Lean_Right(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Sprint_Turn_180(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
}

void CKena_State::End_Sprint_Attack(_float fTimeDelta)
{
}

void CKena_State::End_Teleport_Flower(_float fTimeDelta)
{
}

_bool CKena_State::TruePass()
{
	return true;
}

_bool CKena_State::OnGround()
{
	return m_pKena->m_bOnGround && !m_pKena->m_bJump;
}

_bool CKena_State::CommonHit()
{
	return m_pKena->m_bCommonHit && !m_pKena->m_bHeavyHit && !m_pKena->m_bPulse;
}

_bool CKena_State::HeavyHit()
{
	return !m_pKena->m_bCommonHit && m_pKena->m_bHeavyHit && !m_pKena->m_bPulse;
}

_bool CKena_State::Shield_Small()
{
	return m_pKena->m_bCommonHit && !m_pKena->m_bHeavyHit && m_pKena->m_bPulse;
}

_bool CKena_State::Shield_Medium()
{
	return m_pKena->m_bCommonHit && !m_pKena->m_bHeavyHit && m_pKena->m_bPulse;
}

_bool CKena_State::Shield_Big()
{
	return !m_pKena->m_bCommonHit && m_pKena->m_bHeavyHit && m_pKena->m_bPulse;
}

_bool CKena_State::Shield_Break_Front()
{
	return m_pStatus->Is_ShieldBreak() && m_pKena->m_eDamagedDir == CKena::DAMAGED_FRONT && (m_pKena->m_bCommonHit || m_pKena->m_bHeavyHit);
}

_bool CKena_State::Shield_Break_Back()
{
	return m_pStatus->Is_ShieldBreak() && m_pKena->m_eDamagedDir == CKena::DAMAGED_BACK && (m_pKena->m_bCommonHit || m_pKena->m_bHeavyHit);
}

_bool CKena_State::Pulse_Jump()
{
	return !m_pKena->m_bPulseJump;
}

_bool CKena_State::Animation_Finish()
{
	return m_pAnimationState->Get_AnimationFinish();
}

_bool CKena_State::Animation_Progress(_float fProgress)
{
	return m_pAnimationState->Get_AnimationProgress() > fProgress;
}

_bool CKena_State::Direction_Change()
{
	return m_eDir != m_ePreDir && m_eDir != CTransform::DIR_END;
}

_bool CKena_State::CombatTimeToIdle()
{
	return !m_bCombat && m_fCombatIdleTime > 2.5f;
}

_bool CKena_State::HeavyAttack1_Perfect()
{
	return m_pAnimationState->Get_AnimationProgress() > 0.646f && m_pAnimationState->Get_AnimationProgress() < 0.739f;
}

_bool CKena_State::HeavyAttack2_Perfect()
{
	return m_pAnimationState->Get_AnimationProgress() > 0.45f && m_pAnimationState->Get_AnimationProgress() < 0.552f;
}

_bool CKena_State::HeavyAttack3_Perfect()
{
	return m_pAnimationState->Get_AnimationProgress() > 0.491f && m_pAnimationState->Get_AnimationProgress() < 0.64f;
}

_bool CKena_State::Parry()
{
	return m_pKena->m_bParry;
}

_bool CKena_State::Teleport_Flower()
{
	return m_pKena->m_bTeleportFlower;
}

_bool CKena_State::Interactable()
{
	return m_pKena->m_bRotWispInteractable || m_pKena->m_bChestInteractable;
}

_bool CKena_State::RotWisp_Interactable()
{
	return m_pKena->m_bRotWispInteractable;
}

_bool CKena_State::Chest_Interactable()
{
	return m_pKena->m_bChestInteractable;
}

_bool CKena_State::Damaged_Dir_Front()
{
	return m_pKena->m_eDamagedDir == CKena::DAMAGED_FRONT;
}

_bool CKena_State::Damaged_Dir_Back()
{
	return m_pKena->m_eDamagedDir == CKena::DAMAGED_BACK;
}

_bool CKena_State::Damaged_Dir_Left()
{
	return m_pKena->m_eDamagedDir == CKena::DAMAGED_LEFT;
}

_bool CKena_State::Damaged_Dir_Right()
{
	return m_pKena->m_eDamagedDir == CKena::DAMAGED_RIGHT;
}

_bool CKena_State::Check_PipCount()
{
	return m_pStatus->Get_CurPIPCount() > 0;
}

_bool CKena_State::Check_ArrowCount()
{
	return m_pKena->m_pCurArrow == nullptr && m_pStatus->Get_CurArrowCount() > 0;
}

_bool CKena_State::Check_Shield()
{
	return m_pStatus->Get_Shield() > 0.f;
}

_bool CKena_State::KeyInput_None()
{
	_bool		bDetectInput = true;

	if (m_eDir != CTransform::DIR_END)
		bDetectInput = false;

	return bDetectInput;
}

_bool CKena_State::KeyInput_Direction()
{
	_bool		bDetectInput = false;

	if (m_pGameInstance->Key_Pressing(DIK_W))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_A))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_S))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_D))
		bDetectInput = true;

	return bDetectInput;
}

_bool CKena_State::KeyInput_W()
{
	if (m_pGameInstance->Key_Pressing(DIK_W) && !KeyInput_WA() && !KeyInput_WD())
		return true;

	return false;
}

_bool CKena_State::KeyInput_A()
{
	if (m_pGameInstance->Key_Pressing(DIK_A) && !KeyInput_WA() && !KeyInput_SA())
		return true;

	return false;
}

_bool CKena_State::KeyInput_S()
{
	if (m_pGameInstance->Key_Pressing(DIK_S) && !KeyInput_SA() && !KeyInput_SD())
		return true;

	return false;
}

_bool CKena_State::KeyInput_D()
{
	if (m_pGameInstance->Key_Pressing(DIK_D) && !KeyInput_WD() && !KeyInput_SD())
		return true;

	return false;
}

_bool CKena_State::KeyInput_WA()
{
	if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_A))
		return true;

	return false;
}

_bool CKena_State::KeyInput_WD()
{
	if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_D))
		return true;

	return false;
}

_bool CKena_State::KeyInput_SA()
{
	if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_A))
		return true;

	return false;
}

_bool CKena_State::KeyInput_SD()
{
	if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_D))
		return true;

	return false;
}

_bool CKena_State::KeyInput_E()
{
	if (m_pGameInstance->Key_Pressing(DIK_E))
		return true;

	return false;
}

_bool CKena_State::KeyInput_Space()
{
	if (m_pGameInstance->Key_Pressing(DIK_SPACE))
		return true;

	return false;
}

_bool CKena_State::KeyInput_LShift()
{
	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		/* Turn Off Canvas Aim */
		CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
		CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
		_float fTag = 1.f;
		m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);

		return true;
	}
	
	return false;
}

_bool CKena_State::KeyInput_LCtrl()
{
	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		return true;

	return false;
}

_bool CKena_State::MouseInput_Left()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_LB))
		return true;

	return false;
}

_bool CKena_State::MouseInput_Right()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
		return true;

	return false;
}

_bool CKena_State::NoMouseInput_Right()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB) == false)
		return true;

	return false;
}

_bool CKena_State::KeyDown_E()
{
	if (m_pGameInstance->Key_Down(DIK_E))
		return true;

	return false;
}

_bool CKena_State::KeyDown_F()
{
	if (m_pGameInstance->Key_Down(DIK_F))
		return true;

	return false;
}

_bool CKena_State::KeyDown_Q()
{
	if (m_pGameInstance->Key_Down(DIK_Q))
		return true;

	return false;
}

_bool CKena_State::KeyDown_R()
{
	if (m_pGameInstance->Key_Down(DIK_R))
		return true;

	return false;
}

_bool CKena_State::KeyDown_T()
{
	if (m_pGameInstance->Key_Down(DIK_T))
		return true;

	return false;
}

_bool CKena_State::KeyDown_Space()
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
		return true;

	return false;
}

_bool CKena_State::KeyDown_BackSpace()
{
	if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
		return true;

	return false;
}

_bool CKena_State::KeyDown_LCtrl()
{
	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
		return true;

	return false;
}

_bool CKena_State::MouseDown_Left()
{
	if (m_pGameInstance->Mouse_Down(DIM_LB))
		return true;

	return false;
}

_bool CKena_State::MouseDown_Middle()
{
	if (m_pGameInstance->Mouse_Down(DIM_MB))
		return true;

	return false;
}

_bool CKena_State::MouseDown_Right()
{
	if (m_pGameInstance->Mouse_Down(DIM_RB))
		return true;

	return false;
}

_bool CKena_State::KeyUp_E()
{
	if (m_pGameInstance->Key_Up(DIK_E))
		return true;

	return false;
}

_bool CKena_State::KeyUp_LShift()
{
	if (m_pGameInstance->Key_Up(DIK_LSHIFT))
	{
		/* Turn Off Canvas Aim */
		CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
		CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
		_float fTag = 0.f;
		m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);

		return true;
	}
	return false;
}

_bool CKena_State::MouseUp_Left()
{
	if (m_pGameInstance->Mouse_Up(DIM_LB))
		return true;

	return false;
}

_bool CKena_State::MouseUp_Right()
{
	if (m_pGameInstance->Mouse_Up(DIM_RB))
		return true;

	return false;
}

void CKena_State::Move(_float fTimeDelta, CTransform::DIRECTION eDir, MOVEOPTION eMoveOption)
{
	if (eDir == CTransform::DIR_END)
		return;

	_matrix	matCamWorld = m_pCamera->Get_WorldMatrix();
	_float4	vCamRight = matCamWorld.r[0];
	_float4	vCamLook = matCamWorld.r[2];
	_float4	vKenaLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
	vCamRight.y = vCamLook.y = vKenaLook.y = 0.f;

	_float4	vDir;

	if (eDir == CTransform::DIR_LOOK)
	{
		switch (m_eDir)
		{
		case CTransform::DIR_W:
			vDir = XMVector3Normalize(matCamWorld.r[2]);
			break;

		case CTransform::DIR_A:
			vDir = XMVector3Normalize(matCamWorld.r[0]) * -1.f;
			break;

		case CTransform::DIR_S:
			vDir = XMVector3Normalize(matCamWorld.r[2]) * -1.f;
			break;

		case CTransform::DIR_D:
			vDir = XMVector3Normalize(matCamWorld.r[0]);
			break;

		case CTransform::DIR_WA:
			vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0] * -1.f);
			break;

		case CTransform::DIR_WD:
			vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0]);
			break;

		case CTransform::DIR_SA:
			vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0] * -1.f);
			break;

		case CTransform::DIR_SD:
			vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0]);
			break;
		}

		vDir.y = 0.f;

		m_pTransform->Go_Direction(vDir, fTimeDelta);
		return;
	}

	switch (eDir)
	{
	case CTransform::DIR_W:
		vDir = XMVector3Normalize(matCamWorld.r[2]);
		break;

	case CTransform::DIR_A:
		vDir = XMVector3Normalize(matCamWorld.r[0]) * -1.f;
		break;

	case CTransform::DIR_S:
		vDir = XMVector3Normalize(matCamWorld.r[2]) * -1.f;
		break;

	case CTransform::DIR_D:
		vDir = XMVector3Normalize(matCamWorld.r[0]);
		break;

	case CTransform::DIR_WA:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_WD:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0]);
		break;

	case CTransform::DIR_SA:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_SD:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0]);
		break;
	}

	vDir.y = 0.f;

	/* Static Move */
	/*_float3	vScale = m_pTransform->Get_Scaled();

	_vector	vLook = XMVector3Normalize(vDir) * vScale.z;
	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE_UP, vUp);
	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);

	m_pTransform->Go_Straight(fTimeDelta);*/

	/* Dynamic Move */
	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vKenaLook))));;
	if (isnan(fAngle))
	{
		m_pTransform->Go_Straight(fTimeDelta);
		return;
	}

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamRight)));

	if (fDir > 0.f)	/* ī�޶� Look ���� �������� �ٶ󺸰� ����. */
	{
		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamLook)));

		if (fDir > 0.f)	/* ī�޶� Right ���� �� ���� �ٶ󺸰� ����. 1��и�?*/
		{
			fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamRight + vCamLook * -1.f)));

			if (fDir < 0.f)
			{
				switch (eDir)
				{
				case CTransform::DIR_W:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_A:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WA:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SA:
					fAngle *= -1.f;
					break;
				}
			}
			else
			{
				switch (eDir)
				{
				case CTransform::DIR_W:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_A:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WA:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WD:
					fAngle *= -1.f;
					break;
				}
			}
		}
		else /* ī�޶� Right ���� �� ���� �ٶ󺸰� ����. 4��и�?*/
		{
			fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamRight * -1.f + vCamLook * -1.f)));

			if (fDir < 0.f)
			{
				switch (eDir)
				{
				case CTransform::DIR_W:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_D:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WA:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WD:
					fAngle *= -1.f;
					break;
				}
			}
			else
			{
				switch (eDir)
				{
				case CTransform::DIR_W:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_D:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WD:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SD:
					fAngle *= -1.f;
					break;
				}
			}
		}
	}
	else	/* ī�޶� Look ���� ������ �ٶ󺸰� ����. */
	{
		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamLook)));

		if (fDir > 0.f)	/* ī�޶� Right ���� �� ���� �ٶ󺸰� ����. 2��и�?*/
		{
			fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamRight + vCamLook)));

			if (fDir < 0.f)
			{
				switch (eDir)
				{
				case CTransform::DIR_A:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_S:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SA:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SD:
					fAngle *= -1.f;
					break;
				}
			}
			else
			{
				switch (eDir)
				{
				case CTransform::DIR_A:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_S:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WA:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SA:
					fAngle *= -1.f;
					break;
				}
			}
		}
		else /* ī�޶� Right ���� �� ���� �ٶ󺸰� ����. 3��и�?*/
		{
			fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vKenaLook), XMVector3Normalize(vCamRight * -1.f + vCamLook)));

			if (fDir < 0.f)
			{
				switch (eDir)
				{
				case CTransform::DIR_S:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_D:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_WD:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SD:
					fAngle *= -1.f;
					break;
				}
			}
			else
			{
				switch (eDir)
				{
				case CTransform::DIR_S:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_D:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SA:
					fAngle *= -1.f;
					break;

				case CTransform::DIR_SD:
					fAngle *= -1.f;
					break;
				}
			}
		}
	}

	if (fTimeDelta != 0.f)
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle * fTimeDelta * m_pKena->m_fInertia);
	else
		m_pTransform->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

	if (eMoveOption == CKena_State::MOVEOPTION_COMMON)
		m_pTransform->Go_Straight(fTimeDelta);
}

CTransform::DIRECTION CKena_State::DetectDirectionInput()
{
	CTransform::DIRECTION		eDir = CTransform::DIR_END;

	if (KeyInput_W())
		eDir = CTransform::DIR_W;
	if (KeyInput_A())
		eDir = CTransform::DIR_A;
	if (KeyInput_S())
		eDir = CTransform::DIR_S;
	if (KeyInput_D())
		eDir = CTransform::DIR_D;
	if (KeyInput_WA())
		eDir = CTransform::DIR_WA;
	if (KeyInput_WD())
		eDir = CTransform::DIR_WD;
	if (KeyInput_SA())
		eDir = CTransform::DIR_SA;
	if (KeyInput_SD())
		eDir = CTransform::DIR_SD;

	return eDir;
}
