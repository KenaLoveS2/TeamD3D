#pragma once
#include "Monster.h"

BEGIN(Client)

class CRotEater : public CMonster
{
private:
	enum ANIMATION
	{
		APPEAR,
		ATTACK_LONGRANGE,
		AWAKE,
		AWAKEALT,
		BIND,
		DEATH,
		DEATH3,
		EAT_INTO,
		FATSWIPE,
		HEAVYSTAGGER_FRONT,
		IDLE,
		IDLE_ADDITIVE,//ADDITIVE
		JUMPATTACK, // 밑이랑 같음
		JUMPATTACK1,
		JUMPBACK, // 회피기 느낌
		LOOK_LEFT,
		LOOK_RIGHT,
		LOOK_UP,
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

private:
	CRotEater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotEater(const CRotEater& rhs);
	virtual ~CRotEater() = default;

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
	static CRotEater*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr)  override;
	virtual void							Free() override;
};

END