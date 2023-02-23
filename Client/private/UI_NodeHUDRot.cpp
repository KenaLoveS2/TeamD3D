#include "stdafx.h"
#include "..\public\UI_NodeHUDRot.h"
#include "GameInstance.h"

CUI_NodeHUDRot::CUI_NodeHUDRot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeHUDRot::CUI_NodeHUDRot(const CUI_NodeHUDRot & rhs)
	:CUI_Node(rhs)
{
}

HRESULT CUI_NodeHUDRot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeHUDRot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_tDesc.vSize = { (_float)g_iWinSizeX, (_float)g_iWinSizeY };
		m_tDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f };
		m_pTransformCom->Set_Scaled(_float3(275.f, 23.f, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	/* Test */
	m_bActive = true;
	//XMVectorSet(m_tDesc.vPos.x - g_iWinSizeX * 0.5f, -m_tDesc.vPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));


	XMStoreFloat4x4(&m_tDesc.ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_tDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));



	return S_OK;
}

void CUI_NodeHUDRot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/* 공용함수로 뺄거임 */
	if (m_tSpriteInfo.bFinished)
		return;
	m_tSpriteInfo.fAnimTimeAcc += fTimeDelta;
	if (m_tSpriteInfo.fAnimTimeAcc > m_tSpriteInfo.fAnimTime)
	{
		++m_tSpriteInfo.iXFrameNow;
		if (m_tSpriteInfo.iXFrameNow == m_tSpriteInfo.iXFrames)
		{
			if (m_tSpriteInfo.iYFrameNow == m_tSpriteInfo.iYFrames - 1) /* Last Sprite */
			{
				if (!m_tSpriteInfo.bLoop)
				{
					m_tSpriteInfo.iXFrameNow--;
					m_tSpriteInfo.bFinished = true;
				}
				else
				{
					m_tSpriteInfo.iXFrameNow = 0;
					m_tSpriteInfo.iYFrameNow = 0;

				}
			}
			else
			{
				m_tSpriteInfo.iXFrameNow = 0;
				++m_tSpriteInfo.iYFrameNow;
			}


		}

		m_tSpriteInfo.fAnimTimeAcc = 0.f;
	}

}

void CUI_NodeHUDRot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_NodeHUDRot::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : HUDROT");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_NodeHUDRot::SetUp_Components()
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

	/* Texture */
	/* Carry, Cloud, Hide, Neutral */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_HUDRotNeutral"), TEXT("Com_DiffuseTexture"),
		(CComponent**)&m_pTextureCom[0])))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_NodeHUDRot::SetUp_ShaderResources()
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

	/* Sprite Animation */
	if (FAILED(m_pShaderCom->Set_RawValue("g_XFrames", &m_tSpriteInfo.iXFrames, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_YFrames", &m_tSpriteInfo.iYFrames, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_XFrameNow", &m_tSpriteInfo.iXFrameNow, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_YFrameNow", &m_tSpriteInfo.iYFrameNow, sizeof(_int))))
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

CUI_NodeHUDRot * CUI_NodeHUDRot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeHUDRot*	pInstance = new CUI_NodeHUDRot(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeHUDRot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeHUDRot::Clone(void * pArg)
{
	CUI_NodeHUDRot*	pInstance = new CUI_NodeHUDRot(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeHUDRot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeHUDRot::Free()
{
	__super::Free();
}
