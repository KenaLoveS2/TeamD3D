#include "stdafx.h"
#include "..\public\UI_NodeHUDHP.h"
#include "GameInstance.h"
#include "UI_Event_Barguage.h"

CUI_NodeHUDHP::CUI_NodeHUDHP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeHUDHP::CUI_NodeHUDHP(const CUI_NodeHUDHP & rhs)
	: CUI_Node(rhs)
{
}

void CUI_NodeHUDHP::Set_Guage(_float fGuage)
{
	m_vecEvents[EVENT_GUAGE]->Call_Event(fGuage);
}

HRESULT CUI_NodeHUDHP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDHP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(275.f, 23.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	/* Test */
	m_bActive = true;

	/* Events */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Barguage::Create(tDesc->fileName));

	return S_OK;
}

void CUI_NodeHUDHP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_NodeHUDHP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_NodeHUDHP::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_HUDHP");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_NodeHUDHP::SetUp_Components()
{
	/* Renderer */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))
		return E_FAIL;

	/* Shader */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))
		return E_FAIL;

	/* Texture */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDHPBarNoise"), TEXT("Com_DiffuseTexture"),
		(CComponent**)&m_pTextureCom[0])))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDHP::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

CUI_NodeHUDHP * CUI_NodeHUDHP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeHUDHP*	pInstance = new CUI_NodeHUDHP(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeHUDHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeHUDHP::Clone(void * pArg)
{
	CUI_NodeHUDHP*	pInstance = new CUI_NodeHUDHP(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeHUDHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeHUDHP::Free()
{
	__super::Free();
}
