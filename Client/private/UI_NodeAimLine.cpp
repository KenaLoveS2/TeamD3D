#include "stdafx.h"
#include "..\public\UI_NodeAimLine.h"
#include "GameInstance.h"

CUI_NodeAimLine::CUI_NodeAimLine(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeAimLine::CUI_NodeAimLine(const CUI_NodeAimLine & rhs)
	: CUI_Node(rhs)
{
}

HRESULT CUI_NodeAimLine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeAimLine::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(66.f, 66.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	/* Test */
	m_bActive = true;

	m_bStart = false;
	m_bSetInitialRatio = false;



	return S_OK;
}

void CUI_NodeAimLine::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_bSetInitialRatio)
	{
		if (m_pParent != nullptr)
		{
			_float4x4 matWorldParent;
			XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());
			_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);
			m_fInitialRatioX = matWorldParent._11 / m_matParentInit._11;
			m_fInitialRatioY = matWorldParent._22 / m_matParentInit._22;

			m_bSetInitialRatio = true;
		}
	}
}

void CUI_NodeAimLine::Late_Tick(_float fTimeDelta)
{
	if (m_bStart)
	{
		__super::Late_Tick(fTimeDelta);

		if (m_pParent != nullptr)
		{
			_float4x4 matWorldParent;
			XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

			_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

			float fRatioX = matWorldParent._11 / m_matParentInit._11;
			float fRatioY = matWorldParent._22 / m_matParentInit._22;
			_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

			_matrix matScaleInv = XMMatrixInverse(nullptr, matParentScale);

			_matrix matInitSclaeRatio = XMMatrixScaling(m_fInitialRatioX, m_fInitialRatioY, 1.f);

			_matrix matWorld = matInitSclaeRatio*matScaleInv*m_matLocal*matParentScale*matParentTrans;
			m_pTransformCom->Set_WorldMatrix(matWorld);
		}

		for (auto e : m_vecEvents)
			e->Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom && m_bActive)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	else
		__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeAimLine::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeAimLine::SetUp_Components()
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

HRESULT CUI_NodeAimLine::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
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

CUI_NodeAimLine * CUI_NodeAimLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeAimLine*	pInstance = new CUI_NodeAimLine(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeAimLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeAimLine::Clone(void * pArg)
{
	CUI_NodeAimLine*	pInstance = new CUI_NodeAimLine(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeAimLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeAimLine::Free()
{
	__super::Free();
}
