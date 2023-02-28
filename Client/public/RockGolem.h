#pragma once
#include "Monster.h"

BEGIN(Client)

class CRockGolem : public CMonster
{
private:
	enum ANIMATION
	{
		BIND,
		CHARGEATTACK,
		CHARGESLAM,
		DEPTH,
		EXPLODE,
		IDLE,
		IDLE_ADDITIVE, INTOSLEEP
		,LOOK_DOWN,
		LOOK_LEFT,
		LOOK_RIGHT,
		LOOK_UP,
		SLEEPIDLE,
		TAKEDAMAGE,
		TAUNT1,
		TAUNT2,
		TURN_180,
		TURN_90L,
		TURN_90R,
		WALK,
		WISPIN,
		WISPOUT,
		ANIMATION_END
	};

private:
	CRockGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRockGolem(const CRockGolem& rhs);
	virtual ~CRockGolem() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void					Push_EventFunctions() override;

protected:
	virtual  HRESULT			SetUp_State() override;
	virtual	HRESULT			SetUp_Components() override;
	virtual	HRESULT			SetUp_ShaderResources() override;
	virtual  HRESULT			SetUp_ShadowShaderResources() override;

public:
	static CRockGolem*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr)  override;
	virtual void							Free() override;
};

END