#include "stdafx.h"
#include "..\public\Effect_Trail.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Point_Instancing.h"

CEffect_Trail::CEffect_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail & rhs)
	: CEffect_Base(rhs)
{
}

void CEffect_Trail::ResetInfo()
{
	if (m_pVITrailBufferCom == nullptr)
		return;

	m_pVITrailBufferCom->Get_InstanceInfo()->clear();
}

HRESULT CEffect_Trail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_eEFfectDesc.eEffectType = CEffect_Base::tagEffectDesc::EFFECT_TRAIL;
	return S_OK;
}

void CEffect_Trail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

 	m_pVITrailBufferCom->Tick(fTimeDelta);
}

void CEffect_Trail::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_vecProPos.empty() || m_vecProPos.back() != vPos)
		m_vecProPos.push_back(vPos);

	if (m_vecProPos.size() >= 3)
	{
		while (m_vecProPos.size() > 3)
			m_vecProPos.erase(m_vecProPos.begin());

		CCamera* pCamera = CGameInstance::GetInstance()->Get_WorkCameraPtr();
		_vector  vCamPos = pCamera->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		_float  fSplineLength = XMVectorGetX(XMVector3Length(m_vecProPos[1] - m_vecProPos[2]));

		_vector vPoint0 = m_vecProPos[0];
		_vector vPoint1 = m_vecProPos[1];
		_vector vPoint2 = m_vecProPos[2];
		_vector vPoint3 = m_vecProPos[2];

		_float  fPreLife = 0.0f;
		if (m_pVITrailBufferCom->Get_InstanceInfo()->size())
			fPreLife = m_pVITrailBufferCom->Get_InstanceInfo()->back().vPosition.w;

		_uint  iSegmentCnt = _uint(fSplineLength / m_eEFfectDesc.fSegmentSize);
		_vector vPrepos = vPoint1;

		_float  fWeight = 0.0f, fRadian = 0.0f;
		_vector vSplinePos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		_vector vRight = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		_vector vUp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		_vector vLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		_vector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		for (_uint i = 0; i < iSegmentCnt; ++i)
		{
			fWeight = _float(i + 1) / (_float)iSegmentCnt;
			vSplinePos = XMVectorCatmullRom(vPoint0, vPoint1, vPoint2, vPoint3, fWeight);

			vRight = XMVector3Normalize(vSplinePos - vPrepos);
			vDir = XMVector3Normalize(vCamPos - vSplinePos);

			fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
			if (fRadian < 5.f)
				continue;

			vUp = XMVector3Cross(vRight, vDir);
			vLook = XMVector3Cross(vRight, vUp);

			vSplinePos = XMVectorSetW(vSplinePos, CUtile::FloatLerp(fPreLife, m_eEFfectDesc.fLife, fWeight));

			_smatrix TrailMatrix(vRight, vUp, vLook, vSplinePos);
			m_pVITrailBufferCom->Add_Instance(TrailMatrix);
			vPrepos = vSplinePos;
		}
	}

 	if (nullptr != m_pRendererCom)
 		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Trail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Trail::Trail_LateTick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float  fRadian = 0.0f;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_vector vRight = XMVector3Normalize(pGameInstance->Get_CamRight_Float4());
	_vector vUp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	_vector vLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	_vector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	_vector vCamPos = pGameInstance->Get_CamPosition();

	for (_uint i = 0; i < (_uint)m_vecProPos.size(); ++i)
	{
		if (m_vecProPos[i].w <0.5f)
			continue;

		vDir = XMVector3Normalize(vCamPos - XMLoadFloat4(&m_vecProPos[i]));

		fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
		if (fRadian < 5.f)
			continue;

		vUp = XMVector3Cross(vRight, vDir);
		vLook = XMVector3Cross(vRight, vUp);

		_float fWidth = CUtile::Get_RandomFloat(0.1f, m_eEFfectDesc.fWidth);
		vLook = XMVectorSetW(vLook, fWidth);
		m_vecProPos[i].w = m_eEFfectDesc.fLife;

		_smatrix TrailMatrix(vRight, vUp, vLook, m_vecProPos[i]);
		m_pVITrailBufferCom->Add_Instance(TrailMatrix);
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

void CEffect_Trail::Trail_InputRandomPos(_float4 vPosition)
{
	_vector vmin = XMVectorSet(vPosition.x - 0.5f, vPosition.y - 0.5f, vPosition.z - 0.5f, 1.f);
	_vector vmax = XMVectorSet(vPosition.x + 0.5f, vPosition.y + 0.5f, vPosition.z + 0.5f, 1.f);
	
	_float4 vRandomPos = CUtile::Get_RandomVector(vmin, vmax);
	vRandomPos.w = 1.f;
	if (!m_vecProPos.empty() && m_vecProPos.back() != vRandomPos)
		m_vecProPos.push_back(vRandomPos);

	if(m_vecProPos.empty())
		m_vecProPos.push_back(vRandomPos);
}

void CEffect_Trail::Trail_InputPos(_float4 vPosition)
{
	m_vecProPos.push_back(vPosition);
}

void CEffect_Trail::Trail_KeyFrames(vector<_float4>* vecKeyFrames, _float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		ResetInfo();
		m_vecProPos.clear();
		return;
	}

	__super::Late_Tick(fTimeDelta);

	//m_vecProPos.swap(*vecKeyFrames);
	
	_uint	iSize = (_uint)vecKeyFrames->size();
	_uint	iPrePosSize = (_uint)m_vecProPos.size();
	_uint	iStartIndex;

	if (iPrePosSize == 0)
		iStartIndex = 2;
	else if (iPrePosSize > 0)
		iStartIndex = iPrePosSize;

	for (_uint i = 0; i < iSize; ++i)
		m_vecProPos.push_back((*vecKeyFrames)[i]);

	if ((_int)m_vecProPos.size() >= 3)
	{
		// 			while((_int)m_vecProPos.size() > 3)
		// 				m_vecProPos.erase(m_vecProPos.begin());

		CCamera*	pCamera = CGameInstance::GetInstance()->Get_WorkCameraPtr();
		_vector	vCamPos = pCamera->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		iPrePosSize = (_uint)m_vecProPos.size();

		for (_uint i = iStartIndex; i < iPrePosSize; ++i)
		{
			_float		fSplineLength = XMVectorGetX(XMVector3Length(m_vecProPos[i - 1] - m_vecProPos[i]));

			_vector vPoint0 = m_vecProPos[i - 2];
			_vector vPoint1 = m_vecProPos[i - 1];
			_vector vPoint2 = m_vecProPos[i];
			_vector vPoint3;

			if (i != iPrePosSize - 1)
				vPoint3 = m_vecProPos[i + 1];
			else
				vPoint3 = m_vecProPos[i];

			_float  fPreLife = 0.0f;
			_float	fMaxLife = 0.f;
			if (m_pVITrailBufferCom->Get_InstanceInfo()->size())
			{
				fPreLife = m_pVITrailBufferCom->Get_InstanceInfo()->back().vPosition.w;
				fMaxLife = fPreLife + (i - iStartIndex + 1) / iSize * (m_eEFfectDesc.fLife - fPreLife);
			}
			else
				fMaxLife = m_eEFfectDesc.fLife;


			_uint  iSegmentCnt = _uint(fSplineLength / m_eEFfectDesc.fSegmentSize);

			if (iSegmentCnt == 0)
				continue;

			_vector vPrepos = vPoint1;

			_float  fWeight = 0.0f, fRadian = 0.0f;
			_vector vSplinePos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			_vector vRight = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			_vector vUp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			_vector vLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			_vector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

			for (_uint j = 0; j < iSegmentCnt; ++j)
			{
				fWeight = _float(j + 1) / _float(iSegmentCnt);
				CUtile::Saturate<_float>(fWeight, 0.f, 1.f);

				vSplinePos = XMVectorCatmullRom(vPoint0, vPoint1, vPoint2, vPoint3, fWeight);

				vRight = XMVector3Normalize(vSplinePos - vPrepos);
				vDir = XMVector3Normalize(vCamPos - vSplinePos);

				fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vDir, vRight)))));
				if (fRadian < 5.f)
					continue;

				vUp = XMVector3Cross(vRight, vDir);
				vLook = XMVector3Cross(vRight, vUp);

				vSplinePos = XMVectorSetW(vSplinePos, CUtile::FloatLerp(fPreLife, fMaxLife, fWeight));

				_smatrix TrailMatrix(vRight, vUp, vLook, vSplinePos);
				m_pVITrailBufferCom->Add_Instance(TrailMatrix);
				vPrepos = vSplinePos;
			}
		}
	}
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Trail::SetUp_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectPointInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom), E_FAIL);

	/***********
	*  TEXTURE *
	************/
	/* For.DiffuseTexture */
	for (_uint i = 0; i < m_iTotalDTextureComCnt; ++i)
	{
		_tchar szDTexture[64] = L"";
		wsprintf(szDTexture, L"Com_DTexture_%d", i);

		_tchar* szDTextureComTag = CUtile::Create_String(szDTexture);
		CGameInstance::GetInstance()->Add_String(szDTextureComTag);

		FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[i], this), E_FAIL);
	}

	/* For.MaskTexture */
	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		_tchar szMTexture[64] = L"";
		wsprintf(szMTexture, L"Com_MTexture_%d", i);

		_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
		CGameInstance::GetInstance()->Add_String(szMTextureComTag);

		FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[i], this), E_FAIL);
	}

	return S_OK;
}

HRESULT CEffect_Trail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV()), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	/* Trail */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_IsTrail", &m_eEFfectDesc.IsTrail, sizeof(_bool)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fWidth", &m_eEFfectDesc.fWidth, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fLife", &m_eEFfectDesc.fLife, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDistanceAlpha", &m_eEFfectDesc.bAlpha, sizeof(_bool)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fAlpha", &m_eEFfectDesc.fAlpha, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pVITrailBufferCom->Bind_RawValue(m_pShaderCom, "g_InfoSize"), E_FAIL);

	/* Texture Total Cnt */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iTotalDTextureComCnt", &m_iTotalDTextureComCnt, sizeof _uint), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iTotalMTextureComCnt", &m_iTotalMTextureComCnt, sizeof _uint), E_FAIL);

	_uint iTextureRenderType = (_uint)m_eEFfectDesc.eTextureRenderType;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_TextureRenderType", &iTextureRenderType, sizeof(_uint)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_IsUseMask", &m_eEFfectDesc.IsMask, sizeof(bool)), E_FAIL);

	/* TEX_SPRITE */
	if (m_eEFfectDesc.eTextureRenderType == EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE)
	{
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_WidthFrame", &m_eEFfectDesc.fWidthFrame, sizeof(_float)), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_HeightFrame", &m_eEFfectDesc.fHeightFrame, sizeof(_float)), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_SeparateWidth", &m_eEFfectDesc.iSeparateWidth, sizeof(_int)), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_SeparateHeight", &m_eEFfectDesc.iSeparateHeight, sizeof(_int)), E_FAIL);
	}
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_BlendType", &m_eEFfectDesc.eBlendType, sizeof(_int)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vColor", &m_eEFfectDesc.vColor, sizeof(_float4)), E_FAIL);

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
	return S_OK;
}

void CEffect_Trail::Free()
{
	__super::Free();
}
