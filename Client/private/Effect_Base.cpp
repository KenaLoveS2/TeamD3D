#include "stdafx.h"
#include "..\public\Effect_Base.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Trail.h"
#include "Effect_Point_Instancing.h"

// Json
#include "Json/json.hpp"
#include <fstream>
// ~Json

CEffect_Base::CEffect_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_eEFfectDesc, sizeof(EFFECTDESC));
}

CEffect_Base::CEffect_Base(const CEffect_Base & rhs)
	: CGameObject(rhs)
	, m_iTotalDTextureComCnt(rhs.m_iTotalDTextureComCnt)
	, m_iTotalMTextureComCnt(rhs.m_iTotalMTextureComCnt)
{
	memcpy(&m_eEFfectDesc, &rhs.m_eEFfectDesc, sizeof(EFFECTDESC));
}

HRESULT CEffect_Base::Load_E_Desc(const _tchar * pFilePath)
{
#pragma region LoadData
	Json	jLayer;
	Json	jComponent;
	Json	jPrototypeObjects;
	Json	jCloneObjects;
	Json	jCloneComponentDesc;

	ifstream	file(pFilePath);
	file >> jLayer;
	file.close();

	_int iCurLevel = 0, iSave2LoadLevel = 0;
	string strLayerTag = "";
	jLayer["0.Save Level"].get_to<int>(iCurLevel);
	jLayer["1.Save2Load Level"].get_to<int>(iSave2LoadLevel);
	jLayer["2.Layer Tag"].get_to<string>(strLayerTag);

	// Clone GameObject
	_int iEnum = 0, iEffectType = 0;
	CEffect_Base::EFFECTDESC eLoadEffectDesc;
	for (auto jCloneObjects : jLayer["3.Clone GameObject"])
	{

#pragma  region	EFFECTDESC

		jCloneObjects["2.Effect Type"].get_to<_int>(iEffectType);
		eLoadEffectDesc.eEffectType = (CEffect_Base::EFFECTDESC::EFFECTTYPE)iEffectType;

		iEnum = 0;
		jCloneObjects["Mesh Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eMeshType = (CEffect_Base::EFFECTDESC::MESHTYPE)iEnum;

		jCloneObjects["Particle Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eParticleType = (CEffect_Base::EFFECTDESC::PARTICLETYPE)iEnum;

		jCloneObjects["TextureRender Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eTextureRenderType = (CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE)iEnum;

		jCloneObjects["Texture Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eTextureType = (CEffect_Base::EFFECTDESC::TEXTURETYPE)iEnum;

		jCloneObjects["Blend Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eBlendType = (CEffect_Base::EFFECTDESC::BLENDSTATE)iEnum;

		jCloneObjects["MoveDir Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eMoveDir = (CEffect_Base::EFFECTDESC::MOVEDIR)iEnum;

		jCloneObjects["Rotation Type"].get_to<_int>(iEnum);
		eLoadEffectDesc.eRotation = (CEffect_Base::EFFECTDESC::ROTXYZ)iEnum;

		_int i = 0;
		for (_float fFrame : jCloneObjects["DiffuseTexture Index"])
			memcpy(((_float*)&eLoadEffectDesc.fFrame) + (i++), &fFrame, sizeof(_float));
		i = 0;
		for (_float fMaskFrame : jCloneObjects["MaskTexture Index"])
			memcpy(((_float*)&eLoadEffectDesc.fMaskFrame) + (i++), &fMaskFrame, sizeof(_float));
		i = 0;
		for (_float vColor : jCloneObjects["Color"])
			memcpy(((_float*)&eLoadEffectDesc.vColor) + (i++), &vColor, sizeof(_float));
		i = 0;
		for (_float vScale : jCloneObjects["vScale"])
			memcpy(((_float*)&eLoadEffectDesc.vScale) + (i++), &vScale, sizeof(_float));

		jCloneObjects["NormalTexture Index"].get_to<_float>(eLoadEffectDesc.fNormalFrame);
		jCloneObjects["Width Frame"].get_to<_float>(eLoadEffectDesc.fWidthFrame);
		jCloneObjects["Height Frame"].get_to<_float>(eLoadEffectDesc.fHeightFrame);

		jCloneObjects["SeparateWidth"].get_to<_int>(eLoadEffectDesc.iSeparateWidth);
		jCloneObjects["SeparateHeight"].get_to<_int>(eLoadEffectDesc.iSeparateHeight);
		jCloneObjects["WidthCnt"].get_to<_int>(eLoadEffectDesc.iWidthCnt);
		jCloneObjects["HeightCnt"].get_to<_int>(eLoadEffectDesc.iHeightCnt);

		jCloneObjects["CreateRange"].get_to<_float>(eLoadEffectDesc.fCreateRange);
		jCloneObjects["Range"].get_to<_float>(eLoadEffectDesc.fRange);
		jCloneObjects["Angle"].get_to<_float>(eLoadEffectDesc.fAngle);
		jCloneObjects["MoveDurationTime"].get_to<_float>(eLoadEffectDesc.fMoveDurationTime);
		jCloneObjects["Start"].get_to<_bool>(eLoadEffectDesc.bStart);

		i = 0;
		for (_float vInitPos : jCloneObjects["Init Pos"])
			memcpy(((_float*)&eLoadEffectDesc.vInitPos) + (i++), &vInitPos, sizeof(_float));
		i = 0;
		for (_float vPixedDir : jCloneObjects["Pixed Dir"])
			memcpy(((_float*)&eLoadEffectDesc.vPixedDir) + (i++), &vPixedDir, sizeof(_float));

		jCloneObjects["Have Trail"].get_to<_bool>(eLoadEffectDesc.IsTrail);
		if (eLoadEffectDesc.IsTrail == true)
		{
			jCloneObjects["Active"].get_to<_bool>(eLoadEffectDesc.bActive);
			jCloneObjects["Alpha Trail"].get_to<_bool>(eLoadEffectDesc.bAlpha);
			jCloneObjects["Life"].get_to<_float>(eLoadEffectDesc.fLife);
			jCloneObjects["Width"].get_to<_float>(eLoadEffectDesc.fWidth);
			jCloneObjects["SegmentSize"].get_to<_float>(eLoadEffectDesc.fSegmentSize);
		}

		jCloneObjects["Alpha"].get_to<_float>(eLoadEffectDesc.fAlpha);
		jCloneObjects["Billboard"].get_to<_bool>(eLoadEffectDesc.IsBillboard);
		jCloneObjects["Use Normal"].get_to<_bool>(eLoadEffectDesc.IsNormal);
		jCloneObjects["Use Mask"].get_to<_bool>(eLoadEffectDesc.IsMask);
		jCloneObjects["Trigger"].get_to<_bool>(eLoadEffectDesc.IsTrigger);
		jCloneObjects["MovingPosition"].get_to<_bool>(eLoadEffectDesc.IsMovingPosition);
		jCloneObjects["Use Child"].get_to<_bool>(eLoadEffectDesc.bUseChild);
		jCloneObjects["Spread"].get_to<_bool>(eLoadEffectDesc.bSpread);
		jCloneObjects["FreeMove"].get_to<_bool>(eLoadEffectDesc.bFreeMove);
#pragma  endregion	EFFECTDESC

		_int iDTextureCnt = 0, iMTextureCnt = 0;
		jCloneObjects["Have DTextureCnt"].get_to<_int>(iDTextureCnt);
		jCloneObjects["Have MTextureCnt"].get_to<_int>(iMTextureCnt);
		
		m_iTotalDTextureComCnt = iDTextureCnt;
		m_iTotalMTextureComCnt = iMTextureCnt;
		memcpy(&m_eEFfectDesc, &eLoadEffectDesc, sizeof(CEffect_Base::EFFECTDESC));

		// FreeMove
		_int j = 0;
		vector<_float4> FreePos;
		if (eLoadEffectDesc.bFreeMove == true)
		{
			_float4 fFreePos;
			for (_float fFree : jCloneObjects["99.FreePos"])
			{
				if (j % 4 == 0 && j != 0)
				{
					FreePos.push_back(fFreePos);
					j = 0;
				}
				memcpy((_float*)&fFreePos + (j++), &fFree, sizeof(_float4));
			}
			dynamic_cast<CEffect_Base*>(this)->Set_InitPos(FreePos);
			FreePos.clear();
		}

		// Trail
		string strTrailPrototypeTag = "", strTrailCloneTag = "";
		if (eLoadEffectDesc.IsTrail == true)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_int iParticleIndex = 0, iTrailEffectType = 0;
			jCloneObjects["Trail EffectType"].get_to<_int>(iTrailEffectType);
			if (iEffectType == 1) // VIBuffer_Point_Instancing Data Save
			{
				jCloneObjects["99.Trail ProtoTag"].get_to<string>(strTrailPrototypeTag);
				jCloneObjects["99.Trail CloneTag"].get_to<string>(strTrailCloneTag);
				jCloneObjects["99.Trail Index"].get_to<_int>(iParticleIndex);

				_tchar szConvertTag[128] = L"";
				CUtile::CharToWideChar(strTrailPrototypeTag.c_str(), szConvertTag);
				_tchar* szTrailPrototypeTag = CUtile::Create_String(szConvertTag);
				pGameInstance->Add_String(szTrailPrototypeTag);

				dynamic_cast<CEffect_Point_Instancing*>(this)->Set_Trail(dynamic_cast<CEffect_Point_Instancing*>(this), szTrailPrototypeTag);
			}
			else
			{
				jCloneObjects["99.Trail ProtoTag"].get_to<string>(strTrailPrototypeTag);
				jCloneObjects["99.Trail CloneTag"].get_to<string>(strTrailCloneTag);

				_tchar szConvertTag[128] = L"";
				CUtile::CharToWideChar(strTrailPrototypeTag.c_str(), szConvertTag);
				_tchar* szTrailPrototypeTag = CUtile::Create_String(szConvertTag);
				pGameInstance->Add_String(szTrailPrototypeTag);

				dynamic_cast<CEffect_Base*>(this)->Set_InitTrail(szTrailPrototypeTag, 1);
			}
			RELEASE_INSTANCE(CGameInstance);
		}

		_int iChildCnt = 0;
		jCloneObjects["99.Child Count"].get_to<_int>(iChildCnt);
		string strChildPrototypeTag = "", strChildCloneTag = "";
		// Child
		if (iChildCnt != 0)
		{
			for (_int j = 0; j < iChildCnt; j++)
			{
				string strPrototypeTag = "";
				string strCloneTag = "";

				jCloneObjects["99.Child ProtoTag"].get_to<string>(strPrototypeTag);
				jCloneObjects["99.Child CloneTag"].get_to<string>(strCloneTag);

				char* szProtoTag = CUtile::Create_String(strPrototypeTag.c_str());
				dynamic_cast<CEffect_Base*>(this)->Set_InitChild(iChildCnt, szProtoTag);
			}
		}

		// Component Desc ¿˙¿Â
		if (iEffectType == 1) // VIBuffer_Point_Instancing Data Save
		{

#pragma region Instance Data

			_int iInstanceDataCnt = 0, iInstanceDataIdx = 0;
			jCloneComponentDesc = jCloneObjects["CloneObject Component"];
			jCloneComponentDesc["0. Instance DataCnt"].get_to<_int>(iInstanceDataCnt);

			CVIBuffer_Point_Instancing::INSTANCEDATA* InstanceData = new CVIBuffer_Point_Instancing::INSTANCEDATA[iInstanceDataCnt];
			ZeroMemory(InstanceData, sizeof(CVIBuffer_Point_Instancing::INSTANCEDATA)*iInstanceDataCnt);

			_int k = 0;
			for (_float fPos : jCloneComponentDesc["CloneObject Component InstnaceData Position"])
				memcpy((_float*)&InstanceData->fPos + (k++), &fPos, sizeof(_float));

			jCloneComponentDesc["CloneObject Component InstnaceData Speed"].get_to<_double>(InstanceData->pSpeeds);

			_float2 SpeedMinMax = { 0.0f,0.0f };
			_float2 PSize = { 0.0f,0.0f };

			jCloneComponentDesc["CloneObject Component InstnaceData SpeedMinMin"].get_to<_float>(InstanceData->SpeedMinMax.x);
			jCloneComponentDesc["CloneObject Component InstnaceData SpeedMinMax"].get_to<_float>(InstanceData->SpeedMinMax.y);
			jCloneComponentDesc["CloneObject Component InstnaceData PSizeX"].get_to<_float>(InstanceData->fPSize.x);
			jCloneComponentDesc["CloneObject Component InstnaceData PSizeY"].get_to<_float>(InstanceData->fPSize.y);

#pragma endregion Instance Data

#pragma region Point Desc Data
			CVIBuffer_Point_Instancing::POINTDESC* PointDesc = new CVIBuffer_Point_Instancing::POINTDESC[iInstanceDataCnt];
			ZeroMemory(PointDesc, sizeof(CVIBuffer_Point_Instancing::POINTDESC)*iInstanceDataCnt);
			string strPointDataTag = "";

			jCloneComponentDesc["0. Point Data"].get_to<string>(strPointDataTag);

			k = 0;
			for (_float fMin : jCloneComponentDesc["CloneObject Component PointDesc Min"])
				memcpy((_float*)&PointDesc->fMin + (k++), &fMin, sizeof(_float));

			k = 0;
			for (_float fMax : jCloneComponentDesc["CloneObject Component PointDesc Max"])
				memcpy((_float*)&PointDesc->fMax + (k++), &fMax, sizeof(_float));

			k = 0;
			for (_float fCirclePos : jCloneComponentDesc["CloneObject Component PointDesc CirclePos"])
				memcpy((_float*)&PointDesc->vCirclePos + (k++), &fCirclePos, sizeof(_float));

			k = 0;
			for (_float fConeRange : jCloneComponentDesc["CloneObject Component PointDesc ConeRange"])
				memcpy((_float*)&PointDesc->fConeRange + (k++), &fConeRange, sizeof(_float));

			k = 0;
			for (_float vOriginPos : jCloneComponentDesc["CloneObject Component PointDesc OriginPos"])
				memcpy((_float*)&PointDesc->vOriginPos + (k++), &vOriginPos, sizeof(_float));

			k = 0;
			for (_float vExplosionDir : jCloneComponentDesc["CloneObject Component PointDesc ExplosionDir"])
				memcpy((_float*)&PointDesc->vExplosionDir + (k++), &vExplosionDir, sizeof(_float));

			k = 0;
			for (_float fDir : jCloneComponentDesc["CloneObject Component PointDesc ExplosionDir"])
				memcpy((_float*)&PointDesc->vDir + (k++), &fDir, sizeof(_float));

			iEnum = 0;
			jCloneComponentDesc["CloneObject Component PointDesc ShapeType"].get_to<_int>(iEnum);
			memcpy(&PointDesc->eShapeType, &iEnum, sizeof(_int));
			jCloneComponentDesc["CloneObject Component PointDesc RotXYZ"].get_to<_int>(iEnum);
			memcpy(&PointDesc->eRotXYZ, &iEnum, sizeof(_int));

			jCloneComponentDesc["CloneObject Component PointDesc Range"].get_to<_float>(PointDesc->fRange);
			jCloneComponentDesc["CloneObject Component PointDesc CreateInstance"].get_to<_int>(PointDesc->iCreateInstance);
			jCloneComponentDesc["CloneObject Component PointDesc InstanceIndex"].get_to<_int>(PointDesc->iInstanceIndex);
			jCloneComponentDesc["CloneObject Component PointDesc Spread"].get_to<_bool>(PointDesc->bSpread);
			jCloneComponentDesc["CloneObject Component PointDesc IsAlive"].get_to<_bool>(PointDesc->bIsAlive);
			jCloneComponentDesc["CloneObject Component PointDesc bMoveY"].get_to<_bool>(PointDesc->bMoveY);
			jCloneComponentDesc["CloneObject Component PointDesc Rotation"].get_to<_bool>(PointDesc->bRotation);
			jCloneComponentDesc["CloneObject Component PointDesc MoveY"].get_to<_float>(PointDesc->fMoveY);
			jCloneComponentDesc["CloneObject Component PointDesc CreateRange"].get_to<_float>(PointDesc->fCreateRange);
			jCloneComponentDesc["CloneObject Component PointDesc DurationTime"].get_to<_float>(PointDesc->fDurationTime);
			jCloneComponentDesc["CloneObject Component PointDesc MaxTime"].get_to<_float>(PointDesc->fMaxTime);
			jCloneComponentDesc["CloneObject Component PointDesc RangeOffset"].get_to<_float>(PointDesc->fRangeOffset);
			jCloneComponentDesc["CloneObject Component PointDesc RangeY"].get_to<_float>(PointDesc->fRangeY);

#pragma endregion Point Desc Data

			dynamic_cast<CEffect_Point_Instancing*>(this)->Set_PointInstanceDesc(PointDesc);
			dynamic_cast<CEffect_Point_Instancing*>(this)->Set_InstanceData(InstanceData);
			dynamic_cast<CEffect_Point_Instancing*>(this)->Set_ShapePosition();

			Safe_Delete_Array(PointDesc);
			Safe_Delete_Array(InstanceData);
		}
	}
#pragma endregion LoadData
	return S_OK;
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

HRESULT CEffect_Base::Initialize_Prototype(const _tchar* pFilePath)
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

	if (FAILED(__super::Initialize(&GameObjectDesc)))
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
		
		pEffectBase->Set_Parent(this);
	//	pEffectBase->Set_InitMatrix(m_pTransformCom->Get_WorldMatrix());

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
	if (nullptr != m_pShaderCom)
		Safe_Release(m_pShaderCom);

	if (nullptr != m_pRendererCom)
		Safe_Release(m_pRendererCom);

	// Texture
	if (nullptr != m_pDTextureCom)
	{
		// Diffuse Release
		for (auto& pTextureCom : m_pDTextureCom)
			Safe_Release(pTextureCom);
	}

	if (nullptr != m_pMTextureCom)
	{
		// Mask Release
		for (auto& pTextureCom : m_pMTextureCom)
			Safe_Release(pTextureCom);
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
