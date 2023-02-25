#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

/* Only for Preparing Components and GameObjects */
/* UIs are in static Level, so this class should be in MainApp Class, 
and all works are done before Imgui_UIEditor Starts. */
BEGIN(Client)
class CUI_ClientManager final : public CBase
{
	DECLARE_SINGLETON(CUI_ClientManager);

public:
	enum UI_EVENT { EVENT_BARGUAGE, EVENT_END };
	enum UI_HUD { HUD_HP, HUD_SHIELD, HUD_PIP, HUD_ROT, HUD_END };
private:
	CUI_ClientManager();
	virtual ~CUI_ClientManager() = default;

public:
	/* Ready_UIs : Ready_Proto_TextureComponent & Ready_Proto_GameObject & Ready_Clone_GameObject */
	HRESULT			Ready_UIs(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT			Ready_Proto_TextureComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT			Ready_InformationList();
	HRESULT			Ready_Proto_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT			Ready_Clone_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private: /* Save Tags to Engine::StringManger */
	void			Save_TextureComStrings(CGameInstance* pGameInstance, const _tchar* pTag);
	void			Save_CanvasStrings(CGameInstance* pGameInstance, const _tchar* pTag);
	void			Save_NodeStrings(CGameInstance* pGameInstance, const _tchar* pTag);


public:
	virtual void Free() override;
};
END