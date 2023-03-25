#include "stdafx.h"
#include "..\public\Effect_Texture_Base.h"
#include "GameInstance.h"


CEffect_Texture_Base::CEffect_Texture_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base_S2(pDevice, pContext)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
	for (_uint i = 0; i < TEXTURE_END; ++i)
		m_pTextureCom[i] = nullptr;
}

CEffect_Texture_Base::CEffect_Texture_Base(const CEffect_Texture_Base & rhs)
	: CEffect_Base_S2(rhs)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pVIBufferCom(nullptr)
{
	for (_uint i = 0; i < TEXTURE_END; ++i)
		m_pTextureCom[i] = nullptr;
}

HRESULT CEffect_Texture_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture_Base::Initialize(void * pArg)
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

	/* temp */
	m_pTransformCom->Set_Scaled(_float3(50.f, 50.f, 50.f));

	return S_OK;
}

HRESULT CEffect_Texture_Base::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CEffect_Texture_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEffect_Texture_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

}

HRESULT CEffect_Texture_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();
	return S_OK;
}

void CEffect_Texture_Base::Imgui_RenderProperty()
{
}

HRESULT CEffect_Texture_Base::Save_Data()
{
	return S_OK;
}

HRESULT CEffect_Texture_Base::Load_Data(_tchar * fileName)
{
	return S_OK;
}

HRESULT CEffect_Texture_Base::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_Effect_Texture_S2"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_DiffuseTexture"),
		(CComponent**)&m_pTextureCom[0])))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture_Base::SetUp_ShaderResources()
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

	/* Texture */
	if (FAILED(m_pTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;
	/* ~ Texture */


	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fHDRItensity", &m_fHDRIntensity, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CEffect_Texture_Base * CEffect_Texture_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Texture_Base * pInstance = new CEffect_Texture_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Texture_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Texture_Base::Clone(void * pArg)
{
	CEffect_Texture_Base * pInstance = new CEffect_Texture_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Texture_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Texture_Base::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	for (_uint i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
}
