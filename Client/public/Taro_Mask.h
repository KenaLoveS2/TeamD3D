#pragma once
#include "Kena_Parts.h"

BEGIN(Client)

class CTaro_Mask final : public CKena_Parts
{
private:
	CTaro_Mask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTaro_Mask(const CTaro_Mask& rhs);
	virtual ~CTaro_Mask() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;
	virtual void			Imgui_RenderProperty() override;
	virtual void			ImGui_AnimationProperty() override;
	virtual void			ImGui_ShaderValueProperty() override;

private:
	virtual HRESULT		SetUp_Components() override;
	virtual HRESULT		SetUp_ShaderResource() override;
	virtual HRESULT		SetUp_ShadowShaderResources() override;

public:
	static CTaro_Mask*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END