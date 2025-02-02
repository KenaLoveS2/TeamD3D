#include "stdafx.h"
#include "..\public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "Function_Manager.h"
#include "Utile.h"

CAnimation::CAnimation()
	: m_isLooping(true)
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_isLooping(rhs.m_isLooping)
	, m_pModel(rhs.m_pModel)
	, m_Duration(rhs.m_Duration)
	, m_fBlendDuration(rhs.m_fBlendDuration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_eAnimType(rhs.m_eAnimType)
{
	strcpy_s(m_szName, rhs.m_szName);

	m_iNumChannels = rhs.m_iNumChannels;
	for (_uint i = 0; i < m_iNumChannels; i++)
		m_Channels.push_back(rhs.m_Channels[i]->Clone());
}

HRESULT CAnimation::Save_Animation(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = _uint(strlen(m_szName)) + 1;
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_szName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_fBlendDuration, sizeof(_float), &dwByte, nullptr);
	WriteFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_eAnimType, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	for (auto& pChannel : m_Channels)
	{
		if (FAILED(pChannel->Save_Channel(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Channel");
			return E_FAIL;
		}
	}

	_uint	iEventCnt = (_uint)m_mapEvent.size();
	WriteFile(hFile, &iEventCnt, sizeof(_uint), &dwByte, nullptr);

	_uint	iEventTagLen = 0;
	for (auto& Pair : m_mapEvent)
	{
		WriteFile(hFile, &Pair.first, sizeof(_float), &dwByte, nullptr);
		iEventTagLen = _uint(Pair.second.length()) + 1;
		WriteFile(hFile, &iEventTagLen, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, Pair.second.c_str(), sizeof(char) * iEventTagLen, &dwByte, nullptr);
	}

	return S_OK;
}

HRESULT CAnimation::Load_Animation(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);

	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	strcpy_s(m_szName, pName);

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_fBlendDuration, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_eAnimType, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	m_Channels.reserve(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(nullptr, m_pModel);
		NULL_CHECK_RETURN(pChannel, E_FAIL);
		FAILED_CHECK_RETURN(pChannel->Load_Channel(hFile, dwByte), E_FAIL);

		m_Channels.push_back(pChannel);
	}

	_uint	iEventCnt = 0;
	ReadFile(hFile, &iEventCnt, sizeof(_uint), &dwByte, nullptr);

	_uint	iEventTagLen = 0;
	_float	fEventTime = 0.f;
	char*	pEventTag = nullptr;
	for (_uint i = 0; i < iEventCnt; ++i)
	{
		ReadFile(hFile, &fEventTime, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &iEventTagLen, sizeof(_uint), &dwByte, nullptr);
		pEventTag = new char[iEventTagLen];
		ReadFile(hFile, pEventTag, sizeof(char) * iEventTagLen, &dwByte, nullptr);

		m_mapEvent.emplace(fEventTime, string(pEventTag));

		Safe_Delete_Array(pEventTag);
	}

	return S_OK;
}

HRESULT CAnimation::Add_Event(_float fPlayTime, const string & strFuncName)
{
	if (fPlayTime > m_Duration)
		return E_FAIL;

	m_mapEvent.emplace(fPlayTime, strFuncName);

	return S_OK;
}

CChannel * CAnimation::Find_Channel(CBone * pBone)
{
	const auto	iter = find_if(m_Channels.begin(), m_Channels.end(), [pBone](CChannel* pChannel) {
		return pChannel->Get_BonePtr() == pBone;
	});

	if (iter == m_Channels.end())
		return nullptr;

	return *iter;
}

void CAnimation::Set_PlayTime(_double dPlayTime)
{
	for (auto& pChannel : m_Channels)
		pChannel->Set_KeyFrameIndex(dPlayTime);

	m_PlayTime = dPlayTime;
	m_isFinished = false;
}

HRESULT CAnimation::Initialize_Prototype(HANDLE hFile, CModel* pModel)
{
	m_pModel = pModel;

	if (hFile == nullptr)
		return S_OK;

	_ulong dwByte = 0;
	_uint iLen = 0;
	ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, m_szName, iLen + 1, &dwByte, nullptr);
	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(hFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(void* pArg)
{
	return S_OK;
}

void CAnimation::ImGui_RenderEvents(_int & iSelectEvent, const string & strImGuiKey)
{
	_uint		iEventCnt = (_uint)m_mapEvent.size();
	char**		ppEvents = new char*[iEventCnt];

	string		strShowTag = "";
	_uint		i = 0;
	for (auto& Pair : m_mapEvent)
	{
		strShowTag = to_string(Pair.first) + " | " + Pair.second;
		ppEvents[i] = new char[(_uint)strShowTag.length() + 1];
		strcpy_s(ppEvents[i++], (_uint)strShowTag.length() + 1, strShowTag.c_str());
	}

	char	szAddKey[MAX_PATH] = "Events List";
	if (strImGuiKey != "")
		strcat_s(szAddKey, strImGuiKey.c_str());

	ImGui::ListBox(szAddKey, &iSelectEvent, ppEvents, iEventCnt);

	if (iSelectEvent != -1)
	{
		auto iter = m_mapEvent.begin();
		for (_int i = 0; i < iSelectEvent; ++i)
			++iter;

		m_PlayTime = iter->first;

		if (ImGui::Button("Erase"))
		{
			m_mapEvent.erase(iter);
			iSelectEvent = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Exit"))
		{
			iSelectEvent = -1;
		}
	}

	for (_uint j = 0; j < iEventCnt; ++j)
		Safe_Delete_Array(ppEvents[j]);
	Safe_Delete_Array(ppEvents);
}

void CAnimation::Update_Bones(_float fTimeDelta, const string & strRootBone)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
			m_isFinished = false;
		else
			return;
	}

	m_LastPlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	Call_Event(fTimeDelta);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished && true == m_isLooping)
			m_Channels[i]->Reset_KeyFrameIndex();
	
		if (m_Channels[i]->Get_BoneLocked() == true)
			continue;
	
		if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
			m_Channels[i]->Update_TransformMatrix((_float)m_PlayTime, true);
		else
			m_Channels[i]->Update_TransformMatrix((_float)m_PlayTime);
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;
}

void CAnimation::Update_Bones_Blend(_float fTimeDelta, _float fBlendRatio, const string & strRootBone)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
			m_isFinished = false;
		else
			return;
	}

	m_LastPlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	Call_Event(fTimeDelta);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished && true == m_isLooping)
			m_Channels[i]->Reset_KeyFrameIndex();

		if (m_Channels[i]->Get_BoneLocked() == true)
			continue;

		if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
			m_Channels[i]->Blend_TransformMatrix((_float)m_PlayTime, fBlendRatio, true);
		else
			m_Channels[i]->Blend_TransformMatrix((_float)m_PlayTime, fBlendRatio);
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;
}

void CAnimation::Update_Bones_Additive(_float fTimeDelta, _float fRatio, const string & strRootBone)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
			m_isFinished = false;
	}

	m_LastPlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	Call_Event(fTimeDelta);

	if (m_isLooping == false)
		CUtile::Saturate<_double>(m_PlayTime, 0.0, m_Duration);
	else
	{
		if (m_PlayTime > m_Duration)
			m_isFinished = true;
	}

	for (_uint i = 1; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		if (m_Channels[i]->Get_BoneLocked() == true)
			continue;

		if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
			m_Channels[i]->Additive_TransformMatrix((_float)m_PlayTime, fRatio, true);
		else
			m_Channels[i]->Additive_TransformMatrix((_float)m_PlayTime, fRatio);
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;
}

void CAnimation::Update_Bones_AdditiveForMonster(_float fTimeDelta, _float fRatio, const string & strRootBone)
{
	if (true == m_isFinished &&
		false == m_isLooping)
	{
		return;
	}

	m_PlayTime += m_TickPerSecond * fTimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
			continue;

		m_Channels[i]->Additive_TransformMatrixForMonster((_float)m_PlayTime, fRatio);
	}
}

void CAnimation::Update_Bones_ReturnMat(_float fTimeDelta, _smatrix * matBonesTransformation, const string & strRootBone, CAnimation * pBlendAnim, _bool RootBoneRotationLock, _bool RootBoneTranslationLock, _bool IsLerp)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
		{
			m_isFinished = false;

			if (pBlendAnim != nullptr)
				pBlendAnim->m_isFinished = false;
		}
		else
			fTimeDelta = 0.f;
	}

	m_LastPlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	if (pBlendAnim != nullptr)
	{
		pBlendAnim->m_PlayTime += m_TickPerSecond * fTimeDelta;

		if (pBlendAnim->m_PlayTime >= pBlendAnim->m_Duration)
			pBlendAnim->m_isFinished = true;
	}

	if (IsLerp == false)
		Call_Event(fTimeDelta);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (pBlendAnim == nullptr)
		{
			if (true == m_isFinished && true == m_isLooping)
				m_Channels[i]->Reset_KeyFrameIndex();

			if (m_Channels[i]->Get_BoneLocked() == true)
				continue;

 			if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
 				m_Channels[i]->Update_TransformMatrix_ReturnMat((_float)m_PlayTime, matBonesTransformation[i], RootBoneRotationLock, RootBoneTranslationLock);
 			else
				m_Channels[i]->Update_TransformMatrix_ReturnMat((_float)m_PlayTime, matBonesTransformation[i]);
		}
		else
		{
			if (true == m_isFinished && true == m_isLooping)
			{
				m_Channels[i]->Reset_KeyFrameIndex();
				pBlendAnim->m_Channels[i]->Reset_KeyFrameIndex();
			}

			if (m_Channels[i]->Get_BoneLocked() == true)
				continue;

			if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
				m_Channels[i]->Update_TransformMatrix_ReturnMat((_float)m_PlayTime, matBonesTransformation[i], RootBoneRotationLock, RootBoneTranslationLock, pBlendAnim->m_Channels[i]);
			else
				m_Channels[i]->Update_TransformMatrix_ReturnMat((_float)m_PlayTime, matBonesTransformation[i], false, false, pBlendAnim->m_Channels[i]);
		}
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;

	if (pBlendAnim != nullptr)
	{
		if (pBlendAnim->m_isFinished && pBlendAnim->m_isLooping)
			pBlendAnim->m_PlayTime = 0.0;
	}
}

void CAnimation::Update_Bones_Blend_ReturnMat(_float fTimeDelta, _float fBlendRatio, _smatrix * matBonesTransformation, const string & strRootBone, CAnimation * pBlendAnim, _bool RootBoneRotationLock, _bool RootBoneTranslationLock)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
		{
			m_isFinished = false;

			if (pBlendAnim != nullptr)
				pBlendAnim->m_isFinished = false;
		}
		else
			fTimeDelta = 0.f;
	}

	m_LastPlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	if (pBlendAnim != nullptr)
	{
		pBlendAnim->m_PlayTime += m_TickPerSecond * fTimeDelta;

		if (pBlendAnim->m_PlayTime >= pBlendAnim->m_Duration)
			pBlendAnim->m_isFinished = true;
	}

	Call_Event(fTimeDelta);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (pBlendAnim == nullptr)
		{
			if (true == m_isFinished && true == m_isLooping)
				m_Channels[i]->Reset_KeyFrameIndex();

			if (m_Channels[i]->Get_BoneLocked() == true)
			{
				//m_Channels[i]->Blend_TransformMatrix_ReturnMat((_float)m_PlayTime, fBlendRatio, matBonesTransformation[i], true);
				continue;
			}

			if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
				m_Channels[i]->Blend_TransformMatrix_ReturnMat((_float)m_PlayTime, fBlendRatio, matBonesTransformation[i], RootBoneRotationLock, RootBoneTranslationLock);
			else
				m_Channels[i]->Blend_TransformMatrix_ReturnMat((_float)m_PlayTime, fBlendRatio, matBonesTransformation[i]);
		}
		else
		{
			if (true == m_isFinished && true == m_isLooping)
			{
				m_Channels[i]->Reset_KeyFrameIndex();
				pBlendAnim->m_Channels[i]->Reset_KeyFrameIndex();
			}

			if (m_Channels[i]->Get_BoneLocked() == true)
			{
				//m_Channels[i]->Blend_TransformMatrix_ReturnMat((_float)m_PlayTime, fBlendRatio, matBonesTransformation[i], true, pBlendAnim->m_Channels[i]);
				continue;
			}

			if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
				m_Channels[i]->Blend_TransformMatrix_ReturnMat((_float)m_PlayTime, fBlendRatio, matBonesTransformation[i], RootBoneRotationLock, RootBoneTranslationLock, pBlendAnim->m_Channels[i]);
			else
				m_Channels[i]->Blend_TransformMatrix_ReturnMat((_float)m_PlayTime, fBlendRatio, matBonesTransformation[i], false, false, pBlendAnim->m_Channels[i]);
		}
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;

	if (pBlendAnim != nullptr)
	{
		if (pBlendAnim->m_isFinished && pBlendAnim->m_isLooping)
			pBlendAnim->m_PlayTime = 0.0;
	}
}

void CAnimation::Update_Bones_Additive_ReturnMat(_float fTimeDelta, _float fRatio, _smatrix * matBonesTransformation, const string & strRootBone, _bool RootBoneRotationLock, _bool RootBoneTranslationLock, _bool IsLerp)
{
	if (true == m_isFinished)
	{
		if (m_isLooping)
			m_isFinished = false;
	}

	m_LastPlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	if (IsLerp == false)
		Call_Event(fTimeDelta);

	CUtile::Saturate<_double>(m_PlayTime, 0.0, m_Duration);

	if (m_PlayTime >= m_Duration)
		m_isFinished = true;

	for (_uint i = 1; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished && true == m_isLooping)
			m_Channels[i]->Reset_KeyFrameIndex();

		//if (m_Duration > 1.f && m_Channels[i]->Get_KeyFrameCount() == 2)
		//	continue;

		if (m_Channels[i]->Get_BoneLocked() == true)
		{
			//m_Channels[i]->Additive_TransformMatrix_ReturnMat((_float)m_PlayTime, fRatio, matBonesTransformation[i], true);
			continue;
		}
		
		if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
			m_Channels[i]->Additive_TransformMatrix_ReturnMat((_float)m_PlayTime, fRatio, matBonesTransformation[i], RootBoneRotationLock, RootBoneTranslationLock);
		else
			m_Channels[i]->Additive_TransformMatrix_ReturnMat((_float)m_PlayTime, fRatio, matBonesTransformation[i]);
	}

	if (m_isFinished && m_isLooping)
		m_PlayTime = 0.0;
}

void CAnimation::Blend_BoneMatrices(_float fBlendRatio, _smatrix * matPreAnimBones, _smatrix * matCurAnimBones, const string & strRootBone)
{
	NULL_CHECK_RETURN(matPreAnimBones, );
	NULL_CHECK_RETURN(matCurAnimBones, );

	_vector	vScale, vRotate, vPosition;
	_vector	vPreScale, vPreRotate, vPrePosition;
	_vector	vCurScale, vCurRotate, vCurPosition;

	for (_uint i = 1; i < m_iNumChannels; ++i)
	{
		if (!strcmp(m_Channels[i]->Get_Name(), strRootBone.c_str()))
			continue;

		XMMatrixDecompose(&vPreScale, &vPreRotate, &vPrePosition, matPreAnimBones[i]);
		XMMatrixDecompose(&vCurScale, &vCurRotate, &vCurPosition, matCurAnimBones[i]);

		vScale = XMVectorLerp(vPreScale, vCurScale, fBlendRatio);
		vRotate = XMQuaternionSlerp(vPreRotate, vCurRotate, fBlendRatio);
		vPosition = XMVectorLerp(vPrePosition, vCurPosition, fBlendRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		m_Channels[i]->Set_BoneTranfromMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotate, vPosition));

		matPreAnimBones[i] = matCurAnimBones[i];
	}
}

void CAnimation::Reverse_Play(_float fTimeDelta)
{
	m_PlayTime -= (_double)fTimeDelta * m_TickPerSecond;

	if (m_PlayTime < 0.0)
		m_PlayTime = m_Duration + m_PlayTime;

	for (auto& pChannel : m_Channels)
		pChannel->Set_KeyFrameIndex(m_PlayTime);
}

void CAnimation::Fixed_Animation(_float fTime)
{
	m_PlayTime = fTime;

	for (auto& pChannel : m_Channels)
		pChannel->Set_KeyFrameIndex(m_PlayTime);

	m_isFinished = true;
}

void CAnimation::Reset_Animation()
{
	for (auto& pChannel : m_Channels)
		pChannel->Reset_KeyFrameIndex();

	m_PlayTime = 0.0;
	m_isFinished = false;
}

void CAnimation::Call_Event(_float fTimeDelta)
{
	if (m_mapEvent.empty() == true || fTimeDelta == 0.f)
		return;

	NULL_CHECK_RETURN(m_pModel, );

	for (auto& Pair : m_mapEvent)
	{
		_tchar		wszFunctionTag[128] = L"";

		if ((_float)m_LastPlayTime <= Pair.first && Pair.first <= (_float)m_PlayTime)
		{
			wsprintf(wszFunctionTag, L"");
			CUtile::CharToWideChar(Pair.second.c_str(), wszFunctionTag);
			CFunction_Manager::GetInstance()->Call_Function((CBase*)m_pModel->Get_Owner(), wszFunctionTag, fTimeDelta);
		}
	}
}

CAnimation * CAnimation::Create(HANDLE hFile, CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone(void *pArg)
{
	CAnimation* pInstance = new CAnimation(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

HRESULT CAnimation::SetUp_ChannelsBonePtr(CModel* pModel)
{
	m_pModel = pModel;

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		if (FAILED(m_Channels[i]->SetUp_BonePtr(pModel)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation::Synchronization_ChannelsBonePtr(CModel * pModel, const string & strRootNodeName)
{
	for (auto iter = m_Channels.begin(); iter != m_Channels.end();)
	{
		if (iter == m_Channels.begin())
			(*iter)->Set_ChannelName(strRootNodeName);

		if ((*iter)->Synchronization_BonePtr(pModel) == S_FALSE)
		{
			Safe_Release(*iter);
			iter = m_Channels.erase(iter);
		}
		else
			++iter;
	}

	m_iNumChannels = (_uint)m_Channels.size();

	return S_OK;
}

void CAnimation::Set_DurationRate(_float fRate)
{
	m_Duration *= fRate;
}