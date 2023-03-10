#pragma once
#include "UI_Billboard.h"

BEGIN(Client)
class CMonster;
class CUI_MonsterHP final : public CUI_Billboard
{
public:
	typedef struct tagWorldDesc
	{
		CMonster*	pOwner;
		_float2		vPos;
	}WORLDUIDESC;

private:
	CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHP(const CUI_MonsterHP& rhs);
	virtual ~CUI_MonsterHP() = default;

public:
	void	Setting(WORLDUIDESC tDesc) {
		m_tWorldDesc.pOwner = tDesc.pOwner;

	}
public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	WORLDUIDESC				m_tWorldDesc;

public:
	static	CUI_MonsterHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END