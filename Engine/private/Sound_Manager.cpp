#include "stdafx.h"
#include "Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{	
}

void CSound_Manager::Free()
{
	Stop_All();

	for (auto& Pair : m_Sounds) 
	{
		delete[] Pair.first;
		Safe_Release(Pair.second);
	}
	m_Sounds.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

HRESULT CSound_Manager::Reserve_Manager(const char* pFilePath, _uint iNumManualChannels)
{
	if (pFilePath == nullptr) 
		return E_FAIL;

	m_iNumManualChannels = iNumManualChannels;
	for (_uint i = 0; i < MAX_CHANNEL_COUNT; i++)
	{
		m_Channels[i].first = nullptr;
		m_Channels[i].second = NO_USE_CHNNEL;
	}
	
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSystem, MAX_CHANNEL_COUNT, FMOD_INIT_NORMAL, NULL);
	Load_SoundFile(pFilePath);

	return S_OK;
}

HRESULT CSound_Manager::Load_SoundFile(const char* pFilePath)
{	
	char szPath[MAX_PATH] = "";
	strcat_s(szPath, pFilePath);
	strcat_s(szPath, "*.*");

	_finddata_t FindData;
	intptr_t handle = _findfirst(szPath, &FindData);
	if (handle == -1) 
		return E_FAIL;

	char szFullPath[MAX_PATH] = "";
	char szCurPath[MAX_PATH] = "";	
	strcpy_s(szCurPath, pFilePath);
	
	int iResult = 0;
	while (iResult != -1) 
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, FindData.name);

		FMOD_SOUND* pSound = nullptr;
		FMOD_RESULT Ret = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		if (Ret == FMOD_OK)
		{
			int iStrLen = (int)strlen(FindData.name) + 1;
			_tchar* pSoundKey = new _tchar[iStrLen];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iStrLen);
			
			MultiByteToWideChar(CP_ACP, 0, FindData.name, iStrLen, pSoundKey, iStrLen);
			
			CSound *pSoundObj = CSound::Create(pSound, szFullPath);
			if (pSound == nullptr)
				return E_FAIL;

			m_Sounds.emplace(pSoundKey, pSoundObj);
		}

		iResult = _findnext(handle, &FindData);
	}

	FMOD_System_Update(m_pSystem);
	_findclose(handle);

	return S_OK;
}

_int CSound_Manager::Play_Sound(const _tchar *pSoundKey, _float fVolume, _bool bIsBGM, _int iManualChannelIndex)
{
	if (iManualChannelIndex >= MAX_CHANNEL_COUNT) return -1;

	SOUNDS::iterator Pair = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pSoundKey));
	if (Pair == m_Sounds.end())
		return -1;

	FMOD_BOOL bPlayFlag = FALSE;	
	_int iPlayIndex = -1;
	if (iManualChannelIndex == -1)
	{
		for (_uint i = m_iNumManualChannels; i < MAX_CHANNEL_COUNT; i++)
		{
			// if (m_Channels[i].second == Pair->second) return -1;

			if (iPlayIndex == -1 && FMOD_Channel_IsPlaying(m_Channels[i].first, &bPlayFlag))
			{
				iPlayIndex = i;
				break;
			}
		}
	}
	else
		iPlayIndex = iManualChannelIndex;

	if (iPlayIndex >= MAX_CHANNEL_COUNT || iPlayIndex < 0)
		return -1;
	
	FMOD_CHANNEL* pUseChannel;
	CSound* pUseSound = Pair->second;
		
	FMOD_System_PlaySound(m_pSystem, pUseSound->GetSoundPtr(), nullptr, FALSE, &pUseChannel);
	if(bIsBGM)
		FMOD_Channel_SetMode(pUseChannel, FMOD_LOOP_NORMAL);
	
	pUseSound->Set_Volume(pUseChannel, fVolume);
	FMOD_System_Update(m_pSystem);

	m_Channels[iPlayIndex] = { pUseChannel, pUseSound };

	return iPlayIndex;
}

_int CSound_Manager::Play_ManualSound(const _tchar* pSoundKey, _float fVolume, _bool bIsBGM, _int iManualChannelIndex)
{
	if (iManualChannelIndex >= MAX_CHANNEL_COUNT) return -1;

	SOUNDS::iterator Pair = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pSoundKey));
	if (Pair == m_Sounds.end())
		return -1;

	FMOD_BOOL bPlayFlag = FALSE;

	if (iManualChannelIndex != -1)
	{
		FMOD_Channel_IsPlaying(m_Channels[iManualChannelIndex].first, &bPlayFlag);
		if (bPlayFlag)
			return -1;
	}

	_int iPlayIndex = -1;
	if (iManualChannelIndex == -1)
	{
		for (_uint i = m_iNumManualChannels; i < MAX_CHANNEL_COUNT; i++)
		{
			// if (m_Channels[i].second == Pair->second) return -1;

			if (iPlayIndex == -1 && FMOD_Channel_IsPlaying(m_Channels[i].first, &bPlayFlag))
			{
				iPlayIndex = i;
				break;
			}
		}
	}
	else
		iPlayIndex = iManualChannelIndex;

	if (iPlayIndex >= MAX_CHANNEL_COUNT || iPlayIndex < 0)
		return -1;

	FMOD_CHANNEL* pUseChannel;
	CSound* pUseSound = Pair->second;

	FMOD_System_PlaySound(m_pSystem, pUseSound->GetSoundPtr(), nullptr, FALSE, &pUseChannel);
	if (bIsBGM)
		FMOD_Channel_SetMode(pUseChannel, FMOD_LOOP_NORMAL);

	pUseSound->Set_Volume(pUseChannel, fVolume);
	FMOD_System_Update(m_pSystem);

	m_Channels[iPlayIndex] = { pUseChannel, pUseSound };

	return iPlayIndex;
}

void CSound_Manager::Stop_Sound(_uint iManualChannelIndex)
{
	if (iManualChannelIndex >= MAX_CHANNEL_COUNT) return;

	FMOD_Channel_Stop(m_Channels[iManualChannelIndex].first);
	m_Channels[iManualChannelIndex].second = NO_USE_CHNNEL;
}

const _tchar* CSound_Manager::Get_SoundKey(CHANNEL* pChannel)
{
	NULL_CHECK_RETURN(pChannel, nullptr);
	
	if (pChannel->second == NO_USE_CHNNEL)
		return nullptr;

	CSound* pSound = pChannel->second;

	const auto iter = find_if(m_Sounds.begin(), m_Sounds.end(), [pSound](const pair<const _tchar*, CSound*>& Pair) {
			return Pair.second == pSound;
		});

	if (iter == m_Sounds.end())
		return nullptr;

	return iter->first;
}

void CSound_Manager::Set_Volume(_uint iManualChannelIndex, _float fVolume)
{
	if (iManualChannelIndex >= MAX_CHANNEL_COUNT) return;

	if (m_Channels[iManualChannelIndex].second == NO_USE_CHNNEL) 
		return;

	m_Channels[iManualChannelIndex].second->Set_Volume(m_Channels[iManualChannelIndex].first, fVolume);	
}

void CSound_Manager::Set_MasterVolume(_float fVolume)
{
	fVolume = fVolume > 1.f ? 1.f : fVolume < 0.f ? 0.f : fVolume;
	CSound::Set_MasterVolume(fVolume);

	for (_uint i = 0; i < m_iNumManualChannels; i++)
		m_Channels[i].second ? m_Channels[i].second->UpdateUseVolume(m_Channels[i].first) : 0;
}

void CSound_Manager::Set_SoundDesc(const _tchar *pSoundKey, CSound::SOUND_DESC& SoundDesc)
{
	SOUNDS::iterator Pair = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pSoundKey));
	if (Pair == m_Sounds.end())
		return;

	Pair->second->Set_SoundDesc(SoundDesc);
}

void CSound_Manager::Stop_All()
{
	for (_uint i = 0; i < MAX_CHANNEL_COUNT; ++i)
	{
		FMOD_Channel_Stop(m_Channels[i].first);
		m_Channels[i].second = NO_USE_CHNNEL;
	}	
}

void CSound_Manager::Tick(double fTimeDelta)
{
	/*
	for (auto& Pair : m_Sounds)
	{
		Pair.second->Tick(fTimeDelta, nullptr);
	}
	*/
	CHANNEL* pChannel = nullptr;
	for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
	{
		pChannel = &m_Channels[i];
		if (pChannel->second)
		{
			pChannel->second->Tick(fTimeDelta, nullptr);
			pChannel->second->UpdateUseVolume(m_Channels[i].first);
			
			FMOD_BOOL bPlayFlag = FALSE;
			FMOD_Channel_IsPlaying(m_Channels[i].first, &bPlayFlag);
			if (bPlayFlag == FALSE)
			{
				FMOD_Channel_Stop(pChannel->first);
				pChannel->second = NO_USE_CHNNEL;
			}
		}
	}
}

_bool CSound_Manager::Is_StopSound(_uint iManualChannelIndex)
{
	if (iManualChannelIndex >= MAX_CHANNEL_COUNT) return false;

	if (m_Channels[iManualChannelIndex].first == nullptr) return true;

	return (m_Channels[iManualChannelIndex].second == NO_USE_CHNNEL);
}


HRESULT CSound_Manager::Copy_Sound(_tchar* pOriginSoundKey, _tchar* pCopySoundKeyOut)
{
	SOUNDS::iterator Pair = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pOriginSoundKey));
	if (Pair == m_Sounds.end())
		return E_FAIL;

	FMOD_SOUND* pSound = nullptr;
	FMOD_RESULT Ret = FMOD_System_CreateSound(m_pSystem, Pair->second->Get_FilePath(), FMOD_DEFAULT, 0, &pSound);
	if (Ret != FMOD_OK) return E_FAIL;
	
	_tchar szTemp[MAX_PATH] = { 0, };
	swprintf_s(szTemp, TEXT("%s_%d"), pOriginSoundKey, Pair->second->Get_CopyCount());

	int iStrLen = (int)wcslen(szTemp) + 1;
	_tchar* pSoundKey = new _tchar[iStrLen];
	ZeroMemory(pSoundKey, sizeof(_tchar) * iStrLen);
	lstrcpy(pSoundKey, szTemp);

	CSound *pSoundObj = CSound::Create(pSound, Pair->second->Get_FilePath());
	if (pSound == nullptr)
		return E_FAIL;

	m_Sounds.emplace(pSoundKey, pSoundObj);
	
	lstrcpy(pCopySoundKeyOut, pSoundKey);
	Pair->second->Add_CopyCount();

	return S_OK;
}

HRESULT CSound_Manager::Copy_Sound(const _tchar* pSoundKey, SOUNDS* pSounds)
{
	const auto iter = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pSoundKey));
	if (iter == m_Sounds.end())
		return E_FAIL;

	_bool bResult = pSounds->emplace(*iter).second;
	if (bResult == false)
		return E_FAIL;

	return S_OK;
}
