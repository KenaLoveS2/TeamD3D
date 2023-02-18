#pragma once

#include "Timer.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		Set_TimeRate(const _tchar* pTimerTag, _float Time);
	
public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);
	void		Update_Timer(const _tchar* pTimerTag);

private:
	CTimer*		Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, CTimer*>		m_mapTimers;

public:
	virtual void Free(void);
};

END