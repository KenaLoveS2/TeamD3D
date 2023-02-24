#pragma once
#include "Kena_Parts.h"

BEGIN(Client)

class CKena_MainOutfit final : public CKena_Parts
{
private:
	CKena_MainOutfit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_MainOutfit(const CKena_MainOutfit& rhs);
	virtual ~CKena_MainOutfit() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void				Imgui_RenderProperty() override;

private:
	virtual HRESULT		SetUp_Components() override;
	virtual HRESULT		SetUp_ShaderResource() override;

public:
	static CKena_MainOutfit*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END