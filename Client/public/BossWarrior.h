#pragma once
#include "Monster.h"
#include "Delegator.h"
#include "UI_ClientManager.h"
#include "E_P_ExplosionGravity.h"

#define WARRIR_CLOSE_ATTACK_COUNT			4
#define WARRIR_FAR_ATTACK_COUNT				2

#define COL_WEAPON_TEXT					TEXT("BossWarrior_Weapon")
#define COL_RIGHT_LEG_TEXT				TEXT("BossWarrior_RightLeg")

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

private:
	_float3 m_vWeaPonPivotTrans = {-1.76f, 0.03f, -2.2f};
	_float3 m_vWeaPonPivotRot= {1.57f, 3.13f, -0.31f};
	_float4x4 m_WeaponPivotMatrix;

	_float3 m_vRightLegPivotTrans = { -0.26f, -0.02f, -0.06f};	
	_float4x4 m_RightLegPivotMatrix;

	class CGameObject* m_pHat = nullptr;
	map<const string, class CEffect_Base*>	m_mapEffect;

	_float4 m_fEmissiveColor = _float4(10.f, 0.f, 10.f, 0.f);
	_float	m_fHDRIntensity = 0.2f;

	CBone* m_pWeaponBone = nullptr;
	CBone* m_pRightLegBone = nullptr;

	_uint m_iCloseAttackIndex = 0;
	_uint m_iFarAttackIndex = 0;

	_bool m_bEnRageReady = true;
	_bool m_bBellCall = false;
	_bool m_bBlock = false;
	_bool m_bBlockHit = false;


	_float m_fBlockRange = 2.5f;
	_float m_fJumpBackRange = 2.5;
	_float m_fCloseAttackRange = 5.f;
	_float m_fFarAttackRange = 7.5f;

	_float m_fBlockTime = 0.f;
	_float m_fIdleTimeCheck = 0.f;
	const _float m_fIdleTime = 1.f;

public:
	static CBossWarrior*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;

	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;

	void Attack_End(_uint* pAttackIndex, _uint iMaxAttackIndex, _uint iAnimIndex);
};

END