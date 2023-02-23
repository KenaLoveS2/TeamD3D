#include "stdafx.h"
#include "..\Public\StateMachine.h"

CStateMachine::CStateMachine(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine & rhs)
	: CComponent(rhs)
{
}

CStateMachine & CStateMachine::Set_Root(const wstring & wstrStateName)
{
	m_wstrRootStateName = wstrStateName;

	return *this;
}

CStateMachine & CStateMachine::Add_State(const wstring & wstrStateName)
{
	STATE		tState;
	ZeroMemory(&tState, sizeof(STATE));

	m_mapState.emplace(wstrStateName, tState);
	
	m_wstrCurrentStateName = wstrStateName;

	return *this;
}

HRESULT CStateMachine::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CStateMachine::Initialize(void * pArg, class CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

void CStateMachine::Tick(_double & dTimeDelta)
{
	auto	CurState = find_if(m_mapState.begin(), m_mapState.end(), [&](const pair<const wstring, STATE>& Pair) {
		return m_wstrCurrentStateName == Pair.first;
	});

	assert(CurState != m_mapState.end());

	for (auto& Changer : m_mapChanger[m_wstrCurrentStateName])
	{
		if (Changer.Changer_Func() == true)
		{
			m_wstrNextStateName = Changer.wstrNextState;
			break;
		}
	}

	if (m_wstrNextStateName != L"")
	{
		if (CurState->second.State_End != nullptr)
			CurState->second.State_End(dTimeDelta);

		m_wstrLastStateName = m_wstrCurrentStateName;
		m_wstrCurrentStateName = m_wstrNextStateName;

		if (m_mapState[m_wstrCurrentStateName].State_Start != nullptr)
			m_mapState[m_wstrCurrentStateName].State_Start(dTimeDelta);

		m_wstrNextStateName = L"";
	}
	else
	{
		if (CurState->second.State_Tick != nullptr)
			CurState->second.State_Tick(dTimeDelta);
	}
}

void CStateMachine::Imgui_RenderProperty()
{
}

CStateMachine * CStateMachine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStateMachine*		pInstance = new CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CStateMachine::Clone(void * pArg, class CGameObject * pOwner)
{
	CStateMachine*		pInstance = new CStateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Clone : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();
}
