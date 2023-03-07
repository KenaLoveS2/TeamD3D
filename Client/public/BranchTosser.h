#pragma once
#include "Monster.h"

BEGIN(Client)

class CBranchTosser : public CMonster
{
private:
	enum ANIMATION
	{
		ATTACK,
		BIND,
		DEATH,
		EXIT1,
		EXIT2,
		FLEETONEW,
		HORIZONTAL_EXIT,
		IDLE,
		L_ALERT,
		L_ALERTIDLE,
		L_DROP1,
		L_DROP2,
		L_DROP3,
		R_ALERTLOOP,
		TAKEDAMAGE,
		TAUNT1,
		TAUNT3,
		TAUNT4,
		ANIMATION_END
	};

	enum COLLIDERTYPE
	{
		COLL_WEAPON = 0,
		COLL_END
	};

	enum DROPTYPE
	{
		DROP_1 = 0,
		DROP_2,
		DROP_3,
		DROP_END
	};

private:
	CBranchTosser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBranchTosser(const CBranchTosser& rhs);
	virtual ~CBranchTosser() = default;

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
	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

	class CGameObject* m_pWeapon = nullptr;

	_int	 m_iDropType = DROP_1;
	_float m_fIdleToAttack = 0.f;

public:
	static CBranchTosser*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;
};

END