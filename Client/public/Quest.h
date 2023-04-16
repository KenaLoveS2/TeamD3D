#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "delegator.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CQuest final : public CGameObject
{
public:
	typedef struct tagQuestDesc
	{
		_tchar*		filePath;
	}QUESTDESC;
private:
	CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

public:
	_int		Get_CurSubIndex() { return m_iCurSubIndex; }
	void		Complete_CurSubQuest()
	{
		if (m_iCurSubIndex >= (_int)m_vecSub.size())
			return;
		m_vecSub[m_iCurSubIndex].second = true;
	}
	_int		Get_NumSubs() { return (_int)m_vecSub.size(); }
	wstring		Get_SubQuest(_int iIndex)
	{
		return m_vecSub[iIndex].first;
	}
	wstring		Get_MainQuest() {
		return m_Main.first;   
	}
	_int		Get_QuestStringLength(_int iIndex) 
	{ 
		if (iIndex == 0)
			return (_int)m_Main.first.length();
		else
			return (_int)m_vecSub[iIndex-1].first.length();
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Load_File(const _tchar* filePath);
	HRESULT Save_File();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private: /* For. Data */
	string							m_FilePath;
	pair<wstring, _bool>			m_Main;
	vector<pair<wstring, _bool>>	m_vecSub;
private: /* Check Progress */
	_int							m_iCurSubIndex;
//public:
//	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, _float>		m_QuestDelegator;

public:
	static CQuest*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END