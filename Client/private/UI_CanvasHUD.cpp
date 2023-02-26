#include "stdafx.h"
#include "..\public\UI_CanvasHUD.h"
#include "GameInstance.h"
#include "UI_NodeHUDHP.h"
#include "UI_NodeHUDHPBar.h"
#include "UI_NodeHUDShield.h"
#include "UI_NodeHUDPip.h"
#include "UI_NodeHUDRot.h"

/* Bind Object */
#include "Kena.h"

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
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(670.f, 200.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_tDesc.vSize = { (_float)g_iWinSizeX, (_float)g_iWinSizeY };
		m_tDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		m_pTransformCom->Set_Scaled(_float3(m_tDesc.vSize.x, m_tDesc.vSize.y, 1.f));
		m_pTransformCom->Set_Scaled(_float3(670.f, 200.f, 1.f));

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}
	/* Test */
	m_bActive = true;


	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}


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
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			MSG_BOX("Bind Failed");
			return;
		}

	}

	__super::Tick(fTimeDelta);

	for (auto node : m_vecNode)
		node->Tick(fTimeDelta);
}

void CUI_CanvasHUD::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_bActive = true;
	if (nullptr != m_pRendererCom && m_bActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

		/* Nodes added to RenderList After the canvas. */
		/* So It can be guaranteed that Canvas Draw first */
		for (auto node : m_vecNode)
			node->Late_Tick(fTimeDelta);
	}






}

HRESULT CUI_CanvasHUD::Render()
{
	/* 텍스처 */
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

HRESULT CUI_CanvasHUD::Bind()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(),
		L"Layer_Player", L"Kena"));
	if (pKena == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pKena->m_PlayerDelegator.bind(this, &CUI_CanvasHUD::Function);


	RELEASE_INSTANCE(CGameInstance);


	m_bBindFinished = true;
	return S_OK;
}


HRESULT CUI_CanvasHUD::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI* pUI = nullptr;
	CUI::UIDESC tDesc;
	string str;
	wstring wstr;

	/* Note : the reason using unstable temp address variable "fileName" is that
		fileName(CloneTag) needed while it cloned when loading the data.
		(The cloneTag is stored after the clone process.)
	*/
	str = "Node_HPBar";
	tDesc.fileName.assign(str.begin(), str.end()); /* this file name doesn't exist eternally.(지역변수) */
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_HPBar", L"Node_HPBar",&tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_HP";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_HP", L"Node_HP", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_ShieldBar";
	tDesc.fileName.assign(str.begin(), str.end()); 
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ShieldBar", L"Node_ShieldBar", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_Shield";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Shield", L"Node_Shield", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_RotIcon";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_RotIcon", L"Node_RotIcon", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipBar";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipBar", L"Node_PipBar", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);

	str = "Node_PipGuage";
	tDesc.fileName.assign(str.begin(), str.end());
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_PipGuage", L"Node_PipGuage", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);


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

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
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

void CUI_CanvasHUD::Function(CUI_ClientManager::UI_HUD eType, _float fValue)
{
	switch (eType)
	{
	case CUI_ClientManager::HUD_HP:
		static_cast<CUI_NodeHUDHP*>(m_vecNode[UI_HPGUAGE])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::HUD_SHIELD:
		static_cast<CUI_NodeHUDShield*>(m_vecNode[UI_SHIELD])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::HUD_PIP:
		static_cast<CUI_NodeHUDPip*>(m_vecNode[UI_PIPGAUGE])->Set_Guage(fValue);
		break;
	case CUI_ClientManager::HUD_ROT:
		static_cast<CUI_NodeHUDRot*>(m_vecNode[UI_ROT])->Change_RotIcon(fValue);
		break;
	}
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
