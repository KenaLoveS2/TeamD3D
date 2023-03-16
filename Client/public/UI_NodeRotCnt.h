#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeRotCnt final : public CUI_Node
{
public:
	enum EVENT { EVENT_FADE, EVENT_END };

private:
	CUI_NodeRotCnt(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeRotCnt(const CUI_NodeRotCnt& rhs);
	virtual ~CUI_NodeRotCnt() = default;

public:
	void	Set_Info(_int iRotMax);

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
	_int					m_iRotMax;
	_tchar*					m_szInfo;

public:
	static	CUI_NodeRotCnt*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END