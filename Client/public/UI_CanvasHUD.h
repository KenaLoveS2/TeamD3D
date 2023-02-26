#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasHUD final : public CUI_Canvas
{

public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_HPBAR, UI_HPGUAGE, UI_SHIELDBAR, UI_SHIELD, UI_ROT, UI_PIPBAR, UI_PIPGAUGE, UI_END };
private:
	CUI_CanvasHUD(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasHUD(const CUI_CanvasHUD& rhs);
	virtual ~CUI_CanvasHUD() = default;

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
	void	Function(CUI_ClientManager::UI_PRESENT eType, _float fValue);



public:
	static	CUI_CanvasHUD*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END
