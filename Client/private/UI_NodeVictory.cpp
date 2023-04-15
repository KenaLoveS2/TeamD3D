#include "stdafx.h"
#include "UI_NodeVictory.h"
#include "GameInstance.h"
#include "UI_Event_Animation.h"

CUI_NodeVictory::CUI_NodeVictory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_bFinished(false)
{
}

CUI_NodeVictory::CUI_NodeVictory(const CUI_NodeVictory& rhs)
	: CUI_Node(rhs)
	, m_bFinished(false)
{
}

void CUI_NodeVictory::Sign_On()
{
	m_bActive = true;
	m_vecEvents[EVENT_ANIM]->Call_Event((_uint)0);
}

HRESULT CUI_NodeVictory::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeVictory::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled({ 400.f, 200.f, 1.f });
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLvUp");
		return E_FAIL;
	}

	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Animation::Create(tDesc->fileName, this));

	//m_bActive = true;
	return S_OK;
}

HRESULT CUI_NodeVictory::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CUI_NodeVictory::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Animation*>(m_vecEvents[EVENT_ANIM])->Is_Finished())
		m_bFinished = true;

	__super::Tick(fTimeDelta);
}

void CUI_NodeVictory::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeVictory::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeVictory::SetUp_Components()
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

HRESULT CUI_NodeVictory::SetUp_ShaderResources()
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

CUI_NodeVictory* CUI_NodeVictory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeVictory* pInstance = new CUI_NodeVictory(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeVictory");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeVictory::Clone(void* pArg)
{
	CUI_NodeVictory* pInstance = new CUI_NodeVictory(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeVictory");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeVictory::Free()
{
	__super::Free();
}
