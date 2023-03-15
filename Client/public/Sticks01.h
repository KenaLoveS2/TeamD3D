#pragma once
#include "Monster.h"

BEGIN(Client)

class CSticks01 : public CMonster
{
private:
	enum ANIMATION
	{
		ATTACK,
		ATTACK2,
		BIND,
		CHARGE, // 달려가기
		CHARGEATTACK, // 달려가서 때리기 섞여있음
		COMBATIDLE,
		COMBOATTACK,
		DEATH,
		IDLESITTING,
		INTOCHARGE, //CHARGE 모션 들어가기 전 상태
		JUMPATTACK,
		JUMPBACK,
		JUMPLEFT,
		JUMPRIGHT,
		PARRIED,
		PATROLIDLE,
		RECEIVEBOMB,
		RESURRECT, // 땅에서 올라옴
		ROCKTHROW,
		STRAFELEFT,
		STRAFERIGHT,
		TAKEDAMAGEBIG,
		TAKEDAMAGEL,
		TAKEDAMAGER,
		TWITCH_B, //ADDITIVE
		TWITCH_F, //ADDITIVE
		TWITCH_L, //ADDITIVE
		TWITCH_R, //ADDITIVE
		WALK,
		WALKBACKWARD,
		CHEER,
		PATROLWALK,
		TAKEDAMAGEB,
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_CHARGEATTACK,
		AT_JUMPATTACK,
		AT_ATTACK1,
		AT_ATTACK2,
		AT_COMBOATTACK,
		AT_ROCKTHROW,
		ATTACKTYPE_END
	};

	enum AFTERATTACKTYPE
	{
		AF_CHEER,
		AF_STRAFELEFT,
		AF_STRAFERIGHT,
		AFTYPE_END
	};

	enum COLLIDERTYPE
	{
		COLL_WEAPON = 0,
		COLL_END
	};

private:
	CSticks01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSticks01(const CSticks01& rhs);
	virtual ~CSticks01() = default;

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
	_float	m_fIdletoAttackTime = 0.f;

	_int	m_iAttackType = ATTACKTYPE_END;
	
	_bool	m_bChargeAttack = false;
	_bool	m_bJumpAttack = false;
	_bool   m_bAttack1 = false;
	_bool	m_bAttack2 = false;
	_bool	m_bComboAttack = false;
	_bool	m_bThrowRock = false;

	_int	m_iAfterAttackType = AFTYPE_END;
	_bool	m_bCheer = false;
	_bool	m_bStrafeLeft = false;
	_bool	m_bStrafeRight = false;

	_bool m_bSpawnByMage = false;

private:
	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;

public:
	static CSticks01*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;

	void Spawn_ByMage(_float4 vPos);
};

END