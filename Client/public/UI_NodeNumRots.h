#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeNumRots final : public CUI_Node
{
private:
	CUI_NodeNumRots(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeNumRots(const CUI_NodeNumRots& rhs);
	virtual ~CUI_NodeNumRots() = default;

public:
	_int	Get_NumRots() { return m_iNumRots; }
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
	_int	m_iNumRots;

public:
	static	CUI_NodeNumRots*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END