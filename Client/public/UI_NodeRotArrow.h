#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeRotArrow final : public CUI_Node
{
public:
	enum EVENT { EVENT_FADE, EVENT_END };
private:
	CUI_NodeRotArrow(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeRotArrow(const CUI_NodeRotArrow& rhs);
	virtual ~CUI_NodeRotArrow() = default;

public:
	void	Set_Info(_int iRots);
	void	Set_Arrow(_float fX);

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
	_int					m_iRots;
	_tchar*					m_szInfo;

public:
	static	CUI_NodeRotArrow*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END