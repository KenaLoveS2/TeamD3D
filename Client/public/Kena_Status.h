#pragma once
#include "Status.h"
#include "Client_Defines.h"
#include "Delegator.h"
#include "UI_ClientManager.h"

BEGIN(Client)

class CKena_Status final : public CStatus
{
public:
	enum ROTSTATE { RS_GOOD, RS_HIDE, RS_ACTIVE, RS_LIFT, RS_END };
	enum SKILLTAB { SKILL_MELEE, SKILL_SHIELD, SKILL_BOW, SKILL_BOMB, SKILL_ROT, SKILLTAB_END };

private:
	CKena_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_Status(const CKena_Status& rhs);
	virtual ~CKena_Status() = default;
		
public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg, CGameObject * pOwner);
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Imgui_RenderProperty() override;

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float>	m_StatusDelegator;


private:
	_float				m_fMaxShield = 0.f;
	_float				m_fShield = 0.f;
	_bool				m_bShieldBreak = false;
	_float				m_fInitShieldCoolTime = 1.5f;
	_float				m_fCurShieldCoolTime = 0.f;
	_float				m_fInitShieldRecoveryTime = 5.f;
	_float				m_fCurShieldRecoveryTime = 0.f;

	_int				m_iKarma = 0;
	_int				m_iRotLevel = 1;
	_int				m_iCurrentRotCount = 0;
	_int				m_iRotCountMin = 0;
	_int				m_iRotCountMax = 1;
	ROTSTATE			m_eRotState = RS_END;
	_int				m_iCrystal = 0;

	_int				m_iPipLevel = 1;
	_int				m_iMaxPIPCount = 0;
	_float				m_fCurPIPGuage = 0.f;

	_int				m_iMaxArrowCount = 4;
	_int				m_iCurArrowCount = 4;
	_float				m_fInitArrowCoolTime = 3.f;
	_float				m_fCurArrowCoolTime = 0.f;
	_float				m_fArrowGuage = 1.f;

	_int				m_iMaxBombCount = 0;
	_int				m_iCurBombCount = 0;
	_float				m_fInitBombCoolTime = 0.f;
	_float				m_fCurBombCoolTime[2] = { 0.0f, 0.0f };
	_bool				m_bUsed[2] = { false, false };

	/* UNLOCK SKILL */
	_bool				m_bSkills[SKILLTAB_END][5];

private:
	void				Update_ArrowCoolTime(_float fTimeDelta);
	void				Update_BombCoolTime(_float fTimeDelta);
	void				Update_ShieldRecovery(_float fTimeDelta);

public:
	void				Apply_Skill(SKILLTAB eCategory, _uint iSlot);
	void				Under_Shield(CStatus* pEnemyStatus);

public:
	static CKena_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr);
	virtual void				Free() override;

public:
	void	Respawn();
	HRESULT Save();
	HRESULT Load(const string& strFilePath);
	HRESULT	Save_RunTime(const wstring& wstrFilePath);
	HRESULT	Load_RunTime(const wstring& wstrFilePath);

public:
	inline _float Get_MaxShield() { return m_fMaxShield; }
	inline _float Get_Shield() { return m_fShield; }
	inline _float Get_CurShieldCoolTime() { return m_fCurShieldCoolTime; }
	inline _float Get_InitShieldCoolTime() { return m_fInitShieldCoolTime; }
	inline _float Get_CurShieldRecoveryTime() { return m_fCurShieldRecoveryTime; }
	inline _float Get_InitShieldRecoveryTime() { return m_fInitShieldRecoveryTime; }
	inline _bool Is_ShieldBreak() { return m_bShieldBreak; }

	inline _int Get_Karma() { return m_iKarma; }
	inline _int Add_Karma(_int iValue) { m_iKarma += iValue; }
	inline _int Get_RotLevel() { return m_iRotLevel; }
	inline _int Get_RotCount() { return m_iCurrentRotCount; }
	inline ROTSTATE Get_RotState() { return m_eRotState; }
	_int	Get_RotMax();
	inline _int Get_Crystal() { return m_iCrystal; }
	_int Get_MaxPIPCount();
	inline _int Get_CurPIPCount() { return (_int)m_fCurPIPGuage; }
	inline _float Get_CurPIPGuage() { return m_fCurPIPGuage;  }

	inline _int Get_MaxArrowCount() { return m_iMaxArrowCount; }
	inline _int Get_CurArrowCount() { return m_iCurArrowCount; }
	inline _float Get_InitArrowCoolTime() { return m_fInitArrowCoolTime; }
	inline _float Get_CurArrowCoolTime() { return m_fCurArrowCoolTime; }

	inline _int Get_MaxBombCount() { return m_iMaxBombCount; }
	inline _int Get_CurBombCount() { return m_iCurBombCount; }
	inline _float Get_InitBombCoolTime() { return m_fInitBombCoolTime; }
	//inline _float Get_CurBombCoolTime() { return m_fCurBombCoolTime; }
	inline _float Get_CurBombCoolTime(_int iIndex) { 
		if (iIndex < m_iMaxBombCount)
			return m_fCurBombCoolTime[iIndex];
		else
			return 0.0f;
	}


	const _bool Get_SkillState(SKILLTAB eCategory, _uint iSlot) const;

public:

	inline void Set_MaxShield(_float fValue) { m_fMaxShield = fValue; }
	inline void Set_Shield(_float fValue) { m_fShield = fValue; }
		   
	inline void Set_Karma(_int iValue) { m_iKarma = iValue; }
	inline void Set_RotLevel(_int iValue) { m_iRotLevel = iValue; }
	void Set_RotCount(_int iValue);
	inline void Set_RotState(ROTSTATE iValue) { m_eRotState = iValue; }
	inline void Set_Crystal(_int iValue) { m_iCrystal = iValue; }

	inline void Upgrade_MaxPIPCount() { m_iMaxPIPCount++; }
	inline void Set_MaxPIPCount(_int iValue) { m_iMaxPIPCount = iValue; }
	inline void Set_CurPIPGuage(_float fGuage) { m_fCurPIPGuage = fGuage; }
	void		Add_CurPipGuage();
	void		Plus_CurPIPGuage(_float fGuage);// { m_fCurPIPGuage = min(m_fCurPIPGuage + fGuage, m_iMaxPIPCount); }

	inline void Set_MaxArrowCount(_int iValue) { m_iMaxArrowCount = iValue; }
	void Set_CurArrowCount(_int iValue);
	
	inline void Set_InitArrowCoolTime(_float fValue) { m_fInitArrowCoolTime = fValue; }
	inline void Set_CurArrowCoolTime(_float fValue) { m_fCurArrowCoolTime = fValue; }

	inline void Set_MaxBombCount(_int iValue) { m_iMaxBombCount = iValue; }
	void		Set_CurBombCount(_int iValue);
	inline void Set_InitBombCoolTime(_float fValue) { m_fInitBombCoolTime = fValue; }
	//inline void Set_CurBombCoolTime(_float fValue) { m_fCurBombCoolTime = fValue; }

	void		Add_RotCount();
	void		Unlock_Skill(SKILLTAB eCategory, _uint iSlot);
};

END