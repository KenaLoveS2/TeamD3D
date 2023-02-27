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

HRESULT CEffect_Base::Edit_TextureComponent(_uint iDTextureComCnt, _uint iMTextureComCnt)
{
	CTexture*	pTextureCom = nullptr;

	/* For.DiffuseTexture */
	if (iDTextureComCnt != 0)
	{
		if (m_iTotalDTextureComCnt == iDTextureComCnt)
			return S_OK;

		// 현재 내 컴포넌트 카운트보다 입력된 카운트가 적을때 차 만큼 컴포넌트 삭제 
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
		else if(m_iTotalDTextureComCnt < iDTextureComCnt)// 현재 내 컴포넌트 카운트 보다 입력된 카운트가 클때 => 컴포넌트 추가 
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

	// VIBuffer Release
	if (nullptr != m_pVIBufferCom)
		Safe_Release(m_pVIBufferCom);

	if (nullptr != m_pVIInstancingBufferCom)
		Safe_Release(m_pVIInstancingBufferCom);

	if (nullptr != m_pVITrailBufferCom)
		Safe_Release(m_pVITrailBufferCom);

	for (auto& pChild : m_vecChild)
		Safe_Release(pChild);
	m_vecChild.clear();
}
