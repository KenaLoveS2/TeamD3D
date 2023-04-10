#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
public:
	enum ANIMTYPE { ANIMTYPE_COMMON, ANIMTYPE_ADDITIVE, ANIMTYPE_END };

public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT		Save_Animation(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Load_Animation(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Add_Event(_float fPlayTime, const string& strFuncName);

public:
	const _bool&			IsFinished() const { return m_isFinished; }
	_bool&					IsLooping() { return m_isLooping; }
	const _uint&			Get_AnimIndex() const { return m_iAnimationIndex; }
	const _float			Get_AnimationProgress() const { return _float(m_PlayTime / m_Duration); }
	_double&				Get_AnimationDuration() { return m_Duration; }
	_double&				Get_PlayTime() { return m_PlayTime; }
	_double&				Get_LastPlayTime() { return m_LastPlayTime; }
	const _double			Get_PlayRate() { return m_PlayTime / m_Duration; }
	_double&				Get_AnimationTickPerSecond() { return m_TickPerSecond; }
	const char*				Get_Name() const { return m_szName; }
	_float&					Get_BlendDuration() { return m_fBlendDuration; }

	const _uint&			Get_ChannelCount() const { return m_iNumChannels; }
	class CChannel*			Find_Channel(class CBone* pBone);
	void					Set_AnimIndex(_uint iAnimIndex) { m_iAnimationIndex = iAnimIndex; }
	void					Set_AnimationType(ANIMTYPE eType) { m_eAnimType = eType; }
	void					Set_PlayTime(_double dPlayTime);
	void					Set_Name(const char* pName) { strcpy_s(m_szName, pName); }
	void					Set_BlendDuration(_float fBlendDuration) { m_fBlendDuration = fBlendDuration; }
	ANIMTYPE&				Get_AnimationType() { return m_eAnimType; }

public:
	HRESULT						Initialize_Prototype(HANDLE hFile, class CModel* pModel);
	HRESULT						Initialize(void* pArg);
	void								ImGui_RenderEvents(_int& iSelectEvent, const string& strImGuiKey = "");

	void								Update_Bones(_float fTimeDelta, const string& strRootBone = "");
	void								Update_Bones_Blend(_float fTimeDelta, _float fBlendRatio, const string& strRootBone = "");
	void								Update_Bones_Additive(_float fTimeDelta, _float fRatio, const string& strRootBone = "");
	void								Update_Bones_AdditiveForMonster(_float fTimeDelta, _float fRatio, const string& strRootBone = "");

	/* For CAnimationState */
	void								Update_Bones_ReturnMat(_float fTimeDelta, _smatrix* matBonesTransformation, const string& strRootBone = "", CAnimation* pBlendAnim = nullptr, _bool RootBoneRotationLock = true, _bool RootBoneTranslationLock = true, _bool IsLerp = false);
	void								Update_Bones_Blend_ReturnMat(_float fTimeDelta, _float fBlendRatio, _smatrix* matBonesTransformation, const string& strRootBone = "", CAnimation* pBlendAnim = nullptr, _bool RootBoneRotationLock = true, _bool RootBoneTranslationLock = true);
	void								Update_Bones_Additive_ReturnMat(_float fTimeDelta, _float fRatio, _smatrix* matBonesTransformation, const string& strRootBone = "", _bool RootBoneRotationLock = true, _bool RootBoneTranslationLock = true, _bool IsLerp = false);
	void								Blend_BoneMatrices(_float fBlendRatio, _smatrix* matPreAnimBones, _smatrix* matCurAnimBones, const string& strRootBone = "");
	void								Reverse_Play(_float fTimeDelta);

	void								Fixed_Animation(_float fTime);
	void								Reset_Animation();

private:
	class CModel*				m_pModel = nullptr;
	_uint								m_iAnimationIndex = 0;

	char								m_szName[MAX_PATH];
	_double						m_Duration = 0.0;
	_float							m_fBlendDuration = 0.2f;
	_double						m_TickPerSecond;
	ANIMTYPE					m_eAnimType = ANIMTYPE_END;

	_double						m_PlayTime = 0.0;
	_double						m_LastPlayTime = 0.0;

	_bool							m_isFinished = false;
	_bool							m_isLooping = false;

	_uint								m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

private:
	/* Map for Event Call. <PlayTime, FuncName> */
	multimap<_float, const string>		m_mapEvent;

private:
	void	Call_Event(_float fTimeDelta);

public:
	static CAnimation* Create(HANDLE hFile, class CModel* pModel);
	CAnimation* Clone(void *pArg = nullptr);
	virtual void Free() override;
	HRESULT SetUp_ChannelsBonePtr(CModel* pModel);
	HRESULT Synchronization_ChannelsBonePtr(CModel* pModel, const string& strRootNodeName);

	void Set_DurationRate(_float fRate);
};

END