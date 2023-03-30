#include "stdafx.h"
#include "..\public\E_KenaPulseCloud.h"
#include "Shader.h"
#include "Effect_Trail.h"
#include "GameInstance.h"
#include "Camera.h"
#include "E_KenaPulse.h"

CE_KenaPulseCloud::CE_KenaPulseCloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaPulseCloud::CE_KenaPulseCloud(const CE_KenaPulseCloud & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_KenaPulseCloud::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulseCloud::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_fInitSpriteCnt = _float2(1.0f, 1.0f);
	m_eEFfectDesc.vScale = XMVectorSet(3.2f, 3.2f, 1.f, 1.f);
	return S_OK;
}

void CE_KenaPulseCloud::Tick(_float fTimeDelta)
{
	 __super::Tick(fTimeDelta);

	//if (m_eEFfectDesc.eTextureRenderType == CEffect_Base::tagEffectDesc::TEX_SPRITE)
	//{
	//	if (m_eEFfectDesc.bActive == true)
	//		Set_InitRotation();
//
	//	m_fTimeDelta += fTimeDelta;
	//	if (m_fTimeDelta > 1.f / m_eEFfectDesc.fTimeDelta * fTimeDelta)
	//	{
	//		if (m_bPlay == false)
	//		{
	//			m_eEFfectDesc.fWidthFrame++;
	//			if (m_eEFfectDesc.fWidthFrame >= m_eEFfectDesc.iWidthCnt)
	//			{
	//				m_eEFfectDesc.fHeightFrame++;
	//				if (m_eEFfectDesc.fHeightFrame >= m_eEFfectDesc.iHeightCnt)
	//				{
	//					m_eEFfectDesc.fHeightFrame--;
	//					m_eEFfectDesc.fWidthFrame--;
	//					m_bPlay = true;
	//				}
	//				else
	//					m_eEFfectDesc.fWidthFrame = m_fInitSpriteCnt.x;
	//			}
	//		}
	//		else
	//		{
	//			m_eEFfectDesc.fWidthFrame--;
	//			if (m_eEFfectDesc.fWidthFrame < m_fInitSpriteCnt.x)
	//			{
	//				m_eEFfectDesc.fHeightFrame--;
	//				if (m_eEFfectDesc.fHeightFrame < m_fInitSpriteCnt.y)
	//				{
	//					m_eEFfectDesc.fHeightFrame++;
	//					m_bPlay = false;
	//				}
	//				else
	//					m_eEFfectDesc.fWidthFrame = (_float)(m_eEFfectDesc.iWidthCnt - 1);
	//			}
	//		}
	//	}
	//}
}

void CE_KenaPulseCloud::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	if (m_eEFfectDesc.IsBillboard == true)
		CUtile::Execute_BillBoard(m_pTransformCom, m_eEFfectDesc.vScale);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CE_KenaPulseCloud::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV()), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_WorldCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);

	/* Texture Total Cnt */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iTotalDTextureComCnt", &m_iTotalDTextureComCnt, sizeof _uint), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iTotalMTextureComCnt", &m_iTotalMTextureComCnt, sizeof _uint), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_TextureRenderType", &m_eEFfectDesc.eTextureRenderType, sizeof _uint), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_IsUseMask", &m_eEFfectDesc.IsMask, sizeof _bool), E_FAIL);

	/* Texture Sprite */
	if (m_eEFfectDesc.eTextureRenderType == EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE)
	{
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_WidthFrame", &m_eEFfectDesc.fWidthFrame, sizeof _float), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_HeightFrame", &m_eEFfectDesc.fHeightFrame, sizeof _float), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_SeparateWidth", &m_eEFfectDesc.iSeparateWidth, sizeof _int), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_SeparateHeight", &m_eEFfectDesc.iSeparateHeight, sizeof _int), E_FAIL);
	}
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fShaderBindTime, sizeof _float), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vColor", &m_eEFfectDesc.vColor, sizeof _float4), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_BlendType", &m_eEFfectDesc.eBlendType, sizeof _int), E_FAIL);

	/* Pulse Hp°¡Á®¿Í¼­ ÄÃ·¯ ¹Ù²ãÁÜ */
	if(dynamic_cast<CE_KenaPulse*>(m_pParent))
	{
		CE_KenaPulse* pKenaPulse = dynamic_cast<CE_KenaPulse*>(m_pParent);
		CE_KenaPulse::STATUS eStatus = pKenaPulse->Get_Statue();
		_float fRatio = pKenaPulse->Get_HpRatio();
		if (fRatio <= 0.0f)
		{
			m_eEFfectDesc.bActive = false;
			RELEASE_INSTANCE(CGameInstance);
			return S_OK;
		}

		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_HpRatio", &fRatio, sizeof _float), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_PulseState", &eStatus.eState, sizeof _uint), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_DamageDurationTime", &eStatus.fStateDurationTime, sizeof _float), E_FAIL);
	}
	/* ~Pulse Hp°¡Á®¿Í¼­ ÄÃ·¯ ¹Ù²ãÁÜ */

	for (_uint i = 0; i < m_iTotalDTextureComCnt; ++i)
	{
		wstring strBindDTexture = L"g_DTexture_";
		strBindDTexture += to_wstring(i);

		char szDTexture[64] = "";
		CUtile::WideCharToChar(strBindDTexture.c_str(), szDTexture);

		FAILED_CHECK_RETURN(m_pDTextureCom[i]->Bind_ShaderResource(m_pShaderCom, szDTexture, (_uint)m_eEFfectDesc.fFrame[i]), E_FAIL);
	}

	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		wstring strBindMTexture = L"g_MTexture_";
		strBindMTexture += to_wstring(i);

		char szMTexture[64] = "";
		CUtile::WideCharToChar(strBindMTexture.c_str(), szMTexture);
		FAILED_CHECK_RETURN(m_pMTextureCom[i]->Bind_ShaderResource(m_pShaderCom, szMTexture, (_uint)m_eEFfectDesc.fMaskFrame[i]), E_FAIL);
	}

	RELEASE_INSTANCE(CGameInstance);

	m_pShaderCom->Begin(m_eEFfectDesc.iPassCnt);
	m_pVIBufferCom->Render();

	return S_OK;
}

CE_KenaPulseCloud* CE_KenaPulseCloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaPulseCloud* pInstnace = new CE_KenaPulseCloud(pDevice, pContext);

	if (FAILED(pInstnace->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulseCloud Create Failed");
		Safe_Release(pInstnace);
	}
	return pInstnace;
}

CGameObject * CE_KenaPulseCloud::Clone(void * pArg)
{
	CE_KenaPulseCloud* pInstnace = new CE_KenaPulseCloud(*this);

	if (FAILED(pInstnace->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulseCloud Clone Failed");
		Safe_Release(pInstnace);
	}
	return pInstnace;
}

void CE_KenaPulseCloud::Free()
{
	__super::Free();
}
