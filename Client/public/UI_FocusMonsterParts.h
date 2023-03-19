#pragma once
#include "UI_Billboard.h"

BEGIN(Client)
class CUI_FocusMonsterParts final : public CUI_Billboard
{
public:
	typedef struct tagPartsDesc
	{
		_uint		iType;
	}PARTSDESC;



private:
	CUI_FocusMonsterParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FocusMonsterParts(const CUI_FocusMonsterParts& rhs);
	virtual ~CUI_FocusMonsterParts() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void					Do_Animation() { if(!m_bStart) m_bStart = true; }
	_bool					Is_AnimEnd() { return m_bEnd; }

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	PARTSDESC				m_tPartsDesc;
	_bool					m_bEnd;
	_bool					m_bStart;
	_float					m_fSpeed;

public:
	static	CUI_FocusMonsterParts*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr);
	virtual void						Free() override;
};
END