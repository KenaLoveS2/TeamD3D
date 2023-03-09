#include "stdafx.h"
#include "..\public\UI_CanvasConfirm.h"
#include "GameInstance.h"
#include "UI_NodeButton.h"

CUI_CanvasConfirm::CUI_CanvasConfirm(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice,pContext)
	, m_bConfirm(false)
	, m_Message(nullptr)
	, m_pCaller(nullptr)
{
}

CUI_CanvasConfirm::CUI_CanvasConfirm(const CUI_CanvasConfirm & rhs)
	:CUI_Canvas(rhs)
	, m_bConfirm(false)
	, m_Message(nullptr)
	, m_pCaller(nullptr)
{
}

void CUI_CanvasConfirm::Set_Message(wstring msg, CUI_Canvas* pCaller)
{
	m_bActive = true;
	Safe_Delete_Array(m_Message);
	m_Message = CUtile::Create_String(msg.c_str());
	m_vecNode[UI_YES]->Set_Active(true);
	m_vecNode[UI_NO]->Set_Active(true);
	m_pCaller = pCaller;
}

HRESULT CUI_CanvasConfirm::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasConfirm::Initialize(void * pArg)
{
	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling((_float)600.f, 300.f, 1.f));

	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)600.f, 250.f, 1.f));
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

	//m_bActive = true;

	return S_OK;
}

void CUI_CanvasConfirm::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
		{
			//	MSG_BOX("Bind Failed");
			return;
		}
	}

	if (!m_bActive)
		return;


	Picking();

	__super::Tick(fTimeDelta);
}

void CUI_CanvasConfirm::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	CUI::Late_Tick(fTimeDelta);

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UILAST, this);

		/* Nodes added to RenderList After the canvas. */
		/* So It can be guaranteed that Canvas Draw first */
		for (auto node : m_vecNode)
			node->Late_Tick(fTimeDelta);
	}
}

HRESULT CUI_CanvasConfirm::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_NodeSkillName");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 210.f, g_iWinSizeY*0.5f - vPos.y - 50.f };

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_Message)
	{
		pGameInstance->Render_Font(TEXT("Font_Basic0"), m_Message,
			vNewPos /* position */,
			0.f, _float2(0.9f, 0.9f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	}

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CUI_CanvasConfirm::Bind()
{
	return S_OK;
}

HRESULT CUI_CanvasConfirm::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	{/* Yes Button */
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_Yes";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Button", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
		static_cast<CUI_NodeButton*>(pUI)->Setting(L"확인", CUI_NodeButton::TYPE_CONFIRM);
	}

	{/* No Button */
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_No";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Button", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
		static_cast<CUI_NodeButton*>(pUI)->Setting(L"취소", CUI_NodeButton::TYPE_CONFIRM);
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasConfirm::SetUp_Components()
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

HRESULT CUI_CanvasConfirm::SetUp_ShaderResources()
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

void CUI_CanvasConfirm::Picking()
{
	/* Picking */
	POINT pt = CUtile::GetClientCursorPos(g_hWnd);

	_bool bClicked = false;
	_int iChoice = -1;
	for (_uint i = 0; i < UI_END; ++i)
	{
		_float4 vPos = m_vecNode[i]->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		_float2 vPosConvert = { vPos.x + 0.5f*g_iWinSizeX, -vPos.y + 0.5f*g_iWinSizeY };
		// left, top, right, bottom 
		RECT rc = { LONG(vPosConvert.x - 30.f), LONG(vPosConvert.y - 20.f),
			LONG(vPosConvert.x + 30.f), LONG(vPosConvert.y + 20.f) };

		if (PtInRect(&rc, pt))
		{
			if (bClicked = static_cast<CUI_NodeButton*>(m_vecNode[i])->MouseOverEvent())
				iChoice = i;
		}
		else
			static_cast<CUI_NodeButton*>(m_vecNode[i])->BackToNormal();
	}

	if (bClicked)
	{
		m_bActive = false;

		if (UI_YES == iChoice)
		{
			if (m_pCaller != nullptr)
				m_pCaller->Common_Function(true);
		}
		else
		{
			if (m_pCaller != nullptr)
				m_pCaller->Common_Function(false);
		}
	}

}

void CUI_CanvasConfirm::BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue)
{
}

CUI_CanvasConfirm * CUI_CanvasConfirm::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasConfirm*	pInstance = new CUI_CanvasConfirm(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasConfirm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasConfirm::Clone(void * pArg)
{
	CUI_CanvasConfirm*	pInstance = new CUI_CanvasConfirm(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasConfirm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasConfirm::Free()
{
	__super::Free();
	Safe_Delete_Array(m_Message);
}
