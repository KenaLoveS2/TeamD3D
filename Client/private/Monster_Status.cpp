#include "stdafx.h"
#include "..\public\Monster_Status.h"
#include "GameInstance.h"

CMonster_Status::CMonster_Status(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CStatus(pDevice, pContext)
{
}

CMonster_Status::CMonster_Status(const CMonster_Status & rhs)
	: CStatus(rhs)
{
}

HRESULT CMonster_Status::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CMonster_Status::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(nullptr, pOwner), E_FAIL);
	return S_OK;
}

void CMonster_Status::Tick(_float fTimeDelta)
{
}

void CMonster_Status::Imgui_RenderProperty()
{
}

HRESULT CMonster_Status::Save()
{
	Json jMonsterStatus;

	CStatus::Save(jMonsterStatus);
		
	jMonsterStatus["00. m_fInitAttackCoolTime"] = m_fInitAttackCoolTime;
	jMonsterStatus["01. m_fCurAttackCoolTime"] = m_fCurAttackCoolTime;
	jMonsterStatus["02. m_iNeedPIP"] = m_iNeedPIP;
	
	ofstream file(m_strJsonFilePath.c_str());
	file << jMonsterStatus;
	file.close();

	return S_OK;
}

HRESULT CMonster_Status::Load(const string & strJsonFilePath)
{
	Json jMonsterStatus;

	ifstream file(strJsonFilePath.c_str());
	file >> jMonsterStatus;
	file.close();

	CStatus::Load(jMonsterStatus);

	jMonsterStatus["00. m_fInitAttackCoolTime"].get_to<_float>(m_fInitAttackCoolTime);
	jMonsterStatus["01. m_fCurAttackCoolTime"].get_to<_float>(m_fCurAttackCoolTime);
	jMonsterStatus["02. m_iNeedPIP"].get_to<_int>(m_iNeedPIP);

	m_strJsonFilePath = strJsonFilePath;

	return S_OK;
}

CMonster_Status * CMonster_Status::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster_Status*	pInstance = new CMonster_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMonster_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMonster_Status::Clone(void * pArg, CGameObject * pOwner)
{
	CMonster_Status*	pInstance = new CMonster_Status(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Clone : CMonster_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Status::Free()
{
	__super::Free();
}
