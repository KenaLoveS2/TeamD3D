#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CWaterPlane : public CGameObject
{
private:
	CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterPlane(const CWaterPlane& rhs);
	virtual ~CWaterPlane() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;

private:
	CRenderer*					m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	float								m_fTimeDelta = 0.f;

private:
	HRESULT  SetUp_Components();
	HRESULT  SetUp_ShaderResources();

public:
	static  CWaterPlane*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END