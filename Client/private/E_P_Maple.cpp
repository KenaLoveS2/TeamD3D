#include "stdafx.h"
#include "..\public\E_P_Maple.h"
#include "GameInstance.h"

CE_P_Maple::CE_P_Maple(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnviromentObj(pDevice, pContext)
{
}

CE_P_Maple::CE_P_Maple(const CE_P_Maple & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CE_P_Maple::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Maple::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Maple::Late_Initialize(void* pArg)
{
	_int    iInstanceNum = 450;
	_float  fMinSpeed = 0.5f;
	_float  fmaxSpeed = 2.0f;

	m_pModelCom->Set_InstanceEffect_Info(m_pTransformCom , iInstanceNum, fMinSpeed, fmaxSpeed);
	return S_OK;
}

void CE_P_Maple::Tick(_float fTimeDelta)
{
#ifdef FOR_MAP_GIMMICK

#else
	if (m_bStart_InstMove == false)
	{
		Late_Initialize();
		m_bStart_InstMove = true;
	}
#endif

	__super::Tick(fTimeDelta);	

	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();
	m_bStart_InstMove = m_pModelCom->Culling_InstancingMeshs(100.f, WolrdMat);

	if(!m_bStart_InstMove)
		m_pModelCom->Instaincing_mesh_Effect_tick(0.f, fTimeDelta);
}

void CE_P_Maple::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bRenderActive &&false == m_bStart_InstMove) 
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CE_P_Maple::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
	}
	return S_OK;
}

HRESULT CE_P_Maple::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom), E_FAIL);

	/* For.Com_Model */ 	
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_MapleLeaf", TEXT("Com_Model"),
		(CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Maple::SetUp_ShaderResources()
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


CE_P_Maple * CE_P_Maple::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Maple * pInstance = new CE_P_Maple(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Maple Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Maple::Clone(void * pArg)
{
	CE_P_Maple * pInstance = new CE_P_Maple(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Maple Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Maple::Free()
{
	__super::Free();
	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
