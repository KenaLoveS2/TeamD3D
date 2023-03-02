#include "stdafx.h"
#include "..\public\UI_NodeAmmoArrowGuage.h"
#include "GameInstance.h"
#include "UI_Event_Guage.h"
#include "UI_CanvasAmmo.h"
#include "UI_ClientManager.h"
#include "UI_NodeEffect.h"

CUI_NodeAmmoArrowGuage::CUI_NodeAmmoArrowGuage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_bFullFilled(false)
{
}

CUI_NodeAmmoArrowGuage::CUI_NodeAmmoArrowGuage(const CUI_NodeAmmoArrowGuage & rhs)
	: CUI_Node(rhs)
	, m_bFullFilled(false)
{
}

void CUI_NodeAmmoArrowGuage::Set_Guage(_float fNextGuage)
{
	/* Bomb guage doesn't go down step by step. */
	/* full-filled -> and if use -> it goes zero */
	/* after cool time end -> it goes back to Full */
	m_vecEvents[EVENT_GUAGE]->Call_Event(fNextGuage);
}

HRESULT CUI_NodeAmmoArrowGuage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeAmmoArrowGuage::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(80.f, 80.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}


	/* Events */
	/* �̹����� ����ǵ��� �ϴ� �̺�Ʈ */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Guage::Create(tDesc->fileName));

	return S_OK;
}

void CUI_NodeAmmoArrowGuage::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/* When Fullfilled, Sparkling Effect Needed (once) */
	if (static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_FullFilled()
		&& !m_bFullFilled)
	{
		m_bFullFilled = true;

		if (!(static_cast<CUI_CanvasAmmo*>(m_pParent)->Is_ArrowFull()))
		{
			static_cast<CUI_CanvasAmmo*>(m_pParent)->FillArrow();

			/* Connect With Canvas Aim's Arrow */
			static_cast<CUI_CanvasAmmo*>(m_pParent)->ConnectToAimUI(
				CUI_CanvasAmmo::AIM_ARROW, 1);

			if (!(static_cast<CUI_CanvasAmmo*>(m_pParent)->Is_ArrowFull()))
				m_vecEvents[EVENT_GUAGE]->Call_Event(-1.f);
		}

		CUI_ClientManager::GetInstance()
			->Get_Effect(CUI_ClientManager::EFFECT_ARROWFULL)
			->Start_Effect(this, 0.f, 35.f);

	}

	/* Full To Zero process ended */
	if (m_bFullFilled &&static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_Zero())
	{
		m_bFullFilled = false;
	}

	if (!m_bFullFilled && static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_Zero()
		&& !(static_cast<CUI_CanvasAmmo*>(m_pParent)->Is_ArrowFull()))
	{
		m_vecEvents[EVENT_GUAGE]->Call_Event(1.f);

	}
}

void CUI_NodeAmmoArrowGuage::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_NodeAmmoArrowGuage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeAmmoArrowGuage::SetUp_Components()
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

HRESULT CUI_NodeAmmoArrowGuage::SetUp_ShaderResources()
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

CUI_NodeAmmoArrowGuage * CUI_NodeAmmoArrowGuage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeAmmoArrowGuage*	pInstance = new CUI_NodeAmmoArrowGuage(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeAmmoArrowGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeAmmoArrowGuage::Clone(void * pArg)
{
	CUI_NodeAmmoArrowGuage*	pInstance = new CUI_NodeAmmoArrowGuage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeAmmoArrowGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeAmmoArrowGuage::Free()
{
	__super::Free();
}
