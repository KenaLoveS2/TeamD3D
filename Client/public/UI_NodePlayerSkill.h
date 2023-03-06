#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodePlayerSkill final : public CUI_Node
{
public:
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };
	enum CONDITION { CONDITION_PREVSKILL, CONDITION_LEVEL, CONDITION_ROTLEVEL, CONDITION_KARMA, CONDITION_END };

private:
	CUI_NodePlayerSkill(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodePlayerSkill(const CUI_NodePlayerSkill& rhs);
	virtual ~CUI_NodePlayerSkill() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	STATE		m_eState;
	CONDITION	m_eConditions[CONDITION_END];


public:
	static	CUI_NodePlayerSkill*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END