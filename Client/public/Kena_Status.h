#pragma once
#include "Status.h"
#include "Client_Defines.h"

BEGIN(Client)

class CKena_Status final : public CStatus
{
private:
	CKena_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_Status(const CKena_Status& rhs);
	virtual ~CKena_Status() = default;
		
public:
	virtual HRESULT Initialize_Prototype(const wstring& wstrFilePath) override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Imgui_RenderProperty() override;

private:
	_int				m_iMaxShield = 0;
	_int				m_iShield = 0;

	_int				m_iKarma = 0;
	_int				m_iRotLevel = 0;
	_int				m_iRotCount = 0;
	_int				m_iCrystal = 0;

	_int				m_iMaxPIPCount = 0;
	_int				m_iCurPIPCount = 0;
	_float				m_fInitPIPCoolTime = 0.f;
	_float				m_fCurPIPCoolTime = 0.f;

	_int				m_iMaxArrowCount = 0;
	_int				m_iCurArrowCount = 0;
	_float				m_fInitArrowCoolTime = 0.f;
	_float				m_fCurArrowCoolTime = 0.f;

	_int				m_iMaxBombCount = 0;
	_int				m_iCurBombCount = 0;
	_float				m_fInitBombCoolTime = 0.f;
	_float				m_fCurBombCoolTime = 0.f;

public: /* skill test */
	enum SKILL { SKILL_STICK, SKILL_SHIELD, SKILL_BOW, SKILL_BOBM, SKILL_END };
	enum LEVEL { LEVEL_0, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_END };
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };
	STATE		m_Skill[SKILL_END][LEVEL_END];

private:
	virtual HRESULT	Save_Status(const _tchar* pFilePath) override;
	virtual HRESULT	Load_Status(const _tchar* pFilePath) override;

public:
	static CKena_Status*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFilePath = L"");
	virtual CComponent*	Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void				Free() override;
};

END