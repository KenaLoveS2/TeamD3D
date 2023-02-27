#pragma once
#include "ImguiObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CCamera;
END

BEGIN(Client)

class CTool_Settings final : public CImguiObject
{
private:
	CTool_Settings(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT				Initialize(void* pArg = nullptr);
	virtual void					Imgui_RenderWindow() override;

private:
	CGameInstance*				m_pGameInstance = nullptr;

private:
	map<const _tchar*, CCamera*>*	m_mapCamera = nullptr;

private:
	HRESULT						Camera_Setting();

public:
	static CTool_Settings*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void					Free() override;
};

END