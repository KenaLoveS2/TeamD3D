#include "stdafx.h"
#include "Timer_Manager.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

_float Engine::CTimer_Manager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.0;

	return pTimer->Get_TimeDelta();
}

const _float CTimer_Manager::Get_TimeRate(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.f);

	return pTimer->Get_TimeRate();
}

void CTimer_Manager::Set_TimeRate(const _tchar* pTimerTag, _float Time)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	assert(nullptr != pTimer && "CTimer_Manager::Set_TimeRate");
	
	pTimer->Set_TimeRate(Time);
}

void CTimer_Manager::Set_TimeSleep(const _tchar * pTimerTag, _float fTimeSleep)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	assert(nullptr != pTimer && "CTimer_Manager::Set_TimeSleep");

	pTimer->Set_TimeSleep(fTimeSleep);
}

HRESULT Engine::CTimer_Manager::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

void Engine::CTimer_Manager::Update_Timer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

CTimer* Engine::CTimer_Manager::Find_Timer(const _tchar* pTimerTag)
{
	auto		iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

void Engine::CTimer_Manager::Free(void)
{
	for (auto& Pair : m_mapTimers)
		Safe_Release(Pair.second);

	m_mapTimers.clear();
	
}