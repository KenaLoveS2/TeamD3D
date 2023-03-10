#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeCrystal final : public CUI_Node
{
private:
	CUI_NodeCrystal(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeCrystal(const CUI_NodeCrystal& rhs);
	virtual ~CUI_NodeCrystal() = default;
	
public:
	_int	Get_Crystal() { return m_iCrystal; }
	void	Set_Crystal(_int iCrystal) { m_iCrystal = iCrystal; }
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
	_int	m_iCrystal;

public:
	static	CUI_NodeCrystal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END