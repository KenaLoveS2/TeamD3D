#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeItemBar final : public CUI_Node
{
private:
	CUI_NodeItemBar(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeItemBar(const CUI_NodeItemBar& rhs);
	virtual ~CUI_NodeItemBar() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

	virtual HRESULT			Save_Data();
	virtual HRESULT			Load_Data(wstring fileName);

public:
	_bool	MouseOverEvent();
	void	BackToNormal();

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_tchar*					m_szName;
	_float4					m_vColor;
	_float2					m_vOffset;
	_tchar*					m_szFont;
	_float2					m_vFontSize;

public:
	static	CUI_NodeItemBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END