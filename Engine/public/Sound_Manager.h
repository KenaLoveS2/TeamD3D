#pragma once
#include "Sound.h"

#define MAX_CHANNEL_COUNT		32
#define NO_USE_CHNNEL			nullptr

BEGIN(Engine)

class CSound_Manager : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
	
private:	
	typedef map<const _tchar*, CSound*> SOUNDS;
	SOUNDS m_Sounds;
	
	typedef pair<FMOD_CHANNEL*, CSound*> CHANNEL;
	CHANNEL m_Channels[MAX_CHANNEL_COUNT];
	
	FMOD_SYSTEM* m_pSystem = nullptr;
	_uint m_iNumManualChannels = 0;

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	virtual void Free() override;
	
	HRESULT Load_SoundFile(const char *pFilePath);
	void Tick(double fTimeDelta);
	HRESULT Reserve_Manager(const char *pFilePath, _uint iNumManualChannels = 0);
	
	_int Play_Sound(const _tchar *pSoundKey, _float fVolume, _bool bIsBGM = false, _int iManualChannelIndex = -1);	
	void Stop_Sound(_uint iManualChannelIndex);	
	void Set_Volume(_uint iManualChannelIndex, _float fVolume);		
	void Set_MasterVolume(_float fVolume);	
	void Set_SoundDesc(const _tchar *pSoundKey, CSound::SOUND_DESC& SoundDesc);
	_bool Is_StopSound(_uint iManualChannelIndex);
	HRESULT Copy_Sound(_tchar* pOriginSoundKey, _tchar* pCopySoundKeyOut);
	void Stop_All();
};
END
