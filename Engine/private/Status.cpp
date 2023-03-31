#include "stdafx.h"
#include "..\public\Status.h"
#include "GameInstance.h"

CStatus::CStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus & rhs)
	: CComponent(rhs)
{
}

HRESULT CStatus::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CStatus::Initialize(void* pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(nullptr, pOwner), E_FAIL);
	return S_OK;
}

void CStatus::Tick(_float fTimeDelta)
{
}

void CStatus::Imgui_RenderProperty()
{
}

void CStatus::Free()
{
	__super::Free();
}

HRESULT CStatus::Save(Json& rJson)
{	
	rJson["CStatus::00. m_iMaxHP"] = m_iMaxHP;
	rJson["CStatus::01. m_iHP"] = m_iHP;
	rJson["CStatus::02. m_iAttack"] = m_iAttack;
		
	return S_OK;
}

HRESULT CStatus::Load(Json& rJson)
{
	rJson["CStatus::0. m_iMaxHP"].get_to<_int>(m_iMaxHP);
	rJson["CStatus::1. m_iHP"].get_to<_int>(m_iHP);
	rJson["CStatus::2. m_iAttack"].get_to<_int>(m_iAttack);
	
	return S_OK;
}

_bool CStatus::IsDead()
{
	return m_iHP <= 0;
}

void CStatus::UnderAttack(CStatus* pEnemyStatus)
{
	if (pEnemyStatus == nullptr) return;

	m_iHP -= pEnemyStatus->m_iAttack;
}

void CStatus::Add_CurrentHP(_int iValue)
{
	m_iHP += iValue;

	m_iHP = m_iHP < 0 ? 0 :
		m_iHP > m_iMaxHP ? m_iMaxHP : m_iHP;
}

_float CStatus::Get_PercentHP()
{	
	return m_iHP <= 0.f ? 0.f : (_float)m_iHP / (_float)m_iMaxHP;
}