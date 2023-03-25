#pragma once
#include "Monster.h"

BEGIN(Client)

class CBossWarrior : public CMonster
{
private:
	enum ANIMATION
	{
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

protected:
	virtual HRESULT		    SetUp_State() override;
	virtual	HRESULT		    SetUp_Components() override;
	virtual	HRESULT		    SetUp_ShaderResources() override;
	virtual HRESULT		    SetUp_ShadowShaderResources() override;
	virtual HRESULT		    SetUp_State() override;

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
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

	class CGameObject* m_pHat = nullptr;

	_float4 m_fEmissiveColor = _float4(10.f, 0.f, 10.f, 0.f);
	_float	m_fHDRIntensity = 0.2f;

public:
	static CBossWarrior*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr)  override;
	virtual void						Free() override;
};

END