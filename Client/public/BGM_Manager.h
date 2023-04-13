#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Sound_Manager.h"

BEGIN(Client)

class CBGM_Manager final : public CBase
{
	DECLARE_SINGLETON(CBGM_Manager);

public:
	enum FIELD_STATE {
		FIELD_IDLE, FIELD_VILLAGE, FIELD_SHOP, FIELD_PUZZLE,
		FIELD_INTO_BATTLE, FIELD_FROM_BATTLE, FIELD_BATTLE,
		FIELD_BOSS_BATTLE_WARRIOR, FIELD_BOSS_BATTLE_HUNTER, FIELD_BOSS_BATTLE_SHAMAN,
		FIELD_STATE_END };

private:
	CBGM_Manager();
	virtual ~CBGM_Manager() = default;

public:
	_float&			Get_BGMVolume() { return m_fBGMVolume; }

public:
	HRESULT			Initialize(const string& strFilePath = "");
	void			Tick(_float fTimeDelta);
	void			Change_FieldState(FIELD_STATE eState);
	void			Change_BGM(FIELD_STATE eState);

private:
	CSound_Manager* m_pSoundManager = nullptr;

private:
	Client::SOUND	BGMChannel = SOUND_BGM;

	FIELD_STATE		m_eCurState = FIELD_STATE_END;
	FIELD_STATE		m_ePreState = FIELD_STATE_END;
	LEVEL			m_eCurLevel = LEVEL_END;

	CSound_Manager::CHANNEL*	m_Channel = nullptr;
	CSound_Manager::SOUNDS		m_BGMs[FIELD_STATE_END];

	_float			m_fBGMVolume = 1.f;

private:
	HRESULT			Load_FromFile(const string& strFilePath);

public:
	virtual void	Free() override;
};

END