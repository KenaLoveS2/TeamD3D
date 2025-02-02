#include "stdafx.h"
#include "..\public\Flower.h"
#include "GameInstance.h"
#include "Enviroment_Interaction.h"
#include "ControlMove.h"
#include "Interaction_Com.h"


CEnviroment_Interaction::CEnviroment_Interaction(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
}

CEnviroment_Interaction::CEnviroment_Interaction(const CEnviroment_Interaction & rhs)
	: CEnviromentObj(rhs)
	, m_pGameInstance(rhs.m_pGameInstance)
{
}

HRESULT CEnviroment_Interaction::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnviroment_Interaction::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	if (FAILED(SetUp_Components())) return E_FAIL;

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CEnviroment_Interaction::Late_Initialize(void * pArg)
{
	if (FAILED(__super::Late_Initialize(pArg))) return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

void CEnviroment_Interaction::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEnviroment_Interaction::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEnviroment_Interaction::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	return S_OK;
}

HRESULT CEnviroment_Interaction::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	return S_OK;
}


HRESULT CEnviroment_Interaction::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

HRESULT CEnviroment_Interaction::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom))) return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	//if (FAILED(__super::Add_Component(m_EnviromentDesc.iCurLevel, TEXT("Prototype_Component_Texture_Cave_Rock_MasterDiffuse"), TEXT("Com_MasterTextureD"),
	//	(CComponent**)&m_pMasterDiffuseBlendTexCom)))
	//	return E_FAIL;

	/************************** ex ***********************/
	//_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	//for (_uint i = 0; i < iNumMeshes; ++i)
	//{
	//	m_pModelCom->SetUp_Material(i, WJTextureType_COMP_MSK_CURV, ex(path));
	//	m_pModelCom->SetUp_Material(i, WJTextureType_MASK, ex(path));
	//	m_pModelCom->SetUp_Material(i, WJTextureType_COMP_H_R_AO, ex(path));
	//	m_pModelCom->SetUp_Material(i, WJTextureType_COMP_AMBIENT_OCCLUSION, ex(path));
	//}
	/******************************************************/

	return S_OK;
}

HRESULT CEnviroment_Interaction::SetUp_ShaderResources()
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

HRESULT CEnviroment_Interaction::SetUp_ShadowShaderResources()
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

CEnviroment_Interaction * CEnviroment_Interaction::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnviroment_Interaction* pInstance = new CEnviroment_Interaction(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnviroment_Interaction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnviroment_Interaction::Clone(void * pArg)
{
	CEnviroment_Interaction* pInstance = new CEnviroment_Interaction(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CForkLift");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnviroment_Interaction::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
	Safe_Release(m_pFSM);
}

void CEnviroment_Interaction::ImGui_ShaderValueProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
}