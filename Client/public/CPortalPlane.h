#pragma once
#include "EnviromentObj.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CPortalPlane :  public CEnviromentObj
{
private:
	CPortalPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPortalPlane(const CPortalPlane& rhs);
	virtual ~CPortalPlane() = default;

public:
	CPortalPlane* Get_LinkedPortal();
	void		 Set_GimmickRender(_bool bRenderActive) {
		m_bRendaerPortal_Gimmick = bRenderActive;
	}
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void			Imgui_RenderProperty() override;
	virtual void			ImGui_ShaderValueProperty() override;
	virtual void			ImGui_PhysXValueProperty() override;

private:
	HRESULT			Late_init_For_GimmickLevel();
	
private:
	CModel*					m_pModelCom = nullptr;

private:
	class CKena*			m_pKena = nullptr;
	class CCamera_Player*	m_pCamera = nullptr;
	CPortalPlane*			m_pLinkedPortal = nullptr;

	_float					m_fTimeDelta = 0.f;
	_bool					m_bRendaerPortal_Gimmick = true;

	_bool					m_bTestOnce = false;

private:
	HRESULT  SetUp_Components();
	HRESULT  SetUp_DiffuseTexture();

	HRESULT  SetUp_ShaderResources();

public:
	static  CPortalPlane* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
