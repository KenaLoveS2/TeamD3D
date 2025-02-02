#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

//#define MAX_LINE 4

BEGIN(Client)
class CQuest;
class CUI_NodeEffect;
class CUI_NodeQuest;
class CUI_CanvasQuest final : public CUI_Canvas
{
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_REWARD = 12, UI_END };

	enum QUEST { QUEST_0, QUEST_1, QUEST_2, QUEST_END };

	enum STATE { STATE_NORMAL, STATE_OPEN, STATE_CLOSE, STATE_END };
private:
	CUI_CanvasQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, _bool bValue, _float fValue, wstring wstr);

private:/* Test */
	CQuest* m_Quests[QUEST_END];
	_int		m_iCurQuestIndex;
	_int		m_iCurLineIndex;
	_int		m_iLastClearLine;
	_int		m_iNumsQuestLine[QUEST_END];

private:/* For Shader */
	_float		m_fTimeAcc;
	_bool		m_bOpen;
	_bool		m_bClose;
	_float		m_fAlpha;
	STATE		m_eState;

	_float		m_fTmpAcc;

	_float		m_fTimeOnAcc;
	_float		m_fTimeOn;
	_int		m_iLineIndexIter;
	_int		m_iClearIndexIter;

private:
	_bool		m_bFollowAlpha;

private:
	vector<CUI_NodeEffect*>	m_vecEffects;

private:
	vector<CUI_NodeQuest*>	m_vecOpenQuest;
	_int					m_iClearQuestIndex;

public:
	static	CUI_CanvasQuest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END