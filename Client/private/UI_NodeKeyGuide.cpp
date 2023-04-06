#include "stdafx.h"
#include "UI_NodeKeyGuide.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"
#include "UI_NodeMood.h"


CUI_NodeKeyGuide::CUI_NodeKeyGuide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeKeyGuide::CUI_NodeKeyGuide(const CUI_NodeKeyGuide& rhs)
	: CUI_Node(rhs)
{
}

HRESULT CUI_NodeKeyGuide::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeKeyGuide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ _float(g_iWinSizeX), _float(g_iWinSizeY), 1.f });
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLvUp");
		return E_FAIL;
	}

	m_bActive = true;

	return S_OK;
}

HRESULT CUI_NodeKeyGuide::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeKeyGuide::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_NodeKeyGuide::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeKeyGuide::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeKeyGuide::SetUp_Components()
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

HRESULT CUI_NodeKeyGuide::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CUI_NodeKeyGuide* CUI_NodeKeyGuide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeKeyGuide* pInstance = new CUI_NodeKeyGuide(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeKeyGuide");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeKeyGuide::Clone(void* pArg)
{
	CUI_NodeKeyGuide* pInstance = new CUI_NodeKeyGuide(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeKeyGuide");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeKeyGuide::Free()
{  
	__super::Free();

	for (auto& key : m_vecKeys)
		Safe_Release(key);
}
