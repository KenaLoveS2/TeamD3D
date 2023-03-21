#include "stdafx.h"
#include "..\public\UI_NodeItemBox.h"
#include "GameInstance.h"

CUI_NodeItemBox::CUI_NodeItemBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Node(pDevice, pContext)
	, m_szCount(nullptr)
	, m_vFontPos(0.f, 0.f)
{
}

CUI_NodeItemBox::CUI_NodeItemBox(const CUI_NodeItemBox & rhs)
	: CUI_Node(rhs)
	, m_szCount(nullptr)
	, m_vFontPos(0.f, 0.f)
{
}

void CUI_NodeItemBox::Set_String(_int iCount)
{
	Safe_Delete_Array(m_szCount);
	m_szCount = CUtile::Create_String(to_wstring(iCount).c_str());
}

HRESULT CUI_NodeItemBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeItemBox::Initialize(void * pArg)
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

	m_bActive = true;
	return S_OK;
}

void CUI_NodeItemBox::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;


	__super::Tick(fTimeDelta);
}

void CUI_NodeItemBox::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeItemBox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	/* temp */
	//m_szCount = L"2";
	//m_vFontPos = { 60.f, 0.f };

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 45.f + m_vFontPos.x, g_iWinSizeY*0.5f - vPos.y + m_vFontPos.y};

	if (nullptr != m_szCount)
	{
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_Comic"), m_szCount,
			vNewPos /* position */,
			0.f, _float2(0.8f, 0.8f)/* size */,
			XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	}

	return S_OK;
}

HRESULT CUI_NodeItemBox::SetUp_Components()
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

HRESULT CUI_NodeItemBox::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CUI_NodeItemBox * CUI_NodeItemBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeItemBox*	pInstance = new CUI_NodeItemBox(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeItemBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeItemBox::Clone(void * pArg)
{
	CUI_NodeItemBox*	pInstance = new CUI_NodeItemBox(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeItemBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeItemBox::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szCount);
}
