#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)
class CShieldStick_Weapon final : public CMonsterWeapon
{
private:
	_bool m_bShowDissolve = false;
	_uint m_iShaderPass = 2;

private:
	CShieldStick_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShieldStick_Weapon(const CShieldStick_Weapon& rhs);
	virtual ~CShieldStick_Weapon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;

private:
	virtual  void		 	Update_Collider(_float fTimeDelta) override;
	virtual  HRESULT  SetUp_Components() override;
	virtual  HRESULT  SetUp_ShaderResources() override;
	virtual	HRESULT	SetUp_ShadowShaderResources() override;

public:
	static CShieldStick_Weapon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void Show_Dissolve();
};
END
