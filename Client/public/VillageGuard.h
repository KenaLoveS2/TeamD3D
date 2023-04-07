#pragma once
#include "Monster.h"

BEGIN(Client)

class CVillageGuard : public CMonster
{
private:
	_float3 m_vPivotTranslation = { 0.f, 0.f, 0.f };
	_float3 m_vPivotRotation = { 0.f, 0.f, 0.f };

	_float4x4 m_vPivotMatrix;
	CBone* m_pWeaponBone = nullptr;

	_float m_fIdleTimeCheck = 0.f;

	enum COPY_SOUND_KEY {
		CSK_WALK, CSK_IDLE, CSK_HURT, CSK_PAIN, CSK_DIE, CSK_ATTACK1, CSK_ATTACK2, CSK_ATTACK3, CSK_SWORD1, CSK_SWORD2, CSK_IMPACT,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

private:
	CVillageGuard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVillageGuard(const CVillageGuard& rhs);
	virtual ~CVillageGuard() = default;

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
	static CVillageGuard*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;


	virtual void					ImGui_PhysXValueProperty() override;
	
	void LookAt_Kena(_bool bIsInit, _float fTimeDelta);

	virtual void Create_CopySoundKey() override;
	void Play_WalkSound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Sword1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Sword2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_ImpactSound(_bool bIsInit, _float fTimeDelta);
	void Play_HurtSound(_bool bIsInit, _float fTimeDelta);
	void Play_PainSound(_bool bIsInit, _float fTimeDelta);

private:
	enum ANIMATION
	{
		BACKWARDSUMMERSAULT,
		BIND,
		BLOCK_ATTACK, // 막고 때리기
		BLOCK_EXIT,
		BLOCK_HIT,
		BLOCK_INTO,
		BLOCK_LOOP,
		BLOCK_STAGGER,
		BLOCKROLL_R,
		DASHATTACK, // attack 멀리있는 공격
		DEATH,
		HEAVYSTAGGER_B,
		HEAVYSTAGGER_F,
		IDLE,
		IDLEADDITIVE, // ADDITIVE
		JUMPATTACK, // attack 멀리있는 공격
		JUMPBACK,
		LOOKDOWN,
		LOOKLEFT, 
		LOOKRIGHT,
		LOOKUP,
		PARRIED,
		ROLL_F,
		ROLL_L,
		ROLL_R,
		ROLLATTACK,
		RUN,
		SCATTERBOMB,
		STAGGER_L,
		STAGGER_R,
		STRAFE_L,
		STRAFE_R,
		TAUNT1,
		TAUNT2,
		TURN_180,
		TURN_L,
		TURN_R,
		TWITCH,
		TWITCH_L,
		TWITCH_R,
		WALK,
		WALKBACKWARDS,
		WALLJUMP_ATTACK,
		WALLJUMP_INTO_R,
		WALLJUMP_INTO_L,
		WALLJUMP_ATTACK_L,
		WISPIN,
		ANIMATION_END
	};
};

END