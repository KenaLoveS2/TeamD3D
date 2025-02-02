#include "stdafx.h"
#include "..\public\UI_NodeNumRots.h"
#include "GameInstance.h"

CUI_NodeNumRots::CUI_NodeNumRots(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice,pContext)
	, m_iNumRots(0)
{
}

CUI_NodeNumRots::CUI_NodeNumRots(const CUI_NodeNumRots & rhs)
	:CUI_Node(rhs)
	, m_iNumRots(0)
{
}

HRESULT CUI_NodeNumRots::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeNumRots::Initialize(void * pArg)
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

void CUI_NodeNumRots::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_NodeNumRots::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeNumRots::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + 20.f, g_iWinSizeY*0.5f - vPos.y - 20.f };

	_tchar* str = CUtile::StringToWideChar(to_string(m_iNumRots));
	CGameInstance::GetInstance()->Render_Font(TEXT("Font_Basic0"), str,
		vNewPos /* position */,
		0.f, _float2(0.9f, 0.9f)/* size */,
		XMVectorSet(1.f, 1.f, 1.f, 1.f)/* color */);
	Safe_Delete_Array(str);
	return S_OK;
}

HRESULT CUI_NodeNumRots::SetUp_Components()
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

HRESULT CUI_NodeNumRots::SetUp_ShaderResources()
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

CUI_NodeNumRots * CUI_NodeNumRots::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeNumRots*	pInstance = new CUI_NodeNumRots(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeNumRots");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeNumRots::Clone(void * pArg)
{
	CUI_NodeNumRots*	pInstance = new CUI_NodeNumRots(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeNumRots");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeNumRots::Free()
{
	__super::Free();
}
