#include "stdafx.h"
#include "..\public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
	: m_isLooping(true)
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_isLooping(true)
{
	strcpy_s(m_szName, rhs.m_szName);
	m_Duration = rhs.m_Duration;
	m_TickPerSecond = rhs.m_TickPerSecond;

	m_iNumChannels = rhs.m_iNumChannels;
	for (_uint i = 0; i < m_iNumChannels; i++)
		m_Channels.push_back(rhs.m_Channels[i]->Clone());
}

HRESULT CAnimation::Initialize_Prototype(HANDLE hFile, CModel* pModel)
{
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

void CAnimation::Update_Bones(_float fTimeDelta)
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

		m_Channels[i]->Update_TransformMatrix(m_PlayTime);
	}

	if (true == m_isFinished)
		m_isFinished = false;
}

void CAnimation::Update_Bones_Blend(_float fTimeDelta, _float fBlendRatio)
{
	if (true == m_isFinished && false == m_isLooping)
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

		m_Channels[i]->Blend_TransformMatrix(m_PlayTime, fBlendRatio);
	}

	if (true == m_isFinished)
		m_isFinished = false;
}

void CAnimation::Update_Bones_Addtive(_float fTimeDelta, _float fRatio)
{
	m_PlayTime += m_TickPerSecond * fTimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Additive_TransformMatrix(m_PlayTime, fRatio);
		m_Channels[i]->Reset_KeyFrameIndex();
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

	m_iNumChannels = m_Channels.size();

	return S_OK;
}


