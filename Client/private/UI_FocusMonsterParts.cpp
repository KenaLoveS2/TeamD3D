#include "stdafx.h"
#include "..\public\UI_FocusMonsterParts.h"
#include "GameInstance.h"
#include "Camera.h"

CUI_FocusMonsterParts::CUI_FocusMonsterParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_Billboard(pDevice, pContext)
	, m_bStart(false)
	, m_bEnd(false)
	, m_fSpeed(0.f)
{
}

CUI_FocusMonsterParts::CUI_FocusMonsterParts(const CUI_FocusMonsterParts & rhs)
	: CUI_Billboard(rhs)
	, m_bStart(false)
	, m_bEnd(false)
	, m_fSpeed(0.f)
{
}

HRESULT CUI_FocusMonsterParts::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_FocusMonsterParts::Initialize(void * pArg)
{
	CUI::Initialize(pArg);

	if (pArg != nullptr)
		memcpy(&m_tPartsDesc, pArg, sizeof(PARTSDESC));
		
	/* It might be faster.... */
	m_iRenderPass = 18;

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	switch (m_tPartsDesc.iType)
	{
	case 0: /* left */
		m_pTransformCom->Set_Scaled(_float3(28.f, 28.f, 1.f));
		m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
	//	m_vStartScale.x = 2.f * m_vOriginalSettingScale.x;
		m_vStartScale.y = 0.f * m_vOriginalSettingScale.y;
		break;
	case 1: /* right */
		m_pTransformCom->Set_Scaled(_float3(28.f, 28.f, 1.f));
		m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
	//	m_vStartScale.x = 2.f * m_vOriginalSettingScale.x;
		m_vStartScale.y = 0.f * m_vOriginalSettingScale.y;
		break;
	case 2: /* Center */
		m_pTransformCom->Set_Scaled(_float3(16.8f, 35.f, 1.f));
		m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();
		m_vStartScale.x = 0.f * m_vOriginalSettingScale.x;
	//	m_vStartScale.y = 2.f * m_vOriginalSettingScale.y;
		break;
	}

	m_bActive = true;
	return S_OK;
}

void CUI_FocusMonsterParts::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_pParent == nullptr)
		return;

	__super::Tick(fTimeDelta);
}

void CUI_FocusMonsterParts::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

	_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vLook;
	_float4 vRight;
	_float3 vScale = m_pTransformCom->Get_Scaled();

	switch (m_tPartsDesc.iType)
	{
	case 0: /* left */
		vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		m_pTransformCom->Set_Position(vPos + -0.1f * vRight);// +0.1f * vLook);

		if (m_bStart)
		{
			vScale.y += 0.001f * fTimeDelta;
			if (vScale.y >= m_vOriginalSettingScale.y)
			{
				m_bEnd = true;
				vScale.y = m_vOriginalSettingScale.y;
			}

			m_pTransformCom->Set_Scaled(vScale);
		}

		break;
	case 1: /* right */
		vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		vLook = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_Position(vPos + 0.1f * vRight);// +0.001f * vLook);

		if (m_bStart)
		{
			vScale.y += 0.001f * fTimeDelta;
			if (vScale.y >= m_vOriginalSettingScale.y)
			{
				m_bEnd = true;
				vScale.y = m_vOriginalSettingScale.y;
			}

			m_pTransformCom->Set_Scaled(vScale);
		}
		break;
	case 2: /* center */
		m_pTransformCom->Set_Position(vPos);

		if (m_bStart)
		{
			vScale.x += 0.001f * fTimeDelta;
			if (vScale.x >= m_vOriginalSettingScale.x)
			{
				m_bEnd = true;
				vScale.x = m_vOriginalSettingScale.x;
			}

			m_pTransformCom->Set_Scaled(vScale);
		}
		break;
	}

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CUI_FocusMonsterParts::Render()
{
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

HRESULT CUI_FocusMonsterParts::SetUp_Components()
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
	switch (m_tPartsDesc.iType)
	{
	case 0: /* Left */
		if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockOnSide"), m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
			return E_FAIL;
		break;
	case 1: /* right */
		if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockOnSide2"), m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
			return E_FAIL;
		break;
	case 2: /* center */
		if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LockOnCenter"), m_TextureComTag[TEXTURE_DIFFUSE].c_str(), (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]))
			return E_FAIL;
		break;

	}


	return S_OK;
}

HRESULT CUI_FocusMonsterParts::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
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

CUI_FocusMonsterParts * CUI_FocusMonsterParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_FocusMonsterParts*	pInstance = new CUI_FocusMonsterParts(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_FocusMonsterParts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_FocusMonsterParts::Clone(void * pArg)
{
	CUI_FocusMonsterParts*	pInstance = new CUI_FocusMonsterParts(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_FocusMonsterParts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_FocusMonsterParts::Free()
{
	__super::Free();
}
