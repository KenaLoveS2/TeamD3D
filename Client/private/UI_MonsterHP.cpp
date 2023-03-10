#include "stdafx.h"
#include "..\public\UI_MonsterHP.h"
#include "GameInstance.h"
#include "Monster.h"

CUI_MonsterHP::CUI_MonsterHP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Billboard(pDevice, pContext)
{
	m_tWorldDesc.pOwner = nullptr;
	m_tWorldDesc.vPos = { 0.f, 0.f };
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP & rhs)
	:CUI_Billboard(rhs)
{
	m_tWorldDesc.pOwner = nullptr;
	m_tWorldDesc.vPos = { 0.f, 0.f };
}

HRESULT CUI_MonsterHP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MonsterHP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(20.f, 00.f, 1.f));
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	memcpy(&m_tWorldDesc, pArg, sizeof(WORLDUIDESC));

	m_bActive = true;
	return S_OK;
}

void CUI_MonsterHP::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Tick(fTimeDelta);

	if (m_tWorldDesc.pOwner != nullptr)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,
			(m_tWorldDesc.pOwner->Get_Position()));

}

void CUI_MonsterHP::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_MonsterHP::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : CUI_MonsterHP");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MonsterHP::SetUp_Components()
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

	/* Diffuse Texture */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_SimpleBar"), m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MonsterHP::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = XMVectorSetZ(vPos,1.f);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
	//	return E_FAIL;

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

CUI_MonsterHP * CUI_MonsterHP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_MonsterHP*	pInstance = new CUI_MonsterHP(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_MonsterHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MonsterHP::Clone(void * pArg)
{
	CUI_MonsterHP*	pInstance = new CUI_MonsterHP(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeRotGuage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MonsterHP::Free()
{
	__super::Free();
}
