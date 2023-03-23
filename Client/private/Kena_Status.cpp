#include "stdafx.h"
#include "..\public\Kena_Status.h"
#include "GameInstance.h"

CKena_Status::CKena_Status(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CStatus(pDevice, pContext)
{
}

CKena_Status::CKena_Status(const CKena_Status & rhs)
	: CStatus(rhs)
{
}

HRESULT CKena_Status::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena_Status::Initialize(void* pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

void CKena_Status::Tick(_float fTimeDelta)
{
	Update_ArrowCoolTime(fTimeDelta);
	Update_BombCoolTime(fTimeDelta);
}

void CKena_Status::Imgui_RenderProperty()
{
}

void CKena_Status::Update_ArrowCoolTime(_float fTimeDelta)
{
	CUI_ClientManager::UI_PRESENT eArrow = CUI_ClientManager::AMMO_ARROW;
	CUI_ClientManager::UI_PRESENT eCool = CUI_ClientManager::AMMO_ARROWCOOL;
	CUI_ClientManager::UI_PRESENT eArrowEffect = CUI_ClientManager::AMMO_ARROWEFFECT;
	CUI_ClientManager::UI_PRESENT eReCharge = CUI_ClientManager::AMMO_ARROWRECHARGE;

	//_float fCount = (_float)m_iCurArrowCount;
	_float fGuage;
	//m_StatusDelegator.broadcast(eArrow, fCount);


	if (m_iCurArrowCount == m_iMaxArrowCount) /* Full */
	{
		m_fCurArrowCoolTime = 0.0f;
		fGuage = 1.0f;
		m_StatusDelegator.broadcast(eCool, fGuage);
	}
	else /* Not Full */
	{
		m_fCurArrowCoolTime += fTimeDelta;
		if (m_fCurArrowCoolTime >= m_fInitArrowCoolTime)
		{
			/* Fullfilll Effect Call */
			m_StatusDelegator.broadcast(eArrowEffect, fGuage);

			++m_iCurArrowCount;
			if (m_iCurArrowCount < m_iMaxArrowCount)
				m_fCurArrowCoolTime = 0.0f;
			else
				m_fCurArrowCoolTime = m_fInitArrowCoolTime;

			/* ReCharge */
			_float fIndex = (_float)m_iCurArrowCount - 1;
			_float fCount = (_float)m_iCurArrowCount;
			m_StatusDelegator.broadcast(eReCharge, fIndex);
			m_StatusDelegator.broadcast(eArrow, fCount);
		}
		fGuage = m_fCurArrowCoolTime / m_fInitArrowCoolTime;
		m_StatusDelegator.broadcast(eCool, fGuage);
	}
}

void CKena_Status::Update_BombCoolTime(_float fTimeDelta)
{
	_float fGuage = 0.f;

	if (m_iCurBombCount == m_iMaxBombCount) /* Full */
	{
		m_fCurBombCoolTime = 0.0f;
		fGuage = 1.0f;
		//m_StatusDelegator.broadcast(eCool, fGuage);
	}
	else /* Not Full */
	{
		m_fCurBombCoolTime += fTimeDelta;
		if (m_fCurBombCoolTime >= m_fInitBombCoolTime)
		{
			/* Fullfilll Effect Call */
			//m_StatusDelegator.broadcast(eBombEffect, fGuage);

			++m_iCurBombCount;
			if (m_iCurBombCount < m_iMaxBombCount)
				m_fCurBombCoolTime = 0.0f;
			else
				m_fCurBombCoolTime = m_fInitBombCoolTime;

			/* ReCharge */
			_float fIndex = (_float)m_iCurBombCount - 1;
			_float fCount = (_float)m_iCurBombCount;
			//m_StatusDelegator.broadcast(eReCharge, fIndex);
			//m_StatusDelegator.broadcast(eBomb, fCount);
		}
				fGuage = m_fCurBombCoolTime / m_fInitBombCoolTime;
		//m_StatusDelegator.broadcast(eCool, fGuage);

	}
}

void CKena_Status::Under_Shield(CStatus * pEnemyStatus)
{
	if (pEnemyStatus == nullptr) return;

	m_iShield -= pEnemyStatus->Get_Attack();
}

CKena_Status * CKena_Status::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKena_Status* pInstance = new CKena_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CKena_Status::Clone(void* pArg, class CGameObject* pOwner)
{
	CKena_Status*	pInstance = new CKena_Status(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Clone : CKena_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena_Status::Free()
{
	__super::Free();
}

HRESULT CKena_Status::Save()
{
	Json jKenaStatus;

	CStatus::Save(jKenaStatus);

	jKenaStatus["00. m_iMaxShield"] = m_iMaxShield;
	jKenaStatus["01. m_iShield"] = m_iShield;
	jKenaStatus["02. m_iKarma"] = m_iKarma;
	jKenaStatus["03. m_iRotLevel"] = m_iRotLevel;
	jKenaStatus["04. m_iRotCount"] = m_iRotCount;
	jKenaStatus["05. m_iCrystal"] = m_iCrystal;
	jKenaStatus["06. m_iMaxPIPCount"] = m_iMaxPIPCount;
	jKenaStatus["07. m_fCurPIPGuage"] = m_fCurPIPGuage;
	jKenaStatus["08. m_iMaxArrowCount"] = m_iMaxArrowCount;
	jKenaStatus["09. m_iCurArrowCount"] = m_iCurArrowCount;
	jKenaStatus["10. m_fInitArrowCoolTime"] = m_fInitArrowCoolTime;
	jKenaStatus["11. m_fCurArrowCoolTime"] = m_fCurArrowCoolTime;
	jKenaStatus["12. m_iMaxBombCount"] = m_iMaxBombCount;
	jKenaStatus["13. m_iCurBombCount"] = m_iCurBombCount;
	jKenaStatus["14. m_fInitBombCoolTime"] = m_fInitBombCoolTime;
	jKenaStatus["15. m_fCurBombCoolTime"] = m_fCurBombCoolTime;

	ofstream file(m_strJsonFilePath.c_str());
	file << jKenaStatus;
	file.close();

	return S_OK;
}

HRESULT CKena_Status::Load(const string & strJsonFilePath)
{
	Json jKenaStatus;

	ifstream file(strJsonFilePath.c_str());
	file >> jKenaStatus;
	file.close();

	CStatus::Load(jKenaStatus);

	jKenaStatus["00. m_iMaxShield"].get_to<_int>(m_iMaxShield);
	jKenaStatus["01. m_iShield"].get_to<_int>(m_iShield);
	jKenaStatus["02. m_iKarma"].get_to<_int>(m_iKarma);
	jKenaStatus["03. m_iRotLevel"].get_to<_int>(m_iRotLevel);
	jKenaStatus["04. m_iRotCount"].get_to<_int>(m_iRotCount);
	jKenaStatus["05. m_iCrystal"].get_to<_int>(m_iCrystal);
	jKenaStatus["06. m_iMaxPIPCount"].get_to<_int>(m_iMaxPIPCount);
	jKenaStatus["07. m_fCurPIPGuage"].get_to<_float>(m_fCurPIPGuage);
	jKenaStatus["08. m_iMaxArrowCount"].get_to<_int>(m_iMaxArrowCount);
	jKenaStatus["09. m_iCurArrowCount"].get_to<_int>(m_iCurArrowCount);
	jKenaStatus["10. m_fInitArrowCoolTime"].get_to<_float>(m_fInitArrowCoolTime);
	jKenaStatus["11. m_fCurArrowCoolTime"].get_to<_float>(m_fCurArrowCoolTime);
	jKenaStatus["12. m_iMaxBombCount"].get_to<_int>(m_iMaxBombCount);
	jKenaStatus["13. m_iCurBombCount"].get_to<_int>(m_iCurBombCount);
	jKenaStatus["14. m_fInitBombCoolTime"].get_to<_float>(m_fInitBombCoolTime);
	jKenaStatus["15. m_fCurBombCoolTime"].get_to<_float>(m_fCurBombCoolTime);

	m_iPipLevel = 1;
	m_eRotState = RS_GOOD;
	m_fArrowGuage = 1.f;

	m_strJsonFilePath = strJsonFilePath;

	return S_OK;
}

_int CKena_Status::Get_RotMax()
{
	/* Get MaxRot related to rotLevel*/
	switch (m_iRotLevel)
	{
	case 0:
		return 5;
	case 1:
		return 15;
	case 2:
		return 35;
	case 3:
		return 50;
	}

	return 0;
}

_int CKena_Status::Get_MaxPIPCount()
{
	switch (m_iPipLevel)
	{
	case 1:
		m_iMaxPIPCount = 1;
		break;
	case 2:
		m_iMaxPIPCount = 2;
		break;
	case 3:
		m_iMaxPIPCount = 3;
		break;
	}

	return m_iMaxPIPCount;
}

void CKena_Status::Set_RotCount(_int iValue)
{
	m_iRotCount = iValue;

	CUI_ClientManager::UI_PRESENT eMax = CUI_ClientManager::TOP_ROTMAX;
	CUI_ClientManager::UI_PRESENT eNow = CUI_ClientManager::TOP_ROTCUR;
	CUI_ClientManager::UI_PRESENT eGet = CUI_ClientManager::TOP_ROTGET;

	_float fRotMax = (_float)Get_RotMax();
	_float fRotNow = (_float)m_iRotCount;
	_float fGuage = fRotNow / fRotMax;

	m_StatusDelegator.broadcast(eNow, fRotNow);
	m_StatusDelegator.broadcast(eMax, fRotMax);
	m_StatusDelegator.broadcast(eGet, fGuage);


	/* think later */
	if (Get_RotMax() == m_iRotCount)
		m_iRotLevel++;
}

void CKena_Status::Set_CurArrowCount(_int iValue)
{
	/* Should be used only when arrow shoot */
	m_iCurArrowCount = iValue;

	CUI_ClientManager::UI_PRESENT eArrow = CUI_ClientManager::AMMO_ARROW;
	_float fCount = (_float)m_iCurArrowCount;
	m_StatusDelegator.broadcast(eArrow, fCount);

}
