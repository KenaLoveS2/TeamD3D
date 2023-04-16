#include "stdafx.h"
#include "UI_MousePointer.h"
#include "GameInstance.h"

CUI_MousePointer::CUI_MousePointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
	, m_bActive(false)
	, m_iRenderPass(0)
{
}

CUI_MousePointer::CUI_MousePointer(const CUI_MousePointer& rhs)
	:CGameObject(rhs)
	, m_bActive(false)
	, m_iRenderPass(0)
{
}

HRESULT CUI_MousePointer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MousePointer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed To Initialize Clone : MousePointer");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Faioled To Setup Comp : MousePointer");
		return E_FAIL;
	}

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);
	_uint			iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pTransformCom->Set_Scaled({ 40.f, 40.f, 1.f });

	m_iRenderPass = 1;

	return S_OK;

}

void CUI_MousePointer::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	POINT pt = CUtile::GetClientCursorPos(g_hWnd);
	m_pTransformCom->Set_Position({ (_float)pt.x - 0.5f * g_iWinSizeX, 
				0.5f * g_iWinSizeY  -(_float)pt.y, 0.f, 1.f });
}

void CUI_MousePointer::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIMOUSE, this);
}

HRESULT CUI_MousePointer::Render()
{
	__super::Render();

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_MousePointer");
		return E_FAIL;
	}


	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MousePointer::SetUp_Components()
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
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_MousePointer"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MousePointer::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_matView)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_matProj)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CUI_MousePointer* CUI_MousePointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MousePointer* pInstance = new CUI_MousePointer(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_MousePointer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MousePointer::Clone(void* pArg)
{
	CUI_MousePointer* pInstance = new CUI_MousePointer(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_MousePointer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MousePointer::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
