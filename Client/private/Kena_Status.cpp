#include "stdafx.h"
#include "..\public\Kena_Status.h"
#include "GameInstance.h"
#include "Kena.h"

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
	__super::Tick(fTimeDelta);

	Update_ArrowCoolTime(fTimeDelta);
	Update_BombCoolTime(fTimeDelta);
	Update_ShieldRecovery(fTimeDelta);

	/* TEST */
	if (CGameInstance::GetInstance()->Key_Down(DIK_G))
		Unlock_Skill(CKena_Status::SKILL_SHIELD, 2);
	if (CGameInstance::GetInstance()->Key_Down(DIK_H))
		Unlock_Skill(CKena_Status::SKILL_SHIELD, 4);
	if (CGameInstance::GetInstance()->Key_Down(DIK_J))
		Unlock_Skill(CKena_Status::SKILL_BOW, 3);
	if (CGameInstance::GetInstance()->Key_Down(DIK_K))
		Unlock_Skill(CKena_Status::SKILL_BOW, 4);
	if (CGameInstance::GetInstance()->Key_Down(DIK_L))
		Unlock_Skill(CKena_Status::SKILL_BOMB, 3);
	if (CGameInstance::GetInstance()->Key_Down(DIK_SEMICOLON))
		Add_RotCount();
	if (CGameInstance::GetInstance()->Key_Down(DIK_APOSTROPHE))
	{
		/* RESET */
		m_iRotLevel = 1;
		m_iCurrentRotCount = 0;
		m_iRotCountMax = 2;

		m_iPipLevel = m_iRotLevel;
		m_iMaxPIPCount = m_iRotLevel;
		m_fCurPIPGuage = (_float)m_iMaxPIPCount;

		m_fMaxShield = 100.f;
		m_fShield = 100.f;
		m_iMaxArrowCount = 4;
		m_iCurArrowCount = 4;
		m_iMaxBombCount = 1;
		m_iCurBombCount = 1;
		m_bSkills[SKILL_SHIELD][2] = false;
		m_bSkills[SKILL_SHIELD][4] = false;
		m_bSkills[SKILL_BOW][3] = false;
		m_bSkills[SKILL_BOW][4] = false;
		m_bSkills[SKILL_BOMB][3] = false;
	}
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
	CUI_ClientManager::UI_PRESENT eBomb = CUI_ClientManager::AMMO_BOMB;
	CUI_ClientManager::UI_PRESENT eCool = CUI_ClientManager::AMMO_BOMBCOOL;
	CUI_ClientManager::UI_PRESENT eBombEffect = CUI_ClientManager::AMMO_BOMBEFFECT;
	CUI_ClientManager::UI_PRESENT eReCharge = CUI_ClientManager::AMMO_BOMBRECHARGE;
	CUI_ClientManager::UI_PRESENT eMoment = CUI_ClientManager::AMMO_BOMBMOMENT;

	_float fGuage = 0.f;

	if (m_iCurBombCount == m_iMaxBombCount) /* Full */
	{
		m_fCurBombCoolTime = 0.0f;
		fGuage = 1.0f;
		m_StatusDelegator.broadcast(eCool, fGuage);
	}
	else /* Not Full */
	{
		if (m_fCurBombCoolTime == 0.f ) /* Bomb Use Moment */
		{
			_float fCount = (_float)m_iCurBombCount;
			m_StatusDelegator.broadcast(eMoment, fGuage);
			m_StatusDelegator.broadcast(eBomb, fCount);
		}


		m_fCurBombCoolTime += fTimeDelta;
		if (m_fCurBombCoolTime >= m_fInitBombCoolTime)
		{
			/* Fullfilll Effect Call */
			m_StatusDelegator.broadcast(eBombEffect, fGuage);

			++m_iCurBombCount;
			if (m_iCurBombCount < m_iMaxBombCount)
				m_fCurBombCoolTime = 0.0f;
			else
				m_fCurBombCoolTime = m_fInitBombCoolTime;

			/* ReCharge */
			_float fIndex = (_float)m_iCurBombCount - 1;
			
			m_StatusDelegator.broadcast(eReCharge, fIndex);
		}
				fGuage = m_fCurBombCoolTime / m_fInitBombCoolTime;
		m_StatusDelegator.broadcast(eCool, fGuage);

	}
}

void CKena_Status::Update_ShieldRecovery(_float fTimeDelta)
{
	if (m_bShieldBreak == false)
	{
		if (m_fShield < m_fMaxShield)
		{
			if (m_fCurShieldCoolTime < m_fInitShieldCoolTime)
				m_fCurShieldCoolTime += fTimeDelta;
			else
			{
				m_fShield += fTimeDelta * 3.f;

				/* NEED : UI SHIELD GAGE UPDATE */
			}
		}
		else
		{
			m_fShield = m_fMaxShield;

			/* NEED : UI SHIELD GAGE UPDATE */
		}
	}
	else
	{
		if (m_fCurShieldRecoveryTime < m_fInitShieldRecoveryTime)
			m_fCurShieldRecoveryTime += fTimeDelta;
		else
		{
			m_bShieldBreak = false;
			m_fCurShieldRecoveryTime = 0.f;
			m_fCurShieldCoolTime = m_fInitShieldCoolTime;
		}
	}
	CUI_ClientManager::UI_PRESENT eShield = CUI_ClientManager::HUD_SHIELD;
	m_StatusDelegator.broadcast(eShield, m_fShield);
}

void CKena_Status::Apply_Skill(SKILLTAB eCategory, _uint iSlot)
{
	switch (eCategory)
	{
	case CKena_Status::SKILL_MELEE:
		{
			break;
		}
	case CKena_Status::SKILL_SHIELD:
		{
			if (iSlot == 2)
			{
				m_fMaxShield *= 1.5f;

				/* NEED : UI SHILED GAGE UP */
			}
			else if (iSlot == 4)
			{
				m_fMaxShield *= 1.5f;

				/* NEED : UI SHILED GAGE UP */
			}

			break;
		}
	case CKena_Status::SKILL_BOW:
		{
			if (iSlot == 3)
			{
				m_iMaxArrowCount++;

				/* NEED : UI ARROW COUNT UP */
			}
			else if (iSlot == 4)
			{
				m_iMaxArrowCount++;

				/* NEED : UI ARROW COUNT UP */
			}

			break;
		}
	case CKena_Status::SKILL_BOMB:
		{
			if (iSlot == 3)
			{
				m_iMaxBombCount++;

				/* NEED : UI BOMB COUNT UP */
			}

			break;
		}
	case CKena_Status::SKILL_ROT:
		{
			break;
		}
	}
}

void CKena_Status::Under_Shield(CStatus * pEnemyStatus)
{
	if (pEnemyStatus == nullptr) return;

	m_fShield -= (_float)pEnemyStatus->Get_Attack();
	m_fCurShieldCoolTime = 0.f;

	if (m_fShield <= 0.f)
	{
		m_fShield = 0.f;
		m_bShieldBreak = true;
	}
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

	jKenaStatus["00. m_fMaxShield"] = m_fMaxShield;
	jKenaStatus["01. m_fShield"] = m_fShield;
	jKenaStatus["02. m_fInitShieldCoolTime"] = m_fInitShieldCoolTime;
	jKenaStatus["03. m_fInitShieldRecoveryTime"] = m_fInitShieldRecoveryTime;
	jKenaStatus["04. m_iKarma"] = m_iKarma;
	jKenaStatus["05. m_iRotLevel"] = m_iRotLevel;
	jKenaStatus["06. m_iRotCount"] = m_iCurrentRotCount;
	jKenaStatus["07. m_iCrystal"] = m_iCrystal;
	jKenaStatus["08. m_iMaxPIPCount"] = m_iMaxPIPCount;
	jKenaStatus["09. m_fCurPIPGuage"] = m_fCurPIPGuage;
	jKenaStatus["10. m_iMaxArrowCount"] = m_iMaxArrowCount;
	jKenaStatus["11. m_iCurArrowCount"] = m_iCurArrowCount;
	jKenaStatus["12. m_fInitArrowCoolTime"] = m_fInitArrowCoolTime;
	jKenaStatus["13. m_fCurArrowCoolTime"] = m_fCurArrowCoolTime;
	jKenaStatus["14. m_iMaxBombCount"] = m_iMaxBombCount;
	jKenaStatus["15. m_iCurBombCount"] = m_iCurBombCount;
	jKenaStatus["16. m_fInitBombCoolTime"] = m_fInitBombCoolTime;
	jKenaStatus["17. m_fCurBombCoolTime"] = m_fCurBombCoolTime;

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

	jKenaStatus["00. m_fMaxShield"].get_to<_float>(m_fMaxShield);
	jKenaStatus["01. m_fShield"].get_to<_float>(m_fShield);
	jKenaStatus["02. m_fInitShieldCoolTime"].get_to<_float>(m_fInitShieldCoolTime);
	jKenaStatus["03. m_fInitShieldRecoveryTime"].get_to<_float>(m_fInitShieldRecoveryTime);
	jKenaStatus["04. m_iKarma"].get_to<_int>(m_iKarma);
	jKenaStatus["05. m_iRotLevel"].get_to<_int>(m_iRotLevel);
	jKenaStatus["06. m_iRotCount"].get_to<_int>(m_iCurrentRotCount);
	jKenaStatus["07. m_iCrystal"].get_to<_int>(m_iCrystal);
	jKenaStatus["08. m_iMaxPIPCount"].get_to<_int>(m_iMaxPIPCount);
	jKenaStatus["09. m_fCurPIPGuage"].get_to<_float>(m_fCurPIPGuage);
	jKenaStatus["10. m_iMaxArrowCount"].get_to<_int>(m_iMaxArrowCount);
	jKenaStatus["11. m_iCurArrowCount"].get_to<_int>(m_iCurArrowCount);
	jKenaStatus["12. m_fInitArrowCoolTime"].get_to<_float>(m_fInitArrowCoolTime);
	jKenaStatus["13. m_fCurArrowCoolTime"].get_to<_float>(m_fCurArrowCoolTime);
	jKenaStatus["14. m_iMaxBombCount"].get_to<_int>(m_iMaxBombCount);
	jKenaStatus["15. m_iCurBombCount"].get_to<_int>(m_iCurBombCount);
	jKenaStatus["16. m_fInitBombCoolTime"].get_to<_float>(m_fInitBombCoolTime);
	jKenaStatus["17. m_fCurBombCoolTime"].get_to<_float>(m_fCurBombCoolTime);

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

const _bool CKena_Status::Get_SkillState(SKILLTAB eCategory, _uint iSlot) const
{
	if (eCategory >= CKena_Status::SKILLTAB_END)
		return false;

	if (iSlot >= 5)
		return false;

	return m_bSkills[eCategory][iSlot];
}

void CKena_Status::Set_RotCount(_int iValue)
{
	/* NEED : ADD_ROTCOUNT() 수정 후에 이 함수는 비워줘. */

	m_iCurrentRotCount = iValue;

	CUI_ClientManager::UI_PRESENT eMax = CUI_ClientManager::TOP_ROTMAX;
	CUI_ClientManager::UI_PRESENT eNow = CUI_ClientManager::TOP_ROTCUR;
	CUI_ClientManager::UI_PRESENT eGet = CUI_ClientManager::TOP_ROTGET;

	_float fRotMax = (_float)Get_RotMax();
	_float fRotNow = (_float)m_iCurrentRotCount;
	_float fGuage = fRotNow / fRotMax;

	m_StatusDelegator.broadcast(eNow, fRotNow);
	m_StatusDelegator.broadcast(eMax, fRotMax);
	m_StatusDelegator.broadcast(eGet, fGuage);


	/* think later */
	if (Get_RotMax() == m_iCurrentRotCount)
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

void CKena_Status::Add_RotCount()
{
	/* NEED : UI UPDATE ROT COUNT GAGE */
	m_iCurrentRotCount++;

	if (m_iCurrentRotCount >= m_iRotCountMax && m_iRotLevel != 4)
	{
		dynamic_cast<CKena*>(m_pOwner)->Set_State(CKena::STATE_LEVELUP, true);
		m_iRotLevel++;
		m_iRotCountMin = m_iRotCountMax;

		m_iPipLevel = m_iRotLevel;
		m_iMaxPIPCount = m_iRotLevel;
		m_fCurPIPGuage = (_float)m_iMaxPIPCount;

		if (m_iRotLevel == 2)
			m_iRotCountMax = 5;

		else if (m_iRotLevel == 3)
			m_iRotCountMax = 8;

		else if (m_iRotLevel == 4)
			m_iRotCountMax = 10;

		/* NEED : UI MAX PIP COUNT INCREASE */
		/* NEED : UI ROT COUNT GAGE RESET */
	}
}

void CKena_Status::Unlock_Skill(SKILLTAB eCategory, _uint iSlot)
{
	if (eCategory >= CKena_Status::SKILLTAB_END)
		return;

	if (iSlot >= 5)
		return;

	m_bSkills[eCategory][iSlot] = true;
	
	/* NEED : UI SKILL SLOT STATE CHANGE */

	Apply_Skill(eCategory, iSlot);
}
