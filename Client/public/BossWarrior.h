#pragma once
#include "Monster.h"
#include "Delegator.h"
#include "UI_ClientManager.h"
#include "E_P_ExplosionGravity.h"

#define WARRIR_CLOSE_ATTACK_COUNT			4
#define WARRIR_FAR_ATTACK_COUNT				2

#define COL_WEAPON_TEXT					TEXT("BossWarrior_Weapon")
#define COL_RIGHT_LEG_TEXT				TEXT("BossWarrior_RightLeg")
#define COL_GRAB_HAND_TEXT				TEXT("BossWarrior_GrabHand")

BEGIN(Client)
class CBossWarrior : public CMonster
{
private:
	enum ANIMATION
	{
		AWAKE, 
		BELL_CALL, 
		BIG_JUMP,
		BLOCK_COUNTER_ATTACK, BLOCK_EXIT, BLOCK_INTO, BLOCK_HIT_1, BLOCK_HIT_2,
		CHARGE_ATTACK, COMBO_ATTACK,
		DEATH, 
		DODGE_R, 
		ENRAGE, 
		GRAB, GRAB_ATTACK,
		IDLE_ADDITIVE, IDLE_LOOP, 
		JUMP_ATTACK, JUMP_BACK,
		LOOK_DOWN, LOOK_L, LOOK_R, LOOK_UP, 
		PARRIED, 
		RUN, 
		STAGGER, STAGGER_BACK,
		STRAFE_L, STRAFE_R, 
		SWEEP_ATTACK, 
		TAGE_DAMAGE, 
		TRIP_UPPERCUT,
		TURN_180, TURN_L, TURN_R, 
		UPPER_CUT, 
		WALK, WALK_BACKWARDS, 
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_CHARGE,
		AT_UPPER_CUT,
		AT_COMBO,
		AT_SWEEP,
		AT_GRAB,
		AT_JUMP,
		AT_TRIP_UPPERCUT,
		ATTACKTYPE_END
	};

	enum AFTERATTACKTYPE
	{
		AFTYPE_END
	};

	enum COLLIDERTYPE
	{
		COLL_HAT,
		COLL_END
	};

private:
	CBossWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossWarrior(const CBossWarrior& rhs);
	virtual ~CBossWarrior() = default;


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

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float>		m_BossWarriorDelegator;

protected:
	virtual HRESULT		    SetUp_State() override;
	virtual	HRESULT		    SetUp_Components() override;
	virtual	HRESULT		    SetUp_ShaderResources() override;
	virtual HRESULT		    SetUp_ShadowShaderResources() override;

public:
	HRESULT SetUp_Effects();
	void	Update_Trail(const char* pBoneTag);
	void	Update_ParticleType(CE_P_ExplosionGravity::TYPE eType, _float4 vCreatePos,_bool bSetDir, _fvector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

private:
	void Update_Collider(_float fTimeDelta) override;
	void AdditiveAnim(_float fTimeDelta) override;

private:
	void	BossFight_Start();
	void  BossFight_End();
	_float m_fEndTime = 0.f;

	class CCinematicCamera* m_pCineCam[2] = { nullptr, };

private:
	void Set_AttackType();
	void Reset_Attack();
	void Tick_Attack(_float fTimeDelta);

	void Set_AFType();
	void Reset_AF();

private:	/* Animation Event Func */
	void TurnOnTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOffTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOnSwipesCharged(_bool bIsInit, _float fTimeDelta);
	void TurnOnHieroglyph(_bool bIsInit, _float fTimeDelta);
	void TurnOnShockFrontExtended(_bool bIsInit, _float fTimeDelta);

	void TurnOnFireSwipe(_bool bIsInit, _float fTimeDelta);
	void TurnOnFireSwipe_End(_bool bIsInit, _float fTimeDelta);
	void TurnOnRoot(_bool bIsInit, _float fTimeDelta);
	void TurnOnPlaneRoot(_bool bIsInit, _float fTimeDelta);

	// EnrageInto
	void TurnOnEnrage_Into(_bool bIsInit, _float fTimeDelta);
	void TurnOnEnrage_Attck(_bool bIsInit, _float fTimeDelta);

	void TurnOnCamShake(_bool bIsInit, _float fTimeDelta);
	void TurnOnMotionBlur(_bool bIsInit, _float fTimeDelta);
	void TurnOffMotionBlur(_bool bIsInit, _float fTimeDelta);

private:
	_float3 m_vWeaPonPivotTrans = {-1.36f, 0.0f, -1.73f};
	_float3 m_vWeaPonPivotRot= {1.57f, 3.13f, 0.6f};
	_float4x4 m_WeaponPivotMatrix;

	_float3 m_vRightLegPivotTrans = { -0.26f, -0.02f, -0.06f};	
	_float4x4 m_RightLegPivotMatrix;

	_float3 m_vGrabHandPivotTrans = { 1.f, -0.02f, -2.02f };
	_float4x4 m_GrabHandPivotMatrix;

	class CGameObject* m_pHat = nullptr;
	map<const string, class CEffect_Base*>	m_mapEffect;

	_float4 m_fEmissiveColor = _float4(10.f, 0.f, 10.f, 0.f);
	_float	m_fHDRIntensity = 0.2f;

	CBone* m_pWeaponBone = nullptr;
	CBone* m_pRightLegBone = nullptr;
	
	CBone* m_pGrabHandBone = nullptr;
	CBone* m_pGrabJointBone = nullptr;

	ATTACKTYPE m_eAttackType = AT_CHARGE;

	_bool m_bEnRageReady = true;
	_bool m_bBellCall = false;
	_bool m_bBlock = false;
	_bool m_bBlockHit = false;
	_bool m_bKenaGrab = false;
	
	_float m_fBlockRange = 2.5f;
	_float m_fJumpBackRange = 2.5;
	_float m_fCloseAttackRange = 5.f;
	_float m_fFarAttackRange = 7.5f;

	_float m_fBlockTime = 0.f;
	_float m_fIdleTimeCheck = 0.f;
	const _float m_fIdleTime = 1.f;

	_float	m_fFogRange = 100.f;

	enum COPY_SOUND_KEY {
		CSK_ATTACK1, CSK_ATTACK2, CSK_ATTACK3, CSK_ATTACK4, CSK_ATTACK5, CSK_ATTACK6, CSK_ATTACK7, CSK_ATTACK8, CSK_ATTACK9,
		CSK_HIT1, CSK_HIT2, CSK_HIT3, CSK_HIT4, 
		CSK_HURT1, CSK_HURT2, CSK_HURT3, CSK_HURT4, CSK_HURT5, CSK_HURT6, CSK_HURT7,
		CSK_TENSE1, CSK_TENSE2, CSK_TENSE3, 
		CSK_BACK, 
		CSK_IMPACT2, CSK_IMPACT3, CSK_IMPACT4, CSK_IMPACT5,
		CSK_WALK, CSK_SWING, CSK_SLASH, CSK_BOSS_DING, CSK_BOSS_BASE, CSK_ELEMENTAL1, CSK_ELEMENTAL2, CSK_ELEMENTAL11,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

	class CBossRock_Pool* m_pBossRockPool = nullptr;

public:
	static CBossWarrior*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;

	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

	CBone* Get_GrabHandBonePtr() { return m_pGrabHandBone; }
	CBone* Get_GrabJointBonePtr() { return m_pGrabJointBone; }
	_bool Is_WarriorGrabAnimation() { return m_bKenaGrab; }

	void Attack_End();

	void Grab_Turn(_bool bIsInit, _float fTimeDelta);
	void Create_CopySoundKey();
	void Play_Attack1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack8Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack9Sound(_bool bIsInit, _float fTimeDelta);

	void Play_Hit1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hit2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hit3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hit4Sound(_bool bIsInit, _float fTimeDelta);

	void Play_Hurt1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Hurt7Sound(_bool bIsInit, _float fTimeDelta);
	
	void Play_Tense1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Tense2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Tense3Sound(_bool bIsInit, _float fTimeDelta);
	
	void Play_BackSound(_bool bIsInit, _float fTimeDelta);
	void Play_ImpactSound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Impact5Sound(_bool bIsInit, _float fTimeDelta);

	void Play_WalkSound(_bool bIsInit, _float fTimeDelta);
	void Play_SwingSound(_bool bIsInit, _float fTimeDelta);
	void Play_SlashSound(_bool bIsInit, _float fTimeDelta);

	void Play_BossDingSound(_bool bIsInit, _float fTimeDelta);
	void Play_BossBaseSound(_bool bIsInit, _float fTimeDelta);

	void Play_Elemental1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Elemental11Sound(_bool bIsInit, _float fTimeDelta);

	void Execute_UpRocksPool(_bool bIsInit, _float fTimeDelta);
};

END