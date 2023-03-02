#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CUI;
class CUI_Canvas;
END

/* Only for Preparing Components and GameObjects */
/* UIs are in static Level, so this class should be in MainApp Class,
and all works are done before Imgui_UIEditor Starts. */
BEGIN(Client)
class CUI_NodeEffect;
class CUI_ClientManager final : public CBase
{
	DECLARE_SINGLETON(CUI_ClientManager);

public:
	enum UI_PRESENT {
		HUD_, HUD_HP, HUD_SHIELD, HUD_PIP, HUD_ROT,
		AMMO_BOMB, AMMO_ARROW,
		AIM_, AIM_ARROW, AIM_BOMB,
		UI_PRESENT_END
	};
	enum UI_FUNCTION { FUNC_DEFAULT, FUNC_LEVELUP, FUNC_SWITCH, FUNC_END };
	enum UI_CANVAS { CANVAS_HUD, CANVAS_AMMO, CANVAS_AIM, CANVAS_QUEST, CANVAS_END };
	enum UI_EVENT { EVENT_BARGUAGE, EVENT_CHANGEIMG, EVENT_ANIMATION, EVENT_END };
	enum UI_EFFECT {EFFECT_PIPFULL, EFFECT_ARROWFULL, EFFECT_BOMBFULL, EFFECT_END};

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
	void					Set_Canvas(UI_CANVAS eID, CUI_Canvas* pCanvas);
	CUI_Canvas*				Get_Canvas(UI_CANVAS eID);
	void					Set_Effect(UI_EFFECT eID, CUI_NodeEffect* pEffect);
	CUI_NodeEffect*			Get_Effect(UI_EFFECT eID);

private:
	vector<CUI_Canvas*>		m_vecCanvas;
	vector<CUI_NodeEffect*>	m_vecEffects;

public:
	virtual void Free() override;
};
END