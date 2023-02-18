#include "Timer.h"

CTimer::CTimer() : m_fTimeDelta(0.0)
{
	ZeroMemory(&m_CurrentTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
}


HRESULT CTimer::Ready_Timer(void)
{
	//메인보드가 갖고 있는 고해상도 타이머의 누적값을 얻어오는 함수
	QueryPerformanceCounter(&m_CurrentTime);		// 1000
	QueryPerformanceCounter(&m_OldTime);			// 1020	

	QueryPerformanceCounter(&m_OriginTime);			// 1030	
													// 고해상도 타이머의 주파수를 얻어오는 함수, 주파수는 cpu 초당 클럭수 주기를 말함
	QueryPerformanceFrequency(&m_CpuTick);			// 1600000

	return S_OK;
}

void CTimer::Update_Timer()
{
	QueryPerformanceCounter(&m_CurrentTime);   // 2000   //   3000 // 4000 // 5000

	if (m_CurrentTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurrentTime;
	}

	m_fTimeDelta = (float(m_CurrentTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart);

	m_fTimeDelta = m_fTimeDelta * m_fTimeRate;

	m_OldTime = m_CurrentTime;
}

CTimer * CTimer::Create(void)
{
	CTimer*		pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX("Timer Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free(void)
{

}

