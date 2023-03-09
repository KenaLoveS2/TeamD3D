#pragma once
#include "Monster.h"

BEGIN(Client)

class CSapling : public CMonster
{
private:
	enum ANIMATION
	{
		BIND,
		ALERTED,
		BOMBCHARGEUP, // bag on
		CHARGE, // bag on
		CHARGEATTACK, // bag on
		COMBATIDLE,
		TAKEDAMAGE,
		WALK,
		WISPOUT,
		ANIMATION_END
	};

private:
	CSapling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSapling(const CSapling& rhs);
	virtual ~CSapling() = default;

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
	_bool m_bSpawn = false;
	_bool m_bBombUp = false;

	_float m_fIdletoAttack = 0.f;

public:
	static CSapling*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;
};

END
