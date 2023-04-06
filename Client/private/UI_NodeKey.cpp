#include "stdafx.h"
#include "UI_NodeKey.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"

CUI_NodeKey::CUI_NodeKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_szKeyDesc(nullptr)
{
}

CUI_NodeKey::CUI_NodeKey(const CUI_NodeKey& rhs)
	:CUI_Node(rhs)
	, m_szKeyDesc(nullptr)
{
}

HRESULT CUI_NodeKey::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeKey::Initialize(void* pArg)
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

	//m_bActive = true;

	/* event */
	m_vecEvents.push_back(CUI_Event_Fade::Create(0.08f, 0.5f));

	return S_OK;
}

HRESULT CUI_NodeKey::Late_Initialize(void* pArg)
{
	return S_OK;

}

void CUI_NodeKey::Tick(_float fTimeDelta)
{

	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_NodeKey::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeKey::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeKey::SetUp_Components()
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

HRESULT CUI_NodeKey::SetUp_ShaderResources()
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

CUI_NodeKey* CUI_NodeKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeKey* pInstance = new CUI_NodeKey(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeKey");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeKey::Clone(void* pArg)
{
	CUI_NodeKey* pInstance = new CUI_NodeKey(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeKey");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeKey::Free()
{
	__super::Free();
}
