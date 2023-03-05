#pragma once

#include "ImguiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImGui_PhysX final : public CImguiObject
{
public:
	CImGui_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_FreeRender() override;

	static CImGui_PhysX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	class CGameInstance* m_pGameInstance = nullptr;
	class CPhysX_Manager* m_pPhysXManager = nullptr;
};

END