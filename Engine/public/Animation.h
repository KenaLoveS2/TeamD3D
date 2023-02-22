#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT		Save_Animation(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Load_Animation(HANDLE& hFile, DWORD& dwByte);

public:
	HRESULT Initialize_Prototype(HANDLE hFile, class CModel* pModel);
	HRESULT Initialize(void* pArg);

	void Update_Bones(_float fTimeDelta);
	void Update_Bones_Blend(_float fTimeDelta, _float fBlendRatio);
	void Update_Bones_Addtive(_float ffTimeDelta, _float fRatio);

	_bool IsFinished() { return m_isFinished; }
	_bool& IsLooping() { return m_isLooping; }
	_double&		Get_AnimationDuration() { return m_Duration; }
	_double&		Get_PlayTime() { return m_PlayTime; }
	_double&		Get_AnimationTickPerSecond() { return m_TickPerSecond; }
	const char* Get_Name() const {
		return m_szName;
	}
	void	Set_PlayTime(_double dPlayTime) { m_PlayTime = dPlayTime; }
	void	Set_Name(const char* pName) { strcpy_s(m_szName, pName); }
	void	Reset_Animation();

private:
	class CModel*					m_pModel = nullptr;

	char								m_szName[MAX_PATH];
	/* 애니메이션을 재생하는데 거리른ㄴ 전체 시간. */
	_double								m_Duration = 0.0;

	/* 초당 재생해야할 애니메이션의 속도이다. */
	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_isFinished = false;
	_bool								m_isLooping = false;

	/* 이 애니메이션을 재생하기위해 갱신해야하는 뼈들. */
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;

public:
	static CAnimation* Create(HANDLE hFile, class CModel* pModel);
	CAnimation* Clone(void *pArg = nullptr);
	virtual void Free() override;
	HRESULT SetUp_ChannelsBonePtr(CModel* pModel);
	HRESULT Synchronization_ChannelsBonePtr(CModel* pModel, const string& strRootNodeName);
	
};

END