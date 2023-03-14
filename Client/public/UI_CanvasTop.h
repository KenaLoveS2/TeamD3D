#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasTop final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_MAPTITLE, 
		UI_EFFECT_BACKFLARE, UI_ROTLVUP, UI_EFFECT_FRONTFLAREROUND, UI_EFFECT_FRONTFLARE,
		UI_ROTCNT, UI_ROTGUAGE, UI_ROTARROW, UI_END };
private:
	CUI_CanvasTop(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasTop(const CUI_CanvasTop& rhs);
	virtual ~CUI_CanvasTop() = default;

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

private: /* Bind Functions */
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue);

private:
	vector<CUI_NodeEffect*>		m_vecEffects;
public:
	static	CUI_CanvasTop*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
