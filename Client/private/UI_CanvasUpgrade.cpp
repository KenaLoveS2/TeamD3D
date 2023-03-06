#include "stdafx.h"
#include "..\public\UI_CanvasUpgrade.h"
#include "GameInstance.h"

/* Bind Object */
#include "Kena.h"

CUI_CanvasUpgrade::CUI_CanvasUpgrade(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasUpgrade::CUI_CanvasUpgrade(const CUI_CanvasUpgrade & rhs)
	: CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasUpgrade::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasUpgrade::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling((_float)g_iWinSizeX, (_float)g_iWinSizeY, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, (_float)g_iWinSizeY, 1.f));
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

	//m_bActive = true;

	return S_OK;
}

void CUI_CanvasUpgrade::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			//	MSG_BOX("Bind Failed");
			return;
		}
	}
	m_bActive = true;

	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

}

void CUI_CanvasUpgrade::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasUpgrade::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasUpgrade::Bind()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CKena* pKena = dynamic_cast<CKena*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(),
		L"Layer_Player", L"Kena"));
	if (pKena == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pKena->m_PlayerDelegator.bind(this, &CUI_CanvasUpgrade::BindFunction);

	//m_Quests[0]->m_QuestDelegator.bind(this, &CUI_CanvasQuest::BindFunction);


	RELEASE_INSTANCE(CGameInstance);

	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasUpgrade::Ready_Nodes()
{
	return S_OK;
}

HRESULT CUI_CanvasUpgrade::SetUp_Components()
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

HRESULT CUI_CanvasUpgrade::SetUp_ShaderResources()
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

void CUI_CanvasUpgrade::BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue)
{
}

void CUI_CanvasUpgrade::Default(CUI_ClientManager::UI_PRESENT eType, _float fValue)
{
}

void CUI_CanvasUpgrade::LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel)
{
}

CUI_CanvasUpgrade * CUI_CanvasUpgrade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasUpgrade*	pInstance = new CUI_CanvasUpgrade(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasUpgrade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasUpgrade::Clone(void * pArg)
{
	CUI_CanvasUpgrade*	pInstance = new CUI_CanvasUpgrade(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasUpgrade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasUpgrade::Free()
{
	__super::Free();
}
