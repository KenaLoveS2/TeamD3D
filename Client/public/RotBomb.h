#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CRotBomb final : public CEffect_Mesh
{
public:
	CRotBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotBomb(const CRotBomb& rhs);
	virtual ~CRotBomb() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			Imgui_RenderProperty() override;
	virtual void			ImGui_ShaderValueProperty() override;
	virtual void			ImGui_PhysXValueProperty() override;
	virtual void			ImGui_AnimationProperty() override;

public:
	void					Set_Child();

private:
	class CKena*				m_pKena = nullptr;

private:
	_bool	m_bBomb = false;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();
	virtual _int			Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;

public:
	static CRotBomb*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END