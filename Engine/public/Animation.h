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
	const _bool&				IsFinished() const { return m_isFinished; }
	_bool&							IsLooping() { return m_isLooping; }
	_double&						Get_AnimationDuration() { return m_Duration; }
	_double&						Get_PlayTime() { return m_PlayTime; }
	const _double				Get_PlayRate() { return m_PlayTime / m_Duration; }
	_double&						Get_AnimationTickPerSecond() { return m_TickPerSecond; }
	const char*					Get_Name() const { return m_szName; }
	_float&							Get_BlendDuration() { return m_fBlendDuration; }
	void								Set_AnimationType(ANIMTYPE eType) { m_eAnimType = eType; }
	ANIMTYPE&					Get_AnimationType() { return m_eAnimType; }
	const _uint&					Get_ChannelCount() const { return m_iNumChannels; }
	void								Set_PlayTime(_double dPlayTime);
	void								Set_Name(const char* pName) { strcpy_s(m_szName, pName); }

public:
	HRESULT						Initialize_Prototype(HANDLE hFile, class CModel* pModel);
	HRESULT						Initialize(void* pArg);
	void								ImGui_RenderEvents(_int& iSelectEvent);

	void							Update_Bones(_float fTimeDelta);
	void							Update_Bones_Blend(_float fTimeDelta, _float fBlendRatio);
	void							Update_Bones_Additive(_float fTimeDelta, _float fRatio, CAnimation* pRefAnim);

	/* For CAnimationState */
	void							Update_Bones_ReturnMat(_float fTimeDelta, _smatrix* matBonesTransformation, CAnimation* pBlendAnim = nullptr);
	void							Update_Bones_Blend_ReturnMat(_float fTimeDelta, _float fBlendRatio, _smatrix* matBonesTransformation, CAnimation* pBlendAnim = nullptr);
	void							Update_Bones_Additive_ReturnMat(_float fTimeDelta, _float fRatio, _smatrix* matBonesTransformation, CAnimation* pRefAnim);

	void							Reverse_Play(_float fTimeDelta);
	void							Reset_Animation();

private:
	class CModel*					m_pModel = nullptr;

	char								m_szName[MAX_PATH];
	_double						m_Duration = 0.0;
	_float							m_fBlendDuration = 0.2f;
	_double						m_TickPerSecond;
	ANIMTYPE					m_eAnimType = ANIMTYPE_END;

	_double						m_PlayTime = 0.0;

	_bool							m_isFinished = false;
	_bool							m_isLooping = false;

	_uint								m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

private:
	/* Map for Event Call. <PlayTime, FuncName> */
	map<_float, const string>		m_mapEvent;

private:
	void	Call_Event(_float fLastPlayTime, _float fTimeDelta);

public:
	static CAnimation* Create(HANDLE hFile, class CModel* pModel);
	CAnimation* Clone(void *pArg = nullptr);
	virtual void Free() override;
	HRESULT SetUp_ChannelsBonePtr(CModel* pModel);
	HRESULT Synchronization_ChannelsBonePtr(CModel* pModel, const string& strRootNodeName);
};

END