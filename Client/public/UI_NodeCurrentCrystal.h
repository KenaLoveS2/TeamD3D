#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeCurrentCrystal final : public CUI_Node
{
private:
	CUI_NodeCurrentCrystal(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeCurrentCrystal(const CUI_NodeCurrentCrystal& rhs);
	virtual ~CUI_NodeCurrentCrystal() = default;

public:
	_int	Get_Crystal() { return m_iCrystal; }
	void	Set_Crystal(_int iCrystal) { m_iCrystal = iCrystal; }
	void	Set_Font(wstring fontName, _float4 fontColor, _float2 fontSize, _float2 fontPos);
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
	_int		m_iCrystal;
	_tchar*		m_FontName;
	_float4		m_vFontColor;
	_float2		m_vFontSize;
	_float2		m_vFontPos;

public:
	static	CUI_NodeCurrentCrystal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END