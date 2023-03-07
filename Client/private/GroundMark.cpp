#include "stdafx.h"
#include "..\public\GroundMark.h"
#include "GameInstance.h"

CGroundMark::CGroundMark(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGroundMark::CGroundMark(const CGroundMark & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGroundMark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroundMark::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CGroundMark::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CGroundMark::Late_Tick(_float fTimeDelta)
{	
	CUtile::Execute_BillBoard(m_pTransformCom, _float3(2.f, 2.f, 1.f));

	m_pRendererCom && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CGroundMark::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	if (FAILED(SetUp_ShaderResources())) return E_FAIL;

	m_pShaderCom->Begin(12);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CGroundMark::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_GroundMark"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroundMark::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;	
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2))) return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CGroundMark * CGroundMark::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGroundMark* pInstance = new CGroundMark(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGroundMark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGroundMark::Clone(void * pArg)
{
	CGroundMark* pInstance = new CGroundMark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGroundMark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGroundMark::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
