#include "stdafx.h"
#include "..\public\E_P_Sakura.h"
#include "GameInstance.h"


CE_P_Sakura::CE_P_Sakura(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnviromentObj(pDevice, pContext)
{
}

CE_P_Sakura::CE_P_Sakura(const CE_P_Sakura & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CE_P_Sakura::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Sakura::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	


	return S_OK;
}

HRESULT CE_P_Sakura::Late_Initialize(void* pArg)
{

	_int iInstanceNum = 600;
	_float  fMinSpeed = 0.5f;
	_float  fmaxSpeed = 2.0f;

	m_pModelCom->Set_InstanceEffect_Info(m_pTransformCom , iInstanceNum, fMinSpeed, fmaxSpeed);


	return S_OK;
}

void CE_P_Sakura::Tick(_float fTimeDelta)
{
	//if(ImGui::Button("Late_init"))
	//{
	//	Late_Initialize();
	//}
	__super::Tick(fTimeDelta);	

	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();
	m_bStart_InstMove = m_pModelCom->Culling_InstancingMeshs(100.f, WolrdMat);

	if(!m_bStart_InstMove)
		m_pModelCom->Instaincing_mesh_Effect_tick(0.f, fTimeDelta);
}

void CE_P_Sakura::Late_Tick(_float fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bRenderActive &&false == m_bStart_InstMove) 
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CE_P_Sakura::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 13), E_FAIL);
	}

	return S_OK;
}

HRESULT CE_P_Sakura::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sakura_Flower", TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CE_P_Sakura::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


CE_P_Sakura * CE_P_Sakura::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Sakura * pInstance = new CE_P_Sakura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Sakura Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Sakura::Clone(void * pArg)
{
	CE_P_Sakura * pInstance = new CE_P_Sakura(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Sakura Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Sakura::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
