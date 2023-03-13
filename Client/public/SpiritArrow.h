#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSpiritArrow final : public CEffect_Mesh
{
public:
	enum ARROWSTATE { ARROW_READY, ARROW_FIRE, ARROW_HIT, ARROWSTATE_END };

private:
	CSpiritArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpiritArrow(const CSpiritArrow& rhs);
	virtual ~CSpiritArrow() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual void				Push_EventFunctions() override;

private:
	class CKena*				m_pKena = nullptr;
	class CKena_Staff*		m_pStaff = nullptr;
	class CCamera_Player*	m_pCamera = nullptr;

private:
	ARROWSTATE			eState = ARROW_READY;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();

public:
	static CSpiritArrow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END