#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CE_KenaDashCone final : public CEffect_Mesh
{
private:
	CE_KenaDashCone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaDashCone(const CE_KenaDashCone& rhs);
	virtual ~CE_KenaDashCone() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _tchar* pFilePath) override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

public:
	static CE_KenaDashCone*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END