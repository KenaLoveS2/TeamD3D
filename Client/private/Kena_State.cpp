#include "stdafx.h"
#include <cstddef>
#include "..\public\Kena_State.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Camera_Player.h"
#include "Utile.h"
#include "UI_ClientManager.h"
#include "AnimationState.h"
#include "Effect_Base.h"
#include "E_KenaPulse.h"

#define NULLFUNC	(_bool(CKena_State::*)())nullptr
CKena_State::CKena_State()
{
}

HRESULT CKena_State::Initialize(CKena * pKena, CStateMachine * pStateMachine, CModel * pModel, CAnimationState * pAnimation, CTransform * pTransform, CCamera_Player * pCamera)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pKena = pKena;
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
	FAILED_CHECK_RETURN(SetUp_State_Air_Attack(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack1(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack2(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack3(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack4(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bow(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Combat(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged_Common(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged_Heavy(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Dodge(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Fall(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack1(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack2(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack3(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Heavy_Attack_Combo(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Jump(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Land(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Pulse(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Sprint(), E_FAIL);

	return S_OK;
}

void CKena_State::Tick(_double dTimeDelta)
{
	m_eDir = DetectDirectionInput();

	if (m_pKena->m_bSprint == false)
		m_pTransform->Set_Speed(5.f);
	else
		m_pTransform->Set_Speed(7.f);
}

void CKena_State::Late_Tick(_double dTimeDelta)
{
	m_ePreDir = m_eDir;

// 	m_pKena->m_bCommonHit = false;
// 	m_pKena->m_bHeavyHit = false;
}

void CKena_State::ImGui_RenderProperty()
{
}

CKena_State * CKena_State::Create(CKena * pKena, CStateMachine * pStateMachine, CModel * pModel, CAnimationState * pAnimation, CTransform * pTransform, CCamera_Player * pCamera)
{
	CKena_State*	pInstance = new CKena_State;

	if (FAILED(pInstance->Initialize(pKena, pStateMachine, pModel, pAnimation, pTransform, pCamera)))
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
	//.Init_Changer(L"TAKE_DAMAGE_FRONT", this, &CKena_State::CommonHit)
	//.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
	//.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
	//.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
	//.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
	//.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
	//.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
	//.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
	//.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
 	//.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Run()
{
	m_pStateMachine->Add_State(L"RUN")
		.Init_Start(this, &CKena_State::Start_Run)
		.Init_Tick(this, &CKena_State::Tick_Run)
		.Init_End(this, &CKena_State::End_Run)
		.Init_Changer(L"TAKE_DAMAGE_FRONT", this, &CKena_State::CommonHit)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"RUN_STOP")
		.Init_Start(this, &CKena_State::Start_Run_Stop)
		.Init_Tick(this, &CKena_State::Tick_Run_Stop)
		.Init_End(this, &CKena_State::End_Run_Stop)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_LOOP")
		.Init_Start(this, &CKena_State::Start_Aim_Loop)
		.Init_Tick(this, &CKena_State::Tick_Aim_Loop)
		.Init_End(this, &CKena_State::End_Aim_Loop)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::MouseInput_Left)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)

		.Add_State(L"AIM_RETURN")
		.Init_Start(this, &CKena_State::Start_Aim_Return)
		.Init_Tick(this, &CKena_State::Tick_Aim_Return)
		.Init_End(this, &CKena_State::End_Aim_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_RUN")
		.Init_Start(this, &CKena_State::Start_Aim_Run)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward_Left)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward_Right)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward_Left)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward_Right)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Left)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Right)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.29f)
		.Init_Changer(L"ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)

		.Add_State(L"ATTACK_1_FROM_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_1_From_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_From_Run)
		.Init_End(this, &CKena_State::End_Attack_1_From_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.27f)
		.Init_Changer(L"ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)

		.Add_State(L"ATTACK_1_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_1_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_1_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"ATTACK_3", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"HEAVY_ATTACK_3_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.25f)
		.Init_Changer(L"ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_2_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.45f)

		.Add_State(L"ATTACK_2_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_2_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_2_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_2_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"ATTACK_4", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.14f)
		.Init_Changer(L"ATTACK_3_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.3f)
		.Init_Changer(L"ATTACK_3_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)

		.Add_State(L"ATTACK_3_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_3_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_3_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_3_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ATTACK_3_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_3_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_3_Return)
		.Init_End(this, &CKena_State::End_Attack_3_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.38f)
		.Init_Changer(L"ATTACK_4_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"ATTACK_4_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)

		.Add_State(L"ATTACK_4_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_4_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_4_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_4_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"ATTACK_4_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_4_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_4_Return)
		.Init_End(this, &CKena_State::End_Attack_4_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bow()
{
	m_pStateMachine->Add_State(L"BOW_CHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Charge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge)
		.Init_End(this, &CKena_State::End_Bow_Charge)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_LOOP")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"IDLE", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_FORWARD_LEFT", this, &CKena_State::KeyInput_WA)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_FORWARD_RIGHT", this, &CKena_State::KeyInput_WD)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_BACKWARD_LEFT", this, &CKena_State::KeyInput_SA)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_BACKWARD_RIGHT", this, &CKena_State::KeyInput_SD)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_FORWARD", this, &CKena_State::KeyInput_W)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_BACKWARD", this, &CKena_State::KeyInput_S)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_LEFT", this, &CKena_State::KeyInput_A)
		.Init_Changer(L"BOW_CHARGE_LOOP_RUN_RIGHT", this, &CKena_State::KeyInput_D)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_CHARGE_LOOP_RUN_FORWARD")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop_Run_Forward)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"BOW_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bow_Release)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release)
		.Init_End(this, &CKena_State::End_Bow_Release)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::MouseInput_Left)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RECHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge)
		.Init_End(this, &CKena_State::End_Bow_Recharge)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RETURN")
		.Init_Start(this, &CKena_State::Start_Bow_Return)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return)
		.Init_End(this, &CKena_State::End_Bow_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::MouseInput_Left)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_TO_IDLE", this, &CKena_State::CombatTimeToIdle)

		.Add_State(L"LOCK_ON_TO_IDLE")
		.Init_Start(this, &CKena_State::Start_Lock_On_To_Idle)
		.Init_Tick(this, &CKena_State::Tick_Lock_On_To_Idle)
		.Init_End(this, &CKena_State::End_Lock_On_To_Idle)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"COMBAT_IDLE_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Combat_Idle_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Combat_Idle_Into_Run)
		.Init_End(this, &CKena_State::End_Combat_Idle_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"COMBAT_RUN")
		.Init_Start(this, &CKena_State::Start_Combat_Run)
		.Init_Tick(this, &CKena_State::Tick_Combat_Run)
		.Init_End(this, &CKena_State::End_Combat_Run)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Damaged_Common()
{
	m_pStateMachine->Add_State(L"TAKE_DAMAGE_FRONT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Front)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Front)
		.Init_End(this, &CKena_State::End_Take_Damage_Front)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_BACK")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Back)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Back)
		.Init_End(this, &CKena_State::End_Take_Damage_Back)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_LEFT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Left)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Left)
		.Init_End(this, &CKena_State::End_Take_Damage_Left)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"TAKE_DAMAGE_RIGHT")
		.Init_Start(this, &CKena_State::Start_Take_Damage_Right)
		.Init_Tick(this, &CKena_State::Tick_Take_Damage_Right)
		.Init_End(this, &CKena_State::End_Take_Damage_Right)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Pulse_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Into_Run)
		.Init_End(this, &CKena_State::End_Pulse_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_PARRY")
		.Init_Start(this, &CKena_State::Start_Pulse_Parry)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Parry)
		.Init_End(this, &CKena_State::End_Pulse_Parry)

		.Add_State(L"PULSE_WALK")
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_COMBO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"SPRINT_ATTACK", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"SPRINT_STOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"SPRINT", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Sprint()
{
	m_pStateMachine->Add_State(L"INTO_SPRINT")
		.Init_Start(this, &CKena_State::Start_Into_Sprint)
		.Init_Tick(this, &CKena_State::Tick_Into_Sprint)
		.Init_End(this, &CKena_State::End_Into_Sprint)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"SPRINT_ATTACK", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_COMBO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"SPRINT_STOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"SPRINT", this, &CKena_State::Animation_Finish)

		.Add_State(L"SPRINT")
		.Init_Start(this, &CKena_State::Start_Sprint)
		.Init_Tick(this, &CKena_State::Tick_Sprint)
		.Init_End(this, &CKena_State::End_Sprint)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"HEAVY_ATTACK_COMBO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"SPRINT_ATTACK", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"SPRINT_STOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"SPRINT_STOP")
		.Init_Start(this, &CKena_State::Start_Sprint_Stop)
		.Init_Tick(this, &CKena_State::Tick_Sprint_Stop)
		.Init_End(this, &CKena_State::End_Sprint_Stop)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.26f)
		.Init_Changer(L"ATTACK_4_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"ATTACK_4_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.35f)
		.Init_Changer(L"HEAVY_ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)

		.Add_State(L"HEAVY_ATTACK_1_RELEASE_PERFECT")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Release_Perfect)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Release_Perfect)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Release_Perfect)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_2", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_1_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.35f)
		.Init_Changer(L"HEAVY_ATTACK_1_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)

		.Add_State(L"HEAVY_ATTACK_1_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_1_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_1_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_1_Into_Run)
		.Init_End(this, &CKena_State::End_Heavy_Attack_1_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_3", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_3_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_2_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_2_RELEASE_PERFECT")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Release_Perfect)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Release_Perfect)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Release_Perfect)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"ATTACK_3", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_3_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.21f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_2_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_2_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_2_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_2_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_2_Into_Run)
		.Init_End(this, &CKena_State::End_Heavy_Attack_2_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE_FROM_RUN", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_3_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_3_RELEASE_PERFECT")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_3_Release_Perfect)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_3_Release_Perfect)
		.Init_End(this, &CKena_State::End_Heavy_Attack_3_Release_Perfect)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.24f)
		.Init_Changer(L"HEAVY_ATTACK_2_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.4f)
		.Init_Changer(L"HEAVY_ATTACK_3_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.5f)

		.Add_State(L"HEAVY_ATTACK_3_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_3_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_3_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_3_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"HEAVY_ATTACK_COMBO_INTO_RUN", this, &CKena_State::KeyInput_Direction, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.55f)
		.Init_Changer(L"HEAVY_ATTACK_COMBO_RETURN", this, &CKena_State::KeyInput_None, NULLFUNC, NULLFUNC, &CKena_State::Animation_Progress, 0.6f)
		
		.Add_State(L"HEAVY_ATTACK_COMBO_RETURN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_Combo_Return)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_Combo_Return)
		.Init_End(this, &CKena_State::End_Heavy_Attack_Combo_Return)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"COMBAT_IDLE_INTO_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"LOCK_ON_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"HEAVY_ATTACK_COMBO_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Heavy_Attack_Combo_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Heavy_Attack_Combo_Into_Run)
		.Init_End(this, &CKena_State::End_Heavy_Attack_Combo_Into_Run)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Jump()
{
	m_pStateMachine->Add_State(L"JUMP_SQUAT")
		.Init_Start(this, &CKena_State::Start_Jump_Squat)
		.Init_Tick(this, &CKena_State::Tick_Jump_Squat)
		.Init_End(this, &CKena_State::End_Jump_Squat)
		.Init_Changer(L"JUMP", this, &CKena_State::Animation_Finish)

		.Add_State(L"JUMP")
		.Init_Start(this, &CKena_State::Start_Jump)
		.Init_Tick(this, &CKena_State::Tick_Jump)
		.Init_End(this, &CKena_State::End_Jump)
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
		.Init_Changer(L"RUNNING_JUMP", this, &CKena_State::Animation_Finish)

		.Add_State(L"RUNNING_JUMP")
		.Init_Start(this, &CKena_State::Start_Running_Jump)
		.Init_Tick(this, &CKena_State::Tick_Running_Jump)
		.Init_End(this, &CKena_State::End_Running_Jump)
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
		.Init_Changer(L"AIR_ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIR_ATTACK_SLAM_INTO", this, &CKena_State::MouseDown_Right)
		.Init_Changer(L"LAND", this, &CKena_State::OnGround)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Land()
{
	m_pStateMachine->Add_State(L"LAND")
		.Init_Start(this, &CKena_State::Start_Land)
		.Init_Tick(this, &CKena_State::Tick_Land)
		.Init_End(this, &CKena_State::End_Land)
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"ROLL", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"BACKFLIP", this, &CKena_State::KeyDown_LCtrl, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUNNING_JUMP_SQUAT", this, &CKena_State::KeyDown_Space)
		.Init_Changer(L"ATTACK_1_FROM_RUN", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"HEAVY_ATTACK_1_CHARGE", this, &CKena_State::MouseInput_Right)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_SPRINT", this, &CKena_State::MouseDown_Middle, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"INTO_PULSE_FROM_FUN", this, &CKena_State::KeyInput_E)
		.Init_Changer(L"RUN_STOP", this, &CKena_State::KeyInput_None)
		.Init_Changer(L"RUN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_LAND")
		.Init_Start(this, &CKena_State::Start_Bow_Land)
		.Init_Tick(this, &CKena_State::Tick_Bow_Land)
		.Init_End(this, &CKena_State::End_Bow_Land)

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

void CKena_State::Start_Idle(_float fTimeDelta)
{
	if (m_pAnimationState->Get_CurrentAnimIndex() == (_uint)RUN_STOP ||
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)ROLL_INTO_IDLE)
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
		m_pAnimationState->Get_CurrentAnimIndex() == (_uint)PULSE_INTO_RUN ||
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

	/* Switch On Aim */
	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	_float fTag = 1.f;
	m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);
}

void CKena_State::Start_Aim_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_LOOP");
}

void CKena_State::Start_Aim_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RETURN");

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	_float fTag = 0.f;
	m_PlayerDelegator.broadcast(eAim, funcSwitch, fTag);
}

void CKena_State::Start_Aim_Run(_float fTimeDelta)
{
}

void CKena_State::Start_Aim_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_FORWARD");
}

void CKena_State::Start_Aim_Run_Forward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_FORWARD_LEFT");
}

void CKena_State::Start_Aim_Run_Forward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_FORWARD_RIGHT");
}

void CKena_State::Start_Aim_Run_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_BACKWARD");
}

void CKena_State::Start_Aim_Run_Backward_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_BACKWARD_LEFT");
}

void CKena_State::Start_Aim_Run_Backward_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_BACKWARD_RIGHT");
}

void CKena_State::Start_Aim_Run_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_LEFT");
}

void CKena_State::Start_Aim_Run_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("AIM_RUN_RIGHT");
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

void CKena_State::Start_Bow_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE");

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Charge_Full(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL");
}

void CKena_State::Start_Bow_Charge_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP");
}

void CKena_State::Start_Bow_Charge_Loop_Run_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP_RUN_FORWARD");
}

void CKena_State::Start_Bow_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE");

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 0.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Recharge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE");

	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	_float fTag = 1.0f;
	m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::Start_Bow_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN");


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
}

void CKena_State::Start_Heavy_Attack_1_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_RELEASE");
}

void CKena_State::Start_Heavy_Attack_1_Release_Perfect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_RELEASE_PERFECT");
}

void CKena_State::Start_Heavy_Attack_1_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_RETURN");
}

void CKena_State::Start_Heavy_Attack_1_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_1_INTO_RUN");
}

void CKena_State::Start_Heavy_Attack_2_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_CHARGE");
}

void CKena_State::Start_Heavy_Attack_2_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_RELEASE");
}

void CKena_State::Start_Heavy_Attack_2_Release_Perfect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_RELEASE_PERFECT");
}

void CKena_State::Start_Heavy_Attack_2_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_RETURN");
}

void CKena_State::Start_Heavy_Attack_2_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_2_INTO_RUN");
}

void CKena_State::Start_Heavy_Attack_3_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_CHARGE");
}

void CKena_State::Start_Heavy_Attack_3_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_RELEASE");
}

void CKena_State::Start_Heavy_Attack_3_Release_Perfect(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_RELEASE_PERFECT");
}

void CKena_State::Start_Heavy_Attack_3_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_3_RETURN");
}

void CKena_State::Start_Heavy_Attack_Combo(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_COMBO");
}

void CKena_State::Start_Heavy_Attack_Combo_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_COMBO_RETURN");
}

void CKena_State::Start_Heavy_Attack_Combo_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("HEAVY_ATTACK_COMBO_INTO_RUN");
}

void CKena_State::Start_Jump_Squat(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("JUMP_SQUAT");
}

void CKena_State::Start_Jump(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("JUMP");
	m_pKena->m_fInitJumpSpeed = 0.35f;
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

void CKena_State::Start_Into_Pulse(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTO_PULSE");
	
	for (auto& Effect : m_pKena->m_mapEffect)
	{
		if (Effect.first == "KenaPulse")
			Effect.second->Set_Active(true);
	}
}

void CKena_State::Start_Into_Pulse_From_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("INTO_PULSE_FROM_RUN");
}

void CKena_State::Start_Pulse(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE");

	for (auto& Effect : m_pKena->m_mapEffect)
	{
		if (Effect.first == "KenaPulse")
			dynamic_cast<CE_KenaPulse*>(Effect.second)->Set_NoActive(true);
	}
}

void CKena_State::Start_Pulse_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_LOOP");
}

void CKena_State::Start_Pulse_Into_Combat_End(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_INTO_COMBAT_END");
}

void CKena_State::Start_Pulse_Into_Idle(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_INTO_IDLE");
}

void CKena_State::Start_Pulse_Into_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_INTO_RUN");
}

void CKena_State::Start_Pulse_Parry(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_PARRY");
}

void CKena_State::Start_Pulse_Walk_Forward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_FORWARD");
}

void CKena_State::Start_Pulse_Walk_Front_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_FRONT_LEFT");
}

void CKena_State::Start_Pulse_Walk_Front_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_FRONT_RIGHT");
}

void CKena_State::Start_Pulse_Walk_Backward(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_BACKWARD");
}

void CKena_State::Start_Pulse_Walk_Back_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_BACK_LEFT");
}

void CKena_State::Start_Pulse_Walk_Back_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_BACK_RIGHT");
}

void CKena_State::Start_Pulse_Walk_Left(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_LEFT");
}

void CKena_State::Start_Pulse_Walk_Right(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_WALK_RIGHT");
}

void CKena_State::Start_Pulse_Squat_Sprint(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("PULSE_SQUAT_SPRINT");

	m_pKena->m_bSprint = true;
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
		m_pAnimationState->State_Animation("SPRINT", 0.f);
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

	m_pKena->m_bSprint = false;
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

void CKena_State::Tick_Bow_Charge(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Charge_Full(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Charge_Loop(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Release(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Recharge(_float fTimeDelta)
{
}

void CKena_State::Tick_Bow_Return(_float fTimeDelta)
{
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
}

void CKena_State::Tick_Fall_Into_Run(_float fTimeDelta)
{
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
}

void CKena_State::Tick_Ledge_Land(_float fTimeDelta)
{
}

void CKena_State::Tick_Slide_Land(_float fTimeDelta)
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
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
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
}

void CKena_State::Tick_Pulse_Parry(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse_Walk(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse_Squat_Sprint(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir, CKena_State::MOVEOPTION_ONLYTURN);
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
}

void CKena_State::End_Aim_Loop(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Return(_float fTimeDelta)
{

}

void CKena_State::End_Aim_Run_Forward(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Forward_Left(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Forward_Right(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Backward(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Backward_Left(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Backward_Right(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Left(_float fTimeDelta)
{
}

void CKena_State::End_Aim_Run_Right(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_1(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_2(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_Slam_Into(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_Slam_Loop(_float fTimeDelta)
{
}

void CKena_State::End_Air_Attack_Slam_Finish(_float fTimeDelta)
{
}

void CKena_State::End_Attack_1(_float fTimeDelta)
{
}

void CKena_State::End_Attack_1_From_Run(_float fTimeDelta)
{
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
}

void CKena_State::End_Attack_3_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Attack_3_Return(_float fTimeDelta)
{
}

void CKena_State::End_Attack_4(_float fTimeDelta)
{
}

void CKena_State::End_Attack_4_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Attack_4_Return(_float fTimeDelta)
{
}

void CKena_State::End_Bow_Charge(_float fTimeDelta)
{
}

void CKena_State::End_Bow_Charge_Full(_float fTimeDelta)
{
}

void CKena_State::End_Bow_Charge_Loop(_float fTimeDelta)
{
}

void CKena_State::End_Bow_Release(_float fTimeDelta)
{
	//CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	//CUI_ClientManager::UI_FUNCTION eFunc = CUI_ClientManager::FUNC_DEFAULT;
	//_float fTag = 0.0f;
	//m_PlayerDelegator.broadcast(eAim, eFunc, fTag);
}

void CKena_State::End_Bow_Recharge(_float fTimeDelta)
{

}

void CKena_State::End_Bow_Return(_float fTimeDelta)
{
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
}

void CKena_State::End_Heavy_Attack_1_Release(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_1_Release_Perfect(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_1_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_1_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Charge(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Release(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Release_Perfect(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_2_Into_Run(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_3_Charge(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_3_Release(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_3_Release_Perfect(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_3_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_Combo(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_Combo_Return(_float fTimeDelta)
{
}

void CKena_State::End_Heavy_Attack_Combo_Into_Run(_float fTimeDelta)
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

void CKena_State::End_Into_Pulse(_float fTimeDelta)
{
}

void CKena_State::End_Into_Pulse_From_Run(_float fTimeDelta)
{
}

void CKena_State::End_Pulse(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Loop(_float fTimeDelta)
{
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
}

void CKena_State::End_Pulse_Walk(_float fTimeDelta)
{
}

void CKena_State::End_Pulse_Squat_Sprint(_float fTimeDelta)
{
	m_pKena->m_bSprint = false;
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

_bool CKena_State::OnGround()
{
	return m_pKena->m_bOnGround && !m_pKena->m_bJump;
}

_bool CKena_State::CommonHit()
{
	return m_pKena->m_bCommonHit && !m_pKena->m_bHeavyHit;
}

_bool CKena_State::HeavyHit()
{
	return !m_pKena->m_bCommonHit && m_pKena->m_bHeavyHit;
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
	return m_pAnimationState->Get_AnimationProgress() > 0.7f && m_pAnimationState->Get_AnimationProgress() < 0.75f;
}

_bool CKena_State::HeavyAttack2_Perfect()
{
	return m_pAnimationState->Get_AnimationProgress() > 0.45f && m_pAnimationState->Get_AnimationProgress() < 0.5f;
}

_bool CKena_State::HeavyAttack3_Perfect()
{
	return m_pAnimationState->Get_AnimationProgress() > 0.7f && m_pAnimationState->Get_AnimationProgress() < 0.75f;
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
		return true;
	
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

_bool CKena_State::KeyDown_Space()
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
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
		return true;
	
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
