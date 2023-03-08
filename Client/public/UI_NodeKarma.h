#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeKarma final : public CUI_Node
{
private:
	CUI_NodeKarma(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeKarma(const CUI_NodeKarma& rhs);
	virtual ~CUI_NodeKarma() = default;

public:
	_int	Get_Karma() { return m_iKarma; }

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
	_int		m_iKarma;

public:
	static	CUI_NodeKarma*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
