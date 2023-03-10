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
}

CEffect_Base::CEffect_Base(const CEffect_Base & rhs)
	: CGameObject(rhs)
	, m_iTotalDTextureComCnt(rhs.m_iTotalDTextureComCnt)
	, m_iTotalMTextureComCnt(rhs.m_iTotalMTextureComCnt)
	, m_fInitSpriteCnt(rhs.m_fInitSpriteCnt)
	//, m_iHaveChildCnt(rhs.m_iHaveChildCnt)
	//, m_vecProPos(rhs.m_vecProPos)
	//, m_vecFreePos(rhs.m_vecFreePos)
	//, m_pEffectTrail(rhs.m_pEffectTrail)
	//, m_vecChild(rhs.m_vecChild)
{
	m_vecChild.assign(rhs.m_vecChild.begin(), rhs.m_vecChild.end());

	memcpy(&m_InitWorldMatrix, &rhs.m_InitWorldMatrix, sizeof(_float4x4));
 	memcpy(&m_eEFfectDesc, &rhs.m_eEFfectDesc, sizeof(EFFECTDESC));
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
	for (auto jData : jLayer["Pulse Test"])
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
				for (_float vScale : jDesc["vScale"])
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
					for (_float vScale : jChildDesc["vScale"])
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

	if (nullptr != m_pEffectTrail)
		Safe_Release(m_pEffectTrail);

	for (auto& pChild : m_vecChild)
		Safe_Release(pChild);
	m_vecChild.clear();
}
