#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)
class CBossWarrior_Hat final : public CMonsterWeapon
{
private:
	_bool m_bHeadShot = false;
	_bool m_bEnd = false;

	_float4x4 m_SocketPivotMatrix;
	_uint m_iShaderPass = 14;

	_float3 m_vColliderPivotPos = { 0.f, -0.15f, 0.f };
	_float3 m_vColliderPivotRot = {0.3f, 0.f, 0.f};
	_float4x4 m_ColliderPivotMatrix;

private:
	CBossWarrior_Hat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossWarrior_Hat(const CBossWarrior_Hat& rhs);
	virtual ~CBossWarrior_Hat() = default;

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
	static CBossWarrior_Hat*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);
};
END
