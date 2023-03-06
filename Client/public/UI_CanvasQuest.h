#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CQuest;
class CUI_NodeEffect;
class CUI_CanvasQuest final : public CUI_Canvas
{
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_END };

	enum QUEST { QUEST_0, QUEST_1, QUEST_2, QUEST_END };

	enum STATE { STATE_NORMAL, STATE_OPEN, STATE_CLOSE, STATE_END };
private:
	CUI_CanvasQuest(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasQuest(const CUI_CanvasQuest& rhs);
	virtual ~CUI_CanvasQuest() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;
private:
	HRESULT					Ready_Quests();

private: /* Bind Functions */
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue);
private:
	void	Default(CUI_ClientManager::UI_PRESENT eType, _float fData);
	void	Switch(CUI_ClientManager::UI_PRESENT eType, _float bTrue);
	void	Check(CUI_ClientManager::UI_PRESENT eType, _float fData);

private:/* Test */
	CQuest*		m_Quests[QUEST_END];
	_int		m_iCurQuestIndex;

private:/* For Shader */
	_float		m_fTimeAcc;
	_bool		m_bOpen;
	_bool		m_bClose;
	_float		m_fAlpha;
	STATE		m_eState;

private:
	vector<CUI_NodeEffect*>	m_vecEffects;

public:
	static	CUI_CanvasQuest*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END