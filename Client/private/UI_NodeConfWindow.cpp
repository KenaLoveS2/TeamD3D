#include "stdafx.h"
#include "..\public\UI_NodeConfWindow.h"
#include "GameInstance.h"

CUI_NodeConfWindow::CUI_NodeConfWindow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_Message(nullptr)
{
}

CUI_NodeConfWindow::CUI_NodeConfWindow(const CUI_NodeConfWindow & rhs)
	: CUI_Node(rhs)
	, m_Message(nullptr)
{
}

void CUI_NodeConfWindow::Set_Message(wstring msg, CUI_Canvas * pCaller)
{
	Safe_Delete_Array(m_Message);
	m_Message = CUtile::Create_String(msg.c_str());
	m_pCaller = pCaller;
}

HRESULT CUI_NodeConfWindow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeConfWindow::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	return S_OK;
}

void CUI_NodeConfWindow::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);
}

void CUI_NodeConfWindow::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	CUI::Late_Tick(fTimeDelta);

	/* Calculate with Parent(Canvas) WorldMatrix (Scale, Translation) */
	if (m_pParent != nullptr)
	{
		_float4x4 matWorldParent;
		XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

		_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

		float fRatioX = matWorldParent._11 / m_matParentInit._11;
		float fRatioY = matWorldParent._22 / m_matParentInit._22;
		_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

		_matrix matWorld = m_matLocal*matParentScale*matParentTrans;
		m_pTransformCom->Set_WorldMatrix(matWorld);
	}

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UILAST, this);

}

HRESULT CUI_NodeConfWindow::Render()
{
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

	if (nullptr != m_Message)
	{
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_Basic0"), m_Message,
			vNewPos /* position */,
			0.f, _float2(0.9f, 0.9f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	}

	return S_OK;
}

HRESULT CUI_NodeConfWindow::SetUp_Components()
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

HRESULT CUI_NodeConfWindow::SetUp_ShaderResources()
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

CUI_NodeConfWindow * CUI_NodeConfWindow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeConfWindow*	pInstance = new CUI_NodeConfWindow(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeConfWindow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeConfWindow::Clone(void * pArg)
{
	CUI_NodeConfWindow*	pInstance = new CUI_NodeConfWindow(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeConfWindow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeConfWindow::Free()
{
	__super::Free();

	Safe_Delete_Array(m_Message);
}
