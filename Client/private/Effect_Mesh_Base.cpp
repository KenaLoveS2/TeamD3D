#include "stdafx.h"
#include "..\public\Effect_Mesh_Base.h"
#include "GameInstance.h"

CEffect_Mesh_Base::CEffect_Mesh_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base_S2(pDevice, pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pModelCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pDiffuseTextureCom[i] = nullptr;
		m_pMaskTextureCom[i] = nullptr;
	}
}

CEffect_Mesh_Base::CEffect_Mesh_Base(const CEffect_Mesh_Base & rhs)
	: CEffect_Base_S2(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pModelCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pDiffuseTextureCom[i] = nullptr;
		m_pMaskTextureCom[i] = nullptr;
	}
}

HRESULT CEffect_Mesh_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh_Base::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pfileName = (_tchar*)pArg;
		if (FAILED(Load_Data(m_pfileName)))
		{
		}
	}
	else
	{
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetupComponents : CEffect_Particle_Base");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Mesh_Base::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CEffect_Mesh_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEffect_Mesh_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CEffect_Mesh_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_iRenderPass);

	return S_OK;
}

void CEffect_Mesh_Base::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader(" > ModelCom"))
	{
		Set_ModelCom();
	}
}

HRESULT CEffect_Mesh_Base::Save_Data()
{
	return S_OK;
}

HRESULT CEffect_Mesh_Base::Load_Data(_tchar * fileName)
{
	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh_Base::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
	//	return E_FAIL;

	for (_uint i = 0; i < 5; ++i)
	{
		if (m_pDiffuseTextureCom[i] != nullptr)
		{
			if (FAILED(m_pDiffuseTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
				return E_FAIL;
		}
		if (m_pMaskTextureCom[i] != nullptr)
		{
			if (FAILED(m_pDiffuseTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
				return E_FAIL;
		}
	}


	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL; 

	if (FAILED(m_pShaderCom->Set_RawValue("g_fHDRItensity", &m_fHDRIntensity, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Mesh_Base::Set_ModelCom()
{
	static _int iSelected;
	ImGui::RadioButton("Plane", &iSelected, 0); ImGui::SameLine();
	ImGui::RadioButton("Cube", &iSelected, 1); ImGui::SameLine();
	ImGui::RadioButton("Cone", &iSelected, 2); ImGui::SameLine();
	ImGui::RadioButton("Sphere", &iSelected, 3); ImGui::SameLine();
	ImGui::RadioButton("ShockBall", &iSelected, 4);

	if (m_pModelCom != nullptr)
	{
		Delete_Component(L"Com_Model");
	}

	/* For.Com_Model */
	switch (iSelected)
	{
	case 0:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Plane"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cube"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cone"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_shockball"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
}

CEffect_Mesh_Base * CEffect_Mesh_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Mesh_Base * pInstance = new CEffect_Mesh_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Mesh_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Mesh_Base::Clone(void * pArg)
{
	CEffect_Mesh_Base * pInstance = new CEffect_Mesh_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Mesh_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Mesh_Base::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);

	for (_uint i = 0; i < 5; ++i)
	{
		Safe_Release(m_pDiffuseTextureCom[i]);
		Safe_Release(m_pMaskTextureCom[i]);
	}
}
