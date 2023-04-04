#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)
class CE_FireBulletCover final : public CEffect_Mesh
{
private:
	CE_FireBulletCover(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_FireBulletCover(const CE_FireBulletCover& rhs);
	virtual ~CE_FireBulletCover() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

	_float					m_fDissolveTime = 0.0f;

public:
	static CE_FireBulletCover*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END