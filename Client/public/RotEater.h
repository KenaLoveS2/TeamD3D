#pragma once
#include "Monster.h"

BEGIN(Client)

class CRotEater : public CMonster
{
private:
	enum ANIMATION
	{
		APPEAR, // no
		ATTACK_LONGRANGE,
		AWAKE,
		AWAKEALT,
		BIND,
		DEATH, // 뒤에서 맞고 죽을때
		DEATH3, // 앞에서 맞고 죽을때
		EAT_INTO, // no
		FATSWIPE, // no
		HEAVYSTAGGER_FRONT,
		IDLE,
		IDLE_ADDITIVE,//ADDITIVE
		JUMPATTACK, // 밑이랑 같음
		JUMPATTACK1,
		JUMPBACK, // 회피기 느낌
		LOOK_LEFT,		  //ADDITIVE
		LOOK_RIGHT,		 //ADDITIVE
		LOOK_UP,		   //ADDITIVE
		MAMA_STAGGER, // 포효
		PUKE, // 포효
		RUN,
		RUNLEFT,
		RUNRIGHT,
		STTAGER_BACK,
		STTAGER_FRONT,
		STTAGER_LEFT,
		STTAGER_RIGHT,
		STAGGERLIGHT_LEFT, //약하게 맞았을때
		STAGGERLIGHT_RIGHT,
		SWIPE,
		TAKEDAMAGE, //ADDITIVE
		TAUNT,
		TAUNTHOP,
		TURN180,
		TURN90_L, 
		TURN90_R,
		WALKBACKWARDS,
		WISPOUT,
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_SWIPE,
		AT_JUMPATTACK,
		AT_ATTACKLONGRANGE,
		ATTACKTYPE_END
	};

	enum AWAKETYPE
	{
		AWAKE_0,
		AWAKE_1,
		AWAKETYPE_END
	};

	enum COLLIDERTYPE
	{
		COLL_WEAPON = 0,
		COLL_END
	};

private:
	CRotEater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotEater(const CRotEater& rhs);
	virtual ~CRotEater() = default;

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

	_int		m_iAttackType = ATTACKTYPE_END;
	_bool	m_bRealAttack = false;
	_bool	m_bSwipeAttack = false;
	_bool	m_bJumpAttack = false;
	_bool	m_bLongRangeAttack = false;

	_int      m_iAwakeType = AWAKETYPE_END;

	_int		m_iAfterSwipeAttack = 0;

	_bool	m_bRangedType = false;

private:
	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;

public:
	static CRotEater*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr)  override;
	virtual void							Free() override;
};

END