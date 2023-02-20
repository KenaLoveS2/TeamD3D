#pragma once
#include "Base.h"

/* Releated with Component and GameObject Creation for UI */

BEGIN(Engine)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager);

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//void			Tick(_float fTimeDelta);
	//void			Late_Tick(_float fTimeDelta);
	//HRESULT		Render();



public:	/* For. Imgui Tool */
	void			Imgui_CanvasViewer();


public:
	virtual void Free() override;
};
END

