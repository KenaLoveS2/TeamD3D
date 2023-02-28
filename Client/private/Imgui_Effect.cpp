#include "stdafx.h"
#include "..\public\Imgui_Effect.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Camera.h"

#include "Effect.h"
#include "Effect_Point_Instancing.h"

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

	wstring		strDefault = L"Prototype_GameObject_";
	static char szEffectTag[64] = "";
	static _int iCreateRectCnt = 0;
	static _int  iCreateCnt = 1;

	static _int  iSelectEffectType = 0;
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
	ImGui::BulletText("EffectType : "); ImGui::Combo("##EffectType", &iSelectEffectType, szEffectType, IM_ARRAYSIZE(szEffectType));

	if (m_bIsRectLayer == true || m_bIsParticleLayer == true)
	{
		ImGui::Separator();
		ImGui::BulletText("EffectLayer Tag : ");
		CGameObject* pGameObject = LayerEffects_ListBox(pObjectTag, iLayerSize, pSelectObject, "##EffectLayer Tag", iSelectObj, TAGTYPE::TAG_CLONE);

		if (nullptr != pGameObject && bReset == true)
		{
			if (dynamic_cast<CEffect_Base*>(pGameObject)->Get_EffectDesc().eEffectType == CEffect_Base::tagEffectDesc::EFFECT_PLANE)
			{
				iSelectEffectType = EFFECT_PLANE;
				bIsCreate = true;
			}
			else if (dynamic_cast<CEffect_Base*>(pGameObject)->Get_EffectDesc().eEffectType == CEffect_Base::tagEffectDesc::EFFECT_PARTICLE)
			{
				iSelectEffectType = EFFECT_PARTICLE;
				bIsCreate = true;
			}
			else
			{
				iSelectEffectType = EFFECT_MESH;
				bIsCreate = true;
			}
		}

		ImGui::Button("Save_Data"); ImGui::SameLine();
		ImGui::Button("Load_Data"); ImGui::SameLine();
		if (ImGui::Button("Reset_Data"))
		{
			m_bIsRectLayer = false;
			m_bIsParticleLayer = false;
			iSelectEffectType = 0;
		}

		for (size_t i = 0; i < iLayerSize; ++i)
			Safe_Delete_Array(pObjectTag[i]);
		Safe_Delete_Array(pObjectTag);
	}

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

			if (FAILED(pGameInstance->Add_Prototype(szDefaultTag, CEffect::Create(m_pDevice, m_pContext))))
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

				if (FAILED(pGameInstance->Clone_GameObject(iCurLevel, L"Layer_Effect", szDefaultTag, szEffectCloneTag)))
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

			if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, szComponentTag, CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, iCreateCnt))))
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
		ImGui::Separator();
		ImGui::BulletText("CreateCnt : ");  ImGui::InputInt("##CreateCnt", &iCreateCnt);
		ImGui::InputTextWithHint("##EffectTag", "Only Input Tagname.", szEffectTag, 64); ImGui::SameLine();

		if (ImGui::Button("Confirm"))
		{
		}
		ImGui::Separator();

		if (bIsCreate == true && bIsCreate == true)
			CreateEffect_Mesh(iSelectObj);
		break;
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
	vector<CEffect_Base*>* vecChild = nullptr;
	if (pChild == nullptr)
	{
		vecChild = pEffect->Get_vecChild();
		iHaveChildSize = pEffect->Get_ChildCnt();
	}
	else
	{
		vecChild = pChild;
		iHaveChildSize = pChild->size();
	}
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
		static _int iDeleteCnt = 0;
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
	if (dynamic_cast<CEffect*>(pParents)->Get_ChildCnt() != 0)
	{
		ImGui::Checkbox("Child Option Window", &bChildOptionWindow);
		if (bChildOptionWindow)
		{
			vector<class CEffect_Base*>* pEffect = dynamic_cast<CEffect*>(pParents)->Get_vecChild();
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
		if (dynamic_cast<CEffect*>(pParents)->Get_ChildCnt() != 0)
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
				LayerChild_ListBox(pChildObjectTag, iGrandChildSize, pSelectChildObject, "CreateChild_ProtoeTag", iSelectChildObject, nullptr, vecChild, TAGTYPE::TAG_CLONE);
				for (size_t i = 0; i < iGrandChildSize; ++i)
					Safe_Delete_Array(pChildObjectTag[i]);
				Safe_Delete_Array(pChildObjectTag);
				LayerChild_ListBox(pChildObjectTag, iGrandChildSize, pSelectChildObject, "CreateChild_CloneTag", iSelectChildObject, nullptr, vecChild, TAGTYPE::TAG_PROTO);

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

					CEffect_Base::EFFECTDESC eEffectDesc = (*iter)->Get_EffectDesc();
					dynamic_cast<CEffect_Base*>(pParents)->Set_Child(eEffectDesc, iCreateChild, pSelectChildObject);
					bChild = false;
				}
				for (size_t i = 0; i < iGrandChildSize; ++i)
					Safe_Delete_Array(pChildObjectTag[i]);
				Safe_Delete_Array(pChildObjectTag);
			}

			if(dynamic_cast<CEffect_Base*>(pChildGameObject)->Get_ChildCnt() != 0 ) // 부모 - 자식 - 자식
			{
				static _bool bGrandChild = false;
				static _int iCreateGrandChild = 0;
				static _int iSelectGrandChild = 0;
				if (ImGui::CollapsingHeader("Cur Layer_Effect SuperGrandChild List"))
				{
					ImGui::BulletText("Input Create SuperGrandChild Cnt : ");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(200);
					ImGui::InputInt("##CreateSuperGrandChild", (_int*)&iCreateGrandChild);
					if (iCreateGrandChild <= 0)
						iCreateGrandChild = 0;
					else if (iCreateGrandChild >= 10)
						iCreateGrandChild = 10;

					LayerChild_ListBox(pSelectChildTag, iSuperGrandChildSize, pSelectGrandChildObject, "CreateGrandChild_ProtoeTag",
						iSelectChildObject, dynamic_cast<CEffect_Base*>(pChildGameObject), nullptr, TAGTYPE::TAG_CLONE);

					for (size_t i = 0; i < iSuperGrandChildSize; ++i)
						Safe_Delete_Array(pSelectChildTag[i]);
					Safe_Delete_Array(pSelectChildTag);

					LayerChild_ListBox(pSelectChildTag, iSuperGrandChildSize, pSelectChildObject, "CreateGrandChild_CloneTag",
						iSelectChildObject, dynamic_cast<CEffect_Base*>(pChildGameObject), nullptr, TAGTYPE::TAG_PROTO);

					if (iSuperGrandChildSize != 0)
						TransformView_child(iSelectGrandChild, dynamic_cast<CEffect_Base*>(pChildGameObject));

					ImGui::SameLine();
					if (ImGui::Button("Add Child"))
					{
						vector<CEffect_Base*>* vecChild = dynamic_cast<CEffect_Base*>(pChildGameObject)->Get_vecChild();
						auto& iter = vecChild->begin();
						for (_int i = 0; i < iSelectGrandChild; i++)
							iter++;

						CEffect_Base::EFFECTDESC eEffectDesc = (*iter)->Get_EffectDesc();
						dynamic_cast<CEffect_Base*>(pChildGameObject)->Set_Child(eEffectDesc, iCreateGrandChild, pSelectGrandChildObject);
						bGrandChild = false;
					}
					for (size_t i = 0; i < iSuperGrandChildSize; ++i)
						Safe_Delete_Array(pSelectChildTag[i]);
					Safe_Delete_Array(pSelectChildTag);
				}
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
	// hdr하고 effect=>hdr 먹이면 안되나봄

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
	static _uint vecSize = 0.0f;
	static _int iCurPlayIdx = 0.0f;

	vector<_float4>* vecPosition = dynamic_cast<CEffect*>(pEffect)->Get_FreePos();
	vecSize = (_uint)vecPosition->size();

	static _bool  bStart = true, bPause = false, bStop = false;
	static _bool  bPlay = false;

	if (bStart == true)
	{
		m_eEffectDesc.bStart = true;
		fPlayBackTime = m_eEffectDesc.fPlayBbackTime;
		dynamic_cast<CEffect*>(pEffect)->Set_FreePos();
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
	// 		_bool bNextTime = dynamic_cast<CEffect*>(pEffect)->Play_FreePos(*iter);
	// 		if (bNextTime)
	// 		{
	// 			dynamic_cast<CEffect*>(pEffect)->Set_Lerp(false);
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

		dynamic_cast<CEffect*>(pEffect)->Set_Position(*iter);
		iCurPlayIdx = iSelectObject;
	}
	for (size_t i = 0; i < vecSize; ++i)
		Safe_Delete_Array(pObjectTag[i]);
	Safe_Delete_Array(pObjectTag);

	ImGui::End();
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

	CEffect* pEffect = nullptr;

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
			pEffect = dynamic_cast<CEffect*>(iter->second);
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
				static _int iSelectTextureType = 0;

				const _int  iTotalDTextureCnt = pEffect->Get_TotalDTextureCnt();
				const _int  iTotalMTextureCnt = pEffect->Get_TotalMTextureCnt();

				ImGui::BulletText("Texture Type : ");
				ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
				ImGui::RadioButton("MAskTexture", &iSelectTextureType, 1);

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
				static _int iSelectTextureType = 0;

				const _int  iTotalDTextureCnt = pEffect->Get_TotalDTextureCnt();
				const _int  iTotalMTextureCnt = pEffect->Get_TotalMTextureCnt();

				ImGui::BulletText("Texture Type : ");
				ImGui::RadioButton("DiffuseTexture", &iSelectTextureType, 0); ImGui::SameLine();
				ImGui::RadioButton("MAskTexture", &iSelectTextureType, 1);

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
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImgui_Effect::CreateEffect_Mesh(_int iSelectObject)
{
	ImGui::BulletText("CreateEffect_Mesh");
}

void CImgui_Effect::Show_ChildWindow(CEffect_Base * pEffect)
{
	ImGui::Begin("Child Window");
	if (m_bIsRectLayer == false)
		return;

	static _int iTextureRender = 0;
	static _int iBlendType = 0;
	static _float  fTimeDelta = 0.0f;
	static _float2 fCnt, fSeparateCnt;
	static _float4 fVector = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	static _float4 fColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);

	CEffect* pChildEffect = dynamic_cast<CEffect*>(pEffect);
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
			ImGui::RadioButton("MAskTexture", &iSelectTextureType, 1);

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

	ImGui::End();
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

