#include "stdafx.h"
#include "..\public\UI_CanvasQuest.h"
#include "GameInstance.h"

#include "UI_NodeQuestMain.h"
#include "UI_NodeQuestSub.h"

CUI_CanvasQuest::CUI_CanvasQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasQuest::CUI_CanvasQuest(const CUI_CanvasQuest & rhs)
	: CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasQuest::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(952.f, 1200.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_tDesc.vSize = { (_float)g_iWinSizeX, (_float)g_iWinSizeY };
		m_tDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		m_pTransformCom->Set_Scaled(_float3(m_tDesc.vSize.x, m_tDesc.vSize.y, 1.f));
		m_pTransformCom->Set_Scaled(_float3(952.f, 1200.f, 1.f));

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasQuest");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasQuest");
		return E_FAIL;
	}


	m_bActive = true;

	return S_OK;
}

void CUI_CanvasQuest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_CanvasQuest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasQuest::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasQuest::Bind()
{
	return S_OK;
}

HRESULT CUI_CanvasQuest::Ready_Nodes()
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
	str = "Node_QuestMain";
	tDesc.fileName.assign(str.begin(), str.end()); 
	pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_QuestMain", L"Node_QuestMain", &tDesc));
	if (FAILED(Add_Node(pUI)))
		return E_FAIL;
	m_vecNodeCloneTag.push_back(str);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasQuest::SetUp_Components()
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

HRESULT CUI_CanvasQuest::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources();

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

CUI_CanvasQuest * CUI_CanvasQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasQuest*	pInstance = new CUI_CanvasQuest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasQuest::Clone(void * pArg)
{
	CUI_CanvasQuest*	pInstance = new CUI_CanvasQuest(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasQuest::Free()
{
	__super::Free();
}
