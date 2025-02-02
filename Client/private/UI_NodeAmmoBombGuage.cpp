#include "stdafx.h"
#include "..\public\UI_NodeAmmoBombGuage.h"
#include "GameInstance.h"
#include "UI_Event_ChangeImg.h"
#include "UI_Event_Guage.h"
#include "UI_ClientManager.h"
#include "UI_NodeEffect.h"
#include "UI_CanvasAmmo.h"

CUI_NodeAmmoBombGuage::CUI_NodeAmmoBombGuage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_fGuage(1.f)
{
}

CUI_NodeAmmoBombGuage::CUI_NodeAmmoBombGuage(const CUI_NodeAmmoBombGuage & rhs)
	: CUI_Node(rhs)
	, m_fGuage(1.f)
{
}

void CUI_NodeAmmoBombGuage::Set_Guage(_float fGuage)
{
	m_fGuage = fGuage;

	///* Bomb guage doesn't go down step by step. */
	///* full-filled -> and if use -> it goes zero */
	///* so if fGuage == 0, fullfilled texture changed to normal texture */
	///* and after cool time end -> it goes back to Full */
	//if (fNextGuage == 0.f)
	//{
	//	m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this, )0;
	//	m_vecEvents[EVENT_GUAGE]->Call_Event(-1.f);
	//}

}

void CUI_NodeAmmoBombGuage::Change_To_FullFilledImage()
{
	//m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this, TEX_FULL);
	m_iTextureIdx = TEX_FULL;
}

void CUI_NodeAmmoBombGuage::Change_To_GuageImage()
{
	//m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this, TEX_DEFAULT);
	m_iTextureIdx = TEX_DEFAULT;
}

void CUI_NodeAmmoBombGuage::Plus_Guage(_float fDiff)
{
	m_fGuage += fDiff;

	if (m_fGuage > 1.f)
		m_fGuage = 1.f;
}

HRESULT CUI_NodeAmmoBombGuage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeAmmoBombGuage::Initialize(void * pArg)
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

	/* Move m_fIntervalX when Level Up */
	m_fIntervalX = -90.f;

	/* Events */
	/* 이미지가 변경되도록 하는 이벤트 */
	//UIDESC* tDesc = (UIDESC*)pArg;
	//m_vecEvents.push_back(CUI_Event_Guage::Create(tDesc->fileName));
	//m_vecEvents.push_back(CUI_Event_ChangeImg::Create(tDesc->fileName));
	m_iTextureIdx = TEX_FULL;

	return S_OK;
}

void CUI_NodeAmmoBombGuage::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	///* change to fullfilled texture (once) */
	//if (static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_FullFilled()
	//	&& !m_bFullFilled)
	//{
	//	m_bFullFilled = true;
	//	m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this);
	//	CUI_ClientManager::GetInstance()
	//		->Get_Effect(CUI_ClientManager::EFFECT_BOMBFULL)
	//		->Start_Effect(this, 0.f, 0.f);

	//	if (!(static_cast<CUI_CanvasAmmo*>(m_pParent)->Is_BombFull()))
	//	{
	//		static_cast<CUI_CanvasAmmo*>(m_pParent)->FillBomb();

	//		/* Connect With Canvas Aim's Bomb */
	//		static_cast<CUI_CanvasAmmo*>(m_pParent)->ConnectToAimUI(
	//			CUI_CanvasAmmo::AIM_BOMB, 1, this);
	//	}
	//}

	//if (m_bFullFilled &&static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_Zero())
	//{
	//	m_bFullFilled = false;
	//}

	//if (!m_bFullFilled && static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_Zero())
	//{
	//	m_vecEvents[EVENT_GUAGE]->Call_Event(1.f);
	//}

}

void CUI_NodeAmmoBombGuage::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeAmmoBombGuage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeAmmoBombGuage::SetUp_Components()
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

HRESULT CUI_NodeAmmoBombGuage::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAmount", &m_fGuage, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_NodeAmmoBombGuage * CUI_NodeAmmoBombGuage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeAmmoBombGuage*	pInstance = new CUI_NodeAmmoBombGuage(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeAmmoBombGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeAmmoBombGuage::Clone(void * pArg)
{
	CUI_NodeAmmoBombGuage*	pInstance = new CUI_NodeAmmoBombGuage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeAmmoBombGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeAmmoBombGuage::Free()
{
	__super::Free();
}
