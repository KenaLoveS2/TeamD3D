#pragma once
#include "Monster.h"
#include "HunterArrow.h"

#define TEXT_COL_HUNTER_BODY		TEXT("BossHunter_Body_Collider")
#define TEXT_COL_HUNTER_WEAPON		TEXT("BossHunter_Weapon_Collider")
#define ARROW_COUNT					15
#define FLY_POS_COUNT				8
#define SINGLE_SHOT_FRIEND_COUNT				3

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

		EFFECT_BOWTRAIL1,
		EFFECT_BOWTRAIL2,

		EFFECT_MAGIC_MESH,
		
		EFFECT_AURA_TEXTURE,

		EFFECT_ROAR_TEXTURE1,
		EFFECT_ROAR_TEXTURE2,
		EFFECT_ROAR_TEXTURE3,
		EFFECT_ROAR_TEXTURE4,
		
		EFFECT_HIT_PARTICLE,

		EFFECT_END
	};

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float> m_BossHunterDelegator;

private:
	CBone* m_pWeaponTrailBone = nullptr;
	CBone* m_pBodyBone = nullptr;
	_float4x4 m_BodyPivotMatrix;
	_float3 m_vBodyPivotTrans = { 0.f, 0.f, -0.7f };
	_float3 m_vBodyPivotRot = { 1.65f, 0.f, 0.f };

	_float m_fIdleTimeCheck = 0.f;
	_float m_fStunTimeCheck = 0.f;

	ATTACKTYPE m_eAttackType = (ATTACKTYPE)0;

	const _float m_fIdleTime = 1.5f;
	const _float m_fFlyHeightY = 3.f;
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
	_bool m_bShockArrowUpY = false;
	_bool m_bShockArrowDownY = false;

	enum COPY_SOUND_KEY {
		CSK_ATTACK1, CSK_ATTACK2, CSK_ATTACK3, CSK_ATTACK4, CSK_ATTACK5, CSK_ATTACK6, CSK_ATTACK7, 
		CSK_ATTACK8, CSK_ATTACK9, CSK_ATTACK10, CSK_ATTACK11, CSK_ATTACK12, CSK_ATTACK13, 
		CSK_HURT1, CSK_HURT2, CSK_HURT3, CSK_HURT4, CSK_HURT5, CSK_HURT6, CSK_HURT7,
		CSK_HURT8, CSK_HURT9, CSK_HURT10, CSK_HURT11,

		CSK_WHOOSH1, CSK_WHOOSH2, CSK_WHOOSH3, CSK_WHOOSH4, CSK_WHOOSH5, CSK_WHOOSH6, CSK_WHOOSH7, CSK_WHOOSH8,

		CSK_ELEMENTAL1, CSK_ELEMENTAL2, CSK_ELEMENTAL3, CSK_ELEMENTAL4, CSK_ELEMENTAL5, CSK_ELEMENTAL6, CSK_ELEMENTAL7,
		CSK_ELEMENTAL8, CSK_ELEMENTAL9, CSK_ELEMENTAL10, CSK_ELEMENTAL11, CSK_ELEMENTAL12, CSK_ELEMENTAL13, CSK_ELEMENTAL14, CSK_ELEMENTAL15,

		CSK_IMPACT1, CSK_IMPACT2, CSK_IMPACT3, CSK_IMPACT4, CSK_IMPACT5,
		CSK_KNIFE1, CSK_KNIFE2, CSK_KNIFE3, CSK_ARROW_CHARGE,

		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };
	
	_float4 m_vTargetOffset = { 0.f, 0.1f, 0.f, 0.f };
	_float4 m_vSingleShotTargetPosTable[SINGLE_SHOT_FRIEND_COUNT * 2];
	

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
	virtual _float4		Get_ComputeBonePosition(const char* pBoneName) override;

protected:
	virtual HRESULT SetUp_State() override;
	virtual	HRESULT SetUp_Components() override;
	virtual	HRESULT SetUp_ShaderResources() override;
	virtual HRESULT SetUp_ShadowShaderResources() override;

private:
	void Update_Collider(_float fTimeDelta) override;
	void AdditiveAnim(_float fTimeDelta) override;

private:
	void	BossFight_Start();
	void  BossFight_End();
	_float m_fEndTime = 0.f;

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
	void Fire_Arrow(_bool bArrowIndexUpdate, _bool bTargetLook, _float4 vTargetPos);
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

	/* SY */
	void ShockEffect_On(_bool bIsInit, _float fTimeDelta);
	void ShockEffect_Off(_bool bIsInit, _float fTimeDelta);
	void DustEffect_On(_bool bIsInit, _float fTimeDelta);
	void StunEffect_On(_bool bIsInit, _float fTimeDelta);
	void StunEffect_Off(_bool bIsInit, _float fTimeDelta);
	void BowTrailEffect_On(_bool bIsInit, _float fTimeDelta);
	void BowTrailEffect_Off(_bool bIsInit, _float fTimeDelta);
	void MagicCircleEffect_On(_bool bIsInit, _float fTimeDelta);
	void AuraEffect_On(_bool bIsInit, _float fTimeDelta);
	void AuraEffect_Off(_bool bIsInit, _float fTimeDelta);
	void RoarEffect_On(_bool bIsInit, _float fTimeDelta);
	void HitEffect_On(_bool bIsInit, _float fTimeDelta);
	/* ~SY */


	void TurnOnTrail(_bool bIsInit, _float fTimeDelta);
	void TUrnOffTrail(_bool bIsInit, _float fTimeDelta);

	virtual void Create_CopySoundKey() override;

	void Play_Attack1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack8Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack9Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack10Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack11Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack12Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack13Sound(_bool bIsInit, _float fTimeDelta);

	void Play_Hurt1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt8Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt9Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt10Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt11Sound(_bool bIsInit, _float fTimeDelta);

	void Play_Whoosh1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Whoosh8Sound(_bool bIsInit, _float fTimeDelta);
	
	void Play_Elemental1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental8Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental9Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental10Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental11Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental12Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental13Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental14Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental15Sound(_bool bIsInit, _float fTimeDelta);

	void Play_Impact1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact5Sound(_bool bIsInit, _float fTimeDelta);

	void Play_Knife1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Knife2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Knife3Sound(_bool bIsInit, _float fTimeDelta);
	
	void Play_ArrowChargeSound(_bool bIsInit, _float fTimeDelta);

	void Stop_ShockArrowUpY(_bool bIsInit, _float fTimeDelta);

	/********************************************/
	/*			For. Shader & Effect			*/
	/********************************************/
private:
	HRESULT Create_Effects();
	HRESULT Create_Trail();

private:
	vector<CEffect_Base_S2*>		m_vecEffects;
	class CE_HunterTrail*           m_pHunterTrail = nullptr;

	class CCinematicCamera* m_pCineCam[2] = { nullptr, };

private: /* For. String */
	_float							m_fUVSpeeds[2];
	_float							m_fStringDissolve;
	_float							m_fStringDissolveSpeed;
	_float							m_fStringHDRIntensity;
	_float4							m_vStringDiffuseColor;
	class CRot*					m_pRot = nullptr;

	_float							m_fFogRange = 100.f;
	_float							m_fLightRange = 10.f;

private: /* For. Tool */
	void							ImGui_EffectProperty();

private:
	void							Update_Trail(const char* pBoneTag);

private:
	CEffect_Base_S2* m_pSelectedEffect;
};

END