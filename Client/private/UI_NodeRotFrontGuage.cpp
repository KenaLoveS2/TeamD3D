#include "stdafx.h"
#include "..\public\UI_NodeRotFrontGuage.h"
#include "GameInstance.h"
#include "UI_Event_Guage.h"
#include "UI_Event_Fade.h"

CUI_NodeRotFrontGuage::CUI_NodeRotFrontGuage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szTitle(nullptr)
{
}

CUI_NodeRotFrontGuage::CUI_NodeRotFrontGuage(const CUI_NodeRotFrontGuage & rhs)
	: CUI_Node(rhs)
	, m_szTitle(nullptr)
{
}

void CUI_NodeRotFrontGuage::Set_Guage(_float fGuage)
{
	m_bActive = true;

	m_vecEvents[EVENT_FADE]->Call_Event(true);
	m_vecEvents[EVENT_GUAGE]->Call_Event(fGuage);
}

void CUI_NodeRotFrontGuage::Set_GuageZero()
{
	static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Set_InitState(0.0f);
}

_float CUI_NodeRotFrontGuage::Get_CurrentGuage()
{
	return static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Get_GuageNow();
}

_float CUI_NodeRotFrontGuage::Get_CurrentGuagePosition()
{
	_float fGuage = Get_CurrentGuage();

	return m_matLocal._41 - 0.5f*m_matLocal._11 /* Zero Point */
		+ m_matLocal._11*fGuage;
}

_bool CUI_NodeRotFrontGuage::If_DisAppear_Get_Alpha(_float* pAlpha_Out)
{
	if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_DisAppear())
	{
		*pAlpha_Out = static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Get_Alpha();
		return true;
	}
	else
		return false;
}

HRESULT CUI_NodeRotFrontGuage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeRotFrontGuage::Initialize(void * pArg)
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


	//m_bActive = true;
	m_szTitle = CUtile::Create_StringAuto(L"부식령 발견!");

	/* Events */
	UIDESC* tDesc = (UIDESC*)pArg;
	m_vecEvents.push_back(CUI_Event_Guage::Create(tDesc->fileName));
	static_cast<CUI_Event_Guage*>(m_vecEvents[EVENT_GUAGE])->Set_InitState(0.f);

	m_vecEvents.push_back(CUI_Event_Fade::Create(0.05f, 2.f));
	return S_OK;
}

HRESULT CUI_NodeRotFrontGuage::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CUI_NodeRotFrontGuage::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Is_End())
		m_bActive = false;

	__super::Tick(fTimeDelta);
}

void CUI_NodeRotFrontGuage::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeRotFrontGuage::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//{
	//	MSG_BOX("Failed To Setup ShaderResources : CUI_NodeSkillName");
	//	return E_FAIL;
	//}

	//m_pShaderCom->Begin(m_iRenderPass);
	//m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 80.f, g_iWinSizeY*0.5f - vPos.y -70.f };

	if (m_szTitle != nullptr)
	{
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_SR0"), m_szTitle,
			vNewPos /* position */,
			0.f, _float2(0.7f, 0.7f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, static_cast<CUI_Event_Fade*>(m_vecEvents[EVENT_FADE])->Get_Alpha())/* color */);
	}

	return S_OK;
}

HRESULT CUI_NodeRotFrontGuage::SetUp_Components()
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

HRESULT CUI_NodeRotFrontGuage::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CUI_NodeRotFrontGuage * CUI_NodeRotFrontGuage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeRotFrontGuage*	pInstance = new CUI_NodeRotFrontGuage(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeRotFrontGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeRotFrontGuage::Clone(void * pArg)
{
	CUI_NodeRotFrontGuage*	pInstance = new CUI_NodeRotFrontGuage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeRotFrontGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeRotFrontGuage::Free()
{
	__super::Free();
}
