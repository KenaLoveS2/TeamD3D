#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHUDShieldBar final : public CUI_Node
{
private:
	CUI_NodeHUDShieldBar(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHUDShieldBar(const CUI_NodeHUDShieldBar& rhs);
	virtual ~CUI_NodeHUDShieldBar() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void	Upgrade();

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private: /* For. Upgrade */
	_bool	m_bUpgrade;
	_float	m_fSpeed;
	_float	m_fSourScaleX;
	_float	m_fDestScaleX;
	_float	m_fSourTransX;
	_float	m_fDestTransX;

public:
	static	CUI_NodeHUDShieldBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END