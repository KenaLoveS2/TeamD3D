#pragma once
#include "ImguiObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CTool_Animation final : public CImguiObject
{
private:
	CTool_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT				Initialize(void* pArg = nullptr);
	virtual void					Imgui_RenderWindow() override;
	
private:
	CGameInstance*				m_pGameInstance = nullptr;

private:
	_uint							m_iCurrentLevel = LEVEL_END;
	map<const _tchar*, CGameObject*>*	m_mapAnimObject;

private:
	void							Update_Level();
	CGameObject*				Find_GameObject(_int iIndex);

public:
	static CTool_Animation*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void					Free() override;
};

END