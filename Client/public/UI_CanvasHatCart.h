#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"

BEGIN(Client)
class CUI_CanvasHatCart final : public CUI_Canvas
{
private:
	CUI_CanvasHatCart(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasHatCart(const CUI_CanvasHatCart& rhs);
	virtual ~CUI_CanvasHatCart() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;

private:
	vector<pair<wstring, _int>>		m_vecHats;

public:
	static	CUI_CanvasHatCart*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END