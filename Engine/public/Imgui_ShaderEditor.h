#pragma once

#include "ImguiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImgui_ShaderEditor final : public CImguiObject
{
private:
	CImgui_ShaderEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual  HRESULT Initialize(void* pArg) override;
	virtual void Imgui_FreeRender() override;

	static CImgui_ShaderEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	class CGameInstance* m_pGameInstance = nullptr;
};

END