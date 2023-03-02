#pragma once

/* ���� ȭ�鿡 ��������� ������ �ּҸ� ������ �մ´�. */
/* �Ҵ�� ������ ƽ�� ������ ȣ�߶���. */
/* ���������� ���ƴ� �ڿ��� �����Ѵ�. */
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	HRESULT Render();

public:
	void  Set_bOpenLevel(_bool bOpen) { m_bOpenLevel = bOpen; }
	_bool Get_bOpenLevel() { return m_bOpenLevel; }
	_uint Get_CurrentLevelIndex() { return m_iLevelIndex; }

private:
	class CLevel*			m_pCurrentLevel = nullptr;
	_uint							m_iLevelIndex = 0;
	_bool						m_bOpenLevel = false;

public:
	virtual void Free() override;
};

END