#include "stdafx.h"
#include "..\public\UI_NodeQuest.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Utile.h"
#include "UI_Event_Fade.h"

CUI_NodeQuest::CUI_NodeQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szQuest(nullptr)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bAppear(false)
	, m_bDisappear(false)
{
}

CUI_NodeQuest::CUI_NodeQuest(const CUI_NodeQuest & rhs)
	: CUI_Node(rhs)
	, m_szQuest(nullptr)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bAppear(false)
	, m_bDisappear(false)
{
}

void CUI_NodeQuest::Set_QuestString(wstring str)
{
	m_szQuest = CUtile::Create_String(str.c_str());
}

HRESULT CUI_NodeQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(400.f, 24.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	/* event */
	m_vecEvents.push_back(CUI_Event_Fade::Create(0.08f, 0.5f));

	m_fIntervalY = -10.f; /* Appear From */
	m_fIntervalX = -10.f; /* Dissappear To */
	return S_OK;
}

void CUI_NodeQuest::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
		m_bActive = false;

	if (!m_bAppear)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc < m_fTerm)
			return;

		if
	}
	else
	{
		if (!m_bDisappear)
		{

		}
	}

	__super::Tick(fTimeDelta);

}

void CUI_NodeQuest::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeQuest::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_HUDHPBar");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + 25.f, g_iWinSizeY*0.5f - vPos.y -20.f};

	CGameInstance::GetInstance()->Render_Font(TEXT("Font_Jangmi0"), m_szQuest,
		vNewPos /* position */,
		0.f, _float2(1.f, 1.f)/* size */, 
		XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
			//static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Get_Alpha();

	return S_OK;
}

HRESULT CUI_NodeQuest::SetUp_Components()
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

HRESULT CUI_NodeQuest::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

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

CUI_NodeQuest * CUI_NodeQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeQuest*	pInstance = new CUI_NodeQuest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeQuest::Clone(void * pArg)
{
	CUI_NodeQuest*	pInstance = new CUI_NodeQuest(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeQuestMain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeQuest::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		delete [] m_szQuest;
		m_szQuest = nullptr;
	}
}
