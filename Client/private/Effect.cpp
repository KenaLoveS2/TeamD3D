#include "stdafx.h"
#include "..\public\Effect.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Trail.h"

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

	XMStoreFloat4x4(&m_InitWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	m_eEFfectDesc.eEffectType = CEffect_Base::tagEffectDesc::EFFECT_PLANE;
	m_vPrePos = m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bStart == true)
		m_fFreePosTimeDelta += fTimeDelta;

	if (m_eEFfectDesc.IsMovingPosition == true)
	{
		m_eEFfectDesc.fPlayBbackTime += fTimeDelta;
		if (m_eEFfectDesc.bStart == true &&
			m_eEFfectDesc.fMoveDurationTime > m_eEFfectDesc.fPlayBbackTime)
		{
			_float4 vLook = XMVector3Normalize(m_eEFfectDesc.vPixedDir) * m_eEFfectDesc.fCreateRange;

			if (m_eEFfectDesc.fAngle != 0.0f)
			{
				if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_X)
					vLook = XMVector3TransformNormal(vLook, XMMatrixRotationZ(XMConvertToRadians(m_eEFfectDesc.fAngle)));
				if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_Y)
					vLook = XMVector3TransformNormal(vLook, XMMatrixRotationZ(XMConvertToRadians(m_eEFfectDesc.fAngle)));
				if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_Z)
					vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(m_eEFfectDesc.fAngle)));
			}

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			if (m_eEFfectDesc.bSpread == true)
				vPos += XMVector3Normalize(vLook) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec *  fTimeDelta;
			else
				vPos -= XMVector3Normalize(vLook) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec *  fTimeDelta;

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_eEFfectDesc.vInitPos);
			m_eEFfectDesc.fPlayBbackTime = 0.0f;
		}
	}

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

	// Child Tick
	if (m_vecChild.size() != 0)
	{
		for (auto& pChild : m_vecChild)
			pChild->Tick(fTimeDelta);
	}

	if (m_eEFfectDesc.bFreeMove == true)
	{
		static _int iCurIdx = 0;

		if (m_vecFreePos.empty() || m_vecFreePos.size() == 0)
			return;

		if(! CGameInstance::GetInstance()->Mouse_Pressing(DIM_LB))
		{
			auto& iter = m_vecFreePos.begin();
			if (iCurIdx >= m_vecFreePos.size())
				iCurIdx = 0;

			for (_int i = 0; i < iCurIdx; ++i)
				iter++;

			_bool bNextTime = Play_FreePos(*iter);
			if (bNextTime)
			{
				m_bLerp = false;
				iCurIdx++;
			}
		}
	}

	if (nullptr != m_pEffectTrail)
		dynamic_cast<CEffect_Trail*>(m_pEffectTrail)->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	__super::Compute_CamDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	// Child Late_Tick
	if (m_vecChild.size() != 0)
	{
		for (auto& pChild : m_vecChild)
			pChild->Late_Tick(fTimeDelta);
	}

	if (nullptr != m_pParent)
		Set_Matrix();
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

HRESULT CEffect::Set_Child(EFFECTDESC eEffectDesc, _int iCreateCnt, char * ProtoTag)
{
	CEffect_Base*    pEffectBase = nullptr;
	CGameInstance*   pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar      szChildProto[128];
	CUtile::CharToWideChar(ProtoTag, szChildProto);

	for (_int i = 0; i < iCreateCnt; ++i)
	{
		// ProtoTag
		_tchar* szChildProtoTag = CUtile::Create_String(szChildProto);
		pGameInstance->Add_String(szChildProtoTag);

		// CloneTag
		wstring		strGameObjectTag = L"Prototype_GameObject_";
		size_t		TagLength = strGameObjectTag.length();

		wstring		strProtoTag = szChildProtoTag;
		size_t      ProtoLength = strProtoTag.length();

		wstring     strChildCloneTag = strProtoTag.substr(TagLength, ProtoLength - TagLength);
		strChildCloneTag += '_';
		strChildCloneTag += to_wstring(m_iHaveChildCnt);

		_tchar* szChildClondTag = CUtile::Create_String(strChildCloneTag.c_str());
		pGameInstance->Add_String(szChildClondTag);

		pEffectBase = dynamic_cast<CEffect*>(pGameInstance->Clone_GameObject(szChildProtoTag, szChildClondTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);

		eEffectDesc.bUseChild = false;
		pEffectBase->Set_EffectDesc(eEffectDesc);
		pEffectBase->Set_Parent(this);
		pEffectBase->Set_InitMatrix(m_pTransformCom->Get_WorldMatrix());

		m_vecChild.push_back(pEffectBase);
		m_iHaveChildCnt++;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect::Edit_Child(const _tchar * ProtoTag)
{
	if (m_vecChild.size() == 0)
		return S_OK;

	for (auto& iter = m_vecChild.begin(); iter != m_vecChild.end();)
	{
		if (!lstrcmp((*iter)->Get_ObjectCloneName(), ProtoTag))
		{
			Safe_Release(*iter);
			m_vecChild.erase(iter);
			break;
		}
		else
			iter++;
	}

	return S_OK;
}

HRESULT CEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxEffectTex"), TEXT("Com_Shader"),
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
		_tchar szDTexture[64] = L"";
		wsprintf(szDTexture, L"Com_DTexture_%d", i);

		_tchar* szDTextureComTag = CUtile::Create_String(szDTexture);
		CGameInstance::GetInstance()->Add_String(szDTextureComTag);

		if (FAILED(__super::Add_Component(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[i], this)))
			return E_FAIL;
	}

	/* For.MaskTexture */
	for (_uint i = 0; i < m_iTotalMTextureComCnt; ++i)
	{
		_tchar szMTexture[64] = L"";
		wsprintf(szMTexture, L"Com_MTexture_%d", i);

		_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
		CGameInstance::GetInstance()->Add_String(szMTextureComTag);

		if (FAILED(__super::Add_Component(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[i], this)))
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

	// MaxCnt == 10

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

HRESULT CEffect::Set_Trail(CEffect_Base* pEffect, const _tchar* pProtoTag)
{
	CEffect_Base*   pEffectTrail = nullptr;
	CGameInstance*   pGameInstance = GET_INSTANCE(CGameInstance);

	if (pEffect->Get_HaveTrail() == true)
	{
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}

	// ProtoTag = Prototype_GameObject_OwnerTrail
	// CloneTag = OwnerTrail
	wstring		strGameObjectTag = L"Prototype_GameObject_";
	size_t TagLength = strGameObjectTag.length();

	wstring     strTrailProtoTag = pProtoTag;
	strTrailProtoTag += L"Trail";

	_tchar*     szTrailProtoTag = CUtile::Create_String(strTrailProtoTag.c_str());
	pGameInstance->Add_String(szTrailProtoTag);
	size_t ProtoLength = strTrailProtoTag.length();

	wstring     strTrailCloneTag = strTrailProtoTag.substr(TagLength, ProtoLength - TagLength);
	_tchar*     szTrailCloneTag = CUtile::Create_String(strTrailCloneTag.c_str());
	pGameInstance->Add_String(szTrailCloneTag);

	if (FAILED(pGameInstance->Add_Prototype(szTrailProtoTag, CEffect_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_int iCurLevel = pGameInstance->Get_CurLevelIndex();
	if (FAILED(pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Trail", szTrailProtoTag, szTrailCloneTag)))
		return E_FAIL;
	m_pEffectTrail = dynamic_cast<CEffect_Trail*>(pGameInstance->Get_GameObjectPtr(iCurLevel, L"Layer_Trail", szTrailCloneTag));
	if (m_pEffectTrail == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}
	m_pEffectTrail->Set_Parent(this);
	m_pEffectTrail->Set_HaveTrail(true);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CEffect_Trail * CEffect::Get_Trail()
{
	if (m_pEffectTrail == nullptr)
		return nullptr;

	return dynamic_cast<CEffect_Trail*>(m_pEffectTrail);
}

void CEffect::Delete_Trail(const _tchar* pProtoTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_int iCurLevel = pGameInstance->Get_CurLevelIndex();

	wstring		strGameObjectTag = L"Prototype_GameObject_";
	size_t TagLength = strGameObjectTag.length();

	wstring     strTrailProtoTag = pProtoTag;
	strTrailProtoTag += L"Trail";

	_tchar*     szTrailProtoTag = CUtile::Create_String(strTrailProtoTag.c_str());
	pGameInstance->Add_String(szTrailProtoTag);
	size_t ProtoLength = strTrailProtoTag.length();

	wstring     strTrailCloneTag = strTrailProtoTag.substr(TagLength, ProtoLength - TagLength);
	_tchar*     szTrailCloneTag = CUtile::Create_String(strTrailCloneTag.c_str());
	pGameInstance->Add_String(szTrailCloneTag);

	pGameInstance->Delete_Object(iCurLevel, L"Layer_Trail", szTrailCloneTag);

	m_pEffectTrail = nullptr;
	Set_HaveTrail(false);
	RELEASE_INSTANCE(CGameInstance);
}

void CEffect::Set_FreePos()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Pressing(DIK_LCONTROL) && pGameInstance->Mouse_Pressing(DIM_LB))
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		if (m_vecFreePos.empty() || m_vecFreePos.back() != vPosition)
			m_vecFreePos.push_back(vPosition);
	}
	RELEASE_INSTANCE(CGameInstance);
}

_bool CEffect::Play_FreePos(_float4& vPos)
{
	if (m_vecFreePos.empty() || m_vecFreePos.size() == 0)
		return false;

	m_vPrePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_vCurPos = vPos;

	_float4 vPositon = XMVectorLerp(m_vPrePos, m_vCurPos, m_fLerp);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPositon);
	m_fLerp += 0.9f;

	if (m_fLerp >= 1.0f)
	{
		m_bLerp = true;
		m_fLerp = 0.0f;
	}
	return m_bLerp;
}

vector<_float4>* CEffect::Get_FreePos()
{
	return &m_vecFreePos;
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
