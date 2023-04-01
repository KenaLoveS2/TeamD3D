#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "PhysX_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)
class CRotHat final : public CGameObject
{
private:	
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;	
	class CRot* m_pOwnerRot = nullptr;
	
	_uint m_iNumMeshes = 0;
	_float4x4 m_SocketMatrix = g_IdentityFloat4x4;

private:
	CRotHat(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRotHat(const CRotHat& rhs);
	virtual ~CRotHat() = default;

	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
		
public:
	static CRotHat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	
	virtual void Imgui_RenderProperty() override;	
	virtual void ImGui_ShaderValueProperty() override;
	
};

END