#include "stdafx.h"
#include "..\public\Effect_Point_Instancing.h"
#include "GameInstance.h"
#include "Effect_Trail.h"
#include "Effect_Trail_T.h"

CEffect_Point_Instancing::CEffect_Point_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CEffect_Point_Instancing::CEffect_Point_Instancing(const CEffect_Point_Instancing & rhs)
	: CEffect_Base(rhs)
	, m_szVIBufferProtoTag(rhs.m_szVIBufferProtoTag)
{
}

HRESULT CEffect_Point_Instancing::Set_ShapePosition()
{
	if (nullptr == m_pVIInstancingBufferCom)
		return E_FAIL;

	return dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_ShapePosition();
}

HRESULT CEffect_Point_Instancing::Set_Trail(CEffect_Base * pEffect, const _tchar * pProtoTag)
{
	CEffect_Base*   pEffectTrail = nullptr;
	CGameInstance*   pGameInstance = GET_INSTANCE(CGameInstance);

	if (pEffect->Get_HaveTrail() == true)
	{
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}

	_int   iVIBufferICnt = m_pVIInstancingBufferCom->Get_PointDesc()->iCreateInstance;

	wstring		strGameObjectTag = L"Prototype_GameObject_";
	size_t TagLength = strGameObjectTag.length();

	for (_int i = 0; i < iVIBufferICnt; ++i)
	{
		_matrix  matInitWorld = XMMatrixIdentity();
		wstring  strTrailProtoTag = pProtoTag;
		strTrailProtoTag += L"Trail";
		strTrailProtoTag += to_wstring(i);

		_tchar*     szTrailProtoTag = CUtile::Create_String(strTrailProtoTag.c_str());
		pGameInstance->Add_String(szTrailProtoTag);
		size_t ProtoLength = strTrailProtoTag.length();

		wstring     strTrailCloneTag = strTrailProtoTag.substr(TagLength, ProtoLength - TagLength);
		_tchar*     szTrailCloneTag = CUtile::Create_String(strTrailCloneTag.c_str());
		pGameInstance->Add_String(szTrailCloneTag);

		if (FAILED(pGameInstance->Add_Prototype(szTrailProtoTag, CEffect_Trail_T::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		_int iCurLevel = pGameInstance->Get_CurLevelIndex();
		pEffectTrail = dynamic_cast<CEffect_Trail*>(pGameInstance->Clone_GameObject(szTrailProtoTag, szTrailCloneTag));
		if (pEffectTrail == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return S_OK;
		}
		matInitWorld.r[3] = m_pVIInstancingBufferCom->Get_InstanceData_Idx(i).fPos;
		dynamic_cast<CEffect_Trail*>(pEffectTrail)->Set_InitMatrix(matInitWorld);
		dynamic_cast<CEffect_Trail*>(pEffectTrail)->Set_ParticleIdx(i);
		m_vecTrailEffect.push_back(dynamic_cast<CEffect_Trail*>(pEffectTrail));
	}

	for (auto& pTrailEffect : m_vecTrailEffect)
	{
		pTrailEffect->Set_Parent(this);
		pTrailEffect->Set_HaveTrail(true);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CEffect_Trail * CEffect_Point_Instancing::Get_Trail()
{
	if (m_pEffectTrail == nullptr)
		return nullptr;

	return dynamic_cast<CEffect_Trail*>(m_pEffectTrail);
}

void CEffect_Point_Instancing::Delete_Trail(const _tchar * pProtoTag)
{
	for (auto& iter = m_vecTrailEffect.begin(); iter != m_vecTrailEffect.end();)
	{
		Safe_Release(*iter);
		iter = m_vecTrailEffect.erase(iter);
	}
	m_vecTrailEffect.clear();
}

CVIBuffer_Point_Instancing::POINTDESC* CEffect_Point_Instancing::Get_PointInstanceDesc()
{
	if (nullptr == m_pVIInstancingBufferCom)
		return nullptr;

	return dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Get_PointDesc();
}

void CEffect_Point_Instancing::Set_PointInstanceDesc(CVIBuffer_Point_Instancing::POINTDESC* eEffectDesc)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return;

	dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_PointDesc(eEffectDesc);
}

CVIBuffer_Point_Instancing::INSTANCEDATA* CEffect_Point_Instancing::Get_InstanceData()
{
	if (nullptr == m_pVIInstancingBufferCom)
		return nullptr;

	return dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Get_InstanceData();
}

CVIBuffer_Point_Instancing::INSTANCEDATA CEffect_Point_Instancing::Get_InstanceData_Idx(_int iIndex)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return CVIBuffer_Point_Instancing::INSTANCEDATA();

	return dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Get_InstanceData_Idx(iIndex);
}

void CEffect_Point_Instancing::Set_InstanceData(CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return;

	dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_InstanceData(eInstanceData);
}

void CEffect_Point_Instancing::Set_RandomSpeeds(_float fmin, _float fmax)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return;

	return dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_RandomSpeeds(fmin, fmax);
}

HRESULT CEffect_Point_Instancing::Set_Pos(_float3 fMin, _float3 fMax)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return E_FAIL;

	return dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_Pos(fMin, fMax);
}

void CEffect_Point_Instancing::Set_PSize(_float2 PSize)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return;

	dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_PSize(PSize);
}

void CEffect_Point_Instancing::Set_RandomPSize(_float2 PSizeMinMax)
{
	if (nullptr == m_pVIInstancingBufferCom)
		return;

	dynamic_cast<CVIBuffer_Point_Instancing*>(m_pVIInstancingBufferCom)->Set_RandomPSize(PSizeMinMax);
}

HRESULT CEffect_Point_Instancing::Initialize_Prototype(_tchar* pProtoTag, const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (pProtoTag != nullptr)
		Set_VIBufferProtoTag(pProtoTag);

	return S_OK;
}

HRESULT CEffect_Point_Instancing::Initialize(void * pArg)
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

	m_eEFfectDesc.eEffectType = CEffect_Base::tagEffectDesc::EFFECT_PARTICLE;
	this->Set_ShapePosition();
	return S_OK;
}

void CEffect_Point_Instancing::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pVIInstancingBufferCom->Tick(fTimeDelta);

	if (m_eEFfectDesc.bStart == true)
	{
		m_eEFfectDesc.fPlayBbackTime += fTimeDelta;
		m_pVIInstancingBufferCom->Get_PointDesc()->fTimeDelta = fTimeDelta;

		if (m_eEFfectDesc.bStart == false)
			m_eEFfectDesc.fPlayBbackTime = 0.0f;
	}

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

				m_eEFfectDesc.fWidthFrame = m_fInitSpriteCnt.x;

				if (m_eEFfectDesc.fHeightFrame >= m_eEFfectDesc.iHeightCnt)
					m_eEFfectDesc.fHeightFrame = m_fInitSpriteCnt.y;
			}
		}
	}
	
	if (m_eEFfectDesc.fAngle != 0.0f)
	{
		_float4 vLook = XMVector3Normalize(m_eEFfectDesc.vPixedDir) * m_eEFfectDesc.fCreateRange;

		if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_X)
			vLook = XMVector3TransformNormal(vLook, XMMatrixRotationZ(XMConvertToRadians(m_eEFfectDesc.fAngle)));
		if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_Y)
			vLook = XMVector3TransformNormal(vLook, XMMatrixRotationZ(XMConvertToRadians(m_eEFfectDesc.fAngle)));
		if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_Z)
			vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(m_eEFfectDesc.fAngle)));
	}

	if (m_vecTrailEffect.size() != 0)
	{
		_matrix WorldMatrix = XMMatrixIdentity();
		auto &iter = m_vecTrailEffect.begin();
		for (_int i = 0; i < m_vecTrailEffect.size(); i++, iter++)
		{
			WorldMatrix.r[3] = m_pVIInstancingBufferCom->Get_InstanceData_Idx(i).fPos;
			(*iter)->Tick(fTimeDelta);
			(*iter)->Set_WorldMatrix(WorldMatrix* m_pTransformCom->Get_WorldMatrix());
		}
	}
}

void CEffect_Point_Instancing::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_vecTrailEffect.size() != 0)
	{
		for (auto& pTrailEffect : m_vecTrailEffect)
			pTrailEffect->Late_Tick(fTimeDelta);
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Point_Instancing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_eEFfectDesc.iPassCnt);
	m_pVIInstancingBufferCom->Render();
	return S_OK;
}

HRESULT CEffect_Point_Instancing::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectPointInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer => 버퍼마다 갯수가 다르기 때문에 이름지정함 ~~ */ 
	if (FAILED(__super::Add_Component(g_LEVEL, m_szVIBufferProtoTag, TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIInstancingBufferCom)))
		return E_FAIL;

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

		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[i], this)))
			return E_FAIL;
	}

	/* For.MaskTexture */
	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		_tchar szMTexture[64] = L"";
		wsprintf(szMTexture, L"Com_MTexture_%d", i);

		_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
		CGameInstance::GetInstance()->Add_String(szMTextureComTag);

		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[i], this)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Point_Instancing::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_fLife", &m_fLife, sizeof(_float))))
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

void CEffect_Point_Instancing::Free()
{
	__super::Free();

	for (auto& vecTrail : m_vecTrailEffect)
		Safe_Release(vecTrail);
	m_vecTrailEffect.clear();
}
