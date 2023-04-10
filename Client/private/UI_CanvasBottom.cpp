#include "stdafx.h"
#include "..\public\UI_CanvasBottom.h"
#include "GameInstance.h"
#include "Kena.h"
#include "UI_NodeLetterBox.h"
#include "UI_NodeChat.h"
#include "UI_NodeKey.h"

/* Bind Object */
#include "Saiya.h"
#include "CinematicCamera.h"

CUI_CanvasBottom::CUI_CanvasBottom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
{
}

CUI_CanvasBottom::CUI_CanvasBottom(const CUI_CanvasBottom & rhs)
	: CUI_Canvas(rhs)
{
}

HRESULT CUI_CanvasBottom::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasBottom::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling((_float)g_iWinSizeX, 70.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, 70.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasUpgrade");
		return E_FAIL;
	}


	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasUpgrade");
		return E_FAIL;
	}

	m_bActive = true;

	return S_OK;
}

void CUI_CanvasBottom::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
			return;
	}

	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_CanvasBottom::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasBottom::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI_CanvasBottom::Bind()
{
	CSaiya* pSaiya = dynamic_cast<CSaiya*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_NPC", L"Saiya"));
	if (pSaiya == nullptr)
		return E_FAIL;
	
	map <const _tchar*, class CCamera*>* pMap = CGameInstance::GetInstance()->Get_CameraContainer();

	if (pMap == nullptr)
		return E_FAIL;

	if (!pMap->empty())
	{
		for (auto pair : *pMap)
		{
			CCinematicCamera* pCam = dynamic_cast<CCinematicCamera*>(pair.second);

			if(pCam != nullptr)
				pCam->m_CinemaDelegator.bind(this, &CUI_CanvasBottom::BindFunction);
		}
	}

	pSaiya->m_SaiyaDelegator.bind(this, &CUI_CanvasBottom::BindFunction);


	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasBottom::Ready_Nodes()
{
	/* RotGet, RotLevelUp, MapTitle, */

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	{ /* LetterBox */
		string strTag = "Node_LetterBox";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_LetterBox", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* Chat */
		string strTag = "Node_Chat";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Chat", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* Keys */
		string strTag = "Node_Key1";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Key", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	{ /* Keys */
		string strTag = "Node_Key2";
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;
		_tchar* cloneTag = CUtile::StringToWideChar(strTag);
		tDesc.fileName = cloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Key", cloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strTag);
		pGameInstance->Add_String(cloneTag);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasBottom::SetUp_Components()
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

HRESULT CUI_CanvasBottom::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

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

	return S_OK;
}

void CUI_CanvasBottom::BindFunction(CUI_ClientManager::UI_PRESENT eType, _bool bValue, _float fValue, wstring wstr)
{
	switch (eType)
	{
	case CUI_ClientManager::BOT_LETTERBOX:
		if (bValue)
		{
			CUI_ClientManager::GetInstance()->Switch_FrontUI(false);
			static_cast<CUI_NodeLetterBox*>(m_vecNode[UI_LETTERBOX])->Appear();
		}
		else
		{
			CUI_ClientManager::GetInstance()->Switch_FrontUI(true);
			static_cast<CUI_NodeLetterBox*>(m_vecNode[UI_LETTERBOX])->DisAppear();
		}
		break;

	case CUI_ClientManager::BOT_CHAT:
		if (bValue)
		{
			CUI_ClientManager::GetInstance()->Switch_FrontUI(false);
			m_vecNode[UI_CHAT]->Set_Active(true);
			static_cast<CUI_NodeChat*>(m_vecNode[UI_CHAT])->Set_String(wstr, fValue);
		}
		else
		{
			CUI_ClientManager::GetInstance()->Switch_FrontUI(true);
			m_vecNode[UI_CHAT]->Set_Active(false);
		}
		break;
	}


}

CUI_CanvasBottom * CUI_CanvasBottom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasBottom*	pInstance = new CUI_CanvasBottom(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasBottom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasBottom::Clone(void * pArg)
{
	CUI_CanvasBottom*	pInstance = new CUI_CanvasBottom(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasBottom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasBottom::Free()
{
	__super::Free();
}