#include "stdafx.h"
#include "..\public\Kena_State.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Camera_Player.h"
#include "Utile.h"

CKena_State::CKena_State()
{
}

HRESULT CKena_State::Initialize(CKena * pKena, CStateMachine * pStateMachine, CModel * pModel, CTransform * pTransform, CCamera_Player * pCamera)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pKena = pKena;
	m_pStateMachine = pStateMachine;
	m_pModel = pModel;
	m_pTransform = pTransform;
	m_pCamera = pCamera;

	NULL_CHECK_RETURN(m_pKena, E_FAIL);
	NULL_CHECK_RETURN(m_pStateMachine, E_FAIL);
	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);



	return S_OK;
}

void CKena_State::Tick(_double dTimeDelta)
{
}

void CKena_State::Late_Tick(_double dTimeDelta)
{
}

CKena_State * CKena_State::Create(CKena * pKena, CStateMachine * pStateMachine, CModel * pModel, CTransform * pTransform, CCamera_Player * pCamera)
{
	CKena_State*	pInstance = new CKena_State;

	if (FAILED(pInstance->Initialize(pKena, pStateMachine, pModel, pTransform, pCamera)))
	{
		MSG_BOX("Failed to Create : CKena_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena_State::Free()
{
}