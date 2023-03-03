#include "stdafx.h"
#include "..\public\Effect_Base.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Trail.h"

CEffect_Base::CEffect_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_eEFfectDesc, sizeof(EFFECTDESC));
}

CEffect_Base::CEffect_Base(const CEffect_Base & rhs)
	: CGameObject(rhs)
{
}

void CEffect_Base::Set_Matrix()
{
	_matrix  matParent = m_pParent->Get_TransformCom()->Get_WorldMatrix();
	_matrix  matScaleSet = XMMatrixIdentity();
	_vector vRight, vUp, vLook;

	memcpy(&vRight, &matParent.r[0], sizeof(_vector));
	memcpy(&vUp, &matParent.r[1], sizeof(_vector));
	memcpy(&vLook, &matParent.r[2], sizeof(_vector));

	memcpy(&matScaleSet.r[0], &XMVector3Normalize(vRight), sizeof(_vector));
	memcpy(&matScaleSet.r[1], &XMVector3Normalize(vUp), sizeof(_vector));
	memcpy(&matScaleSet.r[2], &XMVector3Normalize(vLook), sizeof(_vector));
	memcpy(&matScaleSet.r[3], &matParent.r[3], sizeof(_vector));

	m_WorldWithParentMatrix = m_InitWorldMatrix * matScaleSet;
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldWithParentMatrix));
}

void CEffect_Base::Set_TrailDesc()
{
	if (m_pParent == nullptr)
		return;

	EFFECTDESC effectDesc = m_pParent->Get_EffectDesc();

	m_eEFfectDesc.bActive = effectDesc.bActive;
	m_eEFfectDesc.bAlpha = effectDesc.bAlpha;
	m_eEFfectDesc.fLife = effectDesc.fLife;
	m_eEFfectDesc.fWidth = effectDesc.fWidth;
	m_eEFfectDesc.fSegmentSize = effectDesc.fSegmentSize;
	m_eEFfectDesc.fAlpha = effectDesc.fAlpha;
}

HRESULT CEffect_Base::Set_InitTrail(const _tchar * pPrototypeTag, _int iCnt)
{
	CEffect_Base*   pEffectTrail = nullptr;
	CGameInstance*   pGameInstance = GET_INSTANCE(CGameInstance);

	if (this->Get_HaveTrail() == false)
	{
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}

	wstring		strGameObjectTag = L"Prototype_GameObject_";
	size_t TagLength = strGameObjectTag.length();

	wstring     strTrailProtoTag = pPrototypeTag;
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

void CEffect_Base::BillBoardSetting(_float3 vScale)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera* pCamera = pGameInstance->Find_Camera(L"DEBUG_CAM_1");
	CTransform* pTargetTransform = dynamic_cast<CGameObject*>(pCamera)->Get_TransformCom();
	RELEASE_INSTANCE(CGameInstance);

	_float3 cameraPosition, cameraUp, cameraForward;
	_float4 rotateAxis, objectForward;

	XMStoreFloat3(&cameraPosition, pTargetTransform->Get_State(CTransform::STATE_TRANSLATION));
	XMStoreFloat3(&cameraUp, pTargetTransform->Get_State(CTransform::STATE_UP));
	XMStoreFloat3(&cameraForward, pTargetTransform->Get_State(CTransform::STATE_LOOK));

	_matrix worldmatrix = _smatrix::CreateBillboard(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), cameraPosition, cameraUp, &cameraForward);
	m_pTransformCom->Set_WorldMatrix(worldmatrix);
	m_pTransformCom->Set_Scaled(vScale);
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

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (pArg != nullptr)
		memcpy(&m_eEFfectDesc, pArg, sizeof(CEffect_Base::EFFECTDESC));

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

HRESULT CEffect_Base::Set_InitChild(_int iCreateCnt, char * ProtoTag)
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
		const _tchar* szClone = strChildCloneTag.c_str();
		strChildCloneTag += '_';
		strChildCloneTag += to_wstring(m_iHaveChildCnt);

		_tchar* szChildClondTag = CUtile::Create_String(strChildCloneTag.c_str());
		pGameInstance->Add_String(szChildClondTag);

		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(szChildProtoTag, szChildClondTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		
		CGameObject* pGameObject = pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(), L"Layer_Effect", szClone);
		CEffect_Base::EFFECTDESC EffectDesc = dynamic_cast<CEffect_Base*>(pGameObject)->Get_EffectDesc();

		pEffectBase->Set_EffectDesc(EffectDesc);
		pEffectBase->Set_Parent(this);
		pEffectBase->Set_InitMatrix(m_pTransformCom->Get_WorldMatrix());

		m_vecChild.push_back(pEffectBase);
		m_iHaveChildCnt++;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Base::Edit_TextureComponent(_uint iDTextureComCnt, _uint iMTextureComCnt)
{
	CTexture*	pTextureCom = nullptr;

	/* For.DiffuseTexture */
	if (iDTextureComCnt != 0)
	{
		if (m_iTotalDTextureComCnt == iDTextureComCnt)
			return S_OK;

		if (m_iTotalDTextureComCnt > iDTextureComCnt)
		{
			_int iDeleteComponentCnt = m_iTotalDTextureComCnt - iDTextureComCnt;
			for (_int i = 0; i < iDeleteComponentCnt; ++i)
			{
				m_iTotalDTextureComCnt--;

				_tchar szDTexture[64] = L"";
				wsprintf(szDTexture, L"Com_DTexture_%d", m_iTotalDTextureComCnt);

				_tchar* szDTextureComTag = CUtile::Create_String(szDTexture);
				CGameInstance::GetInstance()->Add_String(szDTextureComTag);

				Delete_Component(szDTextureComTag);
				Safe_Release(m_pDTextureCom[m_iTotalDTextureComCnt]);
			}
		}
		else if (m_iTotalDTextureComCnt < iDTextureComCnt)
		{
			for (_uint i = m_iTotalDTextureComCnt; i < iDTextureComCnt; ++i)
			{
				m_iTotalDTextureComCnt++;

				_tchar szDTexture[64] = L"";
				wsprintf(szDTexture, L"Com_DTexture_%d", i);

				_tchar* szDTextureComTag = CUtile::Create_String(szDTexture);
				CGameInstance::GetInstance()->Add_String(szDTextureComTag);

				if (FAILED(__super::Add_Component(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[i], this)))
					return E_FAIL;
			}
		}
	}

	/* For.Mask Texture */
	if (iMTextureComCnt != 0)
	{
		if (m_iTotalMTextureComCnt == iMTextureComCnt)
			return S_OK;

		if (m_iTotalMTextureComCnt > iMTextureComCnt)
		{
			_int iDeleteComponentCnt = m_iTotalMTextureComCnt - iMTextureComCnt;
			for (_int i = 0; i < iDeleteComponentCnt; ++i)
			{
				m_iTotalMTextureComCnt--;

				_tchar szMTexture[64] = L"";
				wsprintf(szMTexture, L"Com_MTexture_%d", m_iTotalMTextureComCnt);

				_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
				CGameInstance::GetInstance()->Add_String(szMTextureComTag);

				Delete_Component(szMTextureComTag);
				Safe_Release(m_pMTextureCom[m_iTotalMTextureComCnt]);
			}
		}
		else if (m_iTotalMTextureComCnt < iMTextureComCnt)
		{
			for (_uint i = m_iTotalMTextureComCnt; i < iMTextureComCnt; ++i)
			{
				m_iTotalDTextureComCnt++;

				_tchar szMTexture[64] = L"";
				wsprintf(szMTexture, L"Com_MTexture_%d", i);

				_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
				CGameInstance::GetInstance()->Add_String(szMTextureComTag);
				if (FAILED(__super::Add_Component(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[i], this)))
					return E_FAIL;
			}
		}
	}
	return S_OK;
}

void CEffect_Base::Free()
{
	__super::Free();

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

	if (nullptr != m_pNTextureCom)
		Safe_Release(m_pNTextureCom);

	// VIBuffer Release
	if (nullptr != m_pVIBufferCom)
		Safe_Release(m_pVIBufferCom);

	if (nullptr != m_pVIInstancingBufferCom)
		Safe_Release(m_pVIInstancingBufferCom);

	if (nullptr != m_pVITrailBufferCom)
		Safe_Release(m_pVITrailBufferCom);

	// Mesh Release
	if (nullptr != m_pModelCom)
		Safe_Release(m_pModelCom);

	for (auto& pChild : m_vecChild)
		Safe_Release(pChild);
	m_vecChild.clear();
}
