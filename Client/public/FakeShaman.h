#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CShader;
END

BEGIN(Client)
class CFakeShaman final : public CGameObject
{
private:
	CFakeShaman(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CFakeShaman(const CFakeShaman& rhs);
	virtual ~CFakeShaman() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

private:
	CRenderer*	m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	_uint				m_iNumMeshes = 0;

public:
	static CFakeShaman* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
