#include "stdafx.h"
#include "..\public\Effect_Trail_T.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Point_Instancing.h"

CEffect_Trail_T::CEffect_Trail_T(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CEffect_Trail_T::CEffect_Trail_T(const CEffect_Trail_T & rhs)
	: CEffect_Base(rhs)
{
}

HRESULT CEffect_Trail_T::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail_T::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_eEFfectDesc.eEffectType = CEffect_Base::tagEffectDesc::EFFECT_TRAIL;
	return S_OK;
}

void CEffect_Trail_T::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Set_TrailDesc();

	m_pVITrailBufferCom->Tick(fTimeDelta);
}

void CEffect_Trail_T::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_vecProPos.empty() || m_vecProPos.back() != vPos)
		m_vecProPos.push_back(vPos);

	if (m_vecProPos.size() >= 3)
	{
		while (m_vecProPos.size() > 3)
			m_vecProPos.erase(m_vecProPos.begin());

		_vector vCampos = pGameInstance->Get_CamPosition();
		_float  fSplineLength = XMVectorGetX(XMVector3Length(m_vecProPos[1] - m_vecProPos[2]));
		_uint   iSegmentCnt = _uint(fSplineLength / m_eEFfectDesc.fSegmentSize);

		_vector vPoint0 = m_vecProPos[0];
		_vector vPoint1 = m_vecProPos[1];
		_vector vPoint2 = m_vecProPos[2];
		_vector vPoint3 = m_vecProPos[2];
		_float  fPreLife = 0.0f;

		if (m_pVITrailBufferCom->Get_InstanceInfo()->size())
			fPreLife = m_pVITrailBufferCom->Get_InstanceInfo()->back().vPosition.w;

		_vector vPrepos = vPoint1;

		_float  fWeight = 0.0f, fRadian = 0.0f;
		_vector vSplinePos, vRight, vUp, vLook, vDir;
		for (_uint i = 0; i < iSegmentCnt; ++i)
		{
			fWeight = _float(i + 1) / (_float)iSegmentCnt;
			vSplinePos = XMVectorCatmullRom(vPoint0, vPoint1, vPoint2, vPoint3, fWeight);

			vRight = XMVector3Normalize(vSplinePos - vPrepos);
			vDir = XMVector3Normalize(vCampos - vSplinePos);

			fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
			if(fRadian < 5.f)
				continue;

			vUp = XMVector3Cross(vRight, vDir);
			vLook = XMVector3Cross(vRight, vUp);

			vSplinePos = XMVectorSetW(vSplinePos, CUtile::FloatLerp(fPreLife, m_eEFfectDesc.fLife, fWeight));

			_smatrix TrailMatrix(vRight, vUp, vLook, vSplinePos);
			m_pVITrailBufferCom->Add_Instance(TrailMatrix);
			vPrepos = vSplinePos;
		}
	}
	RELEASE_INSTANCE(CGameInstance);

 	if (nullptr != m_pRendererCom)
 		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Trail_T::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	//if (m_eEFfectDesc.eBlendType == CEffect_Base::tagEffectDesc::BLENDSTATE_DEFAULT)
	//	m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_TRAIL + EFFECTDESC::BLENDSTATE_DEFAULT);
	//else if (m_eEFfectDesc.eBlendType == CEffect_Base::tagEffectDesc::BLENDSTATE_ALPHA)
	//	m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_TRAIL + EFFECTDESC::BLENDSTATE_ALPHA);
	//else if (m_eEFfectDesc.eBlendType == CEffect_Base::tagEffectDesc::BLENDSTATE_ONEEFFECT)
	//	m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_TRAIL + EFFECTDESC::BLENDSTATE_ONEEFFECT);
	//else
	//	m_pShaderCom->Begin(EFFECTDESC::BLENDSTATE_TRAIL + EFFECTDESC::BLENDSTATE_MIX);
	m_pShaderCom->Begin(m_eEFfectDesc.iPassCnt);
	m_pVITrailBufferCom->Render();
	return S_OK;
}

HRESULT CEffect_Trail_T::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_int iCurLevel = 0;
#ifdef TESTPLAY
	iCurLevel = LEVEL_TESTPLAY;
#else 
	iCurLevel = LEVEL_EFFECT;
#endif // TESTPLAY

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectPointInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(iCurLevel, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVITrailBufferCom)))
		return E_FAIL;

	/***********
	*  TEXTURE *
	************/
	m_iTotalDTextureComCnt = 1;
	m_iTotalMTextureComCnt = 1;

	/* For.DiffuseTexture */
	_tchar szDTexture[64] = L"";
	wsprintf(szDTexture, L"Com_DTexture_%d", 0);

	_tchar* szDTextureComTag = CUtile::Create_String(szDTexture);
	CGameInstance::GetInstance()->Add_String(szDTextureComTag);

	if (FAILED(__super::Add_Component(iCurLevel, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[0], this)))
		return E_FAIL;

	/* For.MaskTexture */
	_tchar szMTexture[64] = L"";
	wsprintf(szMTexture, L"Com_MTexture_%d", 0);

	_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
	CGameInstance::GetInstance()->Add_String(szMTextureComTag);

	if (FAILED(__super::Add_Component(iCurLevel, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[0], this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail_T::SetUp_ShaderResources()
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

	/* Trail */
	if (FAILED(m_pShaderCom->Set_RawValue("g_IsTrail", &m_eEFfectDesc.IsTrail, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fWidth", &m_eEFfectDesc.fWidth, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fLife", &m_eEFfectDesc.fLife, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistanceAlpha", &m_eEFfectDesc.bAlpha, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_eEFfectDesc.fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_InfoMatrix")))
		return E_FAIL;

	/* Texture Total Cnt */
	if (FAILED(m_pShaderCom->Set_RawValue("g_iTotalDTextureComCnt", &m_iTotalDTextureComCnt, sizeof _uint)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iTotalMTextureComCnt", &m_iTotalMTextureComCnt, sizeof _uint)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_TextureRenderType", &m_eEFfectDesc.eTextureRenderType, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_IsUseMask", &m_eEFfectDesc.IsMask, sizeof(bool))))
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_eEFfectDesc.vColor, sizeof(_float4))))
		return E_FAIL;

	for (_uint i = 0; i < m_iTotalDTextureComCnt; ++i)
	{
		wstring strBindDTexture = L"g_DTexture_";
		strBindDTexture += to_wstring(i);

		char szDTexture[64] = "";
		CUtile::WideCharToChar(strBindDTexture.c_str(), szDTexture);

		if (FAILED(m_pDTextureCom[i]->Bind_ShaderResource(m_pShaderCom, szDTexture, (_uint)m_eEFfectDesc.fFrame[i])))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		wstring strBindMTexture = L"g_MTexture_";
		strBindMTexture += to_wstring(i);

		char szMTexture[64] = "";
		CUtile::WideCharToChar(strBindMTexture.c_str(), szMTexture);

		if (FAILED(m_pMTextureCom[i]->Bind_ShaderResource(m_pShaderCom, szMTexture, (_uint)m_eEFfectDesc.fMaskFrame[i])))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CEffect_Trail_T * CEffect_Trail_T::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CEffect_Trail_T*		pInstance = new CEffect_Trail_T(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Trail_T");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Trail_T::Clone(void * pArg)
{
	CEffect_Trail_T*		pInstance = new CEffect_Trail_T(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Trail_T");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Trail_T::Free()
{
	__super::Free();
}
