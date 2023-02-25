#pragma once
#include "Kena_Parts.h"

BEGIN(Client)

class CKena_Staff final : public CKena_Parts
{
private:
	CKena_Staff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_Staff(const CKena_Staff& rhs);
	virtual ~CKena_Staff() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void			Imgui_RenderProperty() override;
	virtual void			ImGui_AnimationProperty() override;

private:
	virtual HRESULT		SetUp_Components() override;
	virtual HRESULT		SetUp_ShaderResource() override;

public:
	static CKena_Staff*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END