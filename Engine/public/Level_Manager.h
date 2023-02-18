#pragma once

/* 현재 화면에 보여줘야할 레벨의 주소를 가지고 잇는다. */
/* 할당된 레벨의 틱과 렌더를 호추랗낟. */
/* 이전레벨에 사용됐던 자원을 정리한다. */
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

private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iLevelIndex = 0;

public:
	virtual void Free() override;

	_uint Get_CurrentLevelIndex() { return m_iLevelIndex; }
};

END