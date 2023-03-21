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
	void					Do_Animation() {
		if (!m_bStart)
		{
			m_bStart = true;
			m_pTransformCom->Set_Scaled(m_vStartScale);
		}
	}
	void					Set_StartFalse() { m_bStart = false; }
	_bool					Is_AnimEnd() { return m_bEnd; }
	void					Reset() { m_bStart = false;  m_bEnd = false; }

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	PARTSDESC				m_tPartsDesc;
	_bool					m_bEnd;
	_bool					m_bStart;
	_float					m_fSpeed;
	_float3					m_vStartScale;

public:
	static	CUI_FocusMonsterParts*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr);
	virtual void						Free() override;
};
END