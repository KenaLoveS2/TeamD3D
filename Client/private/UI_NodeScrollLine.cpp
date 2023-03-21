#include "stdafx.h"
#include "..\public\UI_NodeScrollLine.h"
#include "GameInstance.h"

CUI_NodeScrollLine::CUI_NodeScrollLine(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeScrollLine::CUI_NodeScrollLine(const CUI_NodeScrollLine & rhs)
	: CUI_Node(rhs)
{
}

HRESULT CUI_NodeScrollLine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeScrollLine::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(112.f, 112.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	m_bActive = true;

	return S_OK;
}

void CUI_NodeScrollLine::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_NodeScrollLine::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);

	CUI::Late_Tick(fTimeDelta);

	m_pTransformCom->Set_WorldMatrix_float4x4(m_matLocal);

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_NodeScrollLine::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeScrollLine::SetUp_Components()
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

HRESULT CUI_NodeScrollLine::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	_float4 vColor = { 0.9f, 0.9f, 0.9f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;
	_float fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
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

	return S_OK;
}

CUI_NodeScrollLine * CUI_NodeScrollLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeScrollLine*	pInstance = new CUI_NodeScrollLine(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeScrollLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeScrollLine::Clone(void * pArg)
{
	CUI_NodeScrollLine*	pInstance = new CUI_NodeScrollLine(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeScrollLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeScrollLine::Free()
{
	__super::Free();
}
