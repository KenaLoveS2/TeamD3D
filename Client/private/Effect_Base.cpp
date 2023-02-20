#include "stdafx.h"
#include "..\public\Effect_Base.h"
#include "GameInstance.h"

CEffect_Base::CEffect_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_eEFfectDesc, sizeof(EFFECTDESC));
}

CEffect_Base::CEffect_Base(const CEffect_Base & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Base::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEffect_Base::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));	

	if (nullptr != pArg)
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));
	else
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEffect_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);	
}

HRESULT CEffect_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Base::Add_TextureComponent(_uint iDTextureComCnt, _uint iMTextureComCnt)
{
	CTexture*	pTextureCom = nullptr;
	
	/* For.DiffuseTexture */
	if (iDTextureComCnt != 0)
	{
		m_iDTextureComCnt = m_iTotalDTextureComCnt + iDTextureComCnt;

		for (_uint i = m_iTotalDTextureComCnt; i < m_iDTextureComCnt; ++i)
		{
			m_strDTextureComTag = L"Com_DTexture_";
			m_strDTextureComTag += to_wstring(i);
			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"), m_strDTextureComTag.c_str(), (CComponent**)&m_pDTextureCom[i], this)))
				return E_FAIL;
		}
		m_iTotalDTextureComCnt = m_iDTextureComCnt;
	}

	/* For.Mask Texture */
	if (iMTextureComCnt != 0)
	{
		m_iMTextureComCnt = m_iTotalMTextureComCnt + iMTextureComCnt;

		for (_uint i = m_iTotalMTextureComCnt; i < m_iMTextureComCnt; ++i)
		{
			m_strMTextureComTag = L"Com_MTexture_";
			m_strMTextureComTag += to_wstring(i);
			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"), m_strMTextureComTag.c_str(), (CComponent**)&m_pMTextureCom[i], this)))
				return E_FAIL;
		}
		m_iTotalMTextureComCnt = m_iMTextureComCnt;
	}
	return S_OK;
}

void CEffect_Base::Free()
{
	__super::Free();

	if(m_isCloned)
	{
		// Shader, Renderer Release
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);

		// Texture
 		if (nullptr != m_pDTextureCom)
 		{
 			// Diffuse Release
 			for (_uint i = 0; i < m_iTotalDTextureComCnt; ++i)
 				Safe_Release(m_pDTextureCom[i]);
 		}
 
 		if (nullptr != m_pMTextureCom)
 		{
 			// Mask Release
 			for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
 				Safe_Release(m_pMTextureCom[i]);
 		}

		// VIBuffer Release
		if (nullptr != m_pVIBufferCom)
			Safe_Release(m_pVIBufferCom);

		if (nullptr != m_pVIInstancingBufferCom)
			Safe_Release(m_pVIInstancingBufferCom);
	}
}
