#include "stdafx.h"
#include "..\public\Rot_State.h"
#include "GameInstance.h"
#include "Rot.h"
#include "Camera_Player.h"
#include "Utile.h"

CRot_State::CRot_State()
{
}

HRESULT CRot_State::Initialize(CRot* pRot, CStateMachine* pStateMachine, CModel* pModel, CTransform* pTransform)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pRot = pRot;
	m_pStateMachine = pStateMachine;
	m_pModel = pModel;
	m_pTransform = pTransform;

	NULL_CHECK_RETURN(m_pStateMachine, E_FAIL);
	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);

	return S_OK;
}

void CRot_State::Tick(_double dTimeDelta)
{
}

void CRot_State::Late_Tick(_double dTimeDelta)
{
}

void CRot_State::ImGui_RenderProperty()
{
}

HRESULT CRot_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE")
		.Add_State(L"IDLE")
		.Init_Start(this, &CRot_State::Start_Idle)
		.Init_Tick(this, &CRot_State::Tick_Idle)
		.Init_End(this, &CRot_State::End_Idle)
		.Finish_Setting();

	return S_OK;
}

void CRot_State::Start_Idle(_float fTimeDelta)
{
	m_pModel->Set_AnimIndex(IDLE);
}

void CRot_State::Tick_Idle(_float fTimeDelta)
{
}

void CRot_State::End_Idle(_float fTimeDelta)
{
}

_bool CRot_State::Animation_Finish()
{
	return m_pModel->Get_AnimationFinish();
}

CRot_State* CRot_State::Create(CRot* pRot, CStateMachine* pStateMachine, CModel* pModel, CTransform* pTransform)
{
	CRot_State*	pInstance = new CRot_State;

	if (FAILED(pInstance->Initialize(pRot, pStateMachine, pModel, pTransform)))
	{
		MSG_BOX("Failed to Create : CRot_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRot_State::Free()
{
}

