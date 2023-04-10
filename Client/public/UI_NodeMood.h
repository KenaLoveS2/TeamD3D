#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeMood final : public CUI_Node
{
public:
	enum EVENT { EVENT_FADE, EVENT_END };
	enum STATE { STATE_HIT, STATE_PARRY, STATE_HEAL, STATE_FADE, STATE_DAZZLE, STATE_END };

private:
	CUI_NodeMood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeMood(const CUI_NodeMood& rhs);
	virtual ~CUI_NodeMood() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void					MoodOn(STATE eState);


private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	STATE			m_eState;
	_float4			m_vColor;

public:
	static	CUI_NodeMood*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END
