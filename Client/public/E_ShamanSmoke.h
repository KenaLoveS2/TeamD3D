#pragma once
#include "Effect.h"

BEGIN(Client)
class CE_ShamanSmoke final : public CEffect
{
private:
	CE_ShamanSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanSmoke(const CE_ShamanSmoke& rhs);
	virtual ~CE_ShamanSmoke() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty() override;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	static CE_ShamanSmoke*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath =nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END