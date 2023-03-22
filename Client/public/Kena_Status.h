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

private:
	CKena_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_Status(const CKena_Status& rhs);
	virtual ~CKena_Status() = default;
		
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg, CGameObject * pOwner);
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Imgui_RenderProperty() override;

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float>	m_StatusDelegator;


private:
	_int				m_iMaxShield = 0;
	_int				m_iShield = 0;

	_int				m_iKarma = 0;
	_int				m_iRotLevel = 0;
	_int				m_iRotCount = 0;
	ROTSTATE			m_eRotState = RS_END;
	_int				m_iRotMax = 0;
	_int				m_iCrystal = 0;

	_int				m_iPipLevel = 0;
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
	_float				m_fCurBombCoolTime = 0.f;

private:
	void				Update_ArrowCoolTime(_float fTimeDelta);
	void				Update_BombCoolTime(_float fTimeDelta);

public: /* skill test */
	enum SKILL { SKILL_STICK, SKILL_SHIELD, SKILL_BOW, SKILL_BOBM, SKILL_END };
	enum LEVEL { LEVEL_0, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_END };
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };
	STATE		m_Skill[SKILL_END][LEVEL_END];

public:
	static CKena_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr);
	virtual void				Free() override;

public:
	HRESULT Save();
	HRESULT Load(const string & strFilePath);

public:
	inline _int Get_MaxShield() { return m_iMaxShield; }
	inline _int Get_Shield() { return m_iShield; }

	inline _int Get_Karma() { return m_iKarma; }
	inline _int Get_RotLevel() { return m_iRotLevel; }
	inline _int Get_RotCount() { return m_iRotCount; }
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
	inline _float Get_CurBombCoolTime() { return m_fCurBombCoolTime; }

public:

	inline void Set_MaxShield(_int iValue) { m_iMaxShield = iValue; }
	inline void Set_Shield(_int iValue) { m_iShield = iValue; }
		   
	inline void Set_Karma(_int iValue) { m_iKarma = iValue; }
	inline void Set_RotLevel(_int iValue) { m_iRotLevel = iValue; }
	void Set_RotCount(_int iValue);
	inline void Set_RotState(ROTSTATE iValue) { m_eRotState = iValue; }
	inline void Set_Crystal(_int iValue) { m_iCrystal = iValue; }

	inline void Upgrade_MaxPIPCount() { m_iMaxPIPCount++; }
	inline void Set_MaxPIPCount(_int iValue) { m_iMaxPIPCount = iValue; }
	inline void Set_CurPIPGuage(_float fGuage) { m_fCurPIPGuage = fGuage; }
	inline void Plus_CurPIPGuage(_float fGuage) {	
		m_fCurPIPGuage = min(m_fCurPIPGuage+fGuage, m_iMaxPIPCount); 
	}

	inline void Set_MaxArrowCount(_int iValue) { m_iMaxArrowCount = iValue; }
	void Set_CurArrowCount(_int iValue);
	
	inline void Set_InitArrowCoolTime(_float fValue) { m_fInitArrowCoolTime = fValue; }
	inline void Set_CurArrowCoolTime(_float fValue) { m_fCurArrowCoolTime = fValue; }

	inline void Set_MaxBombCount(_int iValue) { m_iMaxBombCount = iValue; }
	inline void Set_CurBombCount(_int iValue) { m_iCurBombCount = iValue; }
	inline void Set_InitBombCoolTime(_float fValue) { m_fInitBombCoolTime = fValue; }
	inline void Set_CurBombCoolTime(_float fValue) { m_fCurBombCoolTime = fValue; }
};

END