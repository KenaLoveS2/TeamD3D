#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeButton final : public CUI_Node
{
public:
	enum TYPE { TYPE_CONFIRM, TYPE_LOGO, TYPE_END };
private:
	CUI_NodeButton(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeButton(const CUI_NodeButton& rhs);
	virtual ~CUI_NodeButton() = default;

public:
	void	Setting(wstring wstrName, TYPE eType);
	void	Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	_bool	MouseOverEvent();
	void	BackToNormal();

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_tchar*					m_szName;

private: /* Details */
	TYPE					m_eType;
	_float					m_fAlpha;
	_float4					m_vColor;
	_float2					m_vOffset;
	_tchar*					m_szFont;
	_float2					m_vFontSize;

public:
	static	CUI_NodeButton*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
