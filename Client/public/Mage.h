#pragma once
#include "Monster.h"

#define MAGE_STICK_COUNT			3

BEGIN(Client)
class CMage : public CMonster
{
private:
	enum ANIMATION
	{
		ALERT,
		BOMBREACT,
		CLOSEATTACK,
		CLOSEATTACK_R,
		COMMAND,
		DASH_B,
		DASH_F,
		DASH_L,
		DASH_R,
		DEATH,
		ENTER,
		EXIT,
		HEAL,
		IDLE,
		IDLEADDITIVE,
		LEAN_B,
		LEAN_F,
		LEAN_L,
		LEAN_R,
		LOOKDOWN,
		LOOKLEFT,
		LOOKRIGHT,
		LOOKUP,
		PARRY,
		RANGEDATTACK,
		SLEEP,
		STAGGER,
		STAGGER_B,
		STAGGER_L,
		STAGGER_R,
		SUMMON,
		TAKEDAMAGE,
		TAUNT1,
		TURN_180,
		TURN_L,
		TURN_R,
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_SUMMON,
		AT_CLOSEATTACK,
		AT_RANGEDATTACK,
		ATTACKTYPE_END
	};

	enum COLLIDERTYPE
	{
		COLL_WEAPON = 0,
		COLL_END
	};

private:
	CMage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMage(const CMage& rhs);
	virtual ~CMage() = default;

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

	void Summon();

private:
	_float	m_fIdletoAttackTime = 0.f;

	_int m_iAttackType = ATTACKTYPE_END;

	_bool m_bCloseAttack = false;
	_bool m_bRangedAttack = false;
	_bool m_bSummonAttack = false;

	_bool m_bFirstAttack = false;

	_int	m_iIdletoDash = 0;

private:
	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

	list<CGameObject*> m_SticksList;

	class CFireBullet* m_pFireBullet = nullptr;

	class CSticks01* m_pSticks[MAGE_STICK_COUNT] = { nullptr, };

public:
	static CMage*							Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;

	void Create_Sticks();
	void Erase_StickList(CSticks01* pStick);
};

END