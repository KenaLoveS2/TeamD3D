#include "stdafx.h"
#include "..\public\Imgui_Effect.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "Camera.h"


#include "Effect_Point_Instancing.h"

#include "Effect_Trail.h"
#include "Effect_Mesh_T.h"
#include "Effect_T.h"

// Json
#include "Json/json.hpp"
#include <fstream>
// ~Json

#define TOOL

CImgui_Effect::CImgui_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImguiObject(pDevice, pContext)
{
	ZeroMemory(&m_eEffectDesc, sizeof(CEffect_Base::EFFECTDESC));
}

HRESULT CImgui_Effect::Initialize(void * pArg)
{
	m_szWindowName = "Effect_Tool";

	return S_OK;
}

void CImgui_Effect::Imgui_RenderWindow()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma  region	DATA

	static _bool bSave = false;
	static _bool bLoad = false;

	static _bool bComponentSave = false;
	static _bool bComponentLoad = false;

	static _bool bDescSave = false;
	static _bool bDescLoad = false;

#pragma  endregion //DATA

	wstring		strDefault = L"Prototype_GameObject_";
	static char szEffectTag[64] = "";
	static char szEffectMeshTag[64] = "";
	static _int iCreateRectCnt = 0;
	static _int iCreateMeshtCnt[5] = { 0, 0, 0, 0, 0 };
	static _int iCreateCnt = 1;
	static _int iCreateEffectType = 0;

	static _int  iSelectEffectType = 0;
	static _int  iPreSelectEffectType = 0;
	static _int  iSelectEffectMeshType = 0;
	static _bool bIsCreate = false;
	static _bool bReset = false;

	_tchar*		 szEffectCloneTag = L"";

	// Layer_ListBox용 
	char** pObjectTag = nullptr;
	static _uint iLayerSize = 0;
	static char* pSelectObject = "";
	// Layer_ListBox용 

	// Paricle
	wstring		strComponentDefault = L"Prototype_Component_";
	static char szEffectComponentTag[64] = "";
	static _int iCreateParticleCnt = 0;
	//

	_int iCurLevel = pGameInstance->Get_CurLevelIndex();
	static _int iSelectObj = -1;
	static _int iCurSelect = -1;

	// 1. Texture Type Select
	const char* szEffectType[] = { " ~SELECT_EFFECT TYPE~ ", " EFFECT_PLANE ", " EFFECT_PARTICLE", " EFFECT_MESH" };
	ImGui::BulletText("EffectType : "); 
	ImGui::RadioButton("EFFECT_PLANE", &iSelectEffectType, 1); ImGui::SameLine();
	ImGui::RadioButton("EFFECT_PARTICLE", &iSelectEffectType, 2); ImGui::SameLine();
	ImGui::RadioButton("EFFECT_MESH", &iSelectEffectType, 3);

	//ImGui::Combo("##EffectType", &iSelectEffectType, szEffectType, IM_ARRAYSIZE(szEffectType));
	CGameObject* pGameObject = nullptr;

	if (pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect") != nullptr)
	{
		ImGui::Separator();
		ImGui::BulletText("EffectLayer Tag : ");
		pGameObject = LayerEffects_ListBox(pObjectTag, iLayerSize, pSelectObject, "##EffectLayer Tag", iSelectObj, TAGTYPE::TAG_CLONE);

		if (nullptr != pGameObject && -1 != iSelectEffectType )
		{
			CEffect_Base::tagEffectDesc::EFFECTTYPE eEffectType = dynamic_cast<CEffect_Base*>(pGameObject)->Get_EffectDesc().eEffectType;

			if (bReset == true)
			{
				if (iSelectEffectType == 0)
				{
					iPreSelectEffectType = iSelectEffectType = EFFECT_PLANE;
					bIsCreate = true;
					m_bIsRectLayer = true;
					bReset = false;
				}
				if (iSelectEffectType == 1)
				{
					iPreSelectEffectType = iSelectEffectType = EFFECT_PARTICLE;
					bIsCreate = true;
					m_bIsParticleLayer = true;
					bReset = false;
				}
				if (iSelectEffectType == 2)
				{
					iPreSelectEffectType = iSelectEffectType = EFFECT_MESH;
					bIsCreate = true;
					m_bIsMeshLayer = true;
					bReset = false;
				}
			}
		}
		for (size_t i = 0; i < iLayerSize; ++i)
			Safe_Delete_Array(pObjectTag[i]);
		Safe_Delete_Array(pObjectTag);
	}

	if (ImGui::Button("Save_Data"))
	{
		bSave = true;
	};
	ImGui::SameLine();
	if (ImGui::Button("Load_Data"))
	{
		bLoad = true;
	};
	ImGui::SameLine();
	if (ImGui::Button("Delete_Data"))
	{
		if (pGameObject != nullptr)
		{
			pGameInstance->Delete_Object(iCurLevel, L"Layer_Effect", pGameObject->Get_ObjectCloneName());
			m_bIsRectLayer = false;
			m_bIsParticleLayer = false;
			m_bIsMeshLayer = false;
			iSelectEffectType = -1;
			iSelectObj = -1;
			pGameObject = nullptr;
		}
	};
	ImGui::SameLine();
	if (ImGui::Button("Reset_Data"))
	{
		iSelectEffectType = 0;
		bReset = true;
		pGameObject = nullptr;
		m_bIsRectLayer = false;
		m_bIsParticleLayer = false;
		m_bIsMeshLayer = false;
		iSelectEffectType = -1;
	}
	if (ImGui::Button("Component_Save"))
	{
		bComponentSave = true;
	};	ImGui::SameLine();
	if (ImGui::Button("Component_Load"))
	{
		// bComponentLoad = true;
	};	ImGui::SameLine();
	if (ImGui::Button("Desc_Save"))
	{
		bDescSave = true;
	};	ImGui::SameLine();
	if (ImGui::Button("Desc_Load"))
	{
		// bComponentLoad = true;
	};	 

	static	char szSaveFileName[MAX_PATH] = "";
	char*        szLevelName[] = { "Loading", "Logo", "GamePlay", "MapTool", "TestPlay", "EffectTool" };
	static  _int iSaveLevel = 0;

	if (bSave || bComponentSave || bDescSave)
	{
		ImGui::Separator();
		ImGui::BulletText("Save Data");
		ImGui::Combo("SaveLevel", &iSaveLevel, szLevelName, IM_ARRAYSIZE(szLevelName));
		ImGui::SetNextItemWidth(200);
		ImGui::InputTextWithHint("##SaveData", "Input Save Data File Name", szSaveFileName, MAX_PATH);
		ImGui::SameLine();
		if (ImGui::Button("Confirm"))
		{
			if (bSave)
				ImGuiFileDialog::Instance()->OpenDialog("Select EffectData", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
			if (bComponentSave)
				ImGuiFileDialog::Instance()->OpenDialog("Select EffectComponent", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
			if (bDescSave)
				ImGuiFileDialog::Instance()->OpenDialog("Select EffectDesc", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancle"))
		{
			bSave = false;
			bComponentSave = false;
			bDescSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
		}
	}

	if (bLoad || bComponentLoad || bDescLoad)
	{
		ImGui::Separator();
		ImGui::BulletText("Load Data");
		if (bLoad)
			ImGuiFileDialog::Instance()->OpenDialog("Select EffectLoad", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
		if (bComponentLoad)
			ImGuiFileDialog::Instance()->OpenDialog("Select EffectComponentLoad", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
		if (bDescLoad)
			ImGuiFileDialog::Instance()->OpenDialog("Select EffectDescLoad", "Select Json", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
	}

	CGameObject* pEffectBase = nullptr;
	switch (iSelectEffectType)
	{
	case 0:
		iSelectEffectType = 0;
		bIsCreate = false;
		bReset = true;
		strcpy_s(szEffectTag, "");
		break;

	case EFFECT_PLANE:
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
			wstring	strEffectTag(szEffectTag, &szEffectTag[64]);
			strDefault = strDefault + strEffectTag;

			_tchar* szDefaultTag = CUtile::Create_String(strDefault.c_str());
			pGameInstance->Add_String(szDefaultTag);

			if (FAILED(pGameInstance->Add_Prototype(szDefaultTag, CEffect_T::Create(m_pDevice, m_pContext))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			for (_int i = 0; i < iCreateCnt; ++i)
			{
				_tchar   szDefault[64] = L"";
				wsprintf(szDefault, L"Effect_Plane_%d", iCreateRectCnt);

				_tchar* szEffectCloneTag = CUtile::Create_String(szDefault);
				pGameInstance->Add_String(szEffectCloneTag);

				if (FAILED(pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Effect", szDefaultTag, szEffectCloneTag, nullptr, &pEffectBase)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				iCreateRectCnt++;
			}
			iSelectEffectType = 0;
			iCreateCnt = 0;
			bIsCreate = true;
			m_bIsRectLayer = true;
			strcpy_s(szEffectTag, "");

		}
		ImGui::Separator();
		if (iSelectObj != -1 && bIsCreate == true && m_bIsRectLayer == true)
			CreateEffect_Plane(iCurSelect, iSelectObj);

		break;

	case EFFECT_PARTICLE:
		ImGui::Separator();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateParticleCnt", &iCreateCnt);

		ImGui::InputTextWithHint("EffectTag", "Only Input Tagname.", szEffectTag, 64);
		ImGui::InputTextWithHint("VIBufferTag", "Only Input ComponentTag.", szEffectComponentTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
			// VIBuffer Create
			wstring	strVIBufferTag(szEffectComponentTag, &szEffectComponentTag[64]);
			strComponentDefault = strComponentDefault + strVIBufferTag;

			_tchar* szComponentTag = CUtile::Create_String(strComponentDefault.c_str());
			pGameInstance->Add_String(szComponentTag);
			_int iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();

			if (FAILED(pGameInstance->Add_Prototype(iCurLevel, szComponentTag, CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, iCreateCnt))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			// Effect ProtoCreate
			wstring	strEffectTag(szEffectTag, &szEffectTag[64]);
			strDefault = strDefault + strEffectTag;

			_tchar* szDefaultTag = CUtile::Create_String(strDefault.c_str());
			pGameInstance->Add_String(szDefaultTag);

			if (FAILED(pGameInstance->Add_Prototype(szDefaultTag, CEffect_Point_Instancing::Create(m_pDevice, m_pContext, szComponentTag))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			// ~Effect ProtoCreate

			_tchar   szDefault[64] = L"";
			wsprintf(szDefault, L"Effect_Particle_%d", iCreateParticleCnt);

			_tchar* szEffectCloneTag = CUtile::Create_String(szDefault);
			pGameInstance->Add_String(szEffectCloneTag);

			if (FAILED(pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Effect", szDefaultTag, szEffectCloneTag)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			iCreateParticleCnt++;

			iSelectEffectType = 0;
			bIsCreate = true;
			m_bIsParticleLayer = true;
			iCreateCnt = 0;
			strcpy_s(szEffectTag, "");
			strcpy_s(szEffectComponentTag, "");
		}
		ImGui::Separator();

		if (iSelectObj != -1 && bIsCreate == true && m_bIsParticleLayer == true)
			CreateEffect_Particle(iCreateCnt, iCurSelect, iSelectObj);
		break;

	case EFFECT_MESH:
		// iSelectEffectMeshType MESH_PLANE, MESH_CUBE, MESH_CONE, MESH_SPHERE, MESH_CYLINDER
		ImGui::Separator();
		ImGui::BulletText("Select Create Mesh Type : ");
		ImGui::RadioButton("MeshPlane", &iSelectEffectMeshType, 0); ImGui::SameLine();
		ImGui::RadioButton("MeshCube", &iSelectEffectMeshType, 1); ImGui::SameLine();
		ImGui::RadioButton("MeshCone", &iSelectEffectMeshType, 2);
		ImGui::RadioButton("MeshSphere", &iSelectEffectMeshType, 3); ImGui::SameLine();
		ImGui::RadioButton("MeshCylinder", &iSelectEffectMeshType, 4);

		ImGui::Separator();
		ImGui::BulletText("CreateMeshCnt : ");  ImGui::InputInt("##CreateMeshCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectMeshTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
			wstring	strEffectTag(szEffectMeshTag, &szEffectMeshTag[64]);
			strDefault = strDefault + strEffectTag;

			_tchar* szDefaultTag = CUtile::Create_String(strDefault.c_str());
			pGameInstance->Add_String(szDefaultTag);

			if (FAILED(pGameInstance->Add_Prototype(szDefaultTag, CEffect_Mesh_T::Create(m_pDevice, m_pContext))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			const _tchar* szEffectMeshType[] = { L"MeshPlane", L"MeshCube", L"MeshCone", L"MeshSphere" ,L"MeshCylinder" };
			//	static _int iCreateMeshtCnt[5] = { MESH_PLANE, MESH_CUBE, MESH_CONE, MESH_SPHERE, MESH_CYLINDER };
			for (_int i = 0; i < iCreateCnt; ++i)
			{
				wstring strEffectMeshTag = L"Effect_";
				strEffectMeshTag += szEffectMeshType[iSelectEffectMeshType];
				strEffectMeshTag.push_back('_');
				strEffectMeshTag += to_wstring(iCreateMeshtCnt[iSelectEffectMeshType]);

				_tchar* szEffectCloneTag = CUtile::Create_String(strEffectMeshTag.c_str());
				pGameInstance->Add_String(szEffectCloneTag);

				CEffect_Base::EFFECTDESC eEffectType;
				Set_MeshType(eEffectType, iSelectEffectMeshType);

				if (FAILED(pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Effect", szDefaultTag, szEffectCloneTag, &eEffectType)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				iCreateMeshtCnt[iSelectEffectMeshType]++;
			}
			iSelectEffectType = 0;
			iCreateCnt = 0;
			bIsCreate = true;
			m_bIsMeshLayer = true;
			strcpy_s(szEffectTag, "");
		}
		ImGui::Separator();

		if (iSelectObj != -1 && bIsCreate == true && m_bIsMeshLayer == true)
			CreateEffect_Mesh(iSelectObj);
		break;
	}

	// Component => Prototype => Clone(Desc)
	if (ImGuiFileDialog::Instance()->Display("Select EffectData"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region SaveData
			string		strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
			strSaveDirectory += "\\";
			strSaveDirectory += szSaveFileName;
			strSaveDirectory += ".json";

			Json	jLayer;
			Json	jComponent;
			Json	jPrototypeObjects;

			Json	jCloneComponentDesc;

			_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
			map<const _tchar*, CComponent*>*	Components = pGameInstance->Get_ComponentProtoType();
			map<const _tchar*, CGameObject*>	Prototypes = pGameInstance->Get_ProtoTypeObjects();
			CLayer *							Layers = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");

			jLayer["0.Save Level"] = iCurLevel;
			jLayer["1.Save2Load Level"] = iSaveLevel;
			jLayer["2.Layer Tag"] = "Layer_Effect";

			// TOOL
			// 툴에서 생성한 컴포넌트 저장 => Instance컴포넌트 카운트까지 저장함
			for (auto& iter = Components[iCurLevel].begin(); iter != Components[iCurLevel].end(); iter++)
			{
				char     szComponentTag[MAX_PATH] = "";
				if (dynamic_cast<CVIBuffer_Point_Instancing*>(iter->second))
				{
					CUtile::WideCharToChar(iter->first, szComponentTag);
					jComponent["0.Component Tag"] = szComponentTag;
					jComponent["1.Component Cnt"] = dynamic_cast<CVIBuffer_Point_Instancing*>(iter->second)->Get_InstanceNum();
					jLayer["3.Prototype Component"].push_back(jComponent);
				}
			}

			// ProtoType GameObject 
			for (auto& pObject = Prototypes.begin(); pObject != Prototypes.end(); pObject++)
			{
				if (!dynamic_cast<CEffect_Base*>(pObject->second))
					continue;

				char     szPrototypeTag[MAX_PATH] = "";
				CUtile::WideCharToChar(pObject->first, szPrototypeTag);
				jPrototypeObjects["0.Prototype Proto Tag"] = szPrototypeTag;

				_int iEffectType = 0;
				if (dynamic_cast<CEffect_T*>(pObject->second))
					iEffectType = 0;
				if (dynamic_cast<CEffect_Mesh_T*>(pObject->second))
					iEffectType = 2;
				if (dynamic_cast<CEffect_Trail*>(pObject->second))
					iEffectType = 3;

				if (dynamic_cast<CEffect_Point_Instancing*>(pObject->second))
				{
					iEffectType = 1;

					char     szComponentTag[MAX_PATH] = "";
					CUtile::WideCharToChar(dynamic_cast<CEffect_Point_Instancing*>(pObject->second)->Get_VIBufferProtoTag(), szComponentTag);
					jPrototypeObjects["2.Prototype VIBufferComponent Tag"] = szComponentTag;
				}
				jPrototypeObjects["1.Prototype Type"] = iEffectType;
				jLayer["4.Prototype GameObject"].push_back(jPrototypeObjects);
			}

			// Clone GameObject 
			map <const _tchar*, CGameObject*>   GameObjects = Layers->GetGameObjects();
			for (auto& pCloneObject = GameObjects.begin(); pCloneObject != GameObjects.end(); pCloneObject++)
			{
				if (!dynamic_cast<CEffect_Base*>(pCloneObject->second))
					continue;
				Json	jCloneObjects;
				char     szPrototypeTag[MAX_PATH] = "";
				char     szClonetypeTag[MAX_PATH] = "";
				CUtile::WideCharToChar(pCloneObject->second->Get_ProtoObjectName(), szPrototypeTag);
				CUtile::WideCharToChar(pCloneObject->second->Get_ObjectCloneName(), szClonetypeTag);

				jCloneObjects["0.CloneObject Proto Tag"] = szPrototypeTag;
				jCloneObjects["1.CloneObject Clone Tag"] = szClonetypeTag;

#pragma  region	EFFECTDESC
				CEffect_Base* pEffect = dynamic_cast<CEffect_Base*>(pCloneObject->second);
				CEffect_Base::EFFECTDESC eSaveEffectDesc = pEffect->Get_EffectDesc();

				jCloneObjects["2.Effect Type"] = eSaveEffectDesc.eEffectType;
				jCloneObjects["Mesh Type"] = eSaveEffectDesc.eMeshType;
				jCloneObjects["Particle Type"] = eSaveEffectDesc.eParticleType;
				jCloneObjects["TextureRender Type"] = eSaveEffectDesc.eTextureRenderType;
				jCloneObjects["Texture Type"] = eSaveEffectDesc.eTextureType;
				jCloneObjects["Blend Type"] = eSaveEffectDesc.eBlendType;
				jCloneObjects["MoveDir Type"] = eSaveEffectDesc.eMoveDir;
				jCloneObjects["Rotation Type"] = eSaveEffectDesc.eRotation;

				_float fFrame = 0.0f, fMaskFrame;
				for (_int i = 0; i < MAX_TEXTURECNT; ++i) // Texture Idx
				{
					fFrame = 0.0f;
					memcpy(&fFrame, &eSaveEffectDesc.fFrame[i], sizeof(_float));
					jCloneObjects["DiffuseTexture Index"].push_back(fFrame);
				}
				for (_int i = 0; i < MAX_TEXTURECNT; ++i) // Texture Idx
				{
					fMaskFrame = 0.0f;
					memcpy(&fMaskFrame, &eSaveEffectDesc.fMaskFrame[i], sizeof(_float));
					jCloneObjects["MaskTexture Index"].push_back(fMaskFrame);
				}

				_float	vColor = 0.f, vScale = 0.f;
				for (_int i = 0; i < 4; ++i)
				{
					vColor = 0.0f;
					memcpy(&vColor, (_float*)&eSaveEffectDesc.vColor + i, sizeof(_float));
					jCloneObjects["Color"].push_back(vColor);
				}
				for (_int i = 0; i < 4; ++i)
				{
					vScale = 0.0f;
					memcpy(&vScale, (_float*)&eSaveEffectDesc.vScale + i, sizeof(_float));
					jCloneObjects["Scale"].push_back(vColor);
				}

				jCloneObjects["NormalTexture Index"] = eSaveEffectDesc.fNormalFrame;
				jCloneObjects["Width Frame"] = eSaveEffectDesc.fWidthFrame;
				jCloneObjects["Height Frame"] = eSaveEffectDesc.fHeightFrame;

				jCloneObjects["SeparateWidth"] = eSaveEffectDesc.iSeparateWidth;
				jCloneObjects["SeparateHeight"] = eSaveEffectDesc.iSeparateHeight;
				jCloneObjects["WidthCnt"] = eSaveEffectDesc.iWidthCnt;
				jCloneObjects["HeightCnt"] = eSaveEffectDesc.iHeightCnt;

				jCloneObjects["CreateRange"] = eSaveEffectDesc.fCreateRange;
				jCloneObjects["Range"] = eSaveEffectDesc.fRange;
				jCloneObjects["Angle"] = eSaveEffectDesc.fAngle;
				jCloneObjects["MoveDurationTime"] = eSaveEffectDesc.fMoveDurationTime;
				jCloneObjects["Start"] = eSaveEffectDesc.bStart;

				_float	vInitPos = 0.f, vPixedDir = 0.f;
				for (_int i = 0; i < 4; ++i)
				{
					vInitPos = 0.0f;
					memcpy(&vInitPos, (_float*)&eSaveEffectDesc.vInitPos + i, sizeof(_float));
					jCloneObjects["Init Pos"].push_back(vInitPos);
				}
				for (_int i = 0; i < 4; ++i)
				{
					vPixedDir = 0.0f;
					memcpy(&vPixedDir, (_float*)&eSaveEffectDesc.vPixedDir + i, sizeof(_float));
					jCloneObjects["Pixed Dir"].push_back(vPixedDir);
				}

				jCloneObjects["Have Trail"] = eSaveEffectDesc.IsTrail;
				if (eSaveEffectDesc.IsTrail == true)
				{
					jCloneObjects["Active"] = eSaveEffectDesc.bActive;
					jCloneObjects["Alpha Trail"] = eSaveEffectDesc.bAlpha;
					jCloneObjects["Life"] = eSaveEffectDesc.fLife;
					jCloneObjects["Width"] = eSaveEffectDesc.fWidth;
					jCloneObjects["SegmentSize"] = eSaveEffectDesc.fSegmentSize;
				}

				jCloneObjects["Alpha"] = eSaveEffectDesc.fAlpha;
				jCloneObjects["Billboard"] = eSaveEffectDesc.IsBillboard;
				jCloneObjects["Use Normal"] = eSaveEffectDesc.IsNormal;
				jCloneObjects["Use Mask"] = eSaveEffectDesc.IsMask;
				jCloneObjects["Trigger"] = eSaveEffectDesc.IsTrigger;
				jCloneObjects["MovingPosition"] = eSaveEffectDesc.IsMovingPosition;
				jCloneObjects["Use Child"] = eSaveEffectDesc.bUseChild;
				jCloneObjects["Spread"] = eSaveEffectDesc.bSpread;
				jCloneObjects["FreeMove"] = eSaveEffectDesc.bFreeMove;
#pragma  endregion	EFFECTDESC

				// FreeMove
				if (eSaveEffectDesc.bFreeMove == true)
				{
					vector<_float4>* FreePos = pEffect->Get_FreePos();
					_float fFreePos = 0.0f;
					for (_int i = 0; i < FreePos->size(); i++)
					{
						for (_int j = 0; j < 4; j++)
						{
							fFreePos = 0.0f;
							memcpy(&fFreePos, (_float*)&FreePos[i] + j, sizeof(_float));
							jCloneObjects["99.FreePos"].push_back(fFreePos);
						}
					}
				}

				// Trail
				if (eSaveEffectDesc.IsTrail == true)
				{
					char     szPrototypeTag[MAX_PATH] = "";
					char     szClonetypeTag[MAX_PATH] = "";

					//jPrototypeObjects["Trail EffectType"] = dynamic_cast<CEffect_Base*>(pObject->second)->Get_EffectDesc().eEffectType;
					if (dynamic_cast<CEffect_Point_Instancing*>(pEffect)) // VIBuffer_Point_Instancing Data Save
					{
						vector<class CEffect_Trail*> vecTrail = dynamic_cast<CEffect_Point_Instancing*>(pEffect)->Get_TrailEffect();
						_int iTrailSize = (_int)vecTrail.size();
						for (auto& pvecTrail : vecTrail)
						{
							CUtile::WideCharToChar(pvecTrail->Get_ProtoObjectName(), szPrototypeTag);
							CUtile::WideCharToChar(pvecTrail->Get_ObjectCloneName(), szClonetypeTag);
							jCloneObjects["99.Trail Index"] = pvecTrail->Get_ParticleIdx();
						}
					}
					else
					{
						CEffect_Trail* pEffectTrail = dynamic_cast<CEffect_Base*>(pCloneObject->second)->Get_Trail();
						CUtile::WideCharToChar(pEffectTrail->Get_ProtoObjectName(), szPrototypeTag);
						CUtile::WideCharToChar(pEffectTrail->Get_ObjectCloneName(), szClonetypeTag);
					}
					jCloneObjects["99.Trail ProtoTag"] = szPrototypeTag;
					jCloneObjects["99.Trail CloneTag"] = szClonetypeTag;
				}

				_int iChildCnt = pEffect->Get_ChildCnt();
				jCloneObjects["99.Child Count"] = iChildCnt;

				// Child
				if (iChildCnt != 0)
				{
					vector<CEffect_Base*>* pvecChild = dynamic_cast<CEffect_Base*>(pCloneObject->second)->Get_vecChild();
					for (auto& iter = pvecChild->begin(); iter != pvecChild->end(); iter++)
					{
						char     szPrototypeTag[MAX_PATH] = "";
						char     szClonetypeTag[MAX_PATH] = "";

						CUtile::WideCharToChar((*iter)->Get_ProtoObjectName(), szPrototypeTag);
						CUtile::WideCharToChar((*iter)->Get_ObjectCloneName(), szClonetypeTag);
						jCloneObjects["99.Child ProtoTag"] = szPrototypeTag;
						jCloneObjects["99.Child CloneTag"] = szClonetypeTag;
					}
				}

				// Component Desc 저장
				if (dynamic_cast<CEffect_Point_Instancing*>(pEffect)) // VIBuffer_Point_Instancing Data Save
				{
					char     szVIBufferTag[MAX_PATH] = "";
					const _tchar*   szVIBufferWidecharTag = dynamic_cast<CEffect_Point_Instancing*>(pEffect)->Get_VIBufferProtoTag();
					CUtile::WideCharToChar(szVIBufferWidecharTag, szVIBufferTag);

					CEffect_Point_Instancing* pPointInstance = dynamic_cast<CEffect_Point_Instancing*>(pEffect);
					CVIBuffer_Point_Instancing*	pVIBufferPointInstancing = (CVIBuffer_Point_Instancing*)pPointInstance->Find_Component(L"Com_VIBuffer");
					jCloneComponentDesc["CloneObject Component CloneTag"] = szVIBufferTag;

#pragma region Instance Data

					CVIBuffer_Point_Instancing::INSTANCEDATA* InstanceData = pVIBufferPointInstancing->Get_InstanceData();

					_int iInstanceNum = pVIBufferPointInstancing->Get_InstanceNum();
					_float fPos = 0.0f;
					jCloneComponentDesc["0. Instance DataCnt"] = iInstanceNum;
					for (_int k = 0; k < 4; k++)
					{
						fPos = 0.0f;
						memcpy(&fPos, (_float*)&InstanceData->fPos + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component InstnaceData Position"].push_back(fPos);
					}

					jCloneComponentDesc["CloneObject Component InstnaceData Speed"] = InstanceData->pSpeeds;
					jCloneComponentDesc["CloneObject Component InstnaceData SpeedMinMin"] = InstanceData->SpeedMinMax.x;
					jCloneComponentDesc["CloneObject Component InstnaceData SpeedMinMax"] = InstanceData->SpeedMinMax.y;
					jCloneComponentDesc["CloneObject Component InstnaceData PSizeX"] = InstanceData->fPSize.x;
					jCloneComponentDesc["CloneObject Component InstnaceData PSizeY"] = InstanceData->fPSize.y;
#pragma endregion Instance Data

#pragma region Point Desc Data
					CVIBuffer_Point_Instancing::POINTDESC* PointDesc = pVIBufferPointInstancing->Get_PointDesc();

					_float fDir = 0.0f, fMin = 0.0f, fMax = 0.0f, fCirclePos = 0.0f, fConeRange = 0.0f, vOriginPos, vExplosionDir;
					jCloneComponentDesc["0. Point Data"] = "PointDesc Data";
					for (_int k = 0; k < 3; k++)
					{
						fMin = 0.0f;
						memcpy(&fMin, (_float*)&PointDesc->fMin + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc Min"].push_back(fMin);
					}
					for (_int k = 0; k < 3; k++)
					{
						fMax = 0.0f;
						memcpy(&fMax, (_float*)&PointDesc->fMax + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc Max"].push_back(fMax);
					}
					for (_int k = 0; k < 4; k++)
					{
						fCirclePos = 0.0f;
						memcpy(&fCirclePos, (_float*)&PointDesc->vCirclePos + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc CirclePos"].push_back(fCirclePos);
					}
					for (_int k = 0; k < 2; k++)
					{
						fConeRange = 0.0f;
						memcpy(&fCirclePos, (_float*)&PointDesc->fConeRange + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc ConeRange"].push_back(fConeRange);
					}
					for (_int k = 0; k < 4; k++)
					{
						vOriginPos = 0.0f;
						memcpy(&vOriginPos, (_float*)&PointDesc->vOriginPos + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc OriginPos"].push_back(vOriginPos);
					}
					for (_int k = 0; k < 4; k++)
					{
						vExplosionDir = 0.0f;
						memcpy(&vExplosionDir, (_float*)&PointDesc->vExplosionDir + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc ExplosionDir"].push_back(vExplosionDir);
					}
					for (_int k = 0; k < 4; k++)
					{
						fDir = 0.0f;
						memcpy(&fDir, (_float*)&PointDesc->vDir + k, sizeof(_float));
						jCloneComponentDesc["CloneObject Component PointDesc Dir"].push_back(fDir);
					}

					for (_int j = 0; j < iInstanceNum; j++)
					{
						jCloneComponentDesc["1. Instance Num"] = j;
						jCloneComponentDesc["CloneObject Component PointDesc ShapeType"] = PointDesc->eShapeType;
						jCloneComponentDesc["CloneObject Component PointDesc RotXYZ"] = PointDesc->eRotXYZ;
						jCloneComponentDesc["CloneObject Component PointDesc Range"] = PointDesc->fRange;
						jCloneComponentDesc["CloneObject Component PointDesc CreateInstance"] = PointDesc->iCreateInstance;
						jCloneComponentDesc["CloneObject Component PointDesc InstanceIndex"] = PointDesc->iInstanceIndex;
						jCloneComponentDesc["CloneObject Component PointDesc Spread"] = PointDesc->bSpread;
						jCloneComponentDesc["CloneObject Component PointDesc IsAlive"] = PointDesc->bIsAlive;
						jCloneComponentDesc["CloneObject Component PointDesc bMoveY"] = PointDesc->bMoveY;
						jCloneComponentDesc["CloneObject Component PointDesc Rotation"] = PointDesc->bRotation;
						jCloneComponentDesc["CloneObject Component PointDesc MoveY"] = PointDesc->fMoveY;
						jCloneComponentDesc["CloneObject Component PointDesc CreateRange"] = PointDesc->fCreateRange;
						jCloneComponentDesc["CloneObject Component PointDesc DurationTime"] = PointDesc->fDurationTime;
						jCloneComponentDesc["CloneObject Component PointDesc MaxTime"] = PointDesc->fMaxTime;
						jCloneComponentDesc["CloneObject Component PointDesc RangeOffset"] = PointDesc->fRangeOffset;
						jCloneComponentDesc["CloneObject Component PointDesc RangeY"] = PointDesc->fRangeY;
					}
#pragma endregion Point Desc Data

				}
				jCloneObjects["CloneObject Component"] = jCloneComponentDesc;
				jLayer["3.Clone GameObject"].push_back(jCloneObjects);
				jCloneComponentDesc.clear();
			}

			ofstream	file(strSaveDirectory.c_str());
			file << jLayer;
			file.close();
			//
			bSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
#pragma endregion SaveData
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			bSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
			ImGuiFileDialog::Instance()->Close();
		}
	}

	if (ImGuiFileDialog::Instance()->Display("Select EffectComponent"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region SaveData
			string		strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
			strSaveDirectory += "\\";
			strSaveDirectory += szSaveFileName;
			strSaveDirectory += ".json";

			Json	jLayer;
			Json	jComponent;
			Json	jPrototypeObjects;

			Json	jCloneComponentDesc;

			_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
			map<const _tchar*, CComponent*>*	Components = pGameInstance->Get_ComponentProtoType();
			map<const _tchar*, CGameObject*>	Prototypes = pGameInstance->Get_ProtoTypeObjects();
			CLayer *							Layers = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");

			jLayer["0.Save Level"] = iCurLevel;
			jLayer["1.Save2Load Level"] = iSaveLevel;
			jLayer["2.Layer Tag"] = "Layer_Effect";

			// TOOL
			// 툴에서 생성한 컴포넌트 저장 => Instance컴포넌트 카운트까지 저장함
			for (auto& iter = Components[iCurLevel].begin(); iter != Components[iCurLevel].end(); iter++)
			{
				char     szComponentTag[MAX_PATH] = "";
				if (dynamic_cast<CVIBuffer_Point_Instancing*>(iter->second))
				{
					CUtile::WideCharToChar(iter->first, szComponentTag);
					jComponent["0.Component Tag"] = szComponentTag;
					jComponent["1.Component Cnt"] = dynamic_cast<CVIBuffer_Point_Instancing*>(iter->second)->Get_InstanceNum();
					jLayer["3.Prototype Component"].push_back(jComponent);
				}
			}

			// ProtoType GameObject 
			for (auto& pObject = Prototypes.begin(); pObject != Prototypes.end(); pObject++)
			{
				if (!dynamic_cast<CEffect_Base*>(pObject->second))
					continue;

				char     szPrototypeTag[MAX_PATH] = "";
				CUtile::WideCharToChar(pObject->first, szPrototypeTag);
				jPrototypeObjects["0.Prototype Proto Tag"] = szPrototypeTag;

				_int iEffectType = 0;
				if (dynamic_cast<CEffect_T*>(pObject->second))
					iEffectType = 0;
				if (dynamic_cast<CEffect_Mesh_T*>(pObject->second))
					iEffectType = 2;
				if (dynamic_cast<CEffect_Trail*>(pObject->second))
					iEffectType = 3;

				if (dynamic_cast<CEffect_Point_Instancing*>(pObject->second))
				{
					iEffectType = 1;

					char     szComponentTag[MAX_PATH] = "";
					CUtile::WideCharToChar(dynamic_cast<CEffect_Point_Instancing*>(pObject->second)->Get_VIBufferProtoTag(), szComponentTag);
					jPrototypeObjects["2.Prototype VIBufferComponent Tag"] = szComponentTag;
				}
				jPrototypeObjects["1.Prototype Type"] = iEffectType;
				jLayer["4.Prototype GameObject"].push_back(jPrototypeObjects);
			}

			ofstream	file(strSaveDirectory.c_str());
			file << jLayer;
			file.close();
			//
			bSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
#pragma endregion SaveComponentData
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			bSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
			ImGuiFileDialog::Instance()->Close();
		}
	}

	if (ImGuiFileDialog::Instance()->Display("Select EffectDesc"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region SaveData
			if (pGameObject == nullptr)
			{
				bDescSave = false;
				iSaveLevel = 0;
				strcpy_s(szSaveFileName, "");
				ImGuiFileDialog::Instance()->Close();
			}

			string		strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
			strSaveDirectory += "\\";
			strSaveDirectory += szSaveFileName;
			strSaveDirectory += ".json";

			Json	jLayer;
			Json	jComponent;
			Json	jPrototypeObjects;
			Json	jCloneComponentDesc;

			_int    iCurLevel = pGameInstance->Get_CurLevelIndex();

			jLayer["0.Save Level"] = iCurLevel;
			jLayer["1.Save2Load Level"] = iSaveLevel;
			jLayer["2.Layer Tag"] = "Layer_Effect";

			// Clone GameObject 
			Json	jCloneObjects;

#pragma  region	EFFECTDESC
			CEffect_Base* pEffect = dynamic_cast<CEffect_Base*>(pGameObject);
			CEffect_Base::EFFECTDESC eSaveEffectDesc = pEffect->Get_EffectDesc();

			jCloneObjects["2.Effect Type"] = eSaveEffectDesc.eEffectType;
			jCloneObjects["Mesh Type"] = eSaveEffectDesc.eMeshType;
			jCloneObjects["Particle Type"] = eSaveEffectDesc.eParticleType;
			jCloneObjects["TextureRender Type"] = eSaveEffectDesc.eTextureRenderType;
			jCloneObjects["Texture Type"] = eSaveEffectDesc.eTextureType;
			jCloneObjects["Blend Type"] = eSaveEffectDesc.eBlendType;
			jCloneObjects["MoveDir Type"] = eSaveEffectDesc.eMoveDir;
			jCloneObjects["Rotation Type"] = eSaveEffectDesc.eRotation;

			_float fFrame = 0.0f, fMaskFrame;
			for (_int i = 0; i < MAX_TEXTURECNT; ++i) // Texture Idx
			{
				fFrame = 0.0f;
				memcpy(&fFrame, &eSaveEffectDesc.fFrame[i], sizeof(_float));
				jCloneObjects["DiffuseTexture Index"].push_back(fFrame);
			}
			for (_int i = 0; i < MAX_TEXTURECNT; ++i) // Texture Idx
			{
				fMaskFrame = 0.0f;
				memcpy(&fMaskFrame, &eSaveEffectDesc.fMaskFrame[i], sizeof(_float));
				jCloneObjects["MaskTexture Index"].push_back(fMaskFrame);
			}

			_float	vColor = 0.f, vScale = 0.f;
			for (_int i = 0; i < 4; ++i)
			{
				vColor = 0.0f;
				memcpy(&vColor, (_float*)&eSaveEffectDesc.vColor + i, sizeof(_float));
				jCloneObjects["Color"].push_back(vColor);
			}
			for (_int i = 0; i < 4; ++i)
			{
				vScale = 0.0f;
				memcpy(&vScale, (_float*)&eSaveEffectDesc.vScale + i, sizeof(_float));
				jCloneObjects["Scale"].push_back(vColor);
			}

			jCloneObjects["NormalTexture Index"] = eSaveEffectDesc.fNormalFrame;
			jCloneObjects["Width Frame"] = eSaveEffectDesc.fWidthFrame;
			jCloneObjects["Height Frame"] = eSaveEffectDesc.fHeightFrame;

			jCloneObjects["SeparateWidth"] = eSaveEffectDesc.iSeparateWidth;
			jCloneObjects["SeparateHeight"] = eSaveEffectDesc.iSeparateHeight;
			jCloneObjects["WidthCnt"] = eSaveEffectDesc.iWidthCnt;
			jCloneObjects["HeightCnt"] = eSaveEffectDesc.iHeightCnt;

			jCloneObjects["CreateRange"] = eSaveEffectDesc.fCreateRange;
			jCloneObjects["Range"] = eSaveEffectDesc.fRange;
			jCloneObjects["Angle"] = eSaveEffectDesc.fAngle;
			jCloneObjects["MoveDurationTime"] = eSaveEffectDesc.fMoveDurationTime;
			jCloneObjects["Start"] = eSaveEffectDesc.bStart;

			_float	vInitPos = 0.f, vPixedDir = 0.f;
			for (_int i = 0; i < 4; ++i)
			{
				vInitPos = 0.0f;
				memcpy(&vInitPos, (_float*)&eSaveEffectDesc.vInitPos + i, sizeof(_float));
				jCloneObjects["Init Pos"].push_back(vInitPos);
			}
			for (_int i = 0; i < 4; ++i)
			{
				vPixedDir = 0.0f;
				memcpy(&vPixedDir, (_float*)&eSaveEffectDesc.vPixedDir + i, sizeof(_float));
				jCloneObjects["Pixed Dir"].push_back(vPixedDir);
			}

			jCloneObjects["Have Trail"] = eSaveEffectDesc.IsTrail;
			if (eSaveEffectDesc.IsTrail == true)
			{
				jCloneObjects["Active"] = eSaveEffectDesc.bActive;
				jCloneObjects["Alpha Trail"] = eSaveEffectDesc.bAlpha;
				jCloneObjects["Life"] = eSaveEffectDesc.fLife;
				jCloneObjects["Width"] = eSaveEffectDesc.fWidth;
				jCloneObjects["SegmentSize"] = eSaveEffectDesc.fSegmentSize;
			}

			jCloneObjects["Alpha"] = eSaveEffectDesc.fAlpha;
			jCloneObjects["Billboard"] = eSaveEffectDesc.IsBillboard;
			jCloneObjects["Use Normal"] = eSaveEffectDesc.IsNormal;
			jCloneObjects["Use Mask"] = eSaveEffectDesc.IsMask;
			jCloneObjects["Trigger"] = eSaveEffectDesc.IsTrigger;
			jCloneObjects["MovingPosition"] = eSaveEffectDesc.IsMovingPosition;
			jCloneObjects["Use Child"] = eSaveEffectDesc.bUseChild;
			jCloneObjects["Spread"] = eSaveEffectDesc.bSpread;
			jCloneObjects["FreeMove"] = eSaveEffectDesc.bFreeMove;
#pragma  endregion	EFFECTDESC

			_int iDTextureCnt = 0, iMTextureCnt = 0;
			jCloneObjects["Have DTextureCnt"] = pEffect->Get_TotalDTextureCnt();
			jCloneObjects["Have MTextureCnt"] = pEffect->Get_TotalMTextureCnt();

			// FreeMove
			if (eSaveEffectDesc.bFreeMove == true)
			{
				vector<_float4>* FreePos = pEffect->Get_FreePos();
				_float fFreePos = 0.0f;
				for (_int i = 0; i < FreePos->size(); i++)
				{
					for (_int j = 0; j < 4; j++)
					{
						fFreePos = 0.0f;
						memcpy(&fFreePos, (_float*)&FreePos[i] + j, sizeof(_float));
						jCloneObjects["99.FreePos"].push_back(fFreePos);
					}
				}
			}

			// Trail
			if (eSaveEffectDesc.IsTrail == true)
			{
				char     szPrototypeTag[MAX_PATH] = "";
				char     szClonetypeTag[MAX_PATH] = "";

				if (dynamic_cast<CEffect_Point_Instancing*>(pEffect)) // VIBuffer_Point_Instancing Data Save
				{
					vector<class CEffect_Trail*> vecTrail = dynamic_cast<CEffect_Point_Instancing*>(pEffect)->Get_TrailEffect();
					_int iTrailSize = (_int)vecTrail.size();
					for (auto& pvecTrail : vecTrail)
					{
						CUtile::WideCharToChar(pvecTrail->Get_ProtoObjectName(), szPrototypeTag);
						CUtile::WideCharToChar(pvecTrail->Get_ObjectCloneName(), szClonetypeTag);
						jCloneObjects["99.Trail Index"] = pvecTrail->Get_ParticleIdx();
					}
				}
				else
				{
					CEffect_Trail* pEffectTrail = dynamic_cast<CEffect_Base*>(pGameObject)->Get_Trail();
					CUtile::WideCharToChar(pEffectTrail->Get_ProtoObjectName(), szPrototypeTag);
					CUtile::WideCharToChar(pEffectTrail->Get_ObjectCloneName(), szClonetypeTag);
				}
				jCloneObjects["99.Trail ProtoTag"] = szPrototypeTag;
				jCloneObjects["99.Trail CloneTag"] = szClonetypeTag;
			}

			_int iChildCnt = pEffect->Get_ChildCnt();
			jCloneObjects["99.Child Count"] = iChildCnt;

			// Child
			if (iChildCnt != 0)
			{
				vector<CEffect_Base*>* pvecChild = dynamic_cast<CEffect_Base*>(pGameObject)->Get_vecChild();
				for (auto& iter = pvecChild->begin(); iter != pvecChild->end(); iter++)
				{
					char     szPrototypeTag[MAX_PATH] = "";
					char     szClonetypeTag[MAX_PATH] = "";

					CUtile::WideCharToChar((*iter)->Get_ProtoObjectName(), szPrototypeTag);
					CUtile::WideCharToChar((*iter)->Get_ObjectCloneName(), szClonetypeTag);
					jCloneObjects["99.Child ProtoTag"] = szPrototypeTag;
					jCloneObjects["99.Child CloneTag"] = szClonetypeTag;
				}
			}

			// Component Desc 저장
			if (dynamic_cast<CEffect_Point_Instancing*>(pEffect)) // VIBuffer_Point_Instancing Data Save
			{
				char     szVIBufferTag[MAX_PATH] = "";
				const _tchar*   szVIBufferWidecharTag = dynamic_cast<CEffect_Point_Instancing*>(pEffect)->Get_VIBufferProtoTag();
				CUtile::WideCharToChar(szVIBufferWidecharTag, szVIBufferTag);

				CEffect_Point_Instancing* pPointInstance = dynamic_cast<CEffect_Point_Instancing*>(pEffect);
				CVIBuffer_Point_Instancing*	pVIBufferPointInstancing = (CVIBuffer_Point_Instancing*)pPointInstance->Find_Component(L"Com_VIBuffer");
				jCloneComponentDesc["CloneObject Component CloneTag"] = szVIBufferTag;

#pragma region Instance Data

				CVIBuffer_Point_Instancing::INSTANCEDATA* InstanceData = pVIBufferPointInstancing->Get_InstanceData();

				_int iInstanceNum = pVIBufferPointInstancing->Get_InstanceNum();
				_float fPos = 0.0f;
				jCloneComponentDesc["0. Instance DataCnt"] = iInstanceNum;
				for (_int k = 0; k < 4; k++)
				{
					fPos = 0.0f;
					memcpy(&fPos, (_float*)&InstanceData->fPos + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component InstnaceData Position"].push_back(fPos);
				}

				jCloneComponentDesc["CloneObject Component InstnaceData Speed"] = InstanceData->pSpeeds;
				jCloneComponentDesc["CloneObject Component InstnaceData SpeedMinMin"] = InstanceData->SpeedMinMax.x;
				jCloneComponentDesc["CloneObject Component InstnaceData SpeedMinMax"] = InstanceData->SpeedMinMax.y;
				jCloneComponentDesc["CloneObject Component InstnaceData PSizeX"] = InstanceData->fPSize.x;
				jCloneComponentDesc["CloneObject Component InstnaceData PSizeY"] = InstanceData->fPSize.y;
#pragma endregion Instance Data

#pragma region Point Desc Data
				CVIBuffer_Point_Instancing::POINTDESC* PointDesc = pVIBufferPointInstancing->Get_PointDesc();

				_float fDir = 0.0f, fMin = 0.0f, fMax = 0.0f, fCirclePos = 0.0f, fConeRange = 0.0f, vOriginPos, vExplosionDir;
				jCloneComponentDesc["0. Point Data"] = "PointDesc Data";
				for (_int k = 0; k < 3; k++)
				{
					fMin = 0.0f;
					memcpy(&fMin, (_float*)&PointDesc->fMin + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc Min"].push_back(fMin);
				}
				for (_int k = 0; k < 3; k++)
				{
					fMax = 0.0f;
					memcpy(&fMax, (_float*)&PointDesc->fMax + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc Max"].push_back(fMax);
				}
				for (_int k = 0; k < 4; k++)
				{
					fCirclePos = 0.0f;
					memcpy(&fCirclePos, (_float*)&PointDesc->vCirclePos + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc CirclePos"].push_back(fCirclePos);
				}
				for (_int k = 0; k < 2; k++)
				{
					fConeRange = 0.0f;
					memcpy(&fCirclePos, (_float*)&PointDesc->fConeRange + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc ConeRange"].push_back(fConeRange);
				}
				for (_int k = 0; k < 4; k++)
				{
					vOriginPos = 0.0f;
					memcpy(&vOriginPos, (_float*)&PointDesc->vOriginPos + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc OriginPos"].push_back(vOriginPos);
				}
				for (_int k = 0; k < 4; k++)
				{
					vExplosionDir = 0.0f;
					memcpy(&vExplosionDir, (_float*)&PointDesc->vExplosionDir + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc ExplosionDir"].push_back(vExplosionDir);
				}
				for (_int k = 0; k < 4; k++)
				{
					fDir = 0.0f;
					memcpy(&fDir, (_float*)&PointDesc->vDir + k, sizeof(_float));
					jCloneComponentDesc["CloneObject Component PointDesc Dir"].push_back(fDir);
				}

				for (_int j = 0; j < iInstanceNum; j++)
				{
					jCloneComponentDesc["1. Instance Num"] = j;
					jCloneComponentDesc["CloneObject Component PointDesc ShapeType"] = PointDesc->eShapeType;
					jCloneComponentDesc["CloneObject Component PointDesc RotXYZ"] = PointDesc->eRotXYZ;
					jCloneComponentDesc["CloneObject Component PointDesc Range"] = PointDesc->fRange;
					jCloneComponentDesc["CloneObject Component PointDesc CreateInstance"] = PointDesc->iCreateInstance;
					jCloneComponentDesc["CloneObject Component PointDesc InstanceIndex"] = PointDesc->iInstanceIndex;
					jCloneComponentDesc["CloneObject Component PointDesc Spread"] = PointDesc->bSpread;
					jCloneComponentDesc["CloneObject Component PointDesc IsAlive"] = PointDesc->bIsAlive;
					jCloneComponentDesc["CloneObject Component PointDesc bMoveY"] = PointDesc->bMoveY;
					jCloneComponentDesc["CloneObject Component PointDesc Rotation"] = PointDesc->bRotation;
					jCloneComponentDesc["CloneObject Component PointDesc MoveY"] = PointDesc->fMoveY;
					jCloneComponentDesc["CloneObject Component PointDesc CreateRange"] = PointDesc->fCreateRange;
					jCloneComponentDesc["CloneObject Component PointDesc DurationTime"] = PointDesc->fDurationTime;
					jCloneComponentDesc["CloneObject Component PointDesc MaxTime"] = PointDesc->fMaxTime;
					jCloneComponentDesc["CloneObject Component PointDesc RangeOffset"] = PointDesc->fRangeOffset;
					jCloneComponentDesc["CloneObject Component PointDesc RangeY"] = PointDesc->fRangeY;
				}
#pragma endregion Point Desc Data

			}
			jCloneObjects["CloneObject Component"] = jCloneComponentDesc;
			jLayer["3.Clone GameObject"].push_back(jCloneObjects);
			jCloneComponentDesc.clear();

			ofstream	file(strSaveDirectory.c_str());
			file << jLayer;
			file.close();
			//
			bDescSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
#pragma endregion SaveDesc
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			bDescSave = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
			ImGuiFileDialog::Instance()->Close();
		}
	}

	
	if (ImGuiFileDialog::Instance()->Display("Select EffectLoad"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region LoadData
			string		strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

			Json	jLayer;
			Json	jComponent;
			Json	jPrototypeObjects;
			Json	jCloneObjects;
			Json	jCloneComponentDesc;

			ifstream	file(strFilePath.c_str());
			file >> jLayer;
			file.close();

			//pGameInstance->Clear_Level(iCurLevel);

			_int iCurLevel = 0, iSave2LoadLevel = 0;
			string strLayerTag = "";
			jLayer["0.Save Level"].get_to<int>(iCurLevel);
			jLayer["1.Save2Load Level"].get_to<int>(iSave2LoadLevel);
			jLayer["2.Layer Tag"].get_to<string>(strLayerTag);

			pGameInstance->Clear_Level(iCurLevel);
			// Component Create
			for (auto jComponent : jLayer["3.Prototype Component"])
			{
				string		strComponentTag = "";
				_int		iComponentCnt = 0;

				jComponent["0.Component Tag"].get_to<string>(strComponentTag);
				jComponent["1.Component Cnt"].get_to<int>(iComponentCnt);

				_tchar szComponentTag[128] = L"";
				CUtile::CharToWideChar(strComponentTag.c_str(), szComponentTag);
				_tchar* szComponentFinalTag = CUtile::Create_String(szComponentTag);
				pGameInstance->Add_String(szComponentFinalTag);

				if (FAILED(pGameInstance->Add_Prototype(iSave2LoadLevel, szComponentFinalTag, CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, iComponentCnt))))
					return;
			}

			for (auto jPrototypeObjects : jLayer["4.Prototype GameObject"])
			{
				string		strPrototypeTag = "";
				string		strComponentTag = "";
				_tchar*     szComponentTag = nullptr;
				_int		iEffectType = 0;
				_tchar*    szComponentFinalTag = L"";
				_tchar    szVIBufferTag[128] = L"";

				jPrototypeObjects["0.Prototype Proto Tag"].get_to<string>(strPrototypeTag);
				jPrototypeObjects["1.Prototype Type"].get_to<_int>(iEffectType);
				// enum EFFECTTYPE { EFFECT_PLANE, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_TRAIL, EFFECT_END };

				_tchar szProtoTag[128] = L"";
				CUtile::CharToWideChar(strPrototypeTag.c_str(), szProtoTag);
				_tchar* szPrototypeFinalTag = CUtile::Create_String(szProtoTag);
				pGameInstance->Add_String(szPrototypeFinalTag);

				switch (iEffectType)
				{
				case 0:
					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_T::Create(m_pDevice, m_pContext))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				case 1:
					jPrototypeObjects["2.Prototype VIBufferComponent Tag"].get_to<string>(strComponentTag);

					CUtile::CharToWideChar(strComponentTag.c_str(), szVIBufferTag);
					szComponentFinalTag = CUtile::Create_String(szVIBufferTag);
					pGameInstance->Add_String(szComponentFinalTag);

					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_Point_Instancing::Create(m_pDevice, m_pContext, szComponentFinalTag))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				case 2:
					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_Mesh_T::Create(m_pDevice, m_pContext))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				case 3:
					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_Trail::Create(m_pDevice, m_pContext))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				}
			}

			// Clone GameObject
			_int iEnum = 0, iEffectType = 0;
			CEffect_Base::EFFECTDESC eLoadEffectDesc;
			for (auto jCloneObjects : jLayer["3.Clone GameObject"])
			{
				string		strPrototypeTag = "";
				string		strComponentTag = "";

				jCloneObjects["0.CloneObject Proto Tag"].get_to<string>(strPrototypeTag);
				jCloneObjects["1.CloneObject Clone Tag"].get_to<string>(strComponentTag);

				_tchar szProtoTag[128] = L"";
				_tchar szComponentTag[128] = L"";
				CUtile::CharToWideChar(strPrototypeTag.c_str(), szProtoTag);
				CUtile::CharToWideChar(strComponentTag.c_str(), szComponentTag);

				_tchar* szProtoFinalTag = CUtile::Create_String(szProtoTag);
				pGameInstance->Add_String(szProtoFinalTag);

				_tchar* szComponentFinalTag = CUtile::Create_String(szComponentTag);
				pGameInstance->Add_String(szComponentFinalTag);

				// ZeroMemory(&eLoadEffectDesc, sizeof(CEffect_Base::EFFECTDESC));
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

				//_float fFrame = 0.0f, fMaskFrame = 0.0f;
				//_float	vColor = 0.f, vScale = 0.f;
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

				CGameObject* pEffectBase = nullptr;
				if (FAILED(pGameInstance->Clone_GameObject(iSave2LoadLevel, L"Layer_Effect", szProtoFinalTag, szComponentFinalTag, &eLoadEffectDesc, &pEffectBase)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

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
					dynamic_cast<CEffect_Base*>(pEffectBase)->Set_InitPos(FreePos);
					FreePos.clear();
				}

				// Trail
				string strTrailPrototypeTag = "", strTrailCloneTag = "";
				if (eLoadEffectDesc.IsTrail == true)
				{
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

						dynamic_cast<CEffect_Point_Instancing*>(pEffectBase)->Set_Trail(dynamic_cast<CEffect_Point_Instancing*>(pEffectBase), szTrailPrototypeTag);
					}
					else
					{
						jCloneObjects["99.Trail ProtoTag"].get_to<string>(strTrailPrototypeTag);
						jCloneObjects["99.Trail CloneTag"].get_to<string>(strTrailCloneTag);

						_tchar szConvertTag[128] = L"";
						CUtile::CharToWideChar(strTrailPrototypeTag.c_str(), szConvertTag);
						_tchar* szTrailPrototypeTag = CUtile::Create_String(szConvertTag);
						pGameInstance->Add_String(szTrailPrototypeTag);

						dynamic_cast<CEffect_Base*>(pEffectBase)->Set_InitTrail(szTrailPrototypeTag, 1);
					}
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
						dynamic_cast<CEffect_Base*>(pEffectBase)->Set_InitChild(iChildCnt, szProtoTag);
					}
				}

				// Component Desc 저장
				if (iEffectType == 1) // VIBuffer_Point_Instancing Data Save
				{
					//string strComponentTag = "";
					//jCloneComponentDesc["CloneObject Component CloneTag"].get_to<string>(strComponentTag);

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

					dynamic_cast<CEffect_Point_Instancing*>(pEffectBase)->Set_PointInstanceDesc(PointDesc);
					dynamic_cast<CEffect_Point_Instancing*>(pEffectBase)->Set_InstanceData(InstanceData);
					dynamic_cast<CEffect_Point_Instancing*>(pEffectBase)->Set_ShapePosition();

					Safe_Delete_Array(PointDesc);
					Safe_Delete_Array(InstanceData);
				}

			}
#pragma endregion LoadData
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			bLoad = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
			ImGuiFileDialog::Instance()->Close();
		}
	}
	if (ImGuiFileDialog::Instance()->Display("Select EffectComponentLoad"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region LoadData
			string		strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

			Json	jLayer;
			Json	jComponent;
			Json	jPrototypeObjects;
			Json	jCloneObjects;
			Json	jCloneComponentDesc;

			ifstream	file(strFilePath.c_str());
			file >> jLayer;
			file.close();

			//pGameInstance->Clear_Level(iCurLevel);

			_int iCurLevel = 0, iSave2LoadLevel = 0;
			string strLayerTag = "";
			jLayer["0.Save Level"].get_to<int>(iCurLevel);
			jLayer["1.Save2Load Level"].get_to<int>(iSave2LoadLevel);
			jLayer["2.Layer Tag"].get_to<string>(strLayerTag);

			pGameInstance->Clear_Level(iCurLevel);
			// Component Create
			for (auto jComponent : jLayer["3.Prototype Component"])
			{
				string		strComponentTag = "";
				_int		iComponentCnt = 0;

				jComponent["0.Component Tag"].get_to<string>(strComponentTag);
				jComponent["1.Component Cnt"].get_to<int>(iComponentCnt);

				_tchar szComponentTag[128] = L"";
				CUtile::CharToWideChar(strComponentTag.c_str(), szComponentTag);
				_tchar* szComponentFinalTag = CUtile::Create_String(szComponentTag);
				pGameInstance->Add_String(szComponentFinalTag);

				if (FAILED(pGameInstance->Add_Prototype(iSave2LoadLevel, szComponentFinalTag, CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, iComponentCnt))))
					return;
			}

			for (auto jPrototypeObjects : jLayer["4.Prototype GameObject"])
			{
				string		strPrototypeTag = "";
				string		strComponentTag = "";
				_tchar*     szComponentTag = nullptr;
				_int		iEffectType = 0;
				_tchar*    szComponentFinalTag = L"";
				_tchar    szVIBufferTag[128] = L"";

				jPrototypeObjects["0.Prototype Proto Tag"].get_to<string>(strPrototypeTag);
				jPrototypeObjects["1.Prototype Type"].get_to<_int>(iEffectType);
				// enum EFFECTTYPE { EFFECT_PLANE, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_TRAIL, EFFECT_END };

				_tchar szProtoTag[128] = L"";
				CUtile::CharToWideChar(strPrototypeTag.c_str(), szProtoTag);
				_tchar* szPrototypeFinalTag = CUtile::Create_String(szProtoTag);
				pGameInstance->Add_String(szPrototypeFinalTag);

				switch (iEffectType)
				{
				case 0:
					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_T::Create(m_pDevice, m_pContext))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				case 1:
					jPrototypeObjects["2.Prototype VIBufferComponent Tag"].get_to<string>(strComponentTag);

					CUtile::CharToWideChar(strComponentTag.c_str(), szVIBufferTag);
					szComponentFinalTag = CUtile::Create_String(szVIBufferTag);
					pGameInstance->Add_String(szComponentFinalTag);

					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_Point_Instancing::Create(m_pDevice, m_pContext, szComponentFinalTag))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				case 2:
					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_Mesh_T::Create(m_pDevice, m_pContext))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				case 3:
					if (FAILED(pGameInstance->Add_Prototype(szPrototypeFinalTag, CEffect_Trail::Create(m_pDevice, m_pContext))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}
					break;
				}
			}
#pragma endregion LoadComponentData
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			bComponentLoad = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
			ImGuiFileDialog::Instance()->Close();
		}
	}

	// Desc만 저장하는거라 Proto + Clone Object 이름 저장 안함
	if (ImGuiFileDialog::Instance()->Display("Select EffectDescLoad"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
#pragma region LoadData
			string		strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

			Json	jLayer;
			Json	jComponent;
			Json	jPrototypeObjects;
			Json	jCloneObjects;
			Json	jCloneComponentDesc;

			ifstream	file(strFilePath.c_str());
			file >> jLayer;
			file.close();

			//pGameInstance->Clear_Level(iCurLevel);

			_int iCurLevel = 0, iSave2LoadLevel = 0;
			string strLayerTag = "";
			jLayer["0.Save Level"].get_to<int>(iCurLevel);
			jLayer["1.Save2Load Level"].get_to<int>(iSave2LoadLevel);
			jLayer["2.Layer Tag"].get_to<string>(strLayerTag);

			pGameInstance->Clear_Level(iCurLevel);

			// Clone GameObject
			_int iEnum = 0, iEffectType = 0;
			CEffect_Base::EFFECTDESC eLoadEffectDesc;
			for (auto jCloneObjects : jLayer["3.Clone GameObject"])
			{
				//string		strPrototypeTag = "";
				//string		strComponentTag = "";

				//jCloneObjects["0.CloneObject Proto Tag"].get_to<string>(strPrototypeTag);
				//jCloneObjects["1.CloneObject Clone Tag"].get_to<string>(strComponentTag);

				//_tchar szProtoTag[128] = L"";
				//_tchar szComponentTag[128] = L"";
				//CUtile::CharToWideChar(strPrototypeTag.c_str(), szProtoTag);
				//CUtile::CharToWideChar(strComponentTag.c_str(), szComponentTag);

				//_tchar* szProtoFinalTag = CUtile::Create_String(szProtoTag);
				//pGameInstance->Add_String(szProtoFinalTag);

				//_tchar* szComponentFinalTag = CUtile::Create_String(szComponentTag);
				//pGameInstance->Add_String(szComponentFinalTag);

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

				//_float fFrame = 0.0f, fMaskFrame = 0.0f;
				//_float	vColor = 0.f, vScale = 0.f;
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

				//CGameObject* pEffectBase = nullptr;
				//if (FAILED(pGameInstance->Clone_GameObject(iSave2LoadLevel, L"Layer_Effect", szProtoFinalTag, szComponentFinalTag, &eLoadEffectDesc, &pEffectBase)))
				//{
				//	RELEASE_INSTANCE(CGameInstance);
				//	return;
				//}

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
					// dynamic_cast<CEffect_Base*>(pEffectBase)->Set_InitPos(FreePos);
					dynamic_cast<CEffect_Base*>(this)->Set_InitPos(FreePos);
					FreePos.clear();
				}

				// Trail
				string strTrailPrototypeTag = "", strTrailCloneTag = "";
				if (eLoadEffectDesc.IsTrail == true)
				{
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
						//dynamic_cast<CEffect_Point_Instancing*>(pEffectBase)->Set_Trail(dynamic_cast<CEffect_Point_Instancing*>(pEffectBase), szTrailPrototypeTag);
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
						//dynamic_cast<CEffect_Base*>(pEffectBase)->Set_InitTrail(szTrailPrototypeTag, 1);
					}
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
						//dynamic_cast<CEffect_Base*>(pEffectBase)->Set_InitChild(iChildCnt, szProtoTag);
					}
				}

				// Component Desc 저장
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
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			bDescLoad = false;
			iSaveLevel = 0;
			strcpy_s(szSaveFileName, "");
			ImGuiFileDialog::Instance()->Close();
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

CGameObject* CImgui_Effect::LayerEffects_ListBox(OUT char**& pObjectTag, OUT _uint& iLayerSize, OUT char*& pSelectObjectTag, const char* pLabelTag, _int& iSelectObject, TAGTYPE eTag)
{
	// Layer 가져와서 Layer_Effect 출력
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
	CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");
	RELEASE_INSTANCE(CGameInstance);

	// ListBox
	map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();
	iLayerSize = (_uint)pGameObject.size();

	pObjectTag = new char*[iLayerSize];
	auto iter = pGameObject.begin();
	for (size_t i = 0; i < iLayerSize; ++i, iter++)
	{
		if (eTag == TAGTYPE::TAG_CLONE)
		{
			pObjectTag[i] = new char[lstrlen(iter->first) + 1];
			CUtile::WideCharToChar(iter->first, pObjectTag[i]);
		}
		else
		{
			pObjectTag[i] = new char[lstrlen(iter->second->Get_ProtoObjectName()) + 1];
			CUtile::WideCharToChar(iter->second->Get_ProtoObjectName(), pObjectTag[i]);
		}
	}

	ImGui::ListBox(pLabelTag, &iSelectObject, pObjectTag, (_int)pGameObject.size());
	if (eTag == TAGTYPE::TAG_CLONE && iSelectObject != -1)
	{
		pSelectObjectTag = pObjectTag[iSelectObject];
	}
	else
		pSelectObjectTag = "";

	if (eTag == TAGTYPE::TAG_PROTO)
		pSelectObjectTag = pObjectTag[iSelectObject];

	if (iSelectObject != -1)
	{
		iter = pGameObject.begin();
		for (size_t i = 0; i < iSelectObject; ++i)
			iter++;
	}
	else
		return nullptr;

	return iter->second;
}

void CImgui_Effect::LayerChild_ListBox(OUT char**& pObjectTag, OUT _uint& iHaveChildSize, OUT char*& pSelectObjectTag, const char* pLabelTag, _int& iSelectObject, CEffect_Base* pEffect, vector<CEffect_Base*>* pChild, TAGTYPE eTag)
{
	vector<CEffect_Base*>* vecChild = pEffect->Get_vecChild();
	iHaveChildSize = pEffect->Get_ChildCnt();

	if (iHaveChildSize == 0)
		return;

	pObjectTag = new char*[iHaveChildSize];
	auto iter = vecChild->begin();
	for (size_t i = 0; i < iHaveChildSize; ++i, ++iter)
	{
		if (eTag == TAGTYPE::TAG_CLONE)
		{
			pObjectTag[i] = new char[lstrlen((*iter)->Get_ObjectCloneName()) + 1];
			CUtile::WideCharToChar((*iter)->Get_ObjectCloneName(), pObjectTag[i]);
		}
		else
		{
			pObjectTag[i] = new char[lstrlen((*iter)->Get_ProtoObjectName()) + 1];
			CUtile::WideCharToChar((*iter)->Get_ProtoObjectName(), pObjectTag[i]);
		}
	}
	ImGui::ListBox(pLabelTag, &iSelectObject, pObjectTag, (_int)iHaveChildSize);
	pSelectObjectTag = pObjectTag[iSelectObject];

	iter = vecChild->begin();
	for (_int i = 0; i < iSelectObject; ++i)
		iter++;

	if (eTag == TAGTYPE::TAG_PROTO)
	{
		ImGui::SameLine();
		if (ImGui::Button("Delete Child"))
			pEffect->Edit_Child((*iter)->Get_ObjectCloneName());
	}
}

_float4 CImgui_Effect::Set_ColorValue()
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
	vSelectColor = m_eEffectDesc.vColor;

	ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_Float | misc_flags);

	return vSelectColor;
}

void CImgui_Effect::Set_OptionWindow_Rect(CEffect_Base* pEffect)
{
	ImGui::Begin("Moving Position Option Window");
	static _float fSpeed = 0.0f;
	static _float fPlayBackTime = m_eEffectDesc.fPlayBbackTime;

	static _bool  bStart = true, bPause = false, bStop = false;
	if (bStart == true)
	{
		m_eEffectDesc.bStart = true;
		fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
	}
	if (bStop == true)
		m_eEffectDesc.fPlayBbackTime = 0.0f;

	ImGui::BulletText("Play Button    : "); ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		m_eEffectDesc.bStart = false;

		if (bPause == false)
		{
			fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
			bPause = true;
		}
		bStart = false;
	}ImGui::SameLine();

	if (ImGui::Button("ReStart"))
	{
		bStart = true;
		bPause = false;
		bStop = false;

		m_eEffectDesc.fPlayBbackTime = fPlayBackTime;
	}ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		m_eEffectDesc.bStart = false;

		bStart = false;
		bStop = true;
		fPlayBackTime = m_eEffectDesc.fPlayBbackTime = 0.0f;
		fPlayBackTime = 0.0f;
	}

	ImGui::BulletText("Playback Speed : "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150);
	ImGui::InputFloat("##PlaybackSpeed", (_float*)&fSpeed, 0.0f, 1.0f);

	ImGui::BulletText("Playback Time  : "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150);
	ImGui::Text(" %f", fPlayBackTime);

	if (ImGui::CollapsingHeader("Move Detail Setting"))
	{
		ImGui::BulletText("MoveRange : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat("##MoveRange", (_float*)&m_eEffectDesc.fRange, 0.0f, 1.0f);

		ImGui::BulletText("CreateRange : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat("##CreateRange", (_float*)&m_eEffectDesc.fCreateRange, 0.0f, 1.0f);

		ImGui::BulletText("MoveAngle : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat("##MoveAngle", (_float*)&m_eEffectDesc.fAngle, 0.0f, 1.0f);

		ImGui::BulletText("fMoveDurationTime : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat("##fMoveDurationTime", (_float*)&m_eEffectDesc.fMoveDurationTime, 0.0f, 1.0f);

		static _int iSelectMoveDir = 0;
		static _int iSelectRotation = 0;
		ImGui::Separator();
		ImGui::BulletText("MoveDir : ");
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance); // 빌보드를 사용하고 있다는 가정하에 

		CCamera* pCamera = pGameInstance->Find_Camera(L"DEBUG_CAM_1");
		CTransform* pTargetTransform = dynamic_cast<CGameObject*>(pCamera)->Get_TransformCom();

		static _bool   bSpread = true;
		static _int    iSpread = 0;
		if (ImGui::RadioButton("DIR_X", &iSelectMoveDir, 0))
		{
			m_eEffectDesc.eRotation = CEffect_Base::EFFECTDESC::ROTXYZ::ROT_X;
			m_eEffectDesc.vPixedDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		}ImGui::SameLine();

		if (ImGui::RadioButton("DIR_Y", &iSelectMoveDir, 1))
		{
			m_eEffectDesc.eRotation = CEffect_Base::EFFECTDESC::ROTXYZ::ROT_Y;
			m_eEffectDesc.vPixedDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		}ImGui::SameLine();

		if (ImGui::RadioButton("DIR_Z", &iSelectMoveDir, 2))
		{
			m_eEffectDesc.eRotation = CEffect_Base::EFFECTDESC::ROTXYZ::ROT_Z;
			m_eEffectDesc.vPixedDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		}

		ImGui::Separator();
		ImGui::BulletText("Playback Spread or Gather : ");
		if (ImGui::RadioButton("Spread", &iSpread, 0))
			bSpread = true;
		ImGui::SameLine();
		if (ImGui::RadioButton("Gather", &iSpread, 1))
			bSpread = false;
		ImGui::Separator();

		if (bSpread == true)
			m_eEffectDesc.bSpread = true;
		else
			m_eEffectDesc.bSpread = false;

		RELEASE_INSTANCE(CGameInstance);
	}
	// moveposition
	CTransform::TRANSFORMDESC eTransformDesc = pEffect->Get_TransformCom()->Get_TransformDesc();
	eTransformDesc.fSpeedPerSec = fSpeed;
	pEffect->Get_TransformCom()->Set_TransformDesc(eTransformDesc);
	ImGui::End();
}

void CImgui_Effect::Set_OptionWindow_Particle(_int& iCreateCnt, CEffect_Base * pEffect)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	ImGui::Begin("Moving Particle Option Window");
	CEffect_Point_Instancing* pParticle = dynamic_cast<CEffect_Point_Instancing*>(pEffect);
	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = pParticle->Get_PointInstanceDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData = pParticle->Get_InstanceData();

	static _float2 fSpeed = eInstanceData->SpeedMinMax;
	static _float2 fPSize = eInstanceData->fPSize;
	static _float3 fPosMin = _float3(0.0f);
	static _float3 fPosMax = _float3(0.0f);
	static _float  fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
	static _float2 fSaveSpeed = { 0.0f,0.0f };

	static _int iSelectShapes = 0;
	const char* szShapeType[] = { "VIBUFFER_BOX", "VIBUFFER_STRIGHT","VIBUFFER_PLANECIRCLE", "VIBUFFER_CONE", "VIBUFFER_EXPLOSION" };
	static _bool bShape[5] = { true, false, false, false, false };

	if (ImGui::TreeNode("Option"))
	{
		ImGui::BulletText("Shape : ");
		if (ImGui::RadioButton(szShapeType[0], &iSelectShapes, 0))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(bShape); i++)
			{
				if (i == 0)
					bShape[i] = true;
				else
					bShape[i] = false;
			}

			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_BOX;
		}ImGui::SameLine();
		if (ImGui::RadioButton(szShapeType[1], &iSelectShapes, 1))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(bShape); i++)
			{
				if (i == 1)
					bShape[i] = true;
				else
					bShape[i] = false;
			}
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_STRIGHT;
		}ImGui::SameLine();
		if (ImGui::RadioButton(szShapeType[2], &iSelectShapes, 2))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(bShape); i++)
			{
				if (i == 2)
					bShape[i] = true;
				else
					bShape[i] = false;
			}
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_PLANECIRCLE;

		}
		if (ImGui::RadioButton(szShapeType[3], &iSelectShapes, 3))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(bShape); i++)
			{
				if (i == 3)
					bShape[i] = true;
				else
					bShape[i] = false;
			}
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_CONE;
		}ImGui::SameLine();
		if (ImGui::RadioButton(szShapeType[4], &iSelectShapes, 4))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(bShape); i++)
			{
				if (i == 4)
					bShape[i] = true;
				else
					bShape[i] = false;
			}
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_EXPLOSION;
		}

		static _bool  bStart = true, bPause = false, bStop = false;
		if (bStart == true)
		{
			m_eEffectDesc.bStart = true;
			fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
		}
		if (bStop == true)
			m_eEffectDesc.fPlayBbackTime = 0.0f;

		ImGui::BulletText("Play Button			 : "); ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{
			m_eEffectDesc.bStart = false;
			if (bPause == false)
			{
				fSaveSpeed = fSpeed;
				fSpeed = _float2(0.0f, 0.0f);
				fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
				bPause = true;
			}
			bStart = false;
		}ImGui::SameLine();

		if (ImGui::Button("ReStart"))
		{
			bStart = true;
			bPause = false;

			m_eEffectDesc.fPlayBbackTime = fPlayBackTime;

			if (bStop == true)
			{
				pParticle->Set_Pos(fPosMin, fPosMax);
				bStop = false;
			}
			fSpeed = _float2(fSaveSpeed.x, fSaveSpeed.y);

		}ImGui::SameLine();

		if (ImGui::Button("Stop"))
		{
			m_eEffectDesc.bStart = false;

			bStart = false;
			bStop = true;
			fPlayBackTime = m_eEffectDesc.fPlayBbackTime = 0.0f;
			fPlayBackTime = 0.0f;
			fSaveSpeed = fSpeed;
			fSpeed = _float2(0.0f, 0.0f);
		}

		ImGui::BulletText("Playback Particle Time  : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat("##fPlayBackTime", (_float*)&fPlayBackTime);

		static _float fRange = ePointDesc->fRange;
		ImGui::BulletText("Playback Particle Range : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat("##fRange", (_float*)&fRange);
		ePointDesc->fRange = fRange;

		ImGui::BulletText("Playback Particle Speed : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat2("##PlaybackParticleSpeed", (_float*)&fSpeed);
		pParticle->Set_RandomSpeeds(fSpeed.x, fSpeed.y);

		ImGui::BulletText("Playback Particle PSize : "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::InputFloat2("##PlaybackParticlePSize", (_float*)&fPSize); ImGui::SameLine();

		if (ImGui::Button("InputSize"))
			pParticle->Set_PSize(fPSize);
		ImGui::SameLine();

		if (ImGui::Button("RandomSize"))
			pParticle->Set_RandomPSize(fPSize);

		if (ImGui::CollapsingHeader("Move Detail Setting"))
		{
			ImGui::BulletText("MoveRange : "); ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			ImGui::InputFloat("##MoveRange", (_float*)&m_eEffectDesc.fRange, 0.0f, 1.0f);

			ImGui::BulletText("CreateRange : "); ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			ImGui::InputFloat("##CreateRange", (_float*)&m_eEffectDesc.fCreateRange, 0.0f, 1.0f);

			ImGui::BulletText("MoveAngle : "); ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			ImGui::InputFloat("##MoveAngle", (_float*)&m_eEffectDesc.fAngle, 0.0f, 1.0f);

			ImGui::BulletText("fMoveDurationTime : "); ImGui::SameLine();
			ImGui::SetNextItemWidth(150);
			ImGui::InputFloat("##fMoveDurationTime", (_float*)&m_eEffectDesc.fMoveDurationTime, 0.0f, 1.0f);

		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	static _int iSelectMoveDir = 0;
	static _int iSelectRotation = 0;

	if (bShape[0] == true) // BOX TYPE
	{
		if (ImGui::CollapsingHeader("VIBuffer_Box Option"))
		{
			ImGui::BulletText("MoveDir : ");
			CCamera*       pCamera = pGameInstance->Find_Camera(L"DEBUG_CAM_1");
			CTransform*    pTargetTransform = dynamic_cast<CGameObject*>(pCamera)->Get_TransformCom();
			static _bool   bSpread = true;
			static _int    iSpread = 0;

			if (ImGui::RadioButton("DIR_X", &iSelectMoveDir, 0))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_X;
				ePointDesc->vDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			}ImGui::SameLine();

			if (ImGui::RadioButton("DIR_Y", &iSelectMoveDir, 1))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Y;
				ePointDesc->vDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			}ImGui::SameLine();

			if (ImGui::RadioButton("DIR_Z", &iSelectMoveDir, 2))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Z;
				ePointDesc->vDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			}

			ImGui::Separator();
			ImGui::BulletText("Playback Spread or Gather : ");
			if (ImGui::RadioButton("Spread", &iSpread, 0))
				bSpread = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Gather", &iSpread, 1))
				bSpread = false;

			ImGui::Separator();
			ImGui::BulletText("Playback Position Min, Max : ");
			ImGui::InputFloat3("ParticlePos Min", (_float*)&fPosMin);
			ImGui::InputFloat3("ParticlePos Max", (_float*)&fPosMax); ImGui::SameLine();

			if (ImGui::Button("Set Box"))
			{
				ePointDesc->fMin = fPosMin;
				ePointDesc->fMax = fPosMax;
				ePointDesc->bSpread = bSpread;

				pParticle->Set_ShapePosition();
			}
		}
		ImGui::Separator();
	}

	if (bShape[1] == true) // VIBUFFER_STRIGHT TYPE
	{
		if (ImGui::CollapsingHeader("VIBuffer_Stright Option"))
		{
			CCamera*       pCamera = pGameInstance->Find_Camera(L"DEBUG_CAM_1");
			CTransform*    pTargetTransform = dynamic_cast<CGameObject*>(pCamera)->Get_TransformCom();
			static _bool   bSpread = true;
			static _int    iSpread = 0;
			static _float4 vPosition = { 0.0f,0.0f,0.0f,1.0f };

			ImGui::BulletText("Playback OriginPos : ");
			ImGui::InputFloat3("OriginPos", (_float*)&vPosition);

			ImGui::Separator();
			ImGui::BulletText("MoveDir : ");
			if (ImGui::RadioButton("DIR_X", &iSelectMoveDir, 0))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_X;
				ePointDesc->vDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			}ImGui::SameLine();

			if (ImGui::RadioButton("DIR_Y", &iSelectMoveDir, 1))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Y;
				ePointDesc->vDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			}ImGui::SameLine();

			if (ImGui::RadioButton("DIR_Z", &iSelectMoveDir, 2))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Z;
				ePointDesc->vDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			}

			ImGui::Separator();
			ImGui::BulletText("Playback Spread or Gather : ");
			if (ImGui::RadioButton("Spread", &iSpread, 0))
				bSpread = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Gather", &iSpread, 1))
				bSpread = false;

			ImGui::SameLine();
			if (ImGui::Button("Set Straight"))
			{
				ePointDesc->vOriginPos = vPosition;
				ePointDesc->bSpread = bSpread;
				pParticle->Set_ShapePosition();
			}
		}
		ImGui::Separator();
	}

	if (bShape[2] == true) // VIBUFFER_PLANECIRCLE TYPE
	{
		if (ImGui::CollapsingHeader("VIBuffer_PlaneCircle Option"))
		{
			CCamera*       pCamera = pGameInstance->Find_Camera(L"DEBUG_CAM_1");
			CTransform*    pTargetTransform = dynamic_cast<CGameObject*>(pCamera)->Get_TransformCom();
			static _bool   bSpread = true;
			static _int    iSpread = 0;
			static _float    fDurationTime = 0.0f;
			static _float    fCreateRange = 0.0f;
			static _float4 vPosition = { 0.0f,0.0f,0.0f,1.0f };

			ImGui::BulletText("Playback OriginPos : ");
			ImGui::InputFloat3("OriginPos", (_float*)&vPosition);

			ImGui::Separator();
			ImGui::BulletText("Playback Spread or Gather : ");
			if (ImGui::RadioButton("Spread", &iSpread, 0))
				bSpread = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Gather", &iSpread, 1))
				bSpread = false;

			ImGui::Separator();
			ImGui::BulletText("Playback Duration Time : ");
			ImGui::PushItemWidth(150);
			ImGui::InputFloat("##Duration Time", (_float*)&fDurationTime);

			ImGui::BulletText("Playback Creat Range : ");
			ImGui::PushItemWidth(150);
			ImGui::InputFloat("##Creat Range", (_float*)&fCreateRange);

			static _bool bMoveTornado = false;
			static _bool bMoveRotation = false;
			static _float fMoveY = 0.0f;
			ImGui::Checkbox("Move Tornado", &bMoveTornado); ImGui::SameLine();
			ImGui::Checkbox("Move Rotation", &bMoveRotation);
			if (bMoveRotation)
				ImGui::InputFloat("Move RotationY", (_float*)&fMoveY);

			ImGui::SameLine();
			if (ImGui::Button("Set PlaneCircle"))
			{
				ePointDesc->fMin = fPosMin;
				ePointDesc->fMax = fPosMax;
				ePointDesc->bSpread = bSpread;
				ePointDesc->fCreateRange = fCreateRange;
				ePointDesc->fMaxTime = fDurationTime;
				ePointDesc->vOriginPos = vPosition;

				if (bMoveTornado)
					ePointDesc->bMoveY = true;
				else
					ePointDesc->bMoveY = false;

				if (bMoveRotation)
				{
					ePointDesc->bRotation = true;
					ePointDesc->fMoveY = fMoveY;
				}
				else
					ePointDesc->bRotation = false;

				pParticle->Set_ShapePosition();
			}
		}
	}
	if (bShape[3] == true) // CONE TYPE
	{
		if (ImGui::CollapsingHeader("VIBuffer_Sphere Option"))
		{
			static _float4 vPosition = { 0.0f,0.0f,0.0f,1.0f };
			static _float2 vConeRange = { 1.0f, 3.0f };
			static _int    iSpread = 0;
			static _bool   bSpread = true;

			ImGui::BulletText("Playback OriginPos : ");
			ImGui::InputFloat3("OriginPos", (_float*)&vPosition);

			ImGui::Separator();
			ImGui::BulletText("Cone Range : ");
			ImGui::InputFloat2("ConrRange", (_float*)&vConeRange);

			ImGui::Separator();
			if (ImGui::RadioButton("DIR_X", &iSelectMoveDir, 0))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_X;
			}ImGui::SameLine();

			if (ImGui::RadioButton("DIR_Y", &iSelectMoveDir, 1))
			{
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Y;
			}ImGui::SameLine();

			if (ImGui::RadioButton("DIR_Z", &iSelectMoveDir, 2))
				ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Z;

			ImGui::Separator();
			ImGui::BulletText("Playback Spread or Gather : ");
			if (ImGui::RadioButton("Spread", &iSpread, 0))
				bSpread = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Gather", &iSpread, 1))
				bSpread = false;

			ImGui::SameLine();
			if (ImGui::Button("Set Cone"))
			{
				ePointDesc->vOriginPos = vPosition;
				ePointDesc->fConeRange = vConeRange;
				ePointDesc->bSpread = bSpread;

				pParticle->Set_ShapePosition();
			}
			ImGui::Separator();
		}
	}

	if (bShape[4] == true) // EXPLOSION TYPE
	{
		if (ImGui::CollapsingHeader("VIBuffer_Explosion Option"))
		{
			static _float4 vPosition = { 0.0f,0.0f,0.0f,1.0f };
			static _int    iSpread = 0;
			static _bool   bSpread = true;

			ImGui::BulletText("Playback OriginPos : ");
			ImGui::InputFloat3("OriginPos", (_float*)&vPosition);

			ImGui::Separator();
			ImGui::BulletText("Playback Spread or Gather : ");
			if (ImGui::RadioButton("Spread", &iSpread, 0))
				bSpread = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Gather", &iSpread, 1))
				bSpread = false;

			ImGui::SameLine();
			if (ImGui::Button("Set Explosion"))
			{
				ePointDesc->vOriginPos = vPosition;
				ePointDesc->bSpread = bSpread;

				pParticle->Set_ShapePosition();
			}
			ImGui::Separator();
		}
	}

	//pParticle->Set_PointInstanceDesc(ePointDesc);
	ImGui::End();
	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_Effect::Set_Child(CEffect_Base* pEffect)
{
	ImGui::Begin("Setting Child Window");

	// Layer_ListBox용 
	char** pObjectTag = nullptr;
	char** pChildObjectTag = nullptr;
	char** pSelectObjectTag = nullptr;
	char** pSelectChildTag = nullptr;

	static _uint iLayerSize = 0;
	static _uint iChildSize = 0;
	static _uint iGrandChildSize = 0;
	static _uint iSuperGrandChildSize = 0;

	static char* pSelectObject = "";
	static char* pSelectChildObject = "";
	static char* pSelectGrandChildObject = "";
	static char* pChildObject = "";

	static _int iSelectObject = 0;
	static _int iSelectChild = 0;
	static _int iSelectChildObject = 0;
	// Layer_ListBox용 

	static _bool bCreatechild = false;
	static _int  iAddObjctCnt = 0;
	static _int  iCreateCnt = 0;
	static _bool bCheckCreateChildChild = false;
	CGameObject* pChild1 = nullptr;

	ImGui::PushItemWidth(400);
	ImGui::BulletText("Cur Layer_Effect Object List :"); // 부모
	LayerEffects_ListBox(pObjectTag, iLayerSize, pSelectObject, "Set_CloneTag", iSelectObject, TAGTYPE::TAG_CLONE);
	for (size_t i = 0; i < iLayerSize; ++i)
		Safe_Delete_Array(pObjectTag[i]);
	Safe_Delete_Array(pObjectTag);

	CGameObject* pParents = LayerEffects_ListBox(pObjectTag, iLayerSize, pSelectObject, "Set_ProtoTag", iSelectObject, TAGTYPE::TAG_PROTO);
	ImGui::BulletText("Cur Select Parents :");  ImGui::SameLine();	ImGui::Text(pSelectObject); ImGui::SameLine();

	static _bool bChildOptionWindow = false;
	if (dynamic_cast<CEffect_Base*>(pParents)->Get_ChildCnt() != 0)
	{
		ImGui::Checkbox("Child Option Window", &bChildOptionWindow);
		if (bChildOptionWindow)
		{
			vector<class CEffect_Base*>* pEffect = dynamic_cast<CEffect_Base*>(pParents)->Get_vecChild();
			auto& iter = pEffect->begin();
			for (_int i = 0; i < iSelectObject; i++)
				iter++;

			Show_ChildWindow(*iter);
		}
	}
	ImGui::NewLine();
	CGameObject* pChildGameObject = nullptr;
	if (ImGui::CollapsingHeader("Create Child")) // 자식으로 설정할 객체
	{
		ImGui::BulletText("Input Create Child Cnt : ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		ImGui::InputInt("##CreateChild", (_int*)&iCreateCnt);
		if (iCreateCnt <= 0)
			iCreateCnt = 0;
		else if (iCreateCnt >= 10)
			iCreateCnt = 10;

		ImGui::Separator();
		ImGui::BulletText("Choose Child Object :");
		//LayerEffects_ListBox(pSelectObjectTag, iLayerSize, pChildObject, "Child_CloneTag", iSelectChild, TAGTYPE::TAG_CLONE);
		//for (size_t i = 0; i < iLayerSize; ++i)
		//	Safe_Delete_Array(pSelectObjectTag[i]);
		//Safe_Delete_Array(pSelectObjectTag);

		pChildGameObject = LayerEffects_ListBox(pSelectObjectTag, iLayerSize, pChildObject, "Child_ProtoTag", iSelectChild, TAGTYPE::TAG_PROTO);
		ImGui::BulletText("Cur Select ChildObject :");  ImGui::SameLine();	ImGui::Text(pChildObject);

		ImGui::SameLine();
		if (ImGui::Button("Add Child"))
		{
			if (pChildGameObject != nullptr)
			{
				CEffect_Base::EFFECTDESC eEffectDesc = dynamic_cast<CEffect_Base*>(pChildGameObject)->Get_EffectDesc();
				dynamic_cast<CEffect_Base*>(pParents)->Set_Child(eEffectDesc, iCreateCnt, pChildObject);
			}
		}

		for (size_t i = 0; i < iLayerSize; ++i)
			Safe_Delete_Array(pSelectObjectTag[i]);
		Safe_Delete_Array(pSelectObjectTag);
	}
	if (dynamic_cast<CEffect_Base*>(pParents)->Get_ChildCnt() != 0)
	{
		static _bool bChild = false;
		static _int iCreateChild = 0;
		if (ImGui::CollapsingHeader("Cur Layer_Effect Child List")) // 부모 자식
		{
			ImGui::BulletText("Input Create GrandChild Cnt : ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(200);
			ImGui::InputInt("##CreateGrandChild", (_int*)&iCreateChild);
			if (iCreateChild <= 0)
				iCreateChild = 0;
			else if (iCreateChild >= 10)
				iCreateChild = 10;

			vector<CEffect_Base*>* vecChild = dynamic_cast<CEffect_Base*>(pParents)->Get_vecChild();
			LayerChild_ListBox(pChildObjectTag, iGrandChildSize, pSelectChildObject, "CreateChild_ProtoeTag", iSelectChildObject, dynamic_cast<CEffect_Base*>(pParents), nullptr, TAGTYPE::TAG_CLONE);
			for (size_t i = 0; i < iGrandChildSize; ++i)
				Safe_Delete_Array(pChildObjectTag[i]);
			Safe_Delete_Array(pChildObjectTag);

			LayerChild_ListBox(pChildObjectTag, iGrandChildSize, pSelectChildObject, "CreateChild_CloneTag", iSelectChildObject, dynamic_cast<CEffect_Base*>(pParents), nullptr, TAGTYPE::TAG_PROTO);

			auto& iter = vecChild->begin();
			for (_int i = 0; i < iSelectChildObject; i++)
				iter++;

			if (iGrandChildSize != 0)
				TransformView_child(iSelectChildObject, dynamic_cast<CEffect_Base*>(pParents));

			ImGui::SameLine();
			if (ImGui::Button("Add GrandChild"))
			{
				auto& iter = vecChild->begin();
				for (_int i = 0; i < iSelectChildObject; i++)
					iter++;

				CEffect_Base::EFFECTDESC eEffectDesc = dynamic_cast<CEffect_Base*>(pChildGameObject)->Get_EffectDesc();
				(*iter)->Set_Child(eEffectDesc, iCreateChild, pSelectChildObject);
				bChild = false;
			}
			for (size_t i = 0; i < iGrandChildSize; ++i)
				Safe_Delete_Array(pChildObjectTag[i]);
			Safe_Delete_Array(pChildObjectTag);
		}
	}
	for (size_t i = 0; i < iLayerSize; ++i)
		Safe_Delete_Array(pObjectTag[i]);
	Safe_Delete_Array(pObjectTag);

	ImGui::End();
}

void CImgui_Effect::Set_Trail(class CEffect_Base* pEffect)
{
	if (pEffect == nullptr)
		return;

	ImGui::Begin("Setting Trail Window");
	static _bool   bActive = true;
	static _bool   bAlpha = false;
	static _bool   bHaveTrail = false;
	static _float4 fOption = { 0.1f,0.1f,0.1f,1.f };
	static _bool bSettingOptionTrail = false;

	ImGui::BulletText("Setting Option : ");
	ImGui::Checkbox("Active", &bActive); ImGui::SameLine();
	ImGui::Checkbox("Alpha", &bAlpha); ImGui::SameLine();
	ImGui::Checkbox("Add Trail", &bHaveTrail);

	if (bHaveTrail && m_eEffectDesc.IsTrail == false)
	{
		pEffect->Set_Trail(pEffect, pEffect->Get_ProtoObjectName());
		m_eEffectDesc.IsTrail = true;
	}
	if (!bHaveTrail && m_eEffectDesc.IsTrail == true)
	{
		pEffect->Delete_Trail(pEffect->Get_ProtoObjectName());
		m_eEffectDesc.IsTrail = false;
	}

	if (m_eEffectDesc.IsTrail == true)
		ImGui::Checkbox("TrailOption Setting", &bSettingOptionTrail);

	if (bSettingOptionTrail)
		Show_TrailWindow(pEffect->Get_Trail());

	ImGui::PushItemWidth(200);
	ImGui::Separator();
	ImGui::BulletText("Life / Width /  SegmentSize / Alpha : ");
	ImGui::InputFloat4("##fOption", (_float*)&fOption);

	m_eEffectDesc.bActive = bActive;
	m_eEffectDesc.bAlpha = bAlpha;
	m_eEffectDesc.fLife = fOption.x;
	m_eEffectDesc.fWidth = fOption.y;
	m_eEffectDesc.fSegmentSize = fOption.z;
	m_eEffectDesc.fAlpha = fOption.w;
	pEffect->Set_EffectDesc(m_eEffectDesc);
	ImGui::End();
}

void CImgui_Effect::Set_FreePos(CEffect_Base * pEffect)
{
	ImGui::Begin("Free Moving");

	char** pObjectTag = nullptr;

	static _uint  iLayerSize = 0;
	static char*  pSelectObject = "";
	static _int   iSelectObject = 0;
	static _int   iAddObjctCnt = 0;
	static _int   iCreateCnt = 0;
	static _float fSpeed = 0.0f;
	static _float fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
	static _uint vecSize = 0;
	static _int iCurPlayIdx = 0;

	vector<_float4>* vecPosition = nullptr;
	if (dynamic_cast<CEffect_T*>(pEffect))
		vecPosition = dynamic_cast<CEffect_T*>(pEffect)->Get_FreePos();
	if (dynamic_cast<CEffect_Mesh_T*>(pEffect))
		vecPosition = dynamic_cast<CEffect_Mesh_T*>(pEffect)->Get_FreePos();

	vecSize = (_uint)vecPosition->size();

	static _bool  bStart = true, bPause = false, bStop = false;
	static _bool  bPlay = false;

	if (bStart == true)
	{
		m_eEffectDesc.bStart = true;
		fPlayBackTime = m_eEffectDesc.fPlayBbackTime;

		if (dynamic_cast<CEffect_T*>(pEffect))
			dynamic_cast<CEffect_T*>(pEffect)->Set_FreePos();
		if (dynamic_cast<CEffect_Mesh_T*>(pEffect))
			dynamic_cast<CEffect_Mesh_T*>(pEffect)->Set_FreePos();
	}
	if (bStop == true)
		m_eEffectDesc.fPlayBbackTime = 0.0f;

	// 	if (bPlay == true) {
	// 		m_eEffectDesc.bStart = true;
	// 		static _int iCurIdx = 0;
	// 
	// 		auto& iter = vecPosition->begin();
	// 		if (iCurIdx >= vecPosition->size())
	// 			iCurIdx = 0;
	// 
	// 		for (_int i = 0; i < iCurIdx; ++i)
	// 			iter++;
	// 
	// 		_bool bNextTime = dynamic_cast<CEffect_T*>(pEffect)->Play_FreePos(*iter);
	// 		if (bNextTime)
	// 		{
	// 			dynamic_cast<CEffect_T*>(pEffect)->Set_Lerp(false);
	// 			iCurIdx++;
	// 		}
	// 	}

	ImGui::BulletText("Play Button    : "); ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		m_eEffectDesc.bStart = false;
		if (bPause == false)
		{
			fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
			bPause = true;
		}
		bStart = false;
	}ImGui::SameLine();

	if (ImGui::Button("Play"))
	{
		bPlay = true;
		bPause = false;
		bStop = false;

		m_eEffectDesc.bFreeMove = true;
	}ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		m_eEffectDesc.bStart = false;
		m_eEffectDesc.bFreeMove = false;
		bPlay = false;
		bStart = false;
		bStop = true;
		fPlayBackTime = m_eEffectDesc.fPlayBbackTime = 0.0f;
		fPlayBackTime = 0.0f;
	}ImGui::SameLine();

	if (ImGui::Button("Clear"))
	{
		m_eEffectDesc.bStart = false;
		bStart = false;
		bStop = true;
		fPlayBackTime = m_eEffectDesc.fPlayBbackTime = 0.0f;
		fPlayBackTime = 0.0f;
		vecPosition->clear();
		vecSize = (_uint)vecPosition->size();
	}ImGui::SameLine();

	wstring strFreePos;
	if (ImGui::Button("Delete"))
	{
		auto& iter = vecPosition->begin();
		for (_int i = 0; i < iSelectObject; ++i)
			iter++;

		vecPosition->erase(iter);
		vecSize = (_uint)vecPosition->size();
	}

	ImGui::BulletText("Free_Position :");

	pObjectTag = new char*[vecSize];
	auto& iter = vecPosition->begin();
	for (size_t i = 0; i < vecSize; i++, iter++)
	{
		pObjectTag[i] = new char[128];
		strFreePos.clear();

		strFreePos.append(to_wstring((*iter).x));
		strFreePos.push_back('/');
		strFreePos.append(to_wstring((*iter).y));
		strFreePos.push_back('/');
		strFreePos.append(to_wstring((*iter).z));
		strFreePos.push_back('/');
		strFreePos.append(to_wstring((*iter).w));
		CUtile::WideCharToChar(strFreePos.c_str(), pObjectTag[i]);
	}

	ImGui::ListBox("Layer_Position", &iSelectObject, pObjectTag, (_int)vecSize);

	if (ImGui::Button("Move"))
	{
		auto& iter = vecPosition->begin();
		for (_int i = 0; i < iSelectObject; ++i)
			iter++;

		if (dynamic_cast<CEffect_T*>(pEffect))
			dynamic_cast<CEffect_T*>(pEffect)->Set_Position(*iter);
		if (dynamic_cast<CEffect_Mesh_T*>(pEffect))
			dynamic_cast<CEffect_Mesh_T*>(pEffect)->Set_Position(*iter);
		iCurPlayIdx = iSelectObject;
	}
	for (size_t i = 0; i < vecSize; ++i)
		Safe_Delete_Array(pObjectTag[i]);
	Safe_Delete_Array(pObjectTag);

	ImGui::End();
}

void CImgui_Effect::Set_MeshType(OUT CEffect_Base::EFFECTDESC& effectType, _int & iSelectMeshType)
{
	ZeroMemory(&effectType, sizeof(CEffect_Base::EFFECTDESC));

	// const _tchar* szEffectMeshType[] = { L"MeshPlane", L"MeshCube", L"MeshCone", L"MeshSphere" ,L"MeshCylinder" };
	switch (iSelectMeshType)
	{
	case 0:
		effectType.eMeshType = CEffect_Base::tagEffectDesc::MESH_PLANE;
		break;
	case 1:
		effectType.eMeshType = CEffect_Base::tagEffectDesc::MESH_CUBE;
		break;
	case 2:
		effectType.eMeshType = CEffect_Base::tagEffectDesc::MESH_CONE;
		break;
	case 3:
		effectType.eMeshType = CEffect_Base::tagEffectDesc::MESH_SPHERE;
		break;
	case 4:
		effectType.eMeshType = CEffect_Base::tagEffectDesc::MESH_CYLINDER;
		break;
	}
}

void CImgui_Effect::TransformView(_int iSelectObject, CEffect_Base* pEffect)
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

		_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
		CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");

		map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();

		auto iter = pGameObject.begin();
		for (_int i = 0; i < iSelectObject; ++i)
			iter++;

		const _matrix&   matWorld = (*iter).second->Get_TransformCom()->Get_WorldMatrix();

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

		(*iter).second->Get_TransformCom()->Set_WorldMatrix(matWorld);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_Effect::TransformView_child(_int iSelectObject, CEffect_Base* pEffect)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	ImGuizmo::BeginFrame();
	static float snap[3] = { 1.f, 1.f, 1.f };
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::RadioButton("ChildTranslate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("ChildRotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("ChildScale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	static bool useSnap(false);
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	vector<CEffect_Base*>* vecChild = pEffect->Get_vecChild();

	auto iter = vecChild->begin();
	if (vecChild->size() == 0)
	{
		iSelectObject = 0;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	if (iSelectObject >= vecChild->size())
		iSelectObject = 0;

	for (_int i = 0; i < iSelectObject; ++i)
		iter++;

	const _float4x4&   matWorld = (*iter)->Get_InitMatrix();
	ImGuizmo::DecomposeMatrixToComponents((_float*)&matWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("ChildTranslate", matrixTranslation);
	ImGui::InputFloat3("ChildRotate", matrixRotation);
	ImGui::InputFloat3("ChildScale", matrixScale);
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

	(*iter)->Set_InitMatrix(XMLoadFloat4x4(&matWorld));
	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_Effect::CreateEffect_Plane(_int& iCurSelect, _int& iSelectObject)
{
	if (m_bIsRectLayer == false)
		return;

	static _int iTextureRender = 0;
	static _int iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	CEffect_Base* pEffect = nullptr;

	// 2. Texture Create
	if (iSelectObject != -1)
	{
		//////////////////////////////////////////////////////////////////////////
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
		CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");
		map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();
		size_t iLayerObjSize = pGameObject.size();

		auto iter = pGameObject.begin();
		for (size_t i = 0; i < iSelectObject; ++i)
			iter++;

		if (iCurSelect != iSelectObject)
		{
			pEffect = dynamic_cast<CEffect_Base*>(iter->second);
			if (pEffect == nullptr)
				return;

			m_eEffectDesc = pEffect->Get_EffectDesc();

			iTextureRender = (_int)m_eEffectDesc.eTextureRenderType;
			iBlendType = (_int)m_eEffectDesc.eBlendType;
			fTimeDelta = m_eEffectDesc.fTimeDelta;
			fCnt = _float2((_float)m_eEffectDesc.iWidthCnt, (_float)m_eEffectDesc.iHeightCnt);
			fSeparateCnt = _float2((_float)m_eEffectDesc.iSeparateWidth, (_float)m_eEffectDesc.iSeparateHeight);
			fVector = m_eEffectDesc.vScale;
			fColor = m_eEffectDesc.vColor;
		}

		//////////////////////////////////////////////////////////////////////////
		TransformView(iSelectObject, pEffect);
		static _bool bChildWindow = false;
		static _bool bUseTrail = false;
		static _bool bFreeMovingPos = false;
		ImGui::Checkbox("Open Child Setting Window", &bChildWindow);
		ImGui::Checkbox("Use N Open TrailSetting Window", &bUseTrail);
		ImGui::Checkbox("FreeMoving Window", &bFreeMovingPos);

		if (bChildWindow)
			Set_Child(pEffect);

		if (bUseTrail)
			Set_Trail(pEffect);

		if (bFreeMovingPos)
		{
			m_eEffectDesc.bFreeMove = true;
			Set_FreePos(pEffect);
		}
		else
			m_eEffectDesc.bFreeMove = false;

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
		if (ImGui::BeginTabBar("##Value Setting", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Set Item Value"))
			{
				ImGui::BulletText("TextureRender Type : ");
				if (ImGui::RadioButton("TEX_ONE", &iTextureRender, 0))
					m_eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_ONE;
				if (ImGui::RadioButton("TEX_SPRITE", &iTextureRender, 1))
					m_eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE;

				if (iTextureRender == 1)
				{
					ImGui::BulletText("Sprite Option : ");

					ImGui::BulletText("CntWidth, Height : "); ImGui::InputFloat2("##CntWidth, Height", (_float*)&fCnt);
					m_eEffectDesc.iWidthCnt = (_int)fCnt.x;
					m_eEffectDesc.iHeightCnt = (_int)fCnt.y;

					ImGui::BulletText("fSeparateWidthCnt, HeightCnt : "); ImGui::InputFloat2("##fSeparateWidthCnt, HeightCnt", (_float*)&fSeparateCnt);
					m_eEffectDesc.iSeparateWidth = (_int)fSeparateCnt.x;
					m_eEffectDesc.iSeparateHeight = (_int)fSeparateCnt.y;

					ImGui::BulletText("fTimeDelta : "); ImGui::InputFloat("##fTimeDelta", &fTimeDelta);
					m_eEffectDesc.fTimeDelta = fTimeDelta;
					ImGui::Separator();
				}

				ImGui::Separator();
				ImGui::BulletText("Blend Type : ");
				if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
				if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
				if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
				if (ImGui::RadioButton("BlendType_Mix", &iBlendType, 3))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_MIX;
				ImGui::Separator();

				ImGui::BulletText("Option : ");
				ImGui::Checkbox("Use Trigger", &m_eEffectDesc.IsTrigger); ImGui::SameLine();
				ImGui::Checkbox("Use Billboard", &m_eEffectDesc.IsBillboard);
				ImGui::Checkbox("Use MaskTexture", &m_eEffectDesc.IsMask); ImGui::SameLine();
				ImGui::Checkbox("Use MovingPosition", &m_eEffectDesc.IsMovingPosition);

				if (m_eEffectDesc.IsMovingPosition)
					Set_OptionWindow_Rect(pEffect);

				ImGui::Separator();

				ImGui::BulletText("fVector : ");	 ImGui::InputFloat4("##fVector", (_float*)&fVector);
				m_eEffectDesc.vScale = XMVectorSet(fVector.x, fVector.y, fVector.z, fVector.w);

				ImGui::Separator();
				ImGui::BulletText("vColor : ");
				fColor = Set_ColorValue();
				m_eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

				pEffect->Set_EffectDesc(m_eEffectDesc);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Set Item Texture"))
			{
				static _int iCreateCnt = 1;
				static _int iSelectDTexture = 0;
				static _int iSelectMTexture = 0;
				static _int iSelectNTexture = 0;
				static _int iSelectTextureType = 0;

				const _int  iTotalDTextureCnt = pEffect->Get_TotalDTextureCnt();
				const _int  iTotalMTextureCnt = pEffect->Get_TotalMTextureCnt();

				ImGui::BulletText("Texture Type : ");
				ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
				ImGui::RadioButton("MaskTexture", &iSelectTextureType, 1);
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
						pEffect->Edit_TextureComponent(iCreateCnt, 0);

					char** szDTextureType = new char*[iTotalDTextureCnt];
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
					CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szDTexture);

					if (pDiffuseTexture != nullptr)
					{
						_float fFrame = pEffect->Get_EffectDesc().fFrame[iSelectDTexture];
						ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fFrame[iSelectDTexture] = i * 1.0f;
								pEffect->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
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
						pEffect->Edit_TextureComponent(0, iCreateCnt);

					char** szMTextureType = new char*[iTotalMTextureCnt];
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
					CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szMTexture);

					if (pDiffuseTexture != nullptr)
					{
						_float fMaskFrame = pEffect->Get_EffectDesc().fMaskFrame[iSelectMTexture];

						ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
								pEffect->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
							}
						}
					}

					for (size_t i = 0; i < iTotalMTextureCnt; ++i)
						Safe_Delete_Array(szMTextureType[i]);
					Safe_Delete_Array(szMTextureType);
				}
				else // NTexture
				{
					CTexture*		pNormalTexture = (CTexture*)pEffect->Find_Component(L"Com_NTexture");
					if (pNormalTexture != nullptr)
					{
						_float fNormalFrame = pEffect->Get_EffectDesc().fNormalFrame;

						ImGui::BulletText("NTexture _ %d / %d", (_uint)fNormalFrame, pNormalTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pNormalTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pNormalTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fNormalFrame = i * 1.0f;
								pEffect->Set_EffectDescNTexture(i * 1.0f);
							}
						}
					}
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImgui_Effect::CreateEffect_Particle(_int& iCreateCnt, _int& iCurSelect, _int& iSelectObject)
{
	if (m_bIsParticleLayer == false)
		return;

	static _int    iTextureRender = 0;
	static _int    iParticleType = 0;
	static _int    iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	CEffect_Point_Instancing* pEffect = nullptr;
	// 2. Texture Create
	if (iSelectObject != -1)
	{
		//////////////////////////////////////////////////////////////////////////
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
		CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");
		map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();
		size_t iLayerObjSize = pGameObject.size();

		auto iter = pGameObject.begin();
		for (size_t i = 0; i < iSelectObject; ++i)
			iter++;

		if (iCurSelect != iSelectObject)
		{
			pEffect = dynamic_cast<CEffect_Point_Instancing*>(iter->second);
			if (pEffect == nullptr)
				return;

			m_eEffectDesc = pEffect->Get_EffectDesc();

			iTextureRender = (_int)m_eEffectDesc.eTextureRenderType;
			iParticleType = (_int)m_eEffectDesc.eParticleType;
			iBlendType = (_int)m_eEffectDesc.eBlendType;
			fTimeDelta = m_eEffectDesc.fTimeDelta;
			fCnt = _float2((_float)m_eEffectDesc.iWidthCnt, (_float)m_eEffectDesc.iHeightCnt);
			fSeparateCnt = _float2((_float)m_eEffectDesc.iSeparateWidth, (_float)m_eEffectDesc.iSeparateHeight);
			fVector = m_eEffectDesc.vScale;
			fColor = m_eEffectDesc.vColor;
		}
		Set_OptionWindow_Particle(iCreateCnt, pEffect);

		//////////////////////////////////////////////////////////////////////////
		TransformView(iSelectObject, pEffect);
		static _bool bChildWindow = false;
		static _bool bUseTrail = false;
		ImGui::Checkbox("Open Child Setting Window", &bChildWindow);
		ImGui::Checkbox("Use N Open TrailSetting Window", &bUseTrail);

		if (bChildWindow)
			Set_Child(pEffect);

		if (bUseTrail)
			Set_Trail(pEffect);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
		if (ImGui::BeginTabBar("##Value Setting", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Set Item Value"))
			{
				ImGui::BulletText("TextureRender Type : ");
				if (ImGui::RadioButton("TEX_ONE", &iTextureRender, 0))
					m_eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_ONE;
				if (ImGui::RadioButton("TEX_SPRITE", &iTextureRender, 1))
					m_eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE;

				if (iTextureRender == 1)
				{
					ImGui::BulletText("Sprite Option : ");

					ImGui::BulletText("CntWidth, Height : "); ImGui::InputFloat2("##CntWidth, Height", (_float*)&fCnt);
					m_eEffectDesc.iWidthCnt = (_int)fCnt.x;
					m_eEffectDesc.iHeightCnt = (_int)fCnt.y;

					ImGui::BulletText("fSeparateWidthCnt, HeightCnt : "); ImGui::InputFloat2("##fSeparateWidthCnt, HeightCnt", (_float*)&fSeparateCnt);
					m_eEffectDesc.iSeparateWidth = (_int)fSeparateCnt.x;
					m_eEffectDesc.iSeparateHeight = (_int)fSeparateCnt.y;

					ImGui::BulletText("fTimeDelta : "); ImGui::InputFloat("##fTimeDelta", &fTimeDelta);
					m_eEffectDesc.fTimeDelta = fTimeDelta;
					ImGui::Separator();
				}

				ImGui::Separator();
				ImGui::BulletText("Blend Type : ");
				if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
				if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
				if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
				if (ImGui::RadioButton("BlendType_Mix", &iBlendType, 3))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_MIX;
				ImGui::Separator();

				ImGui::BulletText("Option : ");
				ImGui::Checkbox("Use Trigger", &m_eEffectDesc.IsTrigger); ImGui::SameLine();
				ImGui::Checkbox("Use Billboard", &m_eEffectDesc.IsBillboard); ImGui::SameLine();
				ImGui::Checkbox("Use MaskTexture", &m_eEffectDesc.IsMask);

				ImGui::Separator();
				ImGui::BulletText("fVector : ");	 ImGui::InputFloat4("##fVector", (_float*)&fVector);
				m_eEffectDesc.vScale = XMVectorSet(fVector.x, fVector.y, fVector.z, fVector.w);

				ImGui::Separator();
				ImGui::BulletText("vColor : ");
				fColor = Set_ColorValue();
				m_eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

				pEffect->Set_EffectDesc(m_eEffectDesc);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Set Item Texture"))
			{
				static _int iCreateCnt = 1;
				static _int iSelectDTexture = 0;
				static _int iSelectMTexture = 0;
				static _int iSelectNTexture = 0;
				static _int iSelectTextureType = 0;

				const _int  iTotalDTextureCnt = pEffect->Get_TotalDTextureCnt();
				const _int  iTotalMTextureCnt = pEffect->Get_TotalMTextureCnt();

				ImGui::BulletText("Texture Type : ");
				ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
				ImGui::RadioButton("MaskTexture", &iSelectTextureType, 1);
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
						pEffect->Edit_TextureComponent(iCreateCnt, 0);

					char** szDTextureType = new char*[iTotalDTextureCnt];
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
					CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szDTexture);

					if (pDiffuseTexture != nullptr)
					{
						_float fFrame = pEffect->Get_EffectDesc().fFrame[iSelectDTexture];
						ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fFrame[iSelectDTexture] = i * 1.0f;
								pEffect->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
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
						pEffect->Edit_TextureComponent(0, iCreateCnt);

					char** szMTextureType = new char*[iTotalMTextureCnt];
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
					CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szMTexture);

					if (pDiffuseTexture != nullptr)
					{
						_float fMaskFrame = pEffect->Get_EffectDesc().fMaskFrame[iSelectMTexture];

						ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
								pEffect->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
							}
						}
					}

					for (size_t i = 0; i < iTotalMTextureCnt; ++i)
						Safe_Delete_Array(szMTextureType[i]);
					Safe_Delete_Array(szMTextureType);
				}
				else // NTexture
				{
					CTexture*		pNormalTexture = (CTexture*)pEffect->Find_Component(L"Com_NTexture");
					if (pNormalTexture != nullptr)
					{
						_float fNormalFrame = pEffect->Get_EffectDesc().fNormalFrame;

						ImGui::BulletText("NTexture _ %d / %d", (_uint)fNormalFrame, pNormalTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pNormalTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pNormalTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fNormalFrame = i * 1.0f;
								pEffect->Set_EffectDescNTexture(i * 1.0f);
							}
						}
					}
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImgui_Effect::CreateEffect_Mesh(_int& iSelectObject)
{
	if (m_bIsMeshLayer == false)
		return;

	static _int    iTextureRender = 0;
	static _int    iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	CEffect_Mesh_T* pEffect = nullptr;
	// 2. Texture Create
	if (iSelectObject != -1)
	{
		//////////////////////////////////////////////////////////////////////////
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
		CLayer* pLayer = pGameInstance->Find_Layer(iCurLevel, L"Layer_Effect");
		map<const _tchar*, class CGameObject*>& pGameObject = pLayer->GetGameObjects();
		size_t iLayerObjSize = pGameObject.size();

		auto iter = pGameObject.begin();
		for (size_t i = 0; i < iSelectObject; ++i)
			iter++;

		if (iSelectObject != -1)
		{
			pEffect = dynamic_cast<CEffect_Mesh_T*>(iter->second);
			if (pEffect == nullptr)
				return;

			m_eEffectDesc = pEffect->Get_EffectDesc();

			iTextureRender = (_int)m_eEffectDesc.eTextureRenderType;
			iBlendType = (_int)m_eEffectDesc.eBlendType;
			fTimeDelta = m_eEffectDesc.fTimeDelta;
			fCnt = _float2((_float)m_eEffectDesc.iWidthCnt, (_float)m_eEffectDesc.iHeightCnt);
			fSeparateCnt = _float2((_float)m_eEffectDesc.iSeparateWidth, (_float)m_eEffectDesc.iSeparateHeight);
			fVector = m_eEffectDesc.vScale;
			fColor = m_eEffectDesc.vColor;
		}

		//////////////////////////////////////////////////////////////////////////
		TransformView(iSelectObject, pEffect);
		static _bool bChildWindow = false;
		static _bool bUseTrail = false;
		static _bool bFreeMovingPos = false;
		ImGui::Checkbox("Open Child Setting Window", &bChildWindow);
		ImGui::Checkbox("Use N Open TrailSetting Window", &bUseTrail);
		ImGui::Checkbox("FreeMoving Window", &bFreeMovingPos);

		if (bChildWindow)
			Set_Child(pEffect);

		if (bUseTrail)
			Set_Trail(pEffect);

		if (bFreeMovingPos)
		{
			m_eEffectDesc.bFreeMove = true;
			Set_FreePos(pEffect);
		}
		else
			m_eEffectDesc.bFreeMove = false;

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
		if (ImGui::BeginTabBar("##Value Setting", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Set Item Value"))
			{
				ImGui::Separator();
				ImGui::BulletText("Blend Type : ");
				if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
				if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
				if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
				if (ImGui::RadioButton("BlendType_Mix", &iBlendType, 3))
					m_eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_MIX;
				ImGui::Separator();

				ImGui::BulletText("Option : ");
				ImGui::Checkbox("Use Trigger", &m_eEffectDesc.IsTrigger); ImGui::SameLine();
				ImGui::Checkbox("Use Billboard", &m_eEffectDesc.IsBillboard);
				ImGui::Checkbox("Use MaskTexture", &m_eEffectDesc.IsMask); ImGui::SameLine();
				ImGui::Checkbox("Use NormalTexture", &m_eEffectDesc.IsNormal);  ImGui::SameLine();
				ImGui::Checkbox("Use MovingPosition", &m_eEffectDesc.IsMovingPosition);

				if (m_eEffectDesc.IsMovingPosition)
					Set_OptionWindow_Rect(pEffect);

				ImGui::Separator();

				ImGui::BulletText("fVector : ");	 ImGui::InputFloat4("##fVector", (_float*)&fVector);
				m_eEffectDesc.vScale = XMVectorSet(fVector.x, fVector.y, fVector.z, fVector.w);

				ImGui::Separator();
				ImGui::BulletText("vColor : ");
				fColor = Set_ColorValue();
				m_eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

				pEffect->Set_EffectDesc(m_eEffectDesc);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Set Item Texture"))
			{
				static _int iCreateCnt = 1;
				static _int iSelectDTexture = 0;
				static _int iSelectMTexture = 0;
				static _int iSelectNTexture = 0;
				static _int iSelectTextureType = 0;

				const _int  iTotalDTextureCnt = pEffect->Get_TotalDTextureCnt();
				const _int  iTotalMTextureCnt = pEffect->Get_TotalMTextureCnt();

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
						pEffect->Edit_TextureComponent(iCreateCnt, 0);

					char** szDTextureType = new char*[iTotalDTextureCnt];
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
					CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szDTexture);

					if (pDiffuseTexture != nullptr)
					{
						_float fFrame = pEffect->Get_EffectDesc().fFrame[iSelectDTexture];
						ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fFrame[iSelectDTexture] = i * 1.0f;
								pEffect->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
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
						pEffect->Edit_TextureComponent(0, iCreateCnt);

					char** szMTextureType = new char*[iTotalMTextureCnt];
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
					CTexture*		pDiffuseTexture = (CTexture*)pEffect->Find_Component(szMTexture);

					if (pDiffuseTexture != nullptr)
					{
						_float fMaskFrame = pEffect->Get_EffectDesc().fMaskFrame[iSelectMTexture];

						ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
								pEffect->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
							}
						}
					}

					for (size_t i = 0; i < iTotalMTextureCnt; ++i)
						Safe_Delete_Array(szMTextureType[i]);
					Safe_Delete_Array(szMTextureType);
				}
				else // NTexture
				{
					CTexture*		pNormalTexture = (CTexture*)pEffect->Find_Component(L"Com_NTexture");
					if (pNormalTexture != nullptr)
					{
						_float fNormalFrame = pEffect->Get_EffectDesc().fNormalFrame;

						ImGui::BulletText("NTexture _ %d / %d", (_uint)fNormalFrame, pNormalTexture->Get_TextureIdx());
						ImGui::Separator();

						for (_uint i = 0; i < pNormalTexture->Get_TextureIdx(); ++i)
						{
							if (i % 6)
								ImGui::SameLine();

							if (ImGui::ImageButton(pNormalTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
							{
								m_eEffectDesc.fNormalFrame = i * 1.0f;
								pEffect->Set_EffectDescNTexture(i * 1.0f);
							}
						}
					}
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImgui_Effect::Show_ChildWindow(CEffect_Base * pEffect)
{
	ImGui::Begin("Child Window");

	static _int iTextureRender = 0;
	static _int iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	CEffect_Base* pChildEffect = dynamic_cast<CEffect_Base*>(pEffect);
	if (pChildEffect == nullptr)
		return;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 2. Texture Create
	_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
	CEffect_Base::EFFECTDESC eEffectDesc = pChildEffect->Get_EffectDesc();

	iTextureRender = (_int)eEffectDesc.eTextureRenderType;
	iBlendType = (_int)eEffectDesc.eBlendType;
	fTimeDelta = eEffectDesc.fTimeDelta;
	fCnt = _float2((_float)eEffectDesc.iWidthCnt, (_float)eEffectDesc.iHeightCnt);
	fSeparateCnt = _float2((_float)eEffectDesc.iSeparateWidth, (_float)eEffectDesc.iSeparateHeight);
	fVector = eEffectDesc.vScale;
	fColor = eEffectDesc.vColor;

	//////////////////////////////////////////////////////////////////////////
	// TransformView(iSelectObject, pEffect);
	static _bool bChildWindow = false;
	static _bool bUseTrail = false;
	// ImGui::Checkbox("Open Child Setting Window", &bChildWindow);
	ImGui::Checkbox("Use N Open TrailSetting Window", &bUseTrail);

	// 자식의 자식만들기도 가능하게 해야함 
// 	if (bChildWindow)
// 		Set_Child(pEffect);

	if (bUseTrail)
		Set_Trail(pEffect);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
	if (ImGui::BeginTabBar("##Value Setting", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Set Item Value"))
		{
			ImGui::BulletText("TextureRender Type : ");
			if (ImGui::RadioButton("TEX_ONE", &iTextureRender, 0))
				eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_ONE;
			if (ImGui::RadioButton("TEX_SPRITE", &iTextureRender, 1))
				eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_SPRITE;

			if (iTextureRender == 1)
			{
				ImGui::BulletText("Sprite Option : ");

				ImGui::BulletText("CntWidth, Height : "); ImGui::InputFloat2("##CntWidth, Height", (_float*)&fCnt);
				eEffectDesc.iWidthCnt = (_int)fCnt.x;
				eEffectDesc.iHeightCnt = (_int)fCnt.y;

				ImGui::BulletText("fSeparateWidthCnt, HeightCnt : "); ImGui::InputFloat2("##fSeparateWidthCnt, HeightCnt", (_float*)&fSeparateCnt);
				eEffectDesc.iSeparateWidth = (_int)fSeparateCnt.x;
				eEffectDesc.iSeparateHeight = (_int)fSeparateCnt.y;

				ImGui::BulletText("fTimeDelta : "); ImGui::InputFloat("##fTimeDelta", &fTimeDelta);
				eEffectDesc.fTimeDelta = fTimeDelta;
				ImGui::Separator();
			}

			ImGui::Separator();
			ImGui::BulletText("Blend Type : ");
			if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
			if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
			if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
			if (ImGui::RadioButton("BlendType_Mix", &iBlendType, 3))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_MIX;
			ImGui::Separator();

			ImGui::BulletText("Option : ");
			ImGui::Checkbox("Use Trigger", &eEffectDesc.IsTrigger); ImGui::SameLine();
			ImGui::Checkbox("Use Billboard", &eEffectDesc.IsBillboard);
			ImGui::Checkbox("Use MaskTexture", &eEffectDesc.IsMask); ImGui::SameLine();
			ImGui::Checkbox("Use MovingPosition", &eEffectDesc.IsMovingPosition);

			if (m_eEffectDesc.IsMovingPosition)
				Set_OptionWindow_Rect(pEffect);

			ImGui::Separator();

			ImGui::BulletText("fVector : ");	 ImGui::InputFloat4("##fVector", (_float*)&fVector);
			eEffectDesc.vScale = XMVectorSet(fVector.x, fVector.y, fVector.z, fVector.w);

			ImGui::Separator();
			ImGui::BulletText("vColor : ");
			fColor = Set_ColorValue();
			eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

			pChildEffect->Set_EffectDesc(m_eEffectDesc);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Set Item Texture"))
		{
			static _int iCreateCnt = 1;
			static _int iSelectDTexture = 0;
			static _int iSelectMTexture = 0;
			static _int iSelectTextureType = 0;

			const _int  iTotalDTextureCnt = pChildEffect->Get_TotalDTextureCnt();
			const _int  iTotalMTextureCnt = pChildEffect->Get_TotalMTextureCnt();

			ImGui::BulletText("Texture Type : ");
			ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
			ImGui::RadioButton("MaskTexture", &iSelectTextureType, 1);

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
					pChildEffect->Edit_TextureComponent(iCreateCnt, 0);

				char** szDTextureType = new char*[iTotalDTextureCnt];
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
				CTexture*		pDiffuseTexture = (CTexture*)pChildEffect->Find_Component(szDTexture);

				if (pDiffuseTexture != nullptr)
				{
					_float fFrame = pChildEffect->Get_EffectDesc().fFrame[iSelectDTexture];
					ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
					ImGui::Separator();

					for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
					{
						if (i % 6)
							ImGui::SameLine();

						if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
						{
							eEffectDesc.fFrame[iSelectDTexture] = i * 1.0f;
							pChildEffect->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
						}
					}
				}
				for (size_t i = 0; i < iTotalDTextureCnt; ++i)
					Safe_Delete_Array(szDTextureType[i]);
				Safe_Delete_Array(szDTextureType);
			}
			else
			{
				ImGui::BulletText("Edit TextureComponent : "); ImGui::SameLine();

				ImGui::PushItemWidth(100);
				ImGui::InputInt("##EditMTexture", (_int*)&iCreateCnt, 1, 5); ImGui::SameLine();

				if (iCreateCnt <= 1)
					iCreateCnt = 1;
				else if (iCreateCnt >= 5)
					iCreateCnt = 5;

				if (ImGui::Button("Edit Texture Confirm"))
					pChildEffect->Edit_TextureComponent(0, iCreateCnt);

				char** szMTextureType = new char*[iTotalMTextureCnt];
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
				CTexture*		pDiffuseTexture = (CTexture*)pChildEffect->Find_Component(szMTexture);

				if (pDiffuseTexture != nullptr)
				{
					_float fMaskFrame = pChildEffect->Get_EffectDesc().fMaskFrame[iSelectMTexture];

					ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
					ImGui::Separator();

					for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
					{
						if (i % 6)
							ImGui::SameLine();

						if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
						{
							eEffectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
							pChildEffect->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
						}
					}
				}

				for (size_t i = 0; i < iTotalMTextureCnt; ++i)
					Safe_Delete_Array(szMTextureType[i]);
				Safe_Delete_Array(szMTextureType);
			}
			ImGui::EndTabItem();
		}
	}
	pChildEffect->Set_EffectDesc(eEffectDesc);
	ImGui::EndTabBar();
	RELEASE_INSTANCE(CGameInstance);
	ImGui::End();
}

void CImgui_Effect::Show_TrailWindow(CEffect_Base * pEffect)
{
	ImGui::Begin("Trail Window");

	static _int iTextureRender = 0;
	static _int iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	CEffect_Trail* pEffectTrail = dynamic_cast<CEffect_Trail*>(pEffect);
	if (pEffectTrail == nullptr)
		return;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 2. Texture Create
	_int    iCurLevel = pGameInstance->Get_CurLevelIndex();
	CEffect_Base::EFFECTDESC eEffectDesc = pEffectTrail->Get_EffectDesc();

	iTextureRender = (_int)eEffectDesc.eTextureRenderType;
	iBlendType = (_int)eEffectDesc.eBlendType;
	fTimeDelta = eEffectDesc.fTimeDelta;
	fCnt = _float2((_float)eEffectDesc.iWidthCnt, (_float)eEffectDesc.iHeightCnt);
	fSeparateCnt = _float2((_float)eEffectDesc.iSeparateWidth, (_float)eEffectDesc.iSeparateHeight);
	fVector = eEffectDesc.vScale;
	fColor = eEffectDesc.vColor;

	//////////////////////////////////////////////////////////////////////////
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None; // Tabbar Flag
	if (ImGui::BeginTabBar("##Value Setting", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Set Item Value"))
		{
			eEffectDesc.eTextureRenderType = CEffect_Base::EFFECTDESC::TEXTURERENDERTYPE::TEX_ONE;
			ImGui::BulletText("Blend Type : ");
			if (ImGui::RadioButton("BlendType_Default", &iBlendType, 0))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_DEFAULT;
			if (ImGui::RadioButton("BlendType_Alpha", &iBlendType, 1))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ALPHA;
			if (ImGui::RadioButton("BlendType_OneEffect", &iBlendType, 2))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_ONEEFFECT;
			if (ImGui::RadioButton("BlendType_Mix", &iBlendType, 3))
				eEffectDesc.eBlendType = CEffect_Base::EFFECTDESC::BLENDSTATE::BLENDSTATE_MIX;
			ImGui::Separator();

			ImGui::BulletText("Option : ");
			ImGui::Checkbox("Use MaskTexture", &eEffectDesc.IsMask);

			ImGui::Separator();
			ImGui::BulletText("vColor : ");
			fColor = Set_ColorValue();
			eEffectDesc.vColor = XMVectorSet(fColor.x, fColor.y, fColor.z, fColor.w);

			pEffectTrail->Set_EffectDesc(m_eEffectDesc);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Set Item Texture"))
		{
			static _int iCreateCnt = 1;
			static _int iSelectDTexture = 0;
			static _int iSelectMTexture = 0;
			static _int iSelectTextureType = 0;

			const _int  iTotalDTextureCnt = pEffectTrail->Get_TotalDTextureCnt();
			const _int  iTotalMTextureCnt = pEffectTrail->Get_TotalMTextureCnt();

			ImGui::BulletText("Texture Type : ");
			ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
			ImGui::RadioButton("MaskTexture", &iSelectTextureType, 1);

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
					pEffectTrail->Edit_TextureComponent(iCreateCnt, 0);

				char** szDTextureType = new char*[iTotalDTextureCnt];
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
				CTexture*		pDiffuseTexture = (CTexture*)pEffectTrail->Find_Component(szDTexture);

				if (pDiffuseTexture != nullptr)
				{
					_float fFrame = pEffectTrail->Get_EffectDesc().fFrame[iSelectDTexture];
					ImGui::BulletText("DTexture _ %d / %d", (_uint)fFrame, pDiffuseTexture->Get_TextureIdx());
					ImGui::Separator();

					for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
					{
						if (i % 6)
							ImGui::SameLine();

						if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
						{
							eEffectDesc.fFrame[iSelectDTexture] = i * 1.0f;
							pEffectTrail->Set_EffectDescDTexture(iSelectDTexture, i * 1.0f);
						}
					}
				}
				for (size_t i = 0; i < iTotalDTextureCnt; ++i)
					Safe_Delete_Array(szDTextureType[i]);
				Safe_Delete_Array(szDTextureType);
			}
			else
			{
				ImGui::BulletText("Edit TextureComponent : "); ImGui::SameLine();

				ImGui::PushItemWidth(100);
				ImGui::InputInt("##EditMTexture", (_int*)&iCreateCnt, 1, 5); ImGui::SameLine();

				if (iCreateCnt <= 1)
					iCreateCnt = 1;
				else if (iCreateCnt >= 5)
					iCreateCnt = 5;

				if (ImGui::Button("Edit Texture Confirm"))
					pEffectTrail->Edit_TextureComponent(0, iCreateCnt);

				char** szMTextureType = new char*[iTotalMTextureCnt];
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
				CTexture*		pDiffuseTexture = (CTexture*)pEffectTrail->Find_Component(szMTexture);

				if (pDiffuseTexture != nullptr)
				{
					_float fMaskFrame = pEffectTrail->Get_EffectDesc().fMaskFrame[iSelectMTexture];

					ImGui::BulletText("MTexture _ %d / %d", (_uint)fMaskFrame, pDiffuseTexture->Get_TextureIdx());
					ImGui::Separator();

					for (_uint i = 0; i < pDiffuseTexture->Get_TextureIdx(); ++i)
					{
						if (i % 6)
							ImGui::SameLine();

						if (ImGui::ImageButton(pDiffuseTexture->Get_Texture(i), ImVec2(50.f, 50.f)))
						{
							eEffectDesc.fMaskFrame[iSelectMTexture] = i * 1.0f;
							pEffectTrail->Set_EffectDescMTexture(iSelectMTexture, i * 1.0f);
						}
					}
				}

				for (size_t i = 0; i < iTotalMTextureCnt; ++i)
					Safe_Delete_Array(szMTextureType[i]);
				Safe_Delete_Array(szMTextureType);
			}
			ImGui::EndTabItem();
		}
	}
	pEffectTrail->Set_EffectDesc(eEffectDesc);
	ImGui::EndTabBar();
	RELEASE_INSTANCE(CGameInstance);

	ImGui::End();
}

HRESULT CImgui_Effect::Save_EffectData()
{

	return S_OK;
}

HRESULT CImgui_Effect::Load_EffectData()
{
	return S_OK;
}

CImgui_Effect* CImgui_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CImgui_Effect * pInstance = new CImgui_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CImgui_Effect Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Effect::Free()
{
	__super::Free();
}

