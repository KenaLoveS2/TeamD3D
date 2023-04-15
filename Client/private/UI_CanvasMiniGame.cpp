#include "stdafx.h"
#include "UI_CanvasMiniGame.h"
#include "GameInstance.h"

CUI_CanvasMiniGame::CUI_CanvasMiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_bResultShow(false)
{
}

CUI_CanvasMiniGame::CUI_CanvasMiniGame(const CUI_CanvasMiniGame& rhs)
	:CUI_Canvas(rhs)
	,m_bResultShow(false)
{
}

HRESULT CUI_CanvasMiniGame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::Initialize(void* pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(520.f, 840.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(520.f, 840.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_CanvasMiniGame");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CUI_CanvasMiniGame");
		return E_FAIL;
	}

	/*temp*/
	m_bActive = true;

	return S_OK;
}

void CUI_CanvasMiniGame::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			//	MSG_BOX("Bind Failed");
			return;
		}
	}

	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);


}

void CUI_CanvasMiniGame::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasMiniGame::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::Bind()
{
	m_bBindFinished = true;

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::Ready_Nodes()
{
	return S_OK;
}

HRESULT CUI_CanvasMiniGame::SetUp_Components()
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

	return S_OK;
}

HRESULT CUI_CanvasMiniGame::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
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

	return S_OK;
}

void CUI_CanvasMiniGame::BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
}

CUI_CanvasMiniGame* CUI_CanvasMiniGame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CanvasMiniGame* pInstance = new CUI_CanvasMiniGame(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasMiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CanvasMiniGame::Clone(void* pArg)
{
	CUI_CanvasMiniGame* pInstance = new CUI_CanvasMiniGame(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasMiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasMiniGame::Free()
{
	__super::Free();
}
