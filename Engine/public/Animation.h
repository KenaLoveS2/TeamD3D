#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void Update_Bones(_float fTimeDelta);

private:
	char								m_szName[MAX_PATH];
	/* 애니메이션을 재생하는데 거리른ㄴ 전체 시간. */
	_double								m_Duration = 0.f;

	/* 초당 재생해야할 애니메이션의 속도이다. */
	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_isFinished = false;
	_bool								m_isLooping = false;

	/* 이 애니메이션을 재생하기위해 갱신해야하는 뼈들. */
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;
};

END