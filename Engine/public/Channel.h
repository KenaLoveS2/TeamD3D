#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	CChannel(const CChannel& rhs);
	virtual ~CChannel() = default;

public:
	void		Set_ChannelName(const string& strRootNodeName) {
		ZeroMemory(m_szName, MAX_PATH);
		strcpy_s(m_szName, strRootNodeName.c_str());
	}

public:
	HRESULT Initialize_Prototype(HANDLE hFile, class CModel* pModel);
	HRESULT Initialize(void* pArg);
	
	void Update_TransformMatrix(_double PlayTime);
	void Blend_TransformMatrix(_float PlayTime, _float fBlendRadio);
	void Additive_TransformMatrix(_double PlayTime, _float fAdditiveRadio);

	void Reset_KeyFrameIndex() {
		m_iCurrentKeyFrameIndex = 0;
	}
	const char* Get_Name() const {
		return m_szName;
	}

	HRESULT SetUp_BonePtr(CModel* pModel);
	HRESULT Synchronization_BonePtr(CModel* pModel);

private:
	char			m_szName[MAX_PATH] = "";
	class CBone*	m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;

public:
	static CChannel* Create(HANDLE hFile, class CModel* pModel);
	CChannel* Clone(void* pArg = nullptr);
	virtual void Free() override;


};

END