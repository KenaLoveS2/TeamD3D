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

private:
	CSticks01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSticks01(const CSticks01& rhs);
	virtual ~CSticks01() = default;

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
	static CSticks01*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;
};

END