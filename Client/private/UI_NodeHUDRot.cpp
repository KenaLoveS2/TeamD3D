#include "stdafx.h"
#include "..\public\UI_NodeHUDRot.h"
#include "GameInstance.h"
#include "UI_Event_ChangeImg.h"
#include "UI_Event_Animation.h"

CUI_NodeHUDRot::CUI_NodeHUDRot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeHUDRot::CUI_NodeHUDRot(const CUI_NodeHUDRot & rhs)
	:CUI_Node(rhs)
{
}

void CUI_NodeHUDRot::Change_RotIcon(_float fIcon)
{
	m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this, (_uint)fIcon);
	m_vecEvents[EVENT_ANIM]->Call_Event((_uint)fIcon);

}

HRESULT CUI_NodeHUDRot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDRot::Initialize(void * pArg)
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

	/* Event */
	/* Texture Change */
	/* Sprite Animation Event */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_ChangeImg::Create(tDesc->fileName));
	m_vecEvents.push_back(CUI_Event_Animation::Create(tDesc->fileName, this));

	return S_OK;
}

void CUI_NodeHUDRot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	}

void CUI_NodeHUDRot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_NodeHUDRot::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_NodeHUDRot::SetUp_Components()
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

HRESULT CUI_NodeHUDRot::SetUp_ShaderResources()
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

CUI_NodeHUDRot * CUI_NodeHUDRot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeHUDRot*	pInstance = new CUI_NodeHUDRot(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeHUDRot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeHUDRot::Clone(void * pArg)
{
	CUI_NodeHUDRot*	pInstance = new CUI_NodeHUDRot(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeHUDRot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeHUDRot::Free()
{
	__super::Free();
}
