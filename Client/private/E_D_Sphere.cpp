#include "stdafx.h"
#include "..\public\E_D_Sphere.h"
#include "GameInstance.h"

CE_D_Sphere::CE_D_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_D_Sphere::CE_D_Sphere(const CE_D_Sphere & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_D_Sphere::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_D_Sphere::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; 
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);		

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	
	/* Option */
	m_eEFfectDesc.fNormalFrame = 4;
	m_eEFfectDesc.fFrame[0] = 95.f;
	m_eEFfectDesc.iPassCnt = 0;
	m_eEFfectDesc.bActive = false;
	/* Option */
	return S_OK;
}

void CE_D_Sphere::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
}

void CE_D_Sphere::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CE_D_Sphere::Render()
{	
// 	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

HRESULT CE_D_Sphere::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sphere", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), L"Com_DTexture_0", (CComponent**)&m_pDTextureCom[0], this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_D_Sphere::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
		return E_FAIL;

	// NormalTexture
	if (FAILED(m_pDTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_DTexture_0", (_uint)m_eEFfectDesc.fFrame[0])))
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pNTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)m_eEFfectDesc.fNormalFrame), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CE_D_Sphere * CE_D_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_D_Sphere * pInstance = new CE_D_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CE_D_Sphere Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_D_Sphere::Clone(void * pArg)
{
	CE_D_Sphere * pInstance = new CE_D_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_D_Sphere Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_D_Sphere::Free()
{
	__super::Free();
}

