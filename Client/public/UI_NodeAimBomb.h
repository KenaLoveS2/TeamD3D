#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeAimBomb final : public CUI_Node
{
public:
	enum STATE { STATE_EMPTY, STATE_FULL, STATE_END };

private:
	CUI_NodeAimBomb(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeAimBomb(const CUI_NodeAimBomb& rhs);
	virtual ~CUI_NodeAimBomb() = default;

public:
	void	Set_State(STATE eState) { m_eState = eState; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	_bool			m_bSetInitialRatio;
	_float			m_fInitialRatioX;
	_float			m_fInitialRatioY;
private:
	STATE			m_eState;

public:
	static	CUI_NodeAimBomb*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END