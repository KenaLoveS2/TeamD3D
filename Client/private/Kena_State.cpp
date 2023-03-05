#include "stdafx.h"
#include "..\public\Kena_State.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Camera_Player.h"
#include "Utile.h"
#include "UI_ClientManager.h"
#include "AnimationState.h"
#include "Effect_Base.h"
#include "E_KenaPulse.h"

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
	FAILED_CHECK_RETURN(SetUp_State_Attack1(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack2(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack3(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack4(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Bow(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Pulse(), E_FAIL);

	return S_OK;
}

void CKena_State::Tick(_double dTimeDelta)
{
	m_eDir = DetectDirectionInput();
}

void CKena_State::Late_Tick(_double dTimeDelta)
{
	m_ePreDir = m_eDir;
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
		.Init_Changer(L"ATTACK_1", this, &CKena_State::MouseDown_Left)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"INTO_PULSE", this, &CKena_State::KeyInput_E)
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
		.Init_Changer(L"IDLE", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Aim()
{
	m_pStateMachine->Add_State(L"AIM_INTO")
		.Init_Start(this, &CKena_State::Start_Aim_Into)
		.Init_Tick(this, &CKena_State::Tick_Aim_Into)
		.Init_End(this, &CKena_State::End_Aim_Into)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_LOOP")
		.Init_Start(this, &CKena_State::Start_Aim_Loop)
		.Init_Tick(this, &CKena_State::Tick_Aim_Loop)
		.Init_End(this, &CKena_State::End_Aim_Loop)
		.Init_Changer(L"BOW_CHARGE", this, &CKena_State::MouseInput_Left)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::KeyInput_Direction)
		.Init_Changer(L"AIM_RETURN", this, &CKena_State::KeyUp_LShift)

		.Add_State(L"AIM_RETURN")
		.Init_Start(this, &CKena_State::Start_Aim_Return)
		.Init_Tick(this, &CKena_State::Tick_Aim_Return)
		.Init_End(this, &CKena_State::End_Aim_Return)
		.Init_Changer(L"AIM_INTO", this, &CKena_State::KeyInput_LShift)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"AIM_RUN")
		.Init_Start(this, &CKena_State::Start_Aim_Run)
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
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward_Left)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_FORWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Forward_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Forward_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Forward_Right)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward_Left)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_BACKWARD_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Backward_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Backward_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Backward_Right)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_LEFT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Left)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Left)
		.Init_End(this, &CKena_State::End_Aim_Run_Left)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"AIM_RUN_RIGHT")
		.Init_Start(this, &CKena_State::Start_Aim_Run_Right)
		.Init_Tick(this, &CKena_State::Tick_Aim_Run_Right)
		.Init_End(this, &CKena_State::End_Aim_Run_Right)
		.Init_Changer(L"RUN", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"AIM_RUN", this, &CKena_State::Direction_Change)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::KeyInput_None)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack1()
{
	m_pStateMachine->Add_State(L"ATTACK_1")
		.Init_Start(this, &CKena_State::Start_Attack_1)
		.Init_Tick(this, &CKena_State::Tick_Attack_1)
		.Init_End(this, &CKena_State::End_Attack_1)
		.Init_Changer(L"ATTACK_1_RETURN", this, &CKena_State::Animation_Finish)

		.Add_State(L"ATTACK_1_FROM_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_1_From_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_From_Run)
		.Init_End(this, &CKena_State::End_Attack_1_From_Run)

		.Add_State(L"ATTACK_1_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Attack_1_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Into_Run)
		.Init_End(this, &CKena_State::End_Attack_1_Into_Run)

		.Add_State(L"ATTACK_1_INTO_WALK")
		.Init_Start(this, &CKena_State::Start_Attack_1_Into_Walk)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Into_Walk)
		.Init_End(this, &CKena_State::End_Attack_1_Into_Walk)

		.Add_State(L"ATTACK_1_RETURN")
		.Init_Start(this, &CKena_State::Start_Attack_1_Return)
		.Init_Tick(this, &CKena_State::Tick_Attack_1_Return)
		.Init_End(this, &CKena_State::End_Attack_1_Return)
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack2()
{
	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack3()
{
	return S_OK;
}

HRESULT CKena_State::SetUp_State_Attack4()
{
	return S_OK;
}

HRESULT CKena_State::SetUp_State_Bow()
{
	m_pStateMachine->Add_State(L"BOW_CHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Charge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge)
		.Init_End(this, &CKena_State::End_Bow_Charge)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_FULL")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Full)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Full)
		.Init_End(this, &CKena_State::End_Bow_Charge_Full)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_CHARGE_LOOP")
		.Init_Start(this, &CKena_State::Start_Bow_Charge_Loop)
		.Init_Tick(this, &CKena_State::Tick_Bow_Charge_Loop)
		.Init_End(this, &CKena_State::End_Bow_Charge_Loop)
		.Init_Changer(L"IDLE", this, &CKena_State::KeyUp_LShift)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)

		.Add_State(L"BOW_RELEASE")
		.Init_Start(this, &CKena_State::Start_Bow_Release)
		.Init_Tick(this, &CKena_State::Tick_Bow_Release)
		.Init_End(this, &CKena_State::End_Bow_Release)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::MouseInput_Left)
		.Init_Changer(L"BOW_RETURN", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RECHARGE")
		.Init_Start(this, &CKena_State::Start_Bow_Recharge)
		.Init_Tick(this, &CKena_State::Tick_Bow_Recharge)
		.Init_End(this, &CKena_State::End_Bow_Recharge)
		.Init_Changer(L"BOW_RELEASE", this, &CKena_State::MouseUp_Left)
		.Init_Changer(L"BOW_CHARGE_FULL", this, &CKena_State::Animation_Finish)

		.Add_State(L"BOW_RETURN")
		.Init_Start(this, &CKena_State::Start_Bow_Return)
		.Init_Tick(this, &CKena_State::Tick_Bow_Return)
		.Init_End(this, &CKena_State::End_Bow_Return)
		.Init_Changer(L"BOW_RECHARGE", this, &CKena_State::MouseInput_Left)
		.Init_Changer(L"AIM_LOOP", this, &CKena_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKena_State::SetUp_State_Pulse()
{
	m_pStateMachine->Add_State(L"INTO_PULSE")
		.Init_Start(this, &CKena_State::Start_Into_Pulse)
		.Init_Tick(this, &CKena_State::Tick_Into_Pulse)
		.Init_End(this, &CKena_State::End_Into_Pulse)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"INTO_PULSE_FROM_RUN")
		.Init_Start(this, &CKena_State::Start_Into_Pulse_From_Run)
		.Init_Tick(this, &CKena_State::Tick_Into_Pulse_From_Run)
		.Init_End(this, &CKena_State::End_Into_Pulse_From_Run)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE")
		.Init_Start(this, &CKena_State::Start_Pulse)
		.Init_Tick(this, &CKena_State::Tick_Pulse)
		.Init_End(this, &CKena_State::End_Pulse)
		.Init_Changer(L"PULSE_INTO_IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE_LOOP")
		.Init_Start(this, &CKena_State::Start_Pulse_Loop)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Loop)
		.Init_End(this, &CKena_State::End_Pulse_Loop)
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
		.Init_Changer(L"IDLE", this, &CKena_State::Animation_Finish)

		.Add_State(L"PULSE_INTO_RUN")
		.Init_Start(this, &CKena_State::Start_Pulse_Into_Run)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Into_Run)
		.Init_End(this, &CKena_State::End_Pulse_Into_Run)

		.Add_State(L"PULSE_PARRY")
		.Init_Start(this, &CKena_State::Start_Pulse_Parry)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Parry)
		.Init_End(this, &CKena_State::End_Pulse_Parry)

		.Add_State(L"PULSE_WALK")
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
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
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_FRONT_LEFT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Front_Left)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_FRONT_RIGHT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Front_Right)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_BACKWARD")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Backward)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_BACK_LEFT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Back_Left)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_BACK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Back_Right)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_LEFT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Left)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_WALK_RIGHT")
		.Init_Start(this, &CKena_State::Start_Pulse_Walk_Right)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Walk)
		.Init_End(this, &CKena_State::End_Pulse_Walk)
		.Init_Changer(L"PULSE", this, &CKena_State::KeyUp_E)
		.Init_Changer(L"PULSE_WALK", this, &CKena_State::Direction_Change)
		.Init_Changer(L"PULSE_LOOP", this, &CKena_State::KeyInput_None)

		.Add_State(L"PULSE_SQUAT_SPRINT")
		.Init_Start(this, &CKena_State::Start_Pulse_Squat_Sprint)
		.Init_Tick(this, &CKena_State::Tick_Pulse_Squat_Sprint)
		.Init_End(this, &CKena_State::End_Pulse_Squat_Sprint)

		.Finish_Setting();

	return S_OK;
}

void CKena_State::Start_Idle(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("IDLE");
}

void CKena_State::Start_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("RUN");
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

	/* Siwtch Off Aim */
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

void CKena_State::Start_Attack_1(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1");
}

void CKena_State::Start_Attack_1_From_Run(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("ATTACK_1_FROM_RUN");
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

void CKena_State::Start_Bow_Charge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE");
}

void CKena_State::Start_Bow_Charge_Full(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_FULL");
}

void CKena_State::Start_Bow_Charge_Loop(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_CHARGE_LOOP");
}

void CKena_State::Start_Bow_Release(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RELEASE");
}

void CKena_State::Start_Bow_Recharge(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RECHARGE");
}

void CKena_State::Start_Bow_Return(_float fTimeDelta)
{
	m_pAnimationState->State_Animation("BOW_RETURN");
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
}

void CKena_State::Tick_Idle(_float fTimeDelta)
{
}

void CKena_State::Tick_Run(_float fTimeDelta)
{
	Move(fTimeDelta, m_eDir);
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

void CKena_State::Tick_Attack_1(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_From_Run(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_Into_Run(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_Into_Walk(_float fTimeDelta)
{
}

void CKena_State::Tick_Attack_1_Return(_float fTimeDelta)
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

void CKena_State::Tick_Into_Pulse(_float fTimeDelta)
{
}

void CKena_State::Tick_Into_Pulse_From_Run(_float fTimeDelta)
{
}

void CKena_State::Tick_Pulse(_float fTimeDelta)
{
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

void CKena_State::Tick_Pulse_Walk(_float fTimeDleta)
{
}

void CKena_State::Tick_Pulse_Squat_Sprint(_float fTimeDelta)
{
}

void CKena_State::End_Idle(_float fTimeDelta)
{
}

void CKena_State::End_Run(_float fTimeDelta)
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
}

void CKena_State::End_Bow_Recharge(_float fTimeDelta)
{
}

void CKena_State::End_Bow_Return(_float fTimeDelta)
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

void CKena_State::End_Pulse_Walk(_float fTimeDleta)
{
}

void CKena_State::End_Pulse_Squat_Sprint(_float fTimeDelta)
{
}

_bool CKena_State::Animation_Finish()
{
	return m_pAnimationState->Get_AnimationFinish();
}

_bool CKena_State::Direction_Change()
{
	return m_eDir != m_ePreDir && m_eDir != CTransform::DIR_END;
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

void CKena_State::Move(_float fTimeDelta, CTransform::DIRECTION eDir)
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
