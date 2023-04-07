#pragma once
#include "Monster.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)
class CRockGolem : public CMonster
{
private:
	enum ANIMATION
	{
		BIND, 
		CHARGEATTACK,
		CHARGESLAM, 
		DEPTH,
		EXPLODE,
		IDLE,
		IDLE_ADDITIVE,
		INTOSLEEP,
		LOOK_DOWN,	//ADDITIVE
		LOOK_LEFT,		//ADDITIVE
		LOOK_RIGHT,	//ADDITIVE
		LOOK_UP,			//ADDITIVE
		SLEEPIDLE,
		TAKEDAMAGE,
		TAUNT1,
		TAUNT2,
		TURN_180,
		TURN_90L,
		TURN_90R,
		WALK,
		WISPIN,
		WISPOUT,
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_EXPLODE,
		AT_CHARGESLAM,
		
		// AT_CHARGEATTACK,
		ATTACKTYPE_END
	};

private:
	CRockGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRockGolem(const CRockGolem& rhs);
	virtual ~CRockGolem() = default;

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
	virtual void					ImGui_PhysXValueProperty() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void					Push_EventFunctions() override;
	virtual void					Calc_RootBoneDisplacement(_fvector vDisplacement) override;

private:
	virtual  HRESULT			SetUp_State() override;
	virtual	HRESULT			SetUp_Components() override;
	virtual	HRESULT			SetUp_ShaderResources() override;
	virtual  HRESULT			SetUp_ShadowShaderResources() override;

private:
	void	Update_Collider(_float fTimeDelta) override;
	void	AdditiveAnim(_float fTimeDelta) override;

private:
	void Set_AttackType();
	void Reset_Attack();
	void Tick_Attack(_float fTimeDelta);

private:
	_float	m_fIdletoAttackTime = 0.f;

	_int m_iAttackType = AT_EXPLODE;
	
	_bool	m_bChargeAttack = false;
	_bool	m_bSlamAttack = false;
	_bool   m_bExplodeAttack = false;

	_bool	m_bHit = false;

	_int m_iSlamAttackCount = 0;

	CBone* m_pCrystalBone = nullptr;
	_float3 m_vPivotTranslation = { 0.f, 0.16f, -2.93f };

	enum COPY_SOUND_KEY {
		CSK_PAIN, CSK_DIE, CSK_WALK, CSK_IMPACT, CSK_CHARGE, CSK_TENSE, CSK_ATTACK,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

public:
	static CRockGolem*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr)  override;
	virtual void							Free() override;

	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;

	void Create_CopySoundKey();

	void Play_WalkSound(_bool bIsInit, _float fTimeDelta);
	void Play_ImpactSound(_bool bIsInit, _float fTimeDelta);
	void Play_AttackSound(_bool bIsInit, _float fTimeDelta);
	void Play_ChargeSound(_bool bIsInit, _float fTimeDelta);
	void Play_PainSound(_bool bIsInit, _float fTimeDelta);
};

END