#include "stdafx.h"
#include "..\public\UI_CanvasHUD.h"
#include "GameInstance.h"
#include "UI_NodeHUDHP.h"
#include "UI_NodeHUDHPBar.h"

CUI_CanvasHUD::CUI_CanvasHUD(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasHUD::CUI_CanvasHUD(const CUI_CanvasHUD & rhs)
	:CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasHUD::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasHUD::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	/* Test */
	m_bActive = true;
	m_tDesc.vSize	= { (_float)g_iWinSizeX, (_float)g_iWinSizeY};
	m_tDesc.vPos	= { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
//	m_pTransformCom->Set_Scaled(_float3(m_tDesc.vSize.x, m_tDesc.vSize.y, 1.f));
	m_pTransformCom->Set_Scaled(_float3(670.f, 200.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
		//XMVectorSet(m_tDesc.vPos.x - g_iWinSizeX * 0.5f, -m_tDesc.vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_tDesc.ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_tDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes");
		return E_FAIL;
	}


	return S_OK;
}

void CUI_CanvasHUD::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_CanvasHUD::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_CanvasHUD::Render()
{
	/* ÅØ½ºÃ³ */
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_CanvasHUD");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}


HRESULT CUI_CanvasHUD::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI* pUI = nullptr;
	string str;

	pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(), L"Layer_UI",
		L"Prototype_GameObject_UI_Node_HPBar", L"UI_Node_PlayerHPBar", nullptr, (CGameObject**)&pUI);
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	str = "PlayerHPBar";
	m_vecNodeCloneTag.push_back(str);

	//pUI = nullptr;
	//pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(), L"Layer_UI",
	//	L"Prototype_GameObject_UI_Node_HP", L"UI_Node_PlayerHP", nullptr, (CGameObject**)&pUI);
	//if (FAILED(Add_ChildUI(pUI)))
	//	return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CUI_CanvasHUD::SetUp_Components()
{
	/* Renderer */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),(CComponent**)&m_pRendererCom))
		return E_FAIL;

	/* Shader */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),(CComponent**)&m_pVIBufferCom))
		return E_FAIL;

	/* Texture */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDFrame"), TEXT("Com_DiffuseTexture"),
		(CComponent**)&m_pTextureCom[0])))
		return E_FAIL;

	/* Todo : Load FileData */

	return S_OK;
}

HRESULT CUI_CanvasHUD::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
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

CUI_CanvasHUD * CUI_CanvasHUD::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasHUD*	pInstance = new CUI_CanvasHUD(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasHUD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasHUD::Clone(void * pArg)
{
	CUI_CanvasHUD*	pInstance = new CUI_CanvasHUD(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasHUD");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasHUD::Free()
{
	__super::Free();
}
