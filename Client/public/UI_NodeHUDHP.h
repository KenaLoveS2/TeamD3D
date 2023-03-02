#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHUDHP final : public CUI_Node
{
public:
	enum EVENT_ID { EVENT_GUAGE,EVENT_END };
private:
	CUI_NodeHUDHP(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHUDHP(const CUI_NodeHUDHP& rhs);
	virtual ~CUI_NodeHUDHP() = default;

public: /* Events */
	void	Set_Guage(_float fGuage);
public:
	void	Upgrade();

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT		SetUp_Components() override;
	virtual HRESULT		SetUp_ShaderResources() override;

private: /* For. Upgrade */
	_bool	m_bUpgrade;
	_float	m_fSpeed;
	_float	m_fSourScaleX;
	_float	m_fDestScaleX;
	_float	m_fSourTransX;
	_float	m_fDestTransX;

public:
	static	CUI_NodeHUDHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END