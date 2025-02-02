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
class CUI_CanvasConfirm;
class CUI_ClientManager final : public CBase
{
	DECLARE_SINGLETON(CUI_ClientManager);

public:
	enum UI_PRESENT { /* Present UI Function */
		
		HUD_, HUD_HP, HUD_SHIELD, HUD_PIP, HUD_ROT,
		HUD_SHIELD_UPGRADE, HUD_HP_UPGRADE, HUD_PIP_UPGRADE,

		AMMO_ARROW, AMMO_ARROWCOOL, AMMO_ARROWEFFECT, AMMO_ARROWRECHARGE, AMMO_ARROWUPRADE,
		AMMO_BOMB, AMMO_BOMBCOOL, AMMO_BOMBEFFECT, AMMO_BOMBRECHARGE, AMMO_BOMBMOMENT, AMMO_BOMBUPGRADE,

		AIM_, AIM_ARROW, AIM_BOMB,
		QUEST_, QUEST_LINE, QUEST_CLEAR, QUEST_CLEAR_ALL,
		INV_, INV_KARMA, INV_NUMROTS, INV_CRYSTAL,
		INV_UPGRADE, INV_LEVEL_CHANGE,
		
		TOP_TITLE,TOP_ROTMAX, TOP_ROTCUR, TOP_ROTGET/* Calculate */,
		TOP_BOSS, TOP_ROT_LVUP,
		TOP_MOOD_HIT, TOP_MOOD_PARRY, TOP_MOOD_HEAL, TOP_MOOD_FADE, TOP_MOOD_DAZZLE,
		TOP_ALARM_CHEST, TOP_ALARM_HAT,

		BOT_LETTERBOX,BOT_CHAT, BOT_LINE,
		BOT_KEY_USEROT, BOT_KEY_OPENSHOP, BOT_KEY_OPENCHEST, 
		BOT_KEY_AIMARROW, BOT_KEY_AIMBOMB, BOT_KEY_MOVEROT,
		BOT_KEY_OFF,

		HATCART_, 

		INFO_, 

		MINIGAME_TOTAL_TARGET, MINIGAME_HIT, MINIGAME_START,
		MINIGAME_, /* result */

		/*Must Be Last ... ! */
		UI_CONFIRM,
		UI_PRESENT_END
	};
	enum UI_FUNCTION { FUNC_DEFAULT, FUNC_LEVELUP, FUNC_SWITCH, FUNC_CHECK, FUNC_END };
	enum UI_CANVAS { CANVAS_HUD, CANVAS_AMMO, CANVAS_AIM, CANVAS_QUEST, CANVAS_INV, CANVAS_UPGRADE, 
						CANVAS_BOTTOM, CANVAS_TOP,CANVAS_HATCART, CANVAS_INFO, CANVAS_MINIGAME,
					/* Last Canvas */ CANVAS_CONFIRM, CANVAS_END };
	enum UI_EVENT { EVENT_BARGUAGE, EVENT_CHANGEIMG, EVENT_ANIMATION, EVENT_FADE, EVENT_END };
	enum UI_EFFECT { EFFECT_PIPFULL, EFFECT_ARROWFULL, EFFECT_BOMBFULL, EFFECT_QUESTCLEAR, EFFECT_END };

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
	void					Call_ConfirmWindow(wstring msg, _bool bActive, CUI_Canvas* pCaller = nullptr);

public: /* OnOff Canvas Setting */
	void					Switch_FrontUI(_bool bActive); 



private:
	vector<CUI_Canvas*>		m_vecCanvas;
	vector<CUI_NodeEffect*>	m_vecEffects;
	CUI_CanvasConfirm*		m_pConfirmWindow;

public:
	virtual void Free() override;
};
END