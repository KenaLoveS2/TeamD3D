#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

/* Cart Model Needed */
BEGIN(Client)
class CHatCart final : public CGameObject
{
private:
	CHatCart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHatCart(const CHatCart& rhs);
	virtual ~CHatCart() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHatCart*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};
END