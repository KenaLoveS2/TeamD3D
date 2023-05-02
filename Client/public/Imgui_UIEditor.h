#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"

BEGIN(Engine)
class CUI_Canvas;
class CUI_Node;
class CUI;
END

BEGIN(Client)
class CEffect_Base_S2;
class CWorldTrigger_S2;
class CImgui_UIEditor final : public CImguiObject
{
private:
	CImgui_UIEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Imgui_FreeRender();

private:
	HRESULT Ready_CloneCanvasList();
	void	EventList();

private: /* Effect */
	void	Effect_Tool();

	void	Load_List();
	void	Save_List();
	void	Create_Effect(string strEffect);

private: /* Trigger Arrangement */
	void	Trigger_Tool();

private:
	vector<CUI_Canvas*>				m_vecCanvas;
	CUI_Canvas*						m_pCanvas;
	CUI*							m_pUI;

private: /* Effect */
	CEffect_Base_S2*				m_pEffect;
	vector<CEffect_Base_S2*>		m_vecEffects;
	vector<string>					m_vecEffectTag;

private: /* Trigger */
	vector<CWorldTrigger_S2*>		m_vecTriggers;
	CWorldTrigger_S2*				m_pTrigger;


public:
	static	CImgui_UIEditor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;
};
END