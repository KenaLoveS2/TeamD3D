#include "stdafx.h"
#include "..\public\UI_NodeButton.h"
#include "GameInstance.h"


CUI_NodeButton::CUI_NodeButton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szName(nullptr)
	, m_eType(TYPE_END)
	, m_fAlpha(1.f)
	, m_vColor{ 1.f, 1.f, 1.f, 1.f }
	, m_vOffset{ 0.0f, 0.0f }
	, m_vFontSize{ 1.f, 1.f }
{
}

CUI_NodeButton::CUI_NodeButton(const CUI_NodeButton & rhs)
	: CUI_Node(rhs)
	, m_szName(nullptr)
	, m_eType(TYPE_END)
	, m_fAlpha(1.f)
	, m_vColor{ 1.f, 1.f, 1.f, 1.f }
	, m_vOffset{ 0.0f, 0.0f }
	, m_vFontSize{ 1.f, 1.f }
{
}

void CUI_NodeButton::Setting(wstring wstrName, TYPE eType)
{
	m_szName = CUtile::Create_StringAuto(wstrName.c_str());
	m_eType = eType;

	switch (eType)
	{
	case TYPE_CONFIRM:
		m_fAlpha = 0.f;
		m_vOffset = { -23.f, -18.f };
		m_szFont = L"Font_Basic0";
		m_vFontSize = { 0.8f, 0.8f };
		break;
	case TYPE_LOGO:
		m_fAlpha = 0.f;
		m_vOffset = { -110.f, -20.f };
		m_szFont = L"Font_Basic0";
		break;
	}
}

HRESULT CUI_NodeButton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeButton::Initialize(void * pArg)
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

	//m_bActive = true;
	return S_OK;
}

void CUI_NodeButton::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);
}

void CUI_NodeButton::Late_Tick(_float fTimeDelta)
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
	else /* If It doesn't belong to canvas */
	{
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_matLocal));
	}

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UILAST, this);
}

HRESULT CUI_NodeButton::Render()
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

	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + m_vOffset.x, g_iWinSizeY*0.5f - vPos.y + m_vOffset.y };

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_szName)
	{
		pGameInstance->Render_Font(m_szFont, m_szName,
			vNewPos /* position */,
			0.f, m_vFontSize/* size */,
			XMLoadFloat4(&m_vColor)/* color */);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CUI_NodeButton::MouseOverEvent()
{
	switch (m_eType)
	{
	case TYPE_CONFIRM:
	{
		m_fAlpha = 1.f;
		m_vColor = { 0.f, 0.f,0.f,1.f };
		if (CGameInstance::GetInstance()->Mouse_Down(DIM_LB))
			return true;
		else
			return false;
	} break;

	case TYPE_LOGO:
	{
		m_fAlpha = 1.f;
		m_vColor = { 1.f, 1.f,1.f,1.f };
		if (CGameInstance::GetInstance()->Mouse_Down(DIM_LB))
			return true;
		else
			return false;
	} break;
	
	
	}
	return false;
}

void CUI_NodeButton::BackToNormal()
{
	switch (m_eType)
	{
	case TYPE_CONFIRM:
		m_fAlpha = 0.f;
		m_vColor = { 1.f, 1.f,1.f,1.f };
		break;
	case TYPE_LOGO:
		m_fAlpha = 0.f;
		m_vColor = { 1.f, 1.f,1.f,1.f };
		break;
	}
}

HRESULT CUI_NodeButton::SetUp_Components()
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

HRESULT CUI_NodeButton::SetUp_ShaderResources()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
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

CUI_NodeButton * CUI_NodeButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeButton*	pInstance = new CUI_NodeButton(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeButton::Clone(void * pArg)
{
	CUI_NodeButton*	pInstance = new CUI_NodeButton(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeButton::Free()
{
	__super::Free();

}