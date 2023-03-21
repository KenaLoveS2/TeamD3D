#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)
class CE_FireBulletExplosion final : public CEffect
{
private:
	CE_FireBulletExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_FireBulletExplosion(const CE_FireBulletExplosion& rhs);
	virtual ~CE_FireBulletExplosion() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

public:
	static CE_FireBulletExplosion*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath =nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END