#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CUI;
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
		AIM_, 
		UI_PRESENT_END
	};
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
	void			Set_Effect(UI_EFFECT eID, CUI_NodeEffect* pEffect)
	{
		if (eID < 0 || eID >= m_vecEffects.size() || pEffect == nullptr)
			return;
		m_vecEffects[eID] = pEffect;
	}
	CUI_NodeEffect*		Get_Effect(UI_EFFECT eID)
	{
		if (eID < 0 || eID >= m_vecEffects.size())
			return nullptr;
		return m_vecEffects[eID];
	}
private:
	vector<CUI_NodeEffect*>	m_vecEffects;

public:
	virtual void Free() override;
};
END