#pragma once
#include "Base.h"

/* Releated with Component and GameObject Creation for UI */

BEGIN(Engine)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager);

	enum ENGINE_DLL WSTRING_KEY {
		WSTRKEY_TEXTURE_PROTOTAG,
		WSTRKEY_CANVAS_PROTOTAG, WSTRKEY_CANVAS_CLONETAG,
		WSTRKEY_NODE_PROTOTAG, WSTRKEY_NODE_CLONETAG,
		WSTRKEY_END
	};
	enum ENGINE_DLL STRING_KEY { STRKEY_TEXTURE_NAME, STRKEY_CANVAS_NAME, STRKEY_NODE_NAME, 
		STRKEY_RENDERPASS, STRKEY_END };

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);


public:	/* For. Imgui Tool */
	void			Imgui_CanvasViewer();


public:
	virtual void Free() override;
};
END

