#include "stdafx.h"
#include "..\public\UI_NodeLetterBox.h"
#include "GameInstance.h"

CUI_NodeLetterBox::CUI_NodeLetterBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_fSpeed(0.f)
	, m_bStart(false)
	, m_fMinY(0.f)
{
}

CUI_NodeLetterBox::CUI_NodeLetterBox(const CUI_NodeLetterBox & rhs)
	: CUI_Node(rhs)
	, m_fSpeed(0.f)
	, m_bStart(false)
	, m_fMinY(0.f)
{
}

HRESULT CUI_NodeLetterBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeLetterBox::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3((_float)g_iWinSizeX, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CUI_NodeLetterBox");
		return E_FAIL;
	}

	m_fMinY = 900.f;
	return S_OK;
}

HRESULT CUI_NodeLetterBox::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CUI_NodeLetterBox::Tick(_float fTimeDelta)
{
	if (m_bStart)
	{
		m_matLocal._22 += m_fSpeed;
		
		if (m_fSpeed < 0.0f)/* Appear*/
		{
			if (m_matLocal._22 <= m_fMinY)
			{
				m_matLocal._22 = m_fMinY;
				m_bStart = false;
			}
		}
		else if (m_fSpeed > 0.0f) /* disappear */
		{
			if (m_matLocal._22 > m_matLocalOriginal._22)
			{
				m_matLocal._22 = m_matLocalOriginal._22;
				m_bStart = false;
				m_bActive = false;
			}
		}
	}

	__super::Tick(fTimeDelta);
}

void CUI_NodeLetterBox::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CUI_NodeLetterBox::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_NodeSkill");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();
	return S_OK;
}

void CUI_NodeLetterBox::Appear()
{
	m_bActive = true;
	m_bStart = true;
	m_fSpeed = -20.f;
}

void CUI_NodeLetterBox::DisAppear()
{
	m_bStart = true;
	m_fSpeed = 20.f;
}

HRESULT CUI_NodeLetterBox::SetUp_Components()
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

HRESULT CUI_NodeLetterBox::SetUp_ShaderResources()
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

CUI_NodeLetterBox * CUI_NodeLetterBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeLetterBox*	pInstance = new CUI_NodeLetterBox(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeLetterBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeLetterBox::Clone(void * pArg)
{
	CUI_NodeLetterBox*	pInstance = new CUI_NodeLetterBox(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeLetterBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeLetterBox::Free()
{
	__super::Free();
}
