#include "stdafx.h"
#include "..\public\UI_NodeHUDPip.h"
#include "GameInstance.h"
#include "UI_Event_ChangeImg.h"
#include "UI_Event_Guage.h"
#include "UI_ClientManager.h"
#include "UI_NodeEffect.h"
#include "UI_CanvasHUD.h"

CUI_NodeHUDPip::CUI_NodeHUDPip(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_bFullFilled(false)
{
}

CUI_NodeHUDPip::CUI_NodeHUDPip(const CUI_NodeHUDPip & rhs)
	: CUI_Node(rhs)
	, m_bFullFilled(false)
{
}

void CUI_NodeHUDPip::Set_Guage(_float fGuage)
{
	/* pip guage doesn't go down step by step. */
	/* full-filled -> and if use Rot -> it goes zero */
	/* so if fGuage == 0, fullfilled texture changed to normal texture */
	if (fGuage == 0.f)
	{
		m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this, 0);
		m_vecEvents[EVENT_GUAGE]->Call_Event(0.f);
	}
	else
		m_vecEvents[EVENT_GUAGE]->Call_Event(fGuage);
}

_float CUI_NodeHUDPip::Get_Guage()
{
	return static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Get_GuageNow();
}

void CUI_NodeHUDPip::ReArrangeGuage()
{
	static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->ReArrangeSettingOn();
	m_bFullFilled = false;
}

HRESULT CUI_NodeHUDPip::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDPip::Initialize(void * pArg)
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

	m_fIntervalX = 90.f;

	/* Events */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Guage::Create(tDesc->fileName));
	m_vecEvents.push_back(CUI_Event_ChangeImg::Create(tDesc->fileName));

	return S_OK;
}

void CUI_NodeHUDPip::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_FullFilled()
		&& !m_bFullFilled)
	{
		m_vecEvents[EVENT_TEXCHANGE]->Call_Event(this);
		m_bFullFilled = true;
		CUI_ClientManager::GetInstance()
			->Get_Effect(CUI_ClientManager::EFFECT_PIPFULL)
			->Start_Effect(this, 0.f, 50.f);

		CUI_CanvasHUD* pCanv = static_cast<CUI_CanvasHUD*>(m_pParent);
		if (!(pCanv->Is_PipAllFull()))
			pCanv->PlusPipCount();
	}


	_bool test = static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_Zero();
	if (m_bFullFilled &&static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Is_Zero())
		m_bFullFilled = false;

}

void CUI_NodeHUDPip::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeHUDPip::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDPip::SetUp_Components()
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

HRESULT CUI_NodeHUDPip::SetUp_ShaderResources()
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

CUI_NodeHUDPip * CUI_NodeHUDPip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeHUDPip*	pInstance = new CUI_NodeHUDPip(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeHUDPip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeHUDPip::Clone(void * pArg)
{
	CUI_NodeHUDPip*	pInstance = new CUI_NodeHUDPip(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeHUDPip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeHUDPip::Free()
{
	__super::Free();
}
