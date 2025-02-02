#include "stdafx.h"
#include "..\public\UI_NodeQuest.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Utile.h"
#include "UI_Event_Fade.h"

CUI_NodeQuest::CUI_NodeQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_szQuest(nullptr)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bAppear(false)
	, m_bDisappear(false)
	, m_bFinished(false)
	, m_bMoveUp(false)
{
}

CUI_NodeQuest::CUI_NodeQuest(const CUI_NodeQuest& rhs)
	: CUI_Node(rhs)
	, m_szQuest(nullptr)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bAppear(false)
	, m_bDisappear(false)
	, m_bFinished(false)
	, m_bMoveUp(false)
{
}

void CUI_NodeQuest::Set_QuestString(wstring str)
{
	m_szQuest = CUtile::Create_String(str.c_str());
}

void CUI_NodeQuest::Appear(_float fTerm)
{
	m_bActive = true;

	m_bAppear = true;
	m_bDisappear = false;

	//m_fIntervalY = -20.f;
	//m_matLocal._42 += m_fIntervalY;

	/* this variable is not for this action, but just use it */
	m_fIntervalY = m_matLocal._42;
	m_matLocal._42 -= 20.f;

	m_fTimeAcc = 0.0f;
	m_fTerm = fTerm;

	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.03f, 5000.f);
	m_vecEvents[EVENT_FADE]->Call_Event(true);

}

void CUI_NodeQuest::Disappear(_float fTerm)
{
	m_bAppear = false;
	m_bDisappear = true;
	m_bFinished = false;

	m_fTimeAcc = 0.0f;
	m_fTerm = fTerm;

	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.05f, 0.f);
	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->FadeOut();
}

void CUI_NodeQuest::Move_Up()
{
	m_bMoveUp = true;
	m_fIntervalY = m_matLocal._42 + 140.f;
	m_fTimeAcc = 0.0f;
}

_float4 CUI_NodeQuest::Get_ComputePosition()
{
	_float4 vParentPos = { 0.f, 0.f, 0.f, 0.f };

	if (m_pParent != nullptr)
	{
		vParentPos = m_pParent->Get_LocalMatrix().r[3];
	}
	_float4 vPos = { m_matLocal._41, m_matLocal._42, m_matLocal._43, 0.f };
	vPos = vParentPos + vPos;

	return vPos;

}

HRESULT CUI_NodeQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeQuest::Initialize(void* pArg)
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

	m_fIntervalY = -20.f; /* Appear From */
	m_fIntervalX = -10.f; /* Dissappear To */
	return S_OK;
}

void CUI_NodeQuest::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bAppear)
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc < m_fTerm)
			return;

		if (m_matLocal._42 >= m_fIntervalY)
		{
			m_matLocal._42 = m_fIntervalY;
			m_bAppear = false;
		}
		else
			m_matLocal._42 += 1.f * (m_fTimeAcc - m_fTerm);

	}

	if (m_bDisappear)
	{
		m_fTimeAcc += fTimeDelta;
		if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
		{
			m_bActive = false;
			m_bDisappear = false;
			m_bFinished = true;
		}
		else
		{
			m_matLocal._41 -= 1.f * m_fTimeAcc;
		}

	}

	if (m_bMoveUp)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_matLocal._42 >= m_fIntervalY)
		{
			m_matLocal._42 = m_fIntervalY;
			m_bMoveUp = false;
		}
		else
		{
			m_matLocal._42 += 5.f * m_fTimeAcc;
		}
	}

	__super::Tick(fTimeDelta);

}

void CUI_NodeQuest::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);


	//CUI::Late_Tick(fTimeDelta);

	///* Calculate with Parent(Canvas) WorldMatrix (Scale, Translation) */
	//if (m_pParent != nullptr)
	//{
	//	_float4x4 matWorldParent;
	//	XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

	//	_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

	//	float fRatioX = matWorldParent._11 / m_matParentInit._11;
	//	float fRatioY = matWorldParent._22 / m_matParentInit._22;
	//	_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

	//	_matrix matWorld = m_matLocal * matParentScale * matParentTrans;
	//	m_pTransformCom->Set_WorldMatrix(matWorld);
	//}

	//for (auto e : m_vecEvents)
	//	e->Late_Tick(fTimeDelta);

	//if (nullptr != m_pRendererCom && m_bActive)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UILAST, this);

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
	_float2 vNewPos = { vPos.x + g_iWinSizeX * 0.5f - 150.f, g_iWinSizeY * 0.5f - vPos.y - 25.f };

	CGameInstance::GetInstance()->Render_Font(TEXT("Font_Jangmi0"), m_szQuest,
		vNewPos /* position */,
		0.f, _float2(1.f, 1.f)/* size */,
		XMVectorSet(1.f, 1.f, 1.f, static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Get_Alpha()));

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

	CUI::SetUp_ShaderResources();

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

CUI_NodeQuest* CUI_NodeQuest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeQuest* pInstance = new CUI_NodeQuest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeQuest::Clone(void* pArg)
{
	CUI_NodeQuest* pInstance = new CUI_NodeQuest(*this);
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
		delete[] m_szQuest;
		m_szQuest = nullptr;
	}
}
