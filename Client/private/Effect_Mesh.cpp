#include "stdafx.h"
#include "..\public\Effect_Mesh.h"
#include "GameInstance.h"
#include "Effect_Trail.h"
#include "Effect_Trail_T.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh & rhs)
	: CEffect_Base(rhs)
{
}

HRESULT CEffect_Mesh::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	if (pArg == nullptr)
	{
		ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.eEffectType = CEffect_Base::tagEffectDesc::EFFECT_MESH;
	m_eEFfectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_ONE;

	

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_vPrePos = m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	return S_OK;
}

void CEffect_Mesh::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#pragma region nouse

	//if (m_eEFfectDesc.bStart == true)
	//	m_fFreePosTimeDelta += fTimeDelta;

	//if (m_eEFfectDesc.IsMovingPosition == true)
	//{
	//	m_eEFfectDesc.fPlayBbackTime += fTimeDelta;
	//	if (m_eEFfectDesc.bStart == true &&
	//		m_eEFfectDesc.fMoveDurationTime > m_eEFfectDesc.fPlayBbackTime)
	//	{
	//		_float4 vLook = XMVector3Normalize(m_eEFfectDesc.vPixedDir) * m_eEFfectDesc.fCreateRange;

	//		if (m_eEFfectDesc.fAngle != 0.0f)
	//		{
	//			if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_X)
	//				vLook = XMVector3TransformNormal(vLook, XMMatrixRotationZ(XMConvertToRadians(m_eEFfectDesc.fAngle)));
	//			if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_Y)
	//				vLook = XMVector3TransformNormal(vLook, XMMatrixRotationZ(XMConvertToRadians(m_eEFfectDesc.fAngle)));
	//			if (m_eEFfectDesc.eRotation == CEffect_Base::tagEffectDesc::ROT_Z)
	//				vLook = XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(m_eEFfectDesc.fAngle)));
	//		}

	//		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//		if (m_eEFfectDesc.bSpread == true)
	//			vPos += XMVector3Normalize(vLook) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec *  fTimeDelta;
	//		else
	//			vPos -= XMVector3Normalize(vLook) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec *  fTimeDelta;

	//		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	//	}
	//	else
	//	{
	//		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_eEFfectDesc.vInitPos);
	//		m_eEFfectDesc.fPlayBbackTime = 0.0f;
	//	}
	//}

	//// FreeMoveing Tick
	//if (m_eEFfectDesc.bFreeMove == true)
	//{
	//	static _int iCurIdx = 0;

	//	if (m_vecFreePos.empty() || m_vecFreePos.size() == 0)
	//		return;

	//	if (!CGameInstance::GetInstance()->Mouse_Pressing(DIM_LB))
	//	{
	//		auto& iter = m_vecFreePos.begin();
	//		if (iCurIdx >= m_vecFreePos.size())
	//			iCurIdx = 0;

	//		for (_int i = 0; i < iCurIdx; ++i)
	//			iter++;

	//		_bool bNextTime = Play_FreePos(*iter);
	//		if (bNextTime)
	//		{
	//			m_bLerp = false;
	//			iCurIdx++;
	//		}
	//	}
	//}
#pragma endregion nouse

}

void CEffect_Mesh::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}


HRESULT CEffect_Mesh::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Set_Child(EFFECTDESC eEffectDesc, _int iCreateCnt, char * ProtoTag)
{
	CEffect_Base*    pEffectBase = nullptr;
	CGameInstance*   pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar      szChildProto[128];
	CUtile::CharToWideChar(ProtoTag, szChildProto);

	for (_int i = 0; i < iCreateCnt; ++i)
	{
		// ProtoTag
		//wstring strChildProtoTag = szChildProto;
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

		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(szChildProtoTag, szChildClondTag));
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

HRESULT CEffect_Mesh::Edit_Child(const _tchar * ProtoTag)
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

HRESULT CEffect_Mesh::Set_Trail(CEffect_Base * pEffect, const _tchar * pProtoTag)
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

	if (FAILED(pGameInstance->Add_Prototype(szTrailProtoTag, CEffect_Trail_T::Create(m_pDevice, m_pContext))))
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

CEffect_Trail * CEffect_Mesh::Get_Trail()
{
	if (m_pEffectTrail == nullptr)
		return nullptr;

	return dynamic_cast<CEffect_Trail*>(m_pEffectTrail);
}

void CEffect_Mesh::Delete_Trail(const _tchar * pProtoTag)
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

#pragma region FreePos Setting
void CEffect_Mesh::Set_FreePos()
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

_bool CEffect_Mesh::Play_FreePos(_float4 & vPos)
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

vector<_float4>* CEffect_Mesh::Get_FreePos()
{
	return &m_vecFreePos;
}
HRESULT CEffect_Mesh::Set_ModelCom(EFFECTDESC::MESHTYPE eMeshType)
{
	/* For.Com_Model */
	switch (eMeshType)
	{
	case Client::CEffect_Base::tagEffectDesc::MESH_PLANE:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Plane"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case Client::CEffect_Base::tagEffectDesc::MESH_CUBE:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cube"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case Client::CEffect_Base::tagEffectDesc::MESH_CONE:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Cone"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case Client::CEffect_Base::tagEffectDesc::MESH_SPHERE:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case Client::CEffect_Base::tagEffectDesc::MESH_CYLINDER:
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_shockball"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	return S_OK;
}
#pragma endregion FreePos Setting

HRESULT CEffect_Mesh::SetUp_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	/***********
	*  TEXTURE *
	************/
	/* For.DiffuseTexture */
	for (_uint i = 0; i < m_iTotalDTextureComCnt; i++)
	{
		_tchar szDTexture[64] = L"";
		wsprintf(szDTexture, L"Com_DTexture_%d", i);

		_tchar* szDTextureComTag = CUtile::Create_String(szDTexture);
		CGameInstance::GetInstance()->Add_String(szDTextureComTag);

		FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[i]), E_FAIL);
	}

	/* For.MaskTexture */
	for (_uint i = 0; i < m_iTotalMTextureComCnt; i++)
	{
		_tchar szMTexture[64] = L"";
		wsprintf(szMTexture, L"Com_MTexture_%d", i);

		_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
		CGameInstance::GetInstance()->Add_String(szMTextureComTag);

		FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[i]), E_FAIL);
	}

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_NormalEffect"), TEXT("Com_NTexture"), (CComponent**)&m_pNTextureCom), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Mesh::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV()), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_WorldCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	/* Texture Total Cnt */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iTotalDTextureComCnt", &m_iTotalDTextureComCnt, sizeof _uint), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iTotalMTextureComCnt", &m_iTotalMTextureComCnt, sizeof _uint), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_TextureRenderType", &m_eEFfectDesc.eTextureRenderType, sizeof _uint), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_IsUseMask", &m_eEFfectDesc.IsMask, sizeof _bool), E_FAIL);

	/* Normal Texture */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_IsUseNormal", &m_eEFfectDesc.IsNormal, sizeof _bool), E_FAIL);

	if (m_eEFfectDesc.IsNormal == true)
	{
		FAILED_CHECK_RETURN(m_pNTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)m_eEFfectDesc.fNormalFrame), E_FAIL);
	}

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_BlendType", &m_eEFfectDesc.eBlendType, sizeof(_int)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vColor", &m_eEFfectDesc.vColor, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float)), E_FAIL);

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

void CEffect_Mesh::Free()
{
	__super::Free();
}
