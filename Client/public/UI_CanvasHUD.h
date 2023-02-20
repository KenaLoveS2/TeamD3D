#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"

BEGIN(Client)
class CUI_CanvasHUD final : public CUI_Canvas
{
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
	
	virtual void			Imgui_RenderProperty();

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	static	CUI_CanvasHUD*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END
