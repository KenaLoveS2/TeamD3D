#pragma once
#include "Monster.h"
#include "Delegator.h"
#include "UI_ClientManager.h"

#define MINION_COUNT				3
#define ICEDAGGER_COUNT				5
#define TEXT_COL_SHAMAN_BODY		TEXT("BossShaman_Body_Collider")
#define TEXT_COL_SHAMAN_WEAPON		TEXT("BossShaman_Weapon_Collider")

BEGIN(Client)
class CBossShaman : public CMonster
{
private:
	enum ANIMATION
	{
		IDLE_LOOP,
		AWAKE, AWAKE_LOOP,
		BACK_FLIP,
		DASH_ATTACK, 
		DEATH,
		FOG_SNAP, FREEZE_BLAST,
		FRONT_FLIP, 
		ICE_DAGGER_EXIT, ICE_DAGGER_INTO,
		MELEE_ATTACK, 
		SLEEP, 
		STAGGER, STAGGER_B, 
		STRAFE_L, STRAFE_R,
		STUN_TAKE_DAMAGE, 
		SUMMON_EXIT,
		TAKE_DAMAGE, 
		TELEPORT_EXIT, TELEPORT_HINT, TELEPORT_HINT_END, TELEPORT_INTO, TELEPORT_LOOP,
		TRAP, TRAP_ATTACK, TRAP_ESCAPE, TRAP_FAIL, TRAP_LOOP, TRAP_SPIN, 
		TRIPLE_ATTACK, 
		TURN_L, TURN_R, 
		WALK, WALK_BACKWARDS, 
		SUMMON_INTO, SUMMON_LOOP, 
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_MELEE_ATTACK,
		AT_TRIPLE_ATTACK,		
		AT_TRAP,
		AT_DASH_ATTACK,
		AT_FREEZE_BLAST,
		AT_ICE_DAGGER,
		ATTACKTYPE_END
	};

	enum AFTERATTACKTYPE
	{
		AFTYPE_END
	};

	enum COLLIDERTYPE
	{
		COLL_END
	};

	enum SWORD_RENDER
	{
		NO_RENDER,
		CREATE,
		RENDER,
		DISSOLVE,
		SWORD_RENDER_END,
	};

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float> m_BossShamanDelegator;

private:
	SWORD_RENDER m_eSwordRenderState = NO_RENDER;
	_float m_fSwordDissolveTime = 0.f;

	_float m_fIdleTimeCheck = 0.f;
	
	_bool m_bTeleportDissolve = false;
	_float m_fTeleportDissolveTime = 1.f;

	const _float m_fDissolveRate = 1.f / 60.f;
	const _float m_fBackFlipRange = 2.f;
	const _float m_fTeleportRange = 2.f;
	
	_float m_fTrapOffstY = 1.f;
	_int   m_iIceDaggerIdx = 0;

	_float m_fTeleportRangeTable[ATTACKTYPE_END] = { 1.5f, 3.f, 4.f, 5.f, 3.f, };
		
	_bool m_bSummonEnd = false;
	_bool m_bFogSnapEnd = false;
	_bool m_bNoDamage = false;
	
	_bool m_bTrapOffset = false;
	_bool m_bTraptLoop = false;
	_bool m_bTraptBreak = false;

	_float4 m_vTrapLookPos;

	_float m_fStunTime = 0.f;
	ATTACKTYPE m_eAttackType = AT_MELEE_ATTACK;

	class CSticks01* m_pMinions[MINION_COUNT] = { nullptr };
	class CE_ShamanThunderCylinder* m_pShamanThunder[MINION_COUNT] = { nullptr }; // For.CSticks01 Summons Effect
	class CShamanTrapHex* m_pShamanTapHex = nullptr;

	/* For.DashAttackTrail */
	CBone* m_pLeftHandBone = nullptr;
	CBone* m_pHeadBone = nullptr;

	CBone* m_pWeaponBone = nullptr;
	CBone* m_pWeaponTrailBone = nullptr;
	_float4x4 m_WeaponPivotMatrix;
	_float3 m_vWeaPonPivotRot = { 0.f, 0.f, 0.f };
	_float3 m_vWeaPonPivotTrans = { -1.2f, -0.59f, -1.3f };

private:
	CBossShaman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossShaman(const CBossShaman& rhs);
	virtual ~CBossShaman() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void			Imgui_RenderProperty() override;
	virtual void			ImGui_AnimationProperty() override;
	virtual void			ImGui_ShaderValueProperty() override;
	virtual void			ImGui_PhysXValueProperty() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void			Push_EventFunctions() override;

protected:
	virtual  HRESULT		SetUp_State() override;
	virtual  HRESULT		SetUp_Components() override;
	virtual  HRESULT		SetUp_ShaderResources() override;
	virtual  HRESULT		SetUp_ShadowShaderResources() override;

private:
	void	Update_Collider(_float fTimeDelta) override;
	void	AdditiveAnim(_float fTimeDelta) override;

public:
	HRESULT Ready_Effects();
	void	Tick_Effects(_float fTimeDelta);
	void	Update_Trail(const char* pBoneTag);
	void	Update_DashAttackTrail();
	void	Update_IceDagger();
	void	Update_LazerPos();
	void	Update_IceDaggerIdx() {
		m_iIceDaggerIdx++;
		m_iIceDaggerIdx %= ICEDAGGER_COUNT;
	}

 private:
	void Set_AttackType();
	void Reset_Attack();
	void Tick_Attack(_float fTimeDelta);

	void Set_AFType();
	void Reset_AF();

private:
		void	BossFight_Start();
		void  BossFight_End();
		_float m_fEndTime = 0.f;

private:
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

	enum COPY_SOUND_KEY {
		CSK_ATTACK1, CSK_ATTACK2, CSK_ATTACK3, CSK_ATTACK4, CSK_ATTACK5, CSK_ATTACK6, CSK_ATTACK7, CSK_ATTACK8,
		CSK_HIT1, CSK_HIT2, CSK_HIT3, CSK_HIT4,
		CSK_HURT1, CSK_HURT2, CSK_HURT3, CSK_HURT4, CSK_HURT5, CSK_HURT6, CSK_HURT7,
		CSK_MOVE1, CSK_MOVE2, CSK_MOVE3, CSK_MOVE4, CSK_MOVE5, CSK_MOVE6,
		CSK_TENSE1, CSK_TENSE2, CSK_TENSE3, CSK_TENSE4,		
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

private:	/* Animation Event Func */
	void TurnOnTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOffTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOffMoveMentTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOnWeaponCloudTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOffWeaponCloudTrail(_bool bIsInit, _float fTimeDelta);

	/* 소환 이펙트 => 손에 나오는 이펙트 */
	void TurnOnHandSummons(_bool bIsInit, _float fTimeDelta);
	void TurnOffHandSummons(_bool bIsInit, _float fTimeDelta);

	/* 바닥 */
	void TurnOnSummons(_bool bIsInit, _float fTimeDelta);
	void TurnOffSummons(_bool bIsInit, _float fTimeDelta);

	/* For.DashAttack */
	void TurnOnDashAttackTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOffDashAttackTrail(_bool bIsInit, _float fTimeDelta);
	void TurnOnSwipeChareged(_bool bIsInit, _float fTimeDelta);

	/* For.IceDagger */
	void TurnOnIceDagger(_bool bIsInit, _float fTimeDelta);
	void TurnOffIceDagger(_bool bIsInit, _float fTimeDelta);

	/* For.Lazer */
	void TurnOnLazer(_bool bIsInit, _float fTimeDelta);

	/* 텔레포트 이펙트 */
	void TurnOnTeleport(_bool bIsInit, _float fTimeDelta);

	/* 카메라 쉐이크 */
	void TurnOnCameraShake(_bool bIsInit, _float fTimeDelta);

private:
	map<const string, class CEffect_Base*>	m_mapEffect;
	_bool	m_bTrap = false;
	_bool	m_bTrail = false;
	_bool	m_bIceDagger = false;
	_bool	m_bDashAttackTrail = false;

public:
	static CBossShaman* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr)  override;
	virtual void Free() override;

	void SwordRenderProc(_float fTimeDelta);
	void Attack_Start(_bool bSwordRender, _uint iAnimIndex);
	void Attack_End(_bool bSwordRender, _uint iAnimIndex);
	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);

	void Create_Minions();
	void Summon();

public:	
	void LookAt_Kena(_bool bIsInit, _float fTimeDelta);

	void Create_CopySoundKey();
	
	void Play_Attack1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack6Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack7Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Attack8Sound(_bool bIsInit, _float fTimeDelta);

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

	void Play_Move1Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Move2Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Move3Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Move4Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Move5Sound(_bool bIsInit, _float fTimeDelta);
	void Play_Move6Sound(_bool bIsInit, _float fTimeDelta);
};

END