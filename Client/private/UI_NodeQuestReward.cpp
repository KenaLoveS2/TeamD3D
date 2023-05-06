#include "stdafx.h"
#include "UI_NodeQuestReward.h"
#include "GameInstance.h"
#include "UI_Event_Fade.h"
#include "Kena.h"
#include "Kena_Status.h" 

CUI_NodeQuestReward::CUI_NodeQuestReward(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Node(pDevice, pContext)
	, m_bEnd(false)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bStart(false)
{
}

CUI_NodeQuestReward::CUI_NodeQuestReward(const CUI_NodeQuestReward& rhs)
	: CUI_Node(rhs)
	, m_bEnd(false)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_bStart(false)
{
}

void CUI_NodeQuestReward::RewardOn(_float4 vPos)
{
	m_bActive = true;
	//m_pTransformCom->Set_Position(vPos);
	m_matLocal._41 = vPos.x;
	m_matLocal._42 = vPos.y;
	m_matLocal._43 = vPos.z;

	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Change_Data(0.08f, 0.3f);

	CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	if (pKena != nullptr)
		pKena->Get_Status()->Add_Karma(150);


	static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->FadeOut();//->Call_Event(true);
}

_float CUI_NodeQuestReward::Get_Alpha()
{
	return static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Get_Alpha();
}

HRESULT CUI_NodeQuestReward::Initialize_Prototype()
{
	return CUI_Node::Initialize_Prototype();
}

HRESULT CUI_NodeQuestReward::Initialize(void* pArg)
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
	m_vecEvents.push_back(CUI_Event_Fade::Create(0.05f, 3.0f));
	return S_OK;
}

void CUI_NodeQuestReward::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bStart && static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
	{
		m_bActive = false;
		m_bEnd = true;
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeQuestReward::Late_Tick(_float fTimeDelta)
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

HRESULT CUI_NodeQuestReward::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeQuestReward::SetUp_Components()
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

HRESULT CUI_NodeQuestReward::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iTextureIdx)))
			return E_FAIL;
	}

	return S_OK;
}

CUI_NodeQuestReward* CUI_NodeQuestReward::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_NodeQuestReward* pInstance = new CUI_NodeQuestReward(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeQuestReward");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_NodeQuestReward::Clone(void* pArg)
{
	CUI_NodeQuestReward* pInstance = new CUI_NodeQuestReward(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeQuestReward");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeQuestReward::Free()
{
	__super::Free();
}
