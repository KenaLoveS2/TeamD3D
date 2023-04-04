#pragma once
#include "Monster.h"
#include "HunterArrow.h"

#define TEXT_COL_HUNTER_BODY		TEXT("BossHunter_Body_Collider")
#define TEXT_COL_HUNTER_WEAPON		TEXT("BossHunter_Weapon_Collider")
#define ARROW_COUNT					10
#define FLY_POS_COUNT				8

BEGIN(Client)
class CEffect_Base_S2;
class CBossHunter : public CMonster
{
private:
	enum ANIMATION {
		BOMB_REMOVE,
		CHARGE_ARROW_EXIT, CHARGE_ARROW_FIRST_SHOT, CHARGE_ARROW_LOOP_SHOT,
		DASHED,
		DEATH,
		DODGE_DOWN, DODGE_L, DODGE_R, DODGE_FAR_RIGHT,
		FLY_BACK_FAST, FLY_FORWRAD_FAST, FLY_LEFT_FAST, FLY_RIGHT_FAST,
		IDLE,
		KNIFE_ATTACK, KNIFE_PARRY, KNIFE_PARRY_EXIT, KNIFE_PARRY_STUN_LOOP, KNIFE_SLAM_ATTACK, KNIFE_SLAM_TELEPORT,
		MOVE_BACK, MOVE_FRONT, MOVE_LEFT, MOVE_RIGHT,
		QUICK_APPROACH,
		RAMPAGE,
		RAPID_SHOOT, RAPID_SHOOT_GROUND,
		SHOCK_ARROW_ATTACK, SHOCK_ARROW_EXIT, SHOCK_ARROW_LOOP,
		SINGLE_SHOT,
		STUN_EXIT, STUN_INTO, STUN_INTO_FALL, STUN_LOOP, STUNNED_TAKE_DAMAGE,
		SWOOP_ATTACK,
		TAKE_DAMGE_AIR,
		ANIMATION_END
	};

	enum ATTACKTYPE {
		AT_FLY,
		AT_KNIFE,
		AT_KNIFE_SLAM,
		AT_SWOOP_ATTACK,
		AT_SINGLE_SHOT,
		AT_RAPID_SHOOT,
		AT_SHOCK_ARROW,
		AT_CHARGE_ARROW,

		ATTACKTYPE_END
	};

	enum AFTERATTACKTYPE {
		AFTYPE_END
	};

	enum COLLIDERTYPE {
		COLL_END
	};

	enum EFFECT {
		EFFECT_CHARGE_TEXTURE_CIRCLE,
		EFFECT_CHARGE_PARTICLE_GATHER,
		EFFECT_CHARGE_TEXTURE_CENTER,
		EFFECT_CHARGE_TEXTURE_SHINE,
		EFFECT_CHARGE_TEXTURE_LINE1,
		EFFECT_CHARGE_TEXTURE_LINE2,

		EFFECT_DUST_PARTICLE_BIG,
		EFFECT_DUST_PARTICLE_SMALL,
		EFFECT_DUST_MESH_DUSTPLANE,

		EFFECT_STUN_MESH_GUAGE,
		EFFECT_STUN_MESH_BASE,
		EFFECT_STUN_MESH_BASE2,

		EFFECT_END
	};

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float> m_BossHunterDelegator;

private:

	CBone* m_pBodyBone = nullptr;
	_float4x4 m_BodyPivotMatrix;
	_float3 m_vBodyPivotTrans = { 0.f, 0.f, -0.7f };
	_float3 m_vBodyPivotRot = { 1.65f, 0.f, 0.f };

	_float m_fIdleTimeCheck = 0.f;
	_float m_fStunTimeCheck = 0.f;

	ATTACKTYPE m_eAttackType = (ATTACKTYPE)0;

	const _float m_fIdleTime = 1.5f;
	const _float m_fFlyHeightY = 1.5f;
	const _float m_fKenaPosOffsetY = 0.5f;
	const _float m_fStunTime = 3.f;

	class CHunterArrow* m_pArrows[ARROW_COUNT] = { nullptr, };
	_uint m_iArrowIndex = 0;

	_bool m_bReadyStrongArrow = false;

	_uint m_iDodgeAnimIndex = DODGE_DOWN;

	_float4 m_vFlyTargetPos[8];
	_uint m_vFlyTargetIndex = 0;
	_bool m_bFlyEnd = false;

	_bool m_bDodge = false;

private:
	CBossHunter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHunter(const CBossHunter& rhs);
	virtual ~CBossHunter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;
	virtual HRESULT Call_EventFunction(const string& strFuncName) override;
	virtual void Push_EventFunctions() override;

protected:
	virtual HRESULT SetUp_State() override;
	virtual	HRESULT SetUp_Components() override;
	virtual	HRESULT SetUp_ShaderResources() override;
	virtual HRESULT SetUp_ShadowShaderResources() override;

private:
	void Update_Collider(_float fTimeDelta) override;
	void AdditiveAnim(_float fTimeDelta) override;

private:
private:
	void Set_AttackType();
	void Reset_Attack();
	void Tick_Attack(_float fTimeDelta);

	void Set_AFType();
	void Reset_AF();

private:
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

public:
	static CBossHunter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr)  override;
	virtual void Free() override;

	_int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex);

	void ResetAndSet_Animation(_uint iAnimIndex);
	void Set_NextAttack();
	void Create_Arrow();
	void Ready_Arrow(CHunterArrow::FIRE_TYPE eFireType);
	void Fire_Arrow(_bool bArrowIndexUpdate);
	void Update_ArrowIndex();

public:
	// Animation CallBack Function
	void LookAt_Knife(_bool bIsInit, _float fTimeDelta);
	void LookAt_Arrow(_bool bIsInit, _float fTimeDelta);

	void ReadyArrow_Single(_bool bIsInit, _float fTimeDelta);
	void ReadyArrow_Charge(_bool bIsInit, _float fTimeDelta);
	void ReadyArrow_Rapid(_bool bIsInit, _float fTimeDelta);
	void ReadyArrow_Shock(_bool bIsInit, _float fTimeDelta);

	void FireArrow_Single(_bool bIsInit, _float fTimeDelta);
	void FireArrow_Charge(_bool bIsInit, _float fTimeDelta);
	void FireArrow_Rapid(_bool bIsInit, _float fTimeDelta);
	void FireArrow_Shock(_bool bIsInit, _float fTimeDelta);

	void Reset_HitFlag();

	void ShockEffect_On(_bool bIsInit, _float fTimeDelta);
	void ShockEffect_Off(_bool bIsInit, _float fTimeDelta);
	void DustEffect_On(_bool bIsInit, _float fTimeDelta);
	void StunEffect_On(_bool bIsInit, _float fTimeDelta);
	void StunEffect_Off(_bool bIsInit, _float fTimeDelta);



	/********************************************/
	/*			For. Shader & Effect			*/
	/********************************************/
private:
	HRESULT Create_Effects();

private:
	vector<CEffect_Base_S2*>		m_vecEffects;

private: /* For. String */
	_float							m_fUVSpeeds[2];
	_float							m_fStringDissolve;
	_float							m_fStringDissolveSpeed;
	_float							m_fStringHDRIntensity;
	_float4							m_vStringDiffuseColor;

private: /* For. Tool */
	void							ImGui_EffectProperty();
private:
	CEffect_Base_S2* m_pSelectedEffect;



};

END
