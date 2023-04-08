#include "stdafx.h"
#include "..\public\Kena_Status.h"
#include "GameInstance.h"
#include "Kena.h"
#include "SpiritArrow.h"

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

	ZeroMemory(m_bSkills, 25);

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
	CUI_ClientManager::UI_PRESENT eBomb			= CUI_ClientManager::AMMO_BOMB;
	CUI_ClientManager::UI_PRESENT eCool			= CUI_ClientManager::AMMO_BOMBCOOL;
	CUI_ClientManager::UI_PRESENT eBombEffect	= CUI_ClientManager::AMMO_BOMBEFFECT;

	for (_int i = 0; i < m_iMaxBombCount; ++i)
	{
		if (m_bUsed[i] == true)
		{
			m_fCurBombCoolTime[i] += fTimeDelta;

			/* FullFilled */
			if (m_fCurBombCoolTime[i] >= m_fInitBombCoolTime)
			{
				++m_iCurBombCount;
				m_fCurBombCoolTime[i] = m_fInitBombCoolTime;
				_float fIndex = (_float)i;
				m_StatusDelegator.broadcast(eBombEffect, fIndex);
				m_bUsed[i] = false;
			}
			else
			{
				_float fGuage = m_fCurBombCoolTime[i] / m_fInitBombCoolTime;
				if (i == 1)
					fGuage += 10.f;
				m_StatusDelegator.broadcast(eCool, fGuage);
			}
		}
	}
}

void CKena_Status::Update_ShieldRecovery(_float fTimeDelta)
{
	CUI_ClientManager::UI_PRESENT eShield = CUI_ClientManager::HUD_SHIELD;

	if (m_bShieldBreak == false)
	{
		if (m_fShield == m_fMaxShield)
		{
			_float fGuage = 1.f;
			m_StatusDelegator.broadcast(eShield, fGuage);
			return;
		}

		if (m_fShield < m_fMaxShield)
		{
			if (m_fCurShieldCoolTime < m_fInitShieldCoolTime)
				m_fCurShieldCoolTime += fTimeDelta;
			else
			{
				m_fShield += fTimeDelta * 3.f;
				_float fGuage = m_fShield / m_fMaxShield;
				m_StatusDelegator.broadcast(eShield, fGuage);
			}
		}
		else
		{
			m_fShield = m_fMaxShield;
			_float fGuage = 1.f;
			m_StatusDelegator.broadcast(eShield, fGuage);
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

		_float fGuage = 0.f;
		m_StatusDelegator.broadcast(eShield, fGuage);

	}
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
				m_fShield = m_fMaxShield;


				CUI_ClientManager::UI_PRESENT eUpgrade = CUI_ClientManager::HUD_SHIELD_UPGRADE;
				m_StatusDelegator.broadcast(eUpgrade, m_fMaxShield);
			}
			else if (iSlot == 4)
			{
				m_fMaxShield *= 1.5f;
				m_fShield = m_fMaxShield;

				CUI_ClientManager::UI_PRESENT eUpgrade = CUI_ClientManager::HUD_SHIELD_UPGRADE;
				m_StatusDelegator.broadcast(eUpgrade, m_fMaxShield);
			}

			break;
		}
	case CKena_Status::SKILL_BOW:
		{
			CUI_ClientManager::UI_PRESENT eArrowUpgrade = CUI_ClientManager::AMMO_ARROWUPRADE;

			if (iSlot == 2)
			{
				vector<CSpiritArrow*>* pArrows = dynamic_cast<CKena*>(m_pOwner)->Get_Arrows();

				for (auto pArrow : *pArrows)
					pArrow->Set_Damage(8);
			}
			else if (iSlot == 3)
			{
				m_iMaxArrowCount++;
				m_iCurArrowCount = m_iMaxArrowCount;

				_float fMax = (_float)m_iMaxArrowCount;
				m_StatusDelegator.broadcast(eArrowUpgrade, fMax);
			}
			else if (iSlot == 4)
			{
				vector<CSpiritArrow*>* pArrows = dynamic_cast<CKena*>(m_pOwner)->Get_Arrows();

				for (auto pArrow : *pArrows)
					pArrow->Set_Damage(12);
			}

			break;
		}
	case CKena_Status::SKILL_BOMB:
		{
			CUI_ClientManager::UI_PRESENT eBombUpgrade = CUI_ClientManager::AMMO_BOMBUPGRADE;

			if (iSlot == 3)
			{
				m_iMaxBombCount++;
				m_iCurBombCount = m_iMaxBombCount;

				_float fMax = (_float)m_iMaxBombCount;
				m_StatusDelegator.broadcast(eBombUpgrade, fMax);
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
	jKenaStatus["17. m_fCurBombCoolTime"] = m_fCurBombCoolTime[0];

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
	jKenaStatus["17. m_fCurBombCoolTime"].get_to<_float>(m_fCurBombCoolTime[0]);
	jKenaStatus["17. m_fCurBombCoolTime"].get_to<_float>(m_fCurBombCoolTime[1]);


	m_iPipLevel = 1;
	m_eRotState = RS_GOOD;
	m_fArrowGuage = 1.f;
	m_bUsed[0] = false;
	m_bUsed[1] = false;


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

	//m_iCurrentRotCount = iValue;

	//CUI_ClientManager::UI_PRESENT eMax = CUI_ClientManager::TOP_ROTMAX;
	//CUI_ClientManager::UI_PRESENT eNow = CUI_ClientManager::TOP_ROTCUR;
	//CUI_ClientManager::UI_PRESENT eGet = CUI_ClientManager::TOP_ROTGET;

	//_float fRotMax = (_float)Get_RotMax();
	//_float fRotNow = (_float)m_iCurrentRotCount;
	//_float fGuage = fRotNow / fRotMax;

	//m_StatusDelegator.broadcast(eNow, fRotNow);
	//m_StatusDelegator.broadcast(eMax, fRotMax);
	//m_StatusDelegator.broadcast(eGet, fGuage);


	///* think later */
	//if (Get_RotMax() == m_iCurrentRotCount)
	//	m_iRotLevel++;
}

void CKena_Status::Set_CurArrowCount(_int iValue)
{
	/* Should be used only when arrow shoot */
	m_iCurArrowCount = iValue;

	CUI_ClientManager::UI_PRESENT eArrow = CUI_ClientManager::AMMO_ARROW;
	_float fCount = (_float)m_iCurArrowCount;
	m_StatusDelegator.broadcast(eArrow, fCount);

}

void CKena_Status::Set_CurBombCount(_int iValue)
{
	m_iCurBombCount = iValue;

	for (_int i = 0; i < m_iMaxBombCount; ++i)
	{
		if (m_bUsed[i] == false)
		{
			m_bUsed[i] = true;
			m_fCurBombCoolTime[i] = 0.0f;

			CUI_ClientManager::UI_PRESENT eBomb = CUI_ClientManager::AMMO_BOMB;
			_float fIndex = (_float)i;
			m_StatusDelegator.broadcast(eBomb, fIndex);

			return;
		}
	}
}

void CKena_Status::Add_RotCount()
{
	//m_iCurrentRotCount = 0;
	//m_iRotLevel = 1;
	//m_iRotCountMax = 2;
	m_iCurrentRotCount++;

	/* UI Rot Get */
	CUI_ClientManager::UI_PRESENT eMax = CUI_ClientManager::TOP_ROTMAX;
	CUI_ClientManager::UI_PRESENT eNow = CUI_ClientManager::TOP_ROTCUR;
	CUI_ClientManager::UI_PRESENT eGet = CUI_ClientManager::TOP_ROTGET;

	_float fMin = 0.0f;

	if (m_iRotLevel == 2)
		fMin = 2.0f;
	else if (m_iRotLevel == 3)
		fMin = 5.0f;
	else if (m_iRotLevel == 4)
		fMin = 8.0f;

	_float fRotMax = (_float)m_iRotCountMax;
	_float fRotNow = (_float)m_iCurrentRotCount;
	_float fGuage = (fRotNow-fMin) / (fRotMax-fMin);

	m_StatusDelegator.broadcast(eNow, fRotNow);
	m_StatusDelegator.broadcast(eMax, fRotMax);
	m_StatusDelegator.broadcast(eGet, fGuage);
	/* ~UI Rot Get */

	if (m_iCurrentRotCount >= m_iRotCountMax && m_iRotLevel != 5)
	{
		dynamic_cast<CKena*>(m_pOwner)->Set_State(CKena::STATE_LEVELUP, true);
		m_iRotLevel++;
		m_iRotCountMin = m_iRotCountMax;

		//m_iPipLevel = m_iRotLevel;
		//m_iMaxPIPCount = m_iRotLevel;
		//m_fCurPIPGuage = (_float)m_iMaxPIPCount;

		if (m_iRotLevel == 2)
		{
			m_iRotCountMax = 5;
			m_iPipLevel = 2;

			m_iMaxPIPCount = m_iPipLevel;
			m_fCurPIPGuage = (_float)m_iMaxPIPCount;

			/* Pip Level Up */
			CUI_ClientManager::UI_PRESENT ePipUpgrade = CUI_ClientManager::HUD_PIP_UPGRADE;
			m_StatusDelegator.broadcast(ePipUpgrade, m_fCurPIPGuage);
		}

		else if (m_iRotLevel == 3)
		{
			m_iRotCountMax = 8;
		}

		else if (m_iRotLevel == 4)
		{
			m_iRotCountMax = 10;
			m_iPipLevel = 3;

			m_iMaxPIPCount = m_iPipLevel;
			m_fCurPIPGuage = (_float)m_iMaxPIPCount;

			/* Pip Level Up */
			CUI_ClientManager::UI_PRESENT ePipUpgrade = CUI_ClientManager::HUD_PIP_UPGRADE;
			m_StatusDelegator.broadcast(ePipUpgrade, m_fCurPIPGuage);
		}

		/* Rot Level Up */
		CUI_ClientManager::UI_PRESENT eRotLvUp = CUI_ClientManager::TOP_ROT_LVUP;
		_float fLevel = (_float)m_iRotLevel;
		m_StatusDelegator.broadcast(eRotLvUp, fLevel);


	}

}

void CKena_Status::Unlock_Skill(SKILLTAB eCategory, _uint iSlot)
{
	if (eCategory >= CKena_Status::SKILLTAB_END)
		return;

	if (iSlot >= 5)
		return;

	if (m_bSkills[eCategory][iSlot] == false)
	{
		m_bSkills[eCategory][iSlot] = true;
		Apply_Skill(eCategory, iSlot);
	}
}
