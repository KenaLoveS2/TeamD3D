#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL  CTimer final :	public CBase
{
public:
	CTimer();
	virtual ~CTimer() = default;

public:
	_float			Get_TimeDelta() { return m_fTimeDelta; }
	const _float	Get_TimeRate() const { return m_fTimeRate; }
	void			Set_TimeRate(_float fRate) { m_fTimeRate = fRate; }
	void			Set_TimeSleep(_float fTimeSleep) { m_fTimeSleep = fTimeSleep; }

public:
	HRESULT		Ready_Timer();
	void			Update_Timer();

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_float				m_fTimeDelta = 0.0;
	_float				m_fTimeRate = 1.0;
	_float				m_fTimeSleep = 0.f;

public:
	static CTimer*	Create(void);
	virtual void Free(void);
};

END