#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)
class CE_FireBulletCloud final : public CEffect
{
private:
	CE_FireBulletCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_FireBulletCloud(const CE_FireBulletCloud& rhs);
	virtual ~CE_FireBulletCloud() = default;

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
	static CE_FireBulletCloud*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath =nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END