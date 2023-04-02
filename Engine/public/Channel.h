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
	HRESULT	Save_Channel(HANDLE& hFile, DWORD& dwByte);
	HRESULT	Load_Channel(HANDLE& hFile, DWORD& dwByte);
	void		Set_ChannelName(const string& strRootNodeName) {
		ZeroMemory(m_szName, MAX_PATH);
		strcpy_s(m_szName, strRootNodeName.c_str());
	}
	void								Set_BoneTranfromMatrix(_fmatrix matTransform);
	class CBone*				Get_BonePtr() { return m_pBone; }
	_matrix							Get_BoneTransformMatrix();
	_bool							Get_BoneLocked();
	const _uint&					Get_KeyFrameCount() const { return m_iNumKeyframes; }
	vector<KEYFRAME>*	Get_KeyFrames() { return &m_KeyFrames; }

public:
	HRESULT Initialize_Prototype(HANDLE hFile, class CModel* pModel);
	HRESULT Initialize(void* pArg);
	
	void Update_TransformMatrix(_float PlayTime, _bool isRootBone = false);
	void Blend_TransformMatrix(_float PlayTime, _float fBlendRadio, _bool isRootBone = false);
	void Additive_TransformMatrix(_float PlayTime, _float fAdditiveRatio, _bool isRootBone = false);

	void Additive_TransformMatrixForMonster(_float PlayTime, _float fAdditiveRatio, _bool isRootBone = false);

	/* For CAnimationState */
	void Update_TransformMatrix_ReturnMat(_float PlayTime, _smatrix& matBonesTransfomation, _bool RootBoneRotationLock = false, _bool RootBoneTranslationLock = false, CChannel* pBlendChannel = nullptr);
	void Blend_TransformMatrix_ReturnMat(_float PlayTime, _float fBlendRadio, _smatrix& matBonesTransfomation, _bool RootBoneRotationLock = false, _bool RootBoneTranslationLock = false, CChannel* pBlendChannel = nullptr);
	void Additive_TransformMatrix_ReturnMat(_float PlayTime, _float fAdditiveRatio, _smatrix& matBonesTransfomation, _bool RootBoneRotationLock = false, _bool RootBoneTranslationLock = false);

	void Reset_KeyFrameIndex();
	void Set_KeyFrameIndex(_double dPlayTime);
	const char* Get_Name() const { return m_szName; }

	HRESULT SetUp_BonePtr(CModel* pModel);
	HRESULT Synchronization_BonePtr(CModel* pModel);

private:
	class CModel*			m_pModel = nullptr;
	char							m_szName[MAX_PATH] = "";
	class CBone*			m_pBone = nullptr;
	_float4						m_vRootBonePos = { 0.f, 0.f, 0.f, 1.f };

	_uint							m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint							m_iCurrentKeyFrameIndex = 0;

public:
	static CChannel* Create(HANDLE hFile, class CModel* pModel);
	CChannel* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END