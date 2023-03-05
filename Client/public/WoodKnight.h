#pragma once
#include "Monster.h"

BEGIN(Client)

class CWoodKnight : public CMonster
{
private:
	enum ANIMATION
	{
		ATTACK90_L,
		ATTACK90_R,
		ALERT,
		ATTACK180,
		BIND,
		BLOCK_COUNTERATTACK,
		BLOCK_EXIT,
		BLOCK_INTO,
		BLOCK_LOOP,
		BLOCKATTACK_180,
		CHARGEATTACK,
		COMBOATTACK_LUNGE,
		COMBOATTACK_OVERHEAD,
		DEATH,
		DOUBLEATTACK,
		HITDEFLECT,
		IDLE,
		IDLE_ADDITIVE, // ADDITIVE
		INTOCHARGE,
		INTOCHARGE_BACKUP,
		JUMPBACK,
		LOOK_DOWN,
		LOOK_LEFT,
		LOOK_RIGHT,
		LOOK_UP,
		OVERHEADSLAMATTACK,
		PUSHBACK,
		RANGEDATTACK,
		RUN,
		RUN_ROOTMOTION,
		SLEEP,
		STAGGER_ALT,
		STAGGER_B,
		STAGGER_L,
		STAGGER_R,
		STRAFE_L,
		STRAFE_R,
		TAKEDAMAGE, //ADDITIVE
		TAUNT,
		TURN_180,
		TURNL_90,
		TURNR_90,
		UPPERCUTATTACK,
		WALK,
		WALKBACKWARDS,
		ANIMATION_END
	};

	enum COLLIDERTYPE
	{
		COLL_WEAPON = 0,
		COLL_END
	};

private:
	CWoodKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWoodKnight(const CWoodKnight& rhs);
	virtual ~CWoodKnight() = default;

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

private:
	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;

public:
	static CWoodKnight*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;
};

END