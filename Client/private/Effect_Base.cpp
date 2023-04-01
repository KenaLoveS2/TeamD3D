#include "stdafx.h"
#include "..\public\Effect_Base.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Trail.h"
#include "Effect_Point_Instancing.h"

// Json
#include "Json/json.hpp"
#include <fstream>
#include "Effect.h"
#include "Effect_T.h"
#include "Effect_Mesh_T.h"
#include "Effect_Trail_T.h"
#include "Kena_Staff.h"
#include "Layer.h"
// ~Json

CEffect_Base::CEffect_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_eEFfectDesc, sizeof(EFFECTDESC));

	m_eEFfectDesc.vScale = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMStoreFloat4x4(&m_InitWorldMatrix, XMMatrixIdentity());
}

CEffect_Base::CEffect_Base(const CEffect_Base & rhs)
	: CGameObject(rhs)
	, m_iTotalDTextureComCnt(rhs.m_iTotalDTextureComCnt)
	, m_iTotalMTextureComCnt(rhs.m_iTotalMTextureComCnt)
	, m_fInitSpriteCnt(rhs.m_fInitSpriteCnt)
{

	//m_vecChild.assign(rhs.m_vecChild.begin(), rhs.m_vecChild.end());

	memcpy(&m_InitWorldMatrix, &rhs.m_InitWorldMatrix, sizeof(_float4x4));
 	memcpy(&m_eEFfectDesc, &rhs.m_eEFfectDesc, sizeof(EFFECTDESC));

	for (auto pChild : rhs.m_vecChild)
	{
		m_vecChild.push_back(dynamic_cast<CEffect_Base*>(pChild->Clone()));
		m_vecChild.back()->Set_CloneTag(pChild->Get_ObjectCloneName());
		m_vecChild.back()->Set_ProtoTag(pChild->Get_ProtoObjectName());
	}
}

HRESULT CEffect_Base::Load_E_Desc(const _tchar * pFilePath)
{
#pragma region LoadData
	Json	jLayer;

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
	
	Json	jData;
	Json	jDesc;
	Json	jObject;
	Json	jChildDesc;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//jData = jLayer["Pulse Test"];
	for (auto jData : jLayer["Object"])
	{
		for (auto jObject : jData["Object Data"])
		{
			for (auto jDesc : jObject["Desc"])
			{

#pragma  region	EFFECTDESC
				jDesc["Effect Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eEffectType = (CEffect_Base::EFFECTDESC::EFFECTTYPE)iEnum;

				iEnum = 0;
				jDesc["Mesh Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eMeshType = (CEffect_Base::EFFECTDESC::MESHTYPE)iEnum;

				jDesc["Particle Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eParticleType = (CEffect_Base::EFFECTDESC::PARTICLETYPE)iEnum;

				jDesc["TextureRender Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eTextureRenderType = (CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE)iEnum;

				jDesc["Texture Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eTextureType = (CEffect_Base::EFFECTDESC::TEXTURETYPE)iEnum;

				jDesc["Blend Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eBlendType = (CEffect_Base::EFFECTDESC::BLENDSTATE)iEnum;

				jDesc["MoveDir Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eMoveDir = (CEffect_Base::EFFECTDESC::MOVEDIR)iEnum;

				jDesc["Rotation Type"].get_to<_int>(iEnum);
				eLoadEffectDesc.eRotation = (CEffect_Base::EFFECTDESC::ROTXYZ)iEnum;

				iEnum = 0;
				jDesc["PassCnt"].get_to<_int>(iEnum);
				eLoadEffectDesc.iPassCnt = iEnum;

				_int i = 0;
				for (_float fFrame : jDesc["DiffuseTexture Index"])
					memcpy(((_float*)&eLoadEffectDesc.fFrame) + (i++), &fFrame, sizeof(_float));
				i = 0;
				for (_float fMaskFrame : jDesc["MaskTexture Index"])
					memcpy(((_float*)&eLoadEffectDesc.fMaskFrame) + (i++), &fMaskFrame, sizeof(_float));
				i = 0;
				for (_float vColor : jDesc["Color"])
					memcpy(((_float*)&eLoadEffectDesc.vColor) + (i++), &vColor, sizeof(_float));
				i = 0;
				for (_float vScale : jDesc["Scale"])
					memcpy(((_float*)&eLoadEffectDesc.vScale) + (i++), &vScale, sizeof(_float));

				jDesc["NormalTexture Index"].get_to<_float>(eLoadEffectDesc.fNormalFrame);
				jDesc["Width Frame"].get_to<_float>(eLoadEffectDesc.fWidthFrame);
				jDesc["Height Frame"].get_to<_float>(eLoadEffectDesc.fHeightFrame);

				jDesc["SeparateWidth"].get_to<_int>(eLoadEffectDesc.iSeparateWidth);
				jDesc["SeparateHeight"].get_to<_int>(eLoadEffectDesc.iSeparateHeight);
				jDesc["WidthCnt"].get_to<_int>(eLoadEffectDesc.iWidthCnt);
				jDesc["HeightCnt"].get_to<_int>(eLoadEffectDesc.iHeightCnt);
				jDesc["DurationTime"].get_to<_float>(eLoadEffectDesc.fTimeDelta);

				jDesc["CreateRange"].get_to<_float>(eLoadEffectDesc.fCreateRange);
				jDesc["Range"].get_to<_float>(eLoadEffectDesc.fRange);
				jDesc["Angle"].get_to<_float>(eLoadEffectDesc.fAngle);
				jDesc["MoveDurationTime"].get_to<_float>(eLoadEffectDesc.fMoveDurationTime);
				jDesc["Start"].get_to<_bool>(eLoadEffectDesc.bStart);

				i = 0;
				for (_float vInitPos : jDesc["Init Pos"])
					memcpy(((_float*)&eLoadEffectDesc.vInitPos) + (i++), &vInitPos, sizeof(_float));
				i = 0;
				for (_float vPixedDir : jDesc["Pixed Dir"])
					memcpy(((_float*)&eLoadEffectDesc.vPixedDir) + (i++), &vPixedDir, sizeof(_float));

				jDesc["Have Trail"].get_to<_bool>(eLoadEffectDesc.IsTrail);
				if (eLoadEffectDesc.IsTrail == true)
				{
					jDesc["Active"].get_to<_bool>(eLoadEffectDesc.bActive);
					jDesc["Alpha Trail"].get_to<_bool>(eLoadEffectDesc.bAlpha);
					jDesc["Life"].get_to<_float>(eLoadEffectDesc.fLife);
					jDesc["Width"].get_to<_float>(eLoadEffectDesc.fWidth);
					jDesc["SegmentSize"].get_to<_float>(eLoadEffectDesc.fSegmentSize);
				}

				jDesc["Alpha"].get_to<_float>(eLoadEffectDesc.fAlpha);
				jDesc["Billboard"].get_to<_bool>(eLoadEffectDesc.IsBillboard);
				jDesc["Use Normal"].get_to<_bool>(eLoadEffectDesc.IsNormal);
				jDesc["Use Mask"].get_to<_bool>(eLoadEffectDesc.IsMask);
				jDesc["Trigger"].get_to<_bool>(eLoadEffectDesc.IsTrigger);
				jDesc["MovingPosition"].get_to<_bool>(eLoadEffectDesc.IsMovingPosition);
				jDesc["Use Child"].get_to<_bool>(eLoadEffectDesc.bUseChild);
				jDesc["Spread"].get_to<_bool>(eLoadEffectDesc.bSpread);
				jDesc["FreeMove"].get_to<_bool>(eLoadEffectDesc.bFreeMove);
#pragma  endregion	EFFECTDESC
				memcpy(&m_eEFfectDesc, &eLoadEffectDesc, sizeof(CEffect_Base::EFFECTDESC));
			}
				_int iDTextureCnt = 0, iMTextureCnt = 0;
				jObject["Have DTextureCnt"].get_to<_int>(iDTextureCnt);
				jObject["Have MTextureCnt"].get_to<_int>(iMTextureCnt);

				m_iTotalDTextureComCnt = iDTextureCnt;
				m_iTotalMTextureComCnt = iMTextureCnt;

				_matrix WorldMatrix = XMMatrixIdentity();
				_int i = 0;
				for (_float fElement : jObject["WorldMatrix"])	
					memcpy(((_float*)&WorldMatrix) + (i++), &fElement, sizeof(_float));
				dynamic_cast<CEffect_Base*>(this)->Set_InitMatrix(WorldMatrix);

			// FreeMove
			_int j = 0;
			vector<_float4> FreePos;
			if (eLoadEffectDesc.bFreeMove == true)
			{
				_float4 fFreePos;
				for (_float fFree : jObject["FreePos"])
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
				_int iParticleIndex = 0, iTrailEffectType = 0;
			//	jObject["Trail EffectType"].get_to<_int>(iTrailEffectType);

				if (iEffectType == 1) // VIBuffer_Point_Instancing Data Save
				{
					jObject["Trail ProtoTag"].get_to<string>(strTrailPrototypeTag);
					jObject["Trail CloneTag"].get_to<string>(strTrailCloneTag);
					jObject["Trail Index"].get_to<_int>(iParticleIndex);

					_tchar szConvertTag[128] = L"";
					CUtile::CharToWideChar(strTrailPrototypeTag.c_str(), szConvertTag);
					_tchar* szTrailPrototypeTag = CUtile::Create_String(szConvertTag);
					pGameInstance->Add_String(szTrailPrototypeTag);

					dynamic_cast<CEffect_Point_Instancing*>(this)->Set_Trail(dynamic_cast<CEffect_Point_Instancing*>(this), szTrailPrototypeTag);
				}
				else
				{
					jObject["Trail ProtoTag"].get_to<string>(strTrailPrototypeTag);
					jObject["Trail CloneTag"].get_to<string>(strTrailCloneTag);

					_tchar szConvertTag[128] = L"";
					CUtile::CharToWideChar(strTrailPrototypeTag.c_str(), szConvertTag);
					_tchar* szTrailPrototypeTag = CUtile::Create_String(szConvertTag);
					pGameInstance->Add_String(szTrailPrototypeTag);

					dynamic_cast<CEffect_Base*>(this)->Set_InitTrail(szTrailPrototypeTag, 1);
				}
			}

			_int iChildCnt = 0;
			jObject["Child Count"].get_to<_int>(iChildCnt);
			string strChildPrototypeTag = "", strChildCloneTag = "";
			// Child
			if (iChildCnt != 0)
			{
				CEffect_Base::EFFECTDESC eChildEffectDesc;
				ZeroMemory(&eChildEffectDesc, sizeof(CEffect_Base::EFFECTDESC));

				for (auto jChildDesc : jObject["ChildDesc"])
				{
#pragma  region	EFFECTDESC
					string strPrototypeTag = "";
					string strCloneTag = "";

					jChildDesc["Child ProtoTag"].get_to<string>(strPrototypeTag);
					jChildDesc["Child CloneTag"].get_to<string>(strCloneTag);

					jChildDesc["Effect Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eEffectType = (CEffect_Base::EFFECTDESC::EFFECTTYPE)iEnum;

					iEnum = 0;
					jChildDesc["Mesh Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eMeshType = (CEffect_Base::EFFECTDESC::MESHTYPE)iEnum;

					jChildDesc["Particle Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eParticleType = (CEffect_Base::EFFECTDESC::PARTICLETYPE)iEnum;

					jChildDesc["TextureRender Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eTextureRenderType = (CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE)iEnum;

					jChildDesc["Texture Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eTextureType = (CEffect_Base::EFFECTDESC::TEXTURETYPE)iEnum;

					jChildDesc["Blend Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eBlendType = (CEffect_Base::EFFECTDESC::BLENDSTATE)iEnum;

					jChildDesc["MoveDir Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eMoveDir = (CEffect_Base::EFFECTDESC::MOVEDIR)iEnum;

					jChildDesc["Rotation Type"].get_to<_int>(iEnum);
					eChildEffectDesc.eRotation = (CEffect_Base::EFFECTDESC::ROTXYZ)iEnum;

					iEnum = 0;
					jChildDesc["PassCnt"].get_to<_int>(iEnum);
					eChildEffectDesc.iPassCnt = iEnum;

					_int i = 0;
					for (_float fFrame : jChildDesc["DiffuseTexture Index"])
						memcpy(((_float*)&eChildEffectDesc.fFrame) + (i++), &fFrame, sizeof(_float));
					i = 0;
					for (_float fMaskFrame : jChildDesc["MaskTexture Index"])
						memcpy(((_float*)&eChildEffectDesc.fMaskFrame) + (i++), &fMaskFrame, sizeof(_float));
					i = 0;
					for (_float vColor : jChildDesc["Color"])
						memcpy(((_float*)&eChildEffectDesc.vColor) + (i++), &vColor, sizeof(_float));
					i = 0;
					for (_float vScale : jChildDesc["Scale"])
						memcpy(((_float*)&eChildEffectDesc.vScale) + (i++), &vScale, sizeof(_float));

					jChildDesc["NormalTexture Index"].get_to<_float>(eChildEffectDesc.fNormalFrame);
					jChildDesc["Width Frame"].get_to<_float>(eChildEffectDesc.fWidthFrame);
					jChildDesc["Height Frame"].get_to<_float>(eChildEffectDesc.fHeightFrame);

					jChildDesc["SeparateWidth"].get_to<_int>(eChildEffectDesc.iSeparateWidth);
					jChildDesc["SeparateHeight"].get_to<_int>(eChildEffectDesc.iSeparateHeight);
					jChildDesc["WidthCnt"].get_to<_int>(eChildEffectDesc.iWidthCnt);
					jChildDesc["HeightCnt"].get_to<_int>(eChildEffectDesc.iHeightCnt);
					jChildDesc["DurationTime"].get_to<_float>(eChildEffectDesc.fTimeDelta);

					jChildDesc["CreateRange"].get_to<_float>(eChildEffectDesc.fCreateRange);
					jChildDesc["Range"].get_to<_float>(eChildEffectDesc.fRange);
					jChildDesc["Angle"].get_to<_float>(eChildEffectDesc.fAngle);
					jChildDesc["MoveDurationTime"].get_to<_float>(eChildEffectDesc.fMoveDurationTime);
					jChildDesc["Start"].get_to<_bool>(eChildEffectDesc.bStart);

					i = 0;
					for (_float vInitPos : jChildDesc["Init Pos"])
						memcpy(((_float*)&eChildEffectDesc.vInitPos) + (i++), &vInitPos, sizeof(_float));
					i = 0;
					for (_float vPixedDir : jChildDesc["Pixed Dir"])
						memcpy(((_float*)&eChildEffectDesc.vPixedDir) + (i++), &vPixedDir, sizeof(_float));

					jChildDesc["Have Trail"].get_to<_bool>(eChildEffectDesc.IsTrail);
					if (eChildEffectDesc.IsTrail == true)
					{
						jChildDesc["Active"].get_to<_bool>(eChildEffectDesc.bActive);
						jChildDesc["Alpha Trail"].get_to<_bool>(eChildEffectDesc.bAlpha);
						jChildDesc["Life"].get_to<_float>(eChildEffectDesc.fLife);
						jChildDesc["Width"].get_to<_float>(eChildEffectDesc.fWidth);
						jChildDesc["SegmentSize"].get_to<_float>(eChildEffectDesc.fSegmentSize);
					}

					jChildDesc["Alpha"].get_to<_float>(eChildEffectDesc.fAlpha);
					jChildDesc["Billboard"].get_to<_bool>(eChildEffectDesc.IsBillboard);
					jChildDesc["Use Normal"].get_to<_bool>(eChildEffectDesc.IsNormal);
					jChildDesc["Use Mask"].get_to<_bool>(eChildEffectDesc.IsMask);
					jChildDesc["Trigger"].get_to<_bool>(eChildEffectDesc.IsTrigger);
					jChildDesc["MovingPosition"].get_to<_bool>(eChildEffectDesc.IsMovingPosition);
					jChildDesc["Use Child"].get_to<_bool>(eChildEffectDesc.bUseChild);
					jChildDesc["Spread"].get_to<_bool>(eChildEffectDesc.bSpread);
					jChildDesc["FreeMove"].get_to<_bool>(eChildEffectDesc.bFreeMove);

					_matrix WorldMatrix = XMMatrixIdentity();
					i = 0;
					for (_float fElement : jChildDesc["WorldMatrix"])
						memcpy(((_float*)&WorldMatrix) + (i++), &fElement, sizeof(_float));

#pragma  endregion	EFFECTDESC

					dynamic_cast<CEffect_Base*>(this)->Set_InitChild(eChildEffectDesc, iChildCnt, strPrototypeTag.c_str(), WorldMatrix);
					jChildDesc.clear();
				}
			}

			_int iCnt = 0;
			// Component Desc ¿˙¿Â
			if (m_eEFfectDesc.eEffectType == CEffect_Base::EFFECTDESC::EFFECTTYPE::EFFECT_PARTICLE) // VIBuffer_Point_Instancing Data Save
			{
				string strVIBufferTag = "";
				if (jObject.contains("CloneObject Component ProtoTag"))
 					jObject["CloneObject Component ProtoTag"].get_to<string>(strVIBufferTag);

				_tchar szVIBufferTag[MAX_PATH] = L"";
				CUtile::CharToWideChar(strVIBufferTag.c_str(), szVIBufferTag);
				_tchar* szVIBufferFinalTag = CUtile::Create_String(szVIBufferTag);
				pGameInstance->Add_String(szVIBufferFinalTag);

				dynamic_cast<CEffect_Point_Instancing*>(this)->Set_VIBufferProtoTag(szVIBufferFinalTag);

				iCnt = 0; 
				jObject["Instance DataCnt"].get_to<_int>(iCnt);

				CVIBuffer_Point_Instancing* pVIBuffer = nullptr;
				if (FAILED(pGameInstance->Add_Prototype(g_LEVEL, szVIBufferFinalTag, pVIBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, iCnt))))
					return E_FAIL;

#pragma region Instance Data
				CVIBuffer_Point_Instancing::INSTANCEDATA* InstanceData = nullptr;
				InstanceData = pVIBuffer->Get_InstanceData();

				_int k = 0;
				for (_float fPos : jObject["CloneObject Component InstnaceData Position"])
					memcpy((_float*)&InstanceData->fPos + (k++), &fPos, sizeof(_float));

				jObject["CloneObject Component InstnaceData Speed"].get_to<_double>(InstanceData->pSpeeds);
				jObject["CloneObject Component InstnaceData SpeedMinMin"].get_to<_float>(InstanceData->SpeedMinMax.x);
				jObject["CloneObject Component InstnaceData SpeedMinMax"].get_to<_float>(InstanceData->SpeedMinMax.y);
				jObject["CloneObject Component InstnaceData PSizeX"].get_to<_float>(InstanceData->fPSize.x);
				jObject["CloneObject Component InstnaceData PSizeY"].get_to<_float>(InstanceData->fPSize.y);
				
#pragma endregion Instance Data

#pragma region Point Desc Data
				CVIBuffer_Point_Instancing::POINTDESC* PointDesc = nullptr;
				PointDesc = pVIBuffer->Get_PointDesc();

				string strPointDataTag = "";
				jObject["Point Data"].get_to<string>(strPointDataTag);

				k = 0;
				for (_float fMin : jObject["CloneObject Component PointDesc Min"])
					memcpy((_float*)&PointDesc->fMin + (k++), &fMin, sizeof(_float));

				k = 0;
				for (_float fMax : jObject["CloneObject Component PointDesc Max"])
					memcpy((_float*)&PointDesc->fMax + (k++), &fMax, sizeof(_float));

				k = 0;
				for (_float fCirclePos : jObject["CloneObject Component PointDesc CirclePos"])
					memcpy((_float*)&PointDesc->vCirclePos + (k++), &fCirclePos, sizeof(_float));

				k = 0;
				for (_float fConeRange : jObject["CloneObject Component PointDesc ConeRange"])
					memcpy((_float*)&PointDesc->fConeRange + (k++), &fConeRange, sizeof(_float));

				k = 0;
				for (_float vOriginPos : jObject["CloneObject Component PointDesc OriginPos"])
					memcpy((_float*)&PointDesc->vOriginPos + (k++), &vOriginPos, sizeof(_float));

				k = 0;
				for (_float vExplosionDir : jObject["CloneObject Component PointDesc ExplosionDir"])
					memcpy((_float*)&PointDesc->vExplosionDir + (k++), &vExplosionDir, sizeof(_float));

				k = 0;
				for (_float fDir : jObject["CloneObject Component PointDesc Dir"])
					memcpy((_float*)&PointDesc->vDir + (k++), &fDir, sizeof(_float));

				iEnum = 0;
				jObject["CloneObject Component PointDesc ShapeType"].get_to<_int>(iEnum);
				memcpy(&PointDesc->eShapeType, &iEnum, sizeof(_int));
				jObject["CloneObject Component PointDesc RotXYZ"].get_to<_int>(iEnum);
				memcpy(&PointDesc->eRotXYZ, &iEnum, sizeof(_int));

				jObject["CloneObject Component PointDesc Range"].get_to<_float>(PointDesc->fRange);
				jObject["CloneObject Component PointDesc CreateInstance"].get_to<_int>(PointDesc->iCreateInstance);
				jObject["CloneObject Component PointDesc InstanceIndex"].get_to<_int>(PointDesc->iInstanceIndex);
				jObject["CloneObject Component PointDesc Spread"].get_to<_bool>(PointDesc->bSpread);
				jObject["CloneObject Component PointDesc IsAlive"].get_to<_bool>(PointDesc->bIsAlive);
				jObject["CloneObject Component PointDesc bMoveY"].get_to<_bool>(PointDesc->bMoveY);
				jObject["CloneObject Component PointDesc Rotation"].get_to<_bool>(PointDesc->bRotation);
				jObject["CloneObject Component PointDesc MoveY"].get_to<_float>(PointDesc->fMoveY);
				jObject["CloneObject Component PointDesc CreateRange"].get_to<_float>(PointDesc->fCreateRange);
				jObject["CloneObject Component PointDesc DurationTime"].get_to<_float>(PointDesc->fDurationTime);
				jObject["CloneObject Component PointDesc MaxTime"].get_to<_float>(PointDesc->fMaxTime);
				jObject["CloneObject Component PointDesc RangeOffset"].get_to<_float>(PointDesc->fRangeOffset);
				jObject["CloneObject Component PointDesc RangeY"].get_to<_float>(PointDesc->fRangeY);

#pragma endregion Point Desc Data

			}
		}
	}
#pragma endregion LoadData
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEffect_Base::Set_Matrix()
{
	_matrix  matParent = m_pParent->Get_TransformCom()->Get_WorldMatrix();
	_matrix  matScaleSet = XMMatrixIdentity();
	_float4 vScale = m_eEFfectDesc.vScale;
	_vector vRight, vUp, vLook;

	memcpy(&vRight, &matParent.r[0], sizeof(_vector));
	memcpy(&vUp, &matParent.r[1], sizeof(_vector));
	memcpy(&vLook, &matParent.r[2], sizeof(_vector));

	memcpy(&matScaleSet.r[0], &(XMVector3Normalize(vRight)* vScale.x), sizeof(_vector));
	memcpy(&matScaleSet.r[1], &(XMVector3Normalize(vUp)* vScale.y), sizeof(_vector));
	memcpy(&matScaleSet.r[2], &(XMVector3Normalize(vLook)* vScale.z), sizeof(_vector));
	memcpy(&matScaleSet.r[3], &matParent.r[3], sizeof(_vector));

	m_WorldWithParentMatrix = m_InitWorldMatrix * matScaleSet;
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldWithParentMatrix));
}

void CEffect_Base::TurnOffSystem(_float fDurationTime, _float fTimeDelta)
{
	m_fTurnOffTime += fTimeDelta;
	if (m_fTurnOffTime > fDurationTime)
	{
		m_eEFfectDesc.bActive = false;
		m_fTurnOffTime = 0.0f;
	}
}

_bool CEffect_Base::TurnOffSystem(_float fTurnoffTime, _float fDurationTime, _float fTimeDelta)
{ 
	fTurnoffTime += fTimeDelta;
	if (fTurnoffTime > fDurationTime)
	{
		return true;
		m_eEFfectDesc.bActive = false;
		fTurnoffTime = 0.0f;
	}
	return false;
}

void CEffect_Base::Set_TrailDesc()
{
	if (m_pParent == nullptr || dynamic_cast<CEffect_Base*>(m_pParent) == false)
		return;

	EFFECTDESC effectDesc = dynamic_cast<CEffect_Base*>(m_pParent)->Get_EffectDesc();

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

void CEffect_Base::BillBoardSetting(_float3 vScale)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera* pCamera = pGameInstance->Get_WorkCameraPtr();
	CTransform* pTargetTransform = dynamic_cast<CGameObject*>(pCamera)->Get_TransformCom();
	RELEASE_INSTANCE(CGameInstance);

	_float3 cameraPosition, cameraUp, cameraForward;
	_float4 rotateAxis, objectForward;

	XMStoreFloat3(&cameraPosition, pTargetTransform->Get_State(CTransform::STATE_TRANSLATION));
	XMStoreFloat3(&cameraUp, pTargetTransform->Get_State(CTransform::STATE_UP));
	XMStoreFloat3(&cameraForward, pTargetTransform->Get_State(CTransform::STATE_LOOK));

	_matrix worldmatrix = _smatrix::CreateBillboard(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), cameraPosition, cameraUp, &cameraForward);
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * worldmatrix);
}

void CEffect_Base::Set_ShaderOption(_int iPassCnt, _float fHDRValue, _float2 fUV, _bool bActive)
{
	m_eEFfectDesc.iPassCnt = iPassCnt;
	m_fHDRValue = fHDRValue;
	m_fUV = fUV;
	m_eEFfectDesc.bActive = bActive;
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

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Tick(fTimeDelta);
}

void CEffect_Base::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Late_Tick(fTimeDelta);
}

HRESULT CEffect_Base::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Base::Imgui_RenderProperty()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	if (ImGui::CollapsingHeader("Transform : "))
	{
		ImGuizmo::BeginFrame();
		static float snap[3] = { 1.f, 1.f, 1.f };
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::RadioButton("MyTranslate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("MyRotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("MyScale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		static bool useSnap(false);
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		const _matrix&   matWorld = m_pTransformCom->Get_WorldMatrix();

		ImGuizmo::DecomposeMatrixToComponents((_float*)&matWorld, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("MyTranslate", matrixTranslation);
		ImGui::InputFloat3("MyRotate", matrixRotation);
		ImGui::InputFloat3("MyScale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&matWorld);

		ImGuiIO&	io = ImGui::GetIO();
		RECT		rt;
		GetClientRect(g_hWnd, &rt);
		POINT		LT{ rt.left, rt.top };
		ClientToScreen(g_hWnd, &LT);
		ImGuizmo::SetRect((_float)LT.x, (_float)LT.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4		matView, matProj;
		XMStoreFloat4x4(&matView, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		ImGuizmo::Manipulate(
			reinterpret_cast<_float*>(&matView),
			reinterpret_cast<_float*>(&matProj),
			mCurrentGizmoOperation,
			mCurrentGizmoMode,
			(_float*)&matWorld,
			nullptr, useSnap ? &snap[0] : nullptr);

		m_pTransformCom->Set_WorldMatrix(matWorld);
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CEffect_Base::Set_InitChild(EFFECTDESC eEffectDesc, _int iCreateCnt, const char* ProtoTag, _matrix worldmatrix)
{
	CEffect_Base*    pEffectBase = nullptr;
	CGameInstance*   pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar      szChildProto[128];
	CUtile::CharToWideChar(ProtoTag, szChildProto);

	EFFECTDESC eChildeffectDesc;
	memcpy(&eChildeffectDesc, &eEffectDesc, sizeof(eEffectDesc));

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

	CLayer* pLayer = pGameInstance->Find_Layer(g_LEVEL, L"Layer_Effect");
	if(pLayer != nullptr)
	{
		map<const _tchar*, class CGameObject*>& pGameObjects = pLayer->GetGameObjects();
		_uint iLayerSize = (_uint)pGameObjects.size();

		auto iter = pGameObjects.begin();
		for (size_t i = 0; i < iLayerSize; ++i, iter++)
		{
			if (!lstrcmp(iter->second->Get_ProtoObjectName(), szChildProtoTag))
			{
				pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(szChildProtoTag, szChildClondTag, &eChildeffectDesc));
				pEffectBase->Set_InitMatrix(worldmatrix);
				NULL_CHECK_RETURN(pEffectBase, E_FAIL);
				pEffectBase->Set_Parent(this);

				m_vecChild.push_back(pEffectBase);
				m_iHaveChildCnt++;

				RELEASE_INSTANCE(CGameInstance);
				return S_OK;
			}
		}
	}

	switch (eChildeffectDesc.eEffectType)
	{
	case CEffect_Base::tagEffectDesc::EFFECT_PLANE:
		if (FAILED(pGameInstance->Add_Prototype(szChildProtoTag, CEffect_T::Create(m_pDevice, m_pContext))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;

	case CEffect_Base::tagEffectDesc::EFFECT_PARTICLE:
		break;

	case CEffect_Base::tagEffectDesc::EFFECT_MESH:
		if (FAILED(pGameInstance->Add_Prototype(szChildProtoTag, CEffect_Mesh_T::Create(m_pDevice, m_pContext))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		break;
	}
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(szChildProtoTag, szChildClondTag, &eChildeffectDesc));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	pEffectBase->Set_InitMatrix(worldmatrix);

	m_vecChild.push_back(pEffectBase);
	m_iHaveChildCnt++;

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

				if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szDTextureComTag, (CComponent**)&m_pDTextureCom[i], this)))
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
				m_iTotalMTextureComCnt++;

				_tchar szMTexture[64] = L"";
				wsprintf(szMTexture, L"Com_MTexture_%d", i);

				_tchar* szMTextureComTag = CUtile::Create_String(szMTexture);
				CGameInstance::GetInstance()->Add_String(szMTextureComTag);
				if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Effect"), szMTextureComTag, (CComponent**)&m_pMTextureCom[i], this)))
					return E_FAIL;
			}
		}
	}
	return S_OK;
}

void CEffect_Base::ToolOption(const char* pToolTag)
{
	ImGui::Begin(pToolTag);
	static _float fHDRValue = this->m_fHDRValue;
	static _float2 fUV = this->m_fUV;

	TransformView();

	if (ImGui::Button("Recompile")) m_pShaderCom->ReCompile(); ImGui::SameLine();
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
	ImGui::InputFloat4("DiffuseTexture", (_float*)&m_eEFfectDesc.fFrame);
	ImGui::InputFloat4("MaskTexture", (_float*)&m_eEFfectDesc.fMaskFrame);
	ImGui::InputInt("iPassCnt", &m_eEFfectDesc.iPassCnt);
	ImGui::InputFloat("HDRValue", &fHDRValue);
	m_fHDRValue = fHDRValue;
	ImGui::DragFloat2("UV", (_float*)&fUV, 0.01f, -1.0f, 1.0f);
	m_fUV = fUV;

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Set Item Texture"))
	{
		static _int iCreateCnt = 1;
		static _int iSelectDTexture = 0;
		static _int iSelectMTexture = 0;
		static _int iSelectNTexture = 0;
		static _int iSelectTextureType = 0;

		const _int  iTotalDTextureCnt = this->Get_TotalDTextureCnt();
		const _int  iTotalMTextureCnt = this->Get_TotalMTextureCnt();

		ImGui::BulletText("Texture Type : ");
		ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
		ImGui::RadioButton("MaskTexture", &iSelectTextureType, 1); ImGui::SameLine();
		ImGui::RadioButton("NormalTexture", &iSelectTextureType, 2);

		ImGui::Separator();
		if (iSelectTextureType == 0)
		{
			ImGui::BulletText("Edit TextureComponent : "); ImGui::SameLine();

			ImGui::PushItemWidth(100);
			ImGui::InputInt("##EditDTexture", (_int*)&iCreateCnt, 1, 5); ImGui::SameLine();

			if (iCreateCnt <= 1)
				iCreateCnt = 1;
			else if (iCreateCnt >= 5)
				iCreateCnt = 5;

			if (ImGui::Button("Edit Texture Confirm"))
				this->Edit_TextureComponent(iCreateCnt, 0);

			char** szDTextureType = new char* [iTotalDTextureCnt];
			for (_int i = 0; i < iTotalDTextureCnt; ++i)
			{
				szDTextureType[i] = new char[64];

				_tchar   szDefault[64] = L"";
				wsprintf(szDefault, L"Com_DTexture_%d", i);

				CUtile::WideCharToChar(szDefault, szDTextureType[i]);
			}
			ImGui::BulletText("DTexture : "); ImGui::SameLine();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Combo("##DTexture", &iSelectDTexture, szDTextureType, iTotalDTextureCnt);

			_tchar szDTexture[64] = L"";
			CUtile::CharToWideChar(szDTextureType[iSelectDTexture], szDTexture);
			CTexture* pDiffuseTexture = (CTexture*)this->Find_Component(szDTexture);

			if (pDiffuseTexture != nullptr)
			{
				_float fFrame = this->Get_EffectDesc().fFrame[iSelectDTexture];
				ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
				ImGui::Separator();

				for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
				{
					if (i % 6)
						ImGui::SameLine();

					if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
					{
						m_eEFfectDesc.fFrame[iSelectDTexture] = i * 1.0f;
						this->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
					}
				}
			}
			for (size_t i = 0; i < iTotalDTextureCnt; ++i)
				Safe_Delete_Array(szDTextureType[i]);
			Safe_Delete_Array(szDTextureType);
		}
		else if (iSelectTextureType == 1)
		{
			ImGui::BulletText("Edit TextureComponent : "); ImGui::SameLine();

			ImGui::PushItemWidth(100);
			ImGui::InputInt("##EditMTexture", (_int*)&iCreateCnt, 1, 5); ImGui::SameLine();

			if (iCreateCnt <= 1)
				iCreateCnt = 1;
			else if (iCreateCnt >= 5)
				iCreateCnt = 5;

			if (ImGui::Button("Edit Texture Confirm"))
				this->Edit_TextureComponent(0, iCreateCnt);

			char** szMTextureType = new char* [iTotalMTextureCnt];
			if (0 != iTotalMTextureCnt)
			{
				for (_int i = 0; i < iTotalMTextureCnt; ++i)
				{
					szMTextureType[i] = new char[64];

					_tchar   szDefault[64] = L"";
					wsprintf(szDefault, L"Com_MTexture_%d", i);

					CUtile::WideCharToChar(szDefault, szMTextureType[i]);
				}
				ImGui::BulletText("MTexture : "); ImGui::SameLine();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::Combo("##MTexture", &iSelectMTexture, szMTextureType, iTotalMTextureCnt);
			}

			_tchar szMTexture[64] = L"";
			CUtile::CharToWideChar(szMTextureType[iSelectMTexture], szMTexture);
			CTexture* pDiffuseTexture = (CTexture*)this->Find_Component(szMTexture);

			if (pDiffuseTexture != nullptr)
			{
				_float fMaskFrame = this->Get_EffectDesc().fMaskFrame[iSelectMTexture];

				ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
				ImGui::Separator();

				for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
				{
					if (i % 6)
						ImGui::SameLine();

					if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
					{
						m_eEFfectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
						this->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
					}
				}
			}

			for (size_t i = 0; i < iTotalMTextureCnt; ++i)
				Safe_Delete_Array(szMTextureType[i]);
			Safe_Delete_Array(szMTextureType);
		}
		else // NTexture
		{
			CTexture* pNormalTexture = (CTexture*)this->Find_Component(L"Com_NTexture");
			if (pNormalTexture != nullptr)
			{
				_float fNormalFrame = this->Get_EffectDesc().fNormalFrame;

				ImGui::BulletText("NTexture _ %d / %d", (_uint)fNormalFrame, pNormalTexture->Get_TextureIdx());
				ImGui::Separator();

				for (_uint i = 0; i < pNormalTexture->Get_TextureIdx(); ++i)
				{
					if (i % 6)
						ImGui::SameLine();

					if (ImGui::ImageButton(pNormalTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
					{
						m_eEFfectDesc.fNormalFrame = i * 1.0f;
						this->Set_EffectDescNTexture(i * 1.0f);
					}
				}
			}
		}
	}

	Set_Color();

	ImGui::End();
}

void CEffect_Base::TransformView()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (ImGui::CollapsingHeader("Transform : "))
	{
		ImGuizmo::BeginFrame();
		static float snap[3] = { 1.f, 1.f, 1.f };
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::RadioButton("MyTranslate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("MyRotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("MyScale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		static bool useSnap(false);
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		const _matrix& matWorld = this->m_pTransformCom->Get_WorldMatrix();

		ImGuizmo::DecomposeMatrixToComponents((_float*)&matWorld, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("MyTranslate", matrixTranslation);
		ImGui::InputFloat3("MyRotate", matrixRotation);
		ImGui::InputFloat3("MyScale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&matWorld);

		ImGuiIO& io = ImGui::GetIO();
		RECT		rt;
		GetClientRect(g_hWnd, &rt);
		POINT		LT{ rt.left, rt.top };
		ClientToScreen(g_hWnd, &LT);
		ImGuizmo::SetRect((_float)LT.x, (_float)LT.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4		matView, matProj;
		XMStoreFloat4x4(&matView, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		ImGuizmo::Manipulate(
			reinterpret_cast<_float*>(&matView),
			reinterpret_cast<_float*>(&matProj),
			mCurrentGizmoOperation,
			mCurrentGizmoMode,
			(_float*)&matWorld,
			nullptr, useSnap ? &snap[0] : nullptr);

		this->m_pTransformCom->Set_WorldMatrix(matWorld);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Base::Set_Color()
{
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_eEFfectDesc.vColor;

	ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;
}

void CEffect_Base::Free()
{
	__super::Free();

	// Shader, Renderer Release
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	// Diffuse Release
	for (auto& pTextureCom : m_pDTextureCom)
		Safe_Release(pTextureCom);

	// Mask Release
	for (auto& pTextureCom : m_pMTextureCom)
		Safe_Release(pTextureCom);

	Safe_Release(m_pNTextureCom);

	// VIBuffer Release
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIInstancingBufferCom);
	Safe_Release(m_pVITrailBufferCom);

	// Mesh Release
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffectTrail);

	for (auto& pChild : m_vecChild)
		Safe_Release(pChild);
	m_vecChild.clear();
}
