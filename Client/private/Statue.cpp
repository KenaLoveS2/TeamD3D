#include "stdafx.h"
#include "..\public\Statue.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CStatue::CStatue(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CStatue::CStatue(const CStatue & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CStatue::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatue::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);

	return S_OK;
}

HRESULT CStatue::Late_Initialize(void * pArg)
{

	_float3 vPos, vSize;
	vSize = _float3(0.25f, 1.f, 0.25f);
	vPos = _float3(0.0f, 0.f, 0.0f);

	if (m_pModelCom->Get_IStancingModel() == true)
		m_pModelCom->Create_InstModelPxBox(m_szCloneObjectTag, m_pTransformCom, COL_ENVIROMENT, vSize, vPos); //(0~1)
	else
		m_pModelCom->Create_PxBox(m_szCloneObjectTag, m_pTransformCom, COL_ENVIROMENT);

	//m_pRendererCom->Set_PhysXRender(true);

	return S_OK;
}

void CStatue::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStatue::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bRenderActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStatue::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);

			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
			}
			else
			{
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
			}
		}
	}
	else
	{
			if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RotGod_Statue_Crumbled")
			{
				for (_uint i = 0; i < iNumMeshes; ++i)
				{
					m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
					m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
					m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture");
					m_pModelCom->Render(m_pShaderCom, i, nullptr, 6);
				}
			}
			else
			{
				for (_uint i = 0; i < iNumMeshes; ++i)
				{
					m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
					m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
					m_pModelCom->Render(m_pShaderCom, i, nullptr, 4);
				}
			}
	}
	return S_OK;
}

HRESULT CStatue::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_RotGod_Statue_Crumbled")
		return S_OK;

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 0);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);
		}
	}

	return S_OK;
}

void CStatue::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();
	ImGui::Text(CUtile::WstringToString(m_EnviromentDesc.szModelTag).c_str());
	m_pModelCom->Imgui_MaterialPath();
	m_pTransformCom->Imgui_RenderProperty();
}

void CStatue::ImGui_PhysXValueProperty()
{
	
}

HRESULT CStatue::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;
}

HRESULT CStatue::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (m_pModelCom->Get_IStancingModel())
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;

		m_iShaderOption = 1;
	}
	else
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
			(CComponent**)&m_pShaderCom)))
			return E_FAIL;

		m_iShaderOption = 4;
	}

	return S_OK;
}

HRESULT CStatue::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CStatue::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CStatue * CStatue::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatue*		pInstance = new CStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStatue");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStatue::Clone(void * pArg)
{
	CStatue*		pInstance = new CStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStatue");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStatue::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
