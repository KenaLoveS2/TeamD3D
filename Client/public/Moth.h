#pragma once
#include "Monster.h"

BEGIN(Client)

class CMoth : public CMonster
{
private:
	CBone* m_pWeaponBone = nullptr;
	_float m_fIdletoAttackTime = 0.f;

	enum COPY_SOUND_KEY {
		CSK_TENSE, CSK_FLY, CSK_ATTACK, CSK_DIE, CSK_WINGFLAP1, CSK_WINGFLAP2,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

private:
	CMoth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMoth(const CMoth& rhs);
	virtual ~CMoth() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void					Push_EventFunctions() override;

private:
	virtual  HRESULT			SetUp_State() override;
	virtual	HRESULT			SetUp_Components() override;
	virtual	HRESULT			SetUp_ShaderResources() override;
	virtual  HRESULT			SetUp_ShadowShaderResources() override;

private:
	void	Update_Collider(_float fTimeDelta) override;

public:
	static CMoth*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void					Free() override;

	void Create_CopySoundKey();
	void Play_AttackSound(_bool bIsInit, _float fTimeDelta);
	void Play_WingFlap1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_WingFlap2Sound(_bool bIsInit, _float fTimeDelta);

private:
	enum ANIMATION
	{
		BIND,
		BOMB, // 피격
		BOMB_LANDED, // 피격
		COMBATIDLE,
		DEATHSIM,
		DODGE_L,
		DODGE_R,
		DODGE_BACK,
		DODGE_DOWN,
		DODGE_UP,
		MELEEATTACK,
		MELEEATTACK_RETURN,
		MOVEDOWN,
		PARRY,
		RANGEDATTACK,
		TAKEDAMAGE,
		TAUNT1,
		TAUNT2,
		TWITCH, // ADDITIVE
		WAKEUPLOOP,
		WALK,
		WISPIN,
		WISPOUT,
		ANIMATION_END
	};

};

END