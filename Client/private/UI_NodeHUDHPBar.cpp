#include "stdafx.h"
#include "..\public\UI_NodeHUDHPBar.h"
#include "GameInstance.h"

CUI_NodeHUDHPBar::CUI_NodeHUDHPBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeHUDHPBar::CUI_NodeHUDHPBar(const CUI_NodeHUDHPBar & rhs)
	: CUI_Node(rhs)
{
}

HRESULT CUI_NodeHUDHPBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDHPBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(400.f, 24.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	m_bActive = true;

	/* For. Upgrade */
	m_fSourScaleX = m_matLocal._11;
	m_fDestScaleX = m_fSourScaleX;
	m_fSourTransX = m_matLocal._41;
	m_fDestTransX = m_fSourTransX;
	m_bUpgrade = false;
	m_fSpeed = 1.f;

	return S_OK;
}

void CUI_NodeHUDHPBar::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bUpgrade)
	{
		if (m_fDestScaleX > m_matLocal._11)
		{
			m_fSpeed = 40.f;
			m_matLocal._11 += m_fSpeed * fTimeDelta;
			m_matLocal._41 += m_fSpeed * fTimeDelta * 0.5f;

		}
		else
		{
			m_matLocal._11 = m_fDestScaleX;
			m_matLocal._41 = m_fDestTransX;
			m_bUpgrade = false;
		}
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeHUDHPBar::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);


}

HRESULT CUI_NodeHUDHPBar::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_NodeHUDHPBar::Upgrade()
{
	/* LevelUp : Guage Length + 150.f */
	m_fSourScaleX = m_matLocal._11;
	m_fDestScaleX = m_fSourScaleX + 100.f;
	m_fSourTransX = m_matLocal._41;
	m_fDestTransX = m_fSourTransX + 100.f * 0.5f;
	m_bUpgrade = true;
}

HRESULT CUI_NodeHUDHPBar::SetUp_Components()
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

HRESULT CUI_NodeHUDHPBar::SetUp_ShaderResources()
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

CUI_NodeHUDHPBar * CUI_NodeHUDHPBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeHUDHPBar*	pInstance = new CUI_NodeHUDHPBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeHUDHPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeHUDHPBar::Clone(void * pArg)
{
	CUI_NodeHUDHPBar*	pInstance = new CUI_NodeHUDHPBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeHUDHPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeHUDHPBar::Free()
{
	__super::Free();
}
