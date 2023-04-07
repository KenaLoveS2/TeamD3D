#include "stdafx.h"
#include "..\public\FloorTile.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CFloorTile::CFloorTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CFloorTile::CFloorTile(const CFloorTile & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CFloorTile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloorTile::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CFloorTile::Late_Initialize(void * pArg)
{
	//m_pModelCom->InstanceModelPosInit(m_pTransformCom->Get_WorldMatrix());

	if (m_pModelCom->Get_IStancingModel() == true && m_pModelCom->Get_UseTriangleMeshActor())
	{
		m_pModelCom->Create_Px_InstTriangle(m_pTransformCom);
	}
	else if (m_pModelCom->Get_IStancingModel() == false && m_pModelCom->Get_UseTriangleMeshActor())
	{
		m_pModelCom->Create_PxTriangle(Create_PxUserData(this, false, COL_ENVIROMENT));
	}
	else
		return S_OK;

	return S_OK;
}

void CFloorTile::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bOncePosUpdate == false && m_bRenderActive)
	{
		m_pModelCom->InstanceModelPosInit(m_pTransformCom->Get_WorldMatrix());
		m_bOncePosUpdate = true;
	}
}

void CFloorTile::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();

	if (m_pRendererCom && m_bRenderActive /*&& false == m_pModelCom->Culling_InstancingMeshs(100.f, WolrdMat)*/)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFloorTile::Render()
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
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CFloorTile::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 21), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CFloorTile::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

HRESULT CFloorTile::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;
	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CFloorTile::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CFloorTile::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CFloorTile * CFloorTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFloorTile*		pInstance = new CFloorTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFloorTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFloorTile::Clone(void * pArg)
{
	CFloorTile*		pInstance = new CFloorTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFloorTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFloorTile::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);

}
