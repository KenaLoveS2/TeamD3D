#pragma once
#include "Monster.h"
#include "Delegator.h"
#include "UI_ClientManager.h"

#define MINION_COUNT				3
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

private:
	SWORD_RENDER m_eSwordRenderState = NO_RENDER;
	_float m_fSwordDissolveTime = 0.f;

	Delegator<CUI_ClientManager::UI_PRESENT, _float> m_BossWarriorDelegator;


	_float m_fIdleTimeCheck = 0.f;
	
	_bool m_bTeleportDissolve = false;
	_float m_fTeleportDissolveTime = 1.f;

	const _float m_fDissolveRate = 1.f / 60.f;
	const _float m_fBackFlipRange = 2.f;
	const _float m_fTeleportRange = 2.f;
	
	_float m_fTrapOffstY = 1.f;
	
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
	class CShamanTrapHex* m_pShamanTapHex = nullptr;

	CBone* m_pWeaponBone = nullptr;
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

protected:
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

	void Set_AFType();
	void Reset_AF();

private:
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

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
};

END