#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)
class CBossShaman_Mask final : public CMonsterWeapon
{
	enum STATE { RENDER, DISSOLVE, STATE_END, };

private:	
	STATE m_eState = RENDER;
	_float4x4 m_SocketPivotMatrix;
	
	_uint m_iNumMeshes = 0;
	_uint m_iShaderPass = 10;

	_bool m_bDissolve = false;
	_float m_fDissolveTime = 0.f;
	_float m_fDissolveRate = 0.02f;

private:
	CBossShaman_Mask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossShaman_Mask(const CBossShaman_Mask& rhs);
	virtual ~CBossShaman_Mask() = default;

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
	static CBossShaman_Mask*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void Socket_Proc(_float fTimeDelta);
	void ShamanMask_Proc(_float fTimeDelta);
	void Clear();

	void Start_Dissolve() { m_bDissolve = true; }
	_bool Is_DissolveEnd() { return m_eState == STATE_END; }
};
END
