#include "stdafx.h"
#include "..\public\Effect.h"
#include "GameInstance.h"


CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CEffect_Base(rhs)
{
}

HRESULT CEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));	

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.IsBillboard == true)
		BillBoardSetting(m_eEFfectDesc.vScale);
	else
		m_pTransformCom->Set_Scaled(m_eEFfectDesc.vScale);

	if (m_eEFfectDesc.eTextureRenderType == CEffect_Base::tagEffectDesc::TEX_SPRITE)
	{
		m_fTimeDelta += fTimeDelta;
		if (m_fTimeDelta > 1.f / m_eEFfectDesc.fTimeDelta * fTimeDelta)
		{
			if (m_eEFfectDesc.fTimeDelta < 1.f)
				m_eEFfectDesc.fWidthFrame++;
			else
				m_eEFfectDesc.fWidthFrame += floor(m_eEFfectDesc.fTimeDelta);
			m_fTimeDelta = 0.0;

			if (m_eEFfectDesc.fWidthFrame >= m_eEFfectDesc.iWidthCnt)
			{
				if (m_eEFfectDesc.fTimeDelta < 1.f)
					m_eEFfectDesc.fHeightFrame++;
				else
					m_eEFfectDesc.fHeightFrame += floor(m_eEFfectDesc.fTimeDelta);

				m_eEFfectDesc.fWidthFrame = 0.f;

				if (m_eEFfectDesc.fHeightFrame >= m_eEFfectDesc.iHeightCnt)
					m_eEFfectDesc.fHeightFrame = 0.f;
			}
		}
	}
}

void CEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);	
	__super::Compute_CamDistance();

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_eEFfectDesc.eBlendType == CEffect_Base::tagEffectDesc::BLENDSTATE_DEFAULT)
		m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_DEFAULT);
	else if (m_eEFfectDesc.eBlendType == CEffect_Base::tagEffectDesc::BLENDSTATE_ALPHA)
		m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_ALPHA);
	else if (m_eEFfectDesc.eBlendType == CEffect_Base::tagEffectDesc::BLENDSTATE_ONEEFFECT)
		m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_ONEEFFECT);
	else
		m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_MIX);

	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxEffectTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/***********
	*  TEXTURE *
	************/
	m_iTotalDTextureComCnt = 1;
	m_iTotalMTextureComCnt = 1;

	/* For.DiffuseTexture */
	for (_uint i = 0; i < m_iTotalDTextureComCnt; ++i)
	{
		m_strDTextureComTag = L"Com_DTexture_";
		m_strDTextureComTag += to_wstring(i);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"), m_strDTextureComTag.c_str(), (CComponent**)&m_pDTextureCom[i], this)))
			return E_FAIL;
	}

	/* For.MaskTexture */
	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		m_strMTextureComTag = L"Com_MTexture_";
		m_strMTextureComTag += to_wstring(i);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"), m_strMTextureComTag.c_str(), (CComponent**)&m_pMTextureCom[i], this)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
		
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
		return E_FAIL;
	
	/* Texture Total Cnt */	
	if (FAILED(m_pShaderCom->Set_RawValue("g_iTotalDTextureComCnt", &m_iTotalDTextureComCnt, sizeof _uint)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iTotalMTextureComCnt", &m_iTotalMTextureComCnt, sizeof _uint)))
		return E_FAIL;

	/* TEX_SPRITE */
	if (m_eEFfectDesc.eTextureRenderType == EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_WidthFrame", &m_eEFfectDesc.fWidthFrame, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_HeightFrame", &m_eEFfectDesc.fHeightFrame, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_SeparateWidth", &m_eEFfectDesc.iSeparateWidth, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_SeparateHeight", &m_eEFfectDesc.iSeparateHeight, sizeof(_int))))
			return E_FAIL;
	}	
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlendType", &m_eEFfectDesc.eBlendType, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_TextureRenderType", &m_eEFfectDesc.eTextureRenderType, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_eEFfectDesc.vColor, sizeof(_float4))))
		return E_FAIL;

	// MaxCnt == 10

	for (_uint i = 0; i < m_iTotalDTextureComCnt; ++i)
	{
		m_strDTextureComTag = L"g_DTexture_";
		m_strDTextureComTag += to_wstring(i);

		char szDTexture[64] = "";
		CUtile::WideCharToChar(m_strDTextureComTag.c_str(), szDTexture);

		if (FAILED(m_pDTextureCom[i]->Bind_ShaderResource(m_pShaderCom, szDTexture, (_uint)m_eEFfectDesc.fFrame[i])))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		m_strMTextureComTag = L"g_MTexture_";
		m_strMTextureComTag += to_wstring(i);

		char szMTexture[64] = "";
		CUtile::WideCharToChar(m_strMTextureComTag.c_str(), szMTexture);

		if (FAILED(m_pMTextureCom[i]->Bind_ShaderResource(m_pShaderCom, szMTexture, (_uint)m_eEFfectDesc.fMaskFrame[i])))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CEffect*		pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect*		pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect::Free()
{
	__super::Free();
}
