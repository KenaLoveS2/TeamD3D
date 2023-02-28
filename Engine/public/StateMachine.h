#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
protected:
	typedef struct tagState
	{
		std::function<void(_float)>	State_Start = nullptr;
		std::function<void(_float)>	State_Tick = nullptr;
		std::function<void(_float)>	State_End = nullptr;
	} STATE;

	typedef struct tagChanger
	{
		wstring							wstrNextState = L"";
		std::function<_bool(void)>		Changer_Func = nullptr;
	} CHANGER;

protected:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	const wstring&			Get_LastState() { return m_wstrLastStateName; }
	const wstring&			Get_CurrentState() { return m_wstrCurrentStateName; }
	const wstring&			Get_NextState() { return m_wstrNextStateName; }

public:
	CStateMachine&			Set_Root(const wstring& wstrStateName);
	CStateMachine&			Add_State(const wstring& wstrStateName);
	CStateMachine&			Finish_Setting() { m_wstrCurrentStateName = m_wstrRootStateName; return *this; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void				Tick(_float fTimeDelta);
	virtual void				Imgui_RenderProperty() override;

public:
	template<typename T>
	CStateMachine& Init_Start(T* Obj, void (T::*memFunc)(_float))
	{
		const auto iter = find_if(m_mapState.begin(), m_mapState.end(), [&](const pair<const wstring, STATE>& Pair) {
			return m_wstrCurrentStateName == Pair.first;
		});
		assert(iter != m_mapState.end());

		iter->second.State_Start = [Obj, memFunc](_float fTimeDelta)
		{
			(Obj->*memFunc)(fTimeDelta);
		};

		return *this;
	}

	template<typename T>
	CStateMachine&	Init_Tick(T* Obj, void (T::*memFunc)(_float))
	{
		const auto iter = find_if(m_mapState.begin(), m_mapState.end(), [&](const pair<const wstring, STATE>& Pair) {
			return m_wstrCurrentStateName == Pair.first;
		});
		assert(iter != m_mapState.end());

		iter->second.State_Tick = [Obj, memFunc](_float fTimeDelta)
		{
			(Obj->*memFunc)(fTimeDelta);
		};

		return *this;
	}

	template<typename T>
	CStateMachine&	Init_End(T* Obj, void (T::*memFunc)(_float))
	{
		const auto iter = find_if(m_mapState.begin(), m_mapState.end(), [&](const pair<const wstring, STATE>& Pair) {
			return m_wstrCurrentStateName == Pair.first;
		});
		assert(iter != m_mapState.end());

		iter->second.State_End = [Obj, memFunc](_float fTimeDelta)
		{
			(Obj->*memFunc)(fTimeDelta);
		};

		return *this;
	}

	template<typename T>
	CStateMachine&	Init_Changer(const wstring& wstrNextState, T* Obj, _bool(T::*memFunc)())
	{
		const auto iter = find_if(m_mapChanger.begin(), m_mapChanger.end(), [&](const pair<const wstring, list<CHANGER>>& Pair) {
			return m_wstrCurrentStateName == Pair.first;
		});

		CHANGER		tChanger;
		//ZeroMemory(&tChanger, sizeof(CHANGER));

		tChanger.wstrNextState = wstrNextState;
		tChanger.Changer_Func = [Obj, memFunc]()
		{
			return (Obj->*memFunc)();
		};

		if (iter == m_mapChanger.end())
			m_mapChanger.emplace(m_wstrCurrentStateName, list<CHANGER>{tChanger});
		else
			iter->second.push_back(tChanger);

		return *this;
	}

private:
	map<const wstring, STATE>				m_mapState;
	wstring									m_wstrRootStateName = L"";
	wstring									m_wstrCurrentStateName = L"";
	wstring									m_wstrNextStateName = L"";
	wstring									m_wstrLastStateName = L"";
		
private:
	map<const wstring, list<CHANGER>>	m_mapChanger;

private:
	list<wstring>								m_wstrDebugList;
	_uint										m_iDebugQueSize = 10;

private:
	void StateHistoryUpdate(const wstring& wstrLastStateName);

public:
	static CStateMachine*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*					Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void								Free() override;
};

END