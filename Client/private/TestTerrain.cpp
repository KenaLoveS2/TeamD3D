#include "stdafx.h"
#include "..\public\TestTerrain.h"
#include "GameInstance.h"

CTestTerrain::CTestTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CTestTerrain::CTestTerrain(const CTestTerrain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTestTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestTerrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

void CTestTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTestTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG
		//m_pRendererCom->Add_DebugRenderGroup(m_pNavigationCom);
#endif
	}
}

HRESULT CTestTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//#ifdef _DEBUG
	//	m_pNavigationCom->Render();
	//#endif

	return S_OK;
}

HRESULT CTestTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/*Tess*/
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain_Tess"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxTestTess"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/*~Tess*/

	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
	//	(CComponent**)&m_pVIBufferCom)))
	//	return E_FAIL;
	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
	//	(CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;


	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
	//	(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
	//	return E_FAIL;

	///* For.Com_Brush*/
	//if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"),
	//	(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
	//	return E_FAIL;

	///* For.Com_Filter */
	//if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
	//	(CComponent**)&m_pTextureCom[TYPE_FILTER])))
	//	return E_FAIL;

	///* For.Com_Navigation */
	//if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
	//	(CComponent**)&m_pNavigationCom)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CTestTerrain::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	//if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
	//	return E_FAIL;

	//if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterTexture", 0)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &_float4(15.f, 0.f, 15.f, 1.f), sizeof(_float4))))
	//	return E_FAIL;






	return S_OK;
}

CTestTerrain * CTestTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestTerrain*		pInstance = new CTestTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestTerrain::Clone(void * pArg)
{
	CTestTerrain*		pInstance = new CTestTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestTerrain::Free()
{
	__super::Free();

	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	//Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
