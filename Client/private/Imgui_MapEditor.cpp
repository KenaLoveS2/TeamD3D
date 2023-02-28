#include "stdafx.h"
#include "..\public\Imgui_MapEditor.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"

#include "Json/json.hpp"
#include <fstream>

#include "EnviromentObj.h"
#include "ModelViewerObject.h"

#define IMGUI_TEXT_COLOR_CHANGE				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
#define IMGUI_TEXT_COLOR_CHANGE_END		ImGui::PopStyleColor();

static string strTexturePathNum[WJTextureType_UNKNOWN] =
{
	"NONE_path",					"DIFFUSE_path",							"SPECULAR_path",					"AMBIENT_path",							"EMISSIVE_path",
	"EMISSIVEMASK_path",		"NORMALS_path",						"MASK_path",							"SSS_MASK_path",						"SPRINT_EMISSIVE_path",
	"HAIR_DEPTH_Path",			"HAIR_ALPHA_path",						"HAIR_ROOT_path",				"COMP_MSK_CURV_path",			"COMP_H_R_AO_path",
	"METALNESS_path",			"COMP_AMBIENT_OCCLUSION_path",		"AMBIENT_OCCLUSION_path"
};



CImgui_MapEditor::CImgui_MapEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_MapEditor::Initialize(void * pArg)
{
	m_bComOptions.fill(false);

	return S_OK;
}

void CImgui_MapEditor::Imgui_FreeRender()
{
	ImGui::Text("<MapTool>");
	if (ImGui::CollapsingHeader("Selcte_Option"))
	{
		Imgui_SelectOption();
		Imgui_CreateEnviromentObj();
		Imgui_Save_Load_Json();

		Imgui_SelectObject_Add_TexturePath();
		Imgui_Control_ViewerCamTransform();
	}

	ImGui::End();
}

void CImgui_MapEditor::Imgui_SelectOption()
{
#pragma region 생성시 사용되는 챕터
	ImGui::RadioButton("CHAPTER_ONE_CAVE", &m_iChapterOption, _int(CEnviromentObj::CHAPTER_ONE_CAVE));
	ImGui::RadioButton("CHAPTER_TWO_FOREST", &m_iChapterOption, _int(CEnviromentObj::CHAPTER_TWO_FOREST));
#pragma endregion  ~생성시 사용되는 챕터

#pragma region 생성시 사용되는 프로토 타입
	if (ImGui::CollapsingHeader("ProtoType"))
	{
		static string FindProtoObjectlTag = "";		// ! == No Find
		ImGui::InputText("Find_ProtoObjectlTag", &FindProtoObjectlTag);

		if (ImGui::BeginListBox("#GameObject_Proto#"))
		{
			const bool bObjectSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ProtoTypeObjects())
			{
				if (dynamic_cast<CEnviromentObj*>(ProtoPair.second) == nullptr)
					continue;

				char szViewName[512], szProtoName[256];

				if (FindProtoObjectlTag != "")
				{
					wstring  Temp = ProtoPair.first;
					string str = CUtile::WstringToString(Temp);

					if (str.find(FindProtoObjectlTag, 20) != std::string::npos)
					{
						CUtile::WideCharToChar(ProtoPair.first, szProtoName);
						sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());

						if (ImGui::Selectable(szViewName, bObjectSelected))
							m_wstrModelName = ProtoPair.first;
					}
				}
				else
				{

					CUtile::WideCharToChar(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bObjectSelected))
						m_wstrProtoName = ProtoPair.first;			// 리스트 박스를 누르면 현재 프로토 타입 이름을 가져옴
				}

			}
			ImGui::EndListBox();
		}
	}
#pragma endregion ~생성시 사용되는 프로토 타입

#pragma region 생성시 사용되는 모델 이름
	if (ImGui::CollapsingHeader("Model_Proto"))
	{
		static string FindModelTag = "";		// ! == No Find
		ImGui::InputText("Find_Model_Tag", &FindModelTag);

		if (ImGui::BeginListBox("#Model_No_Instancing_Proto#"))
		{
			char szViewName[512], szProtoName[256];
			const bool bModelSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ComponentProtoType()[CGameInstance::GetInstance()->Get_CurLevelIndex()])
			{
				if (dynamic_cast<CModel*>(ProtoPair.second) == nullptr)
					continue;
				if (dynamic_cast<CModel*>(ProtoPair.second)->Get_IStancingModel() == true)	// nInstacing
					continue;
				if (dynamic_cast<CModel*>(ProtoPair.second)->Get_Type() != CModel::TYPE_NONANIM)	// nInstacing
					continue;

				if (FindModelTag != "")
				{
					wstring  Temp = ProtoPair.first;
					string str = CUtile::WstringToString(Temp);

					if (str.find(FindModelTag, 25) != std::string::npos)
					{
						CUtile::WideCharToChar(ProtoPair.first, szProtoName);
						sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());

						if (ImGui::Selectable(szViewName, bModelSelected))
						{
							m_wstrModelName = ProtoPair.first;
							m_bModelChange = true;
						}
					}
				}
				else
				{
					CUtile::WideCharToChar(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bModelSelected))
					{
						m_wstrModelName = ProtoPair.first;			// 리스트 박스를 누르면 현재 모델프로토 타입 이름을 가져옴
						m_bModelChange = true;
					}
				}
			}
			ImGui::EndListBox();
		}
		ImGui::Separator();
		if (ImGui::BeginListBox("#Mode_Instancing_Proto#"))
		{
			char szViewName[512], szProtoName[256];
			const bool bModelSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ComponentProtoType()[CGameInstance::GetInstance()->Get_CurLevelIndex()])
			{
				if (dynamic_cast<CModel*>(ProtoPair.second) == nullptr)	// nInstacing
					continue;
				
				if (dynamic_cast<CModel*>(ProtoPair.second)->Get_IStancingModel()==false)	// nInstacing
					continue;
				if (dynamic_cast<CModel*>(ProtoPair.second)->Get_Type() != CModel::TYPE_NONANIM)	// nInstacing
					continue;

				if (FindModelTag != "")
				{
					wstring  Temp = ProtoPair.first;
					string str = CUtile::WstringToString(Temp);

					if (str.find(FindModelTag, 25) != std::string::npos)
					{
						CUtile::WideCharToChar(ProtoPair.first, szProtoName);
						sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());

						if (ImGui::Selectable(szViewName, bModelSelected))
						{
							m_wstrModelName = ProtoPair.first;
							m_bModelChange = true;
						}
					}
				}
				else
				{
					CUtile::WideCharToChar(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bModelSelected))
					{
						m_wstrModelName = ProtoPair.first;			// 리스트 박스를 누르면 현재 모델프로토 타입 이름을 가져옴
						m_bModelChange = true;
					}
				}
			}
			ImGui::EndListBox();
		}
		ImGui::Separator();
		if (ImGui::BeginListBox("#Mode_Anim_Proto#"))
		{
			char szViewName[512], szProtoName[256];
			const bool bModelSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ComponentProtoType()[CGameInstance::GetInstance()->Get_CurLevelIndex()])
			{
				if (dynamic_cast<CModel*>(ProtoPair.second) == nullptr)	// nInstacing
					continue;
				if (dynamic_cast<CModel*>(ProtoPair.second)->Get_Type() != CModel::TYPE_ANIM)	
					continue;

				if (FindModelTag != "")
				{
					wstring  Temp = ProtoPair.first;
					string str = CUtile::WstringToString(Temp);

					if (str.find(FindModelTag, 25) != std::string::npos)
					{
						CUtile::WideCharToChar(ProtoPair.first, szProtoName);
						sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());

						if (ImGui::Selectable(szViewName, bModelSelected))
						{
							m_wstrModelName = ProtoPair.first;
							m_bModelChange = true;
						}
					}
				}
				else
				{
					CUtile::WideCharToChar(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bModelSelected))
					{
						m_wstrModelName = ProtoPair.first;			// 리스트 박스를 누르면 현재 모델프로토 타입 이름을 가져옴
						m_bModelChange = true;
					}
				}
			}
			ImGui::EndListBox();
		}
	}




#pragma endregion ~생성시 사용되는 모델 이름

#pragma region 생성시 사용되는 클론 이름짓기
	ImGui::InputText("Clone_Tag ", m_strCloneTag, CLONE_TAG_BUFF_SIZE);

#pragma endregion ~생성시 사용되는 모델 이름

#pragma  region		생성시 사용되는 옵션 종류

	if (ImGui::CollapsingHeader("Selcted Obj Option"))
	{
		ImGui::InputInt("Chose_Room_Option", &m_iCreateObjRoom_Option);
		ImGui::Checkbox("Add_MoveCtrlCom", &m_bComOptions[CEnviromentObj::COMPONENTS_CONTROL_MOVE]);  ImGui::SameLine();
		ImGui::Checkbox("Add_InteractionCom", &m_bComOptions[CEnviromentObj::COMPONENTS_INTERACTION]);
	}

#pragma endregion ~생성시 사용되는 모델 이름

#pragma region		선택된 오브젝트들 보여주기

	char szSelctedObject_Name[256], szSelctedModel_Name[256];
	CUtile::WideCharToChar(m_wstrProtoName.c_str(), szSelctedObject_Name);
	CUtile::WideCharToChar(m_wstrModelName.c_str(), szSelctedModel_Name);

	ImGui::Text("Selected_ProtoObj_Tag : %s", szSelctedObject_Name);
	ImGui::Text("Selected_Model_Tag : %s", szSelctedModel_Name);
	ImGui::Text("Selected_Clone_Tag : %s", m_strCloneTag);

	if(m_bModelChange ==true)
	{
		/* 모델 프로토 타입이 널이 아닐때 바꾸기*/
		if (m_wstrModelName != L"")
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (false == m_bOnceSearch)
			{
				m_pViewerObject = static_cast<CModelViewerObject*>(pGameInstance->Get_GameObjectPtr(pGameInstance->Get_CurLevelIndex(), TEXT("Layer_BackGround"), TEXT("VIEWER_Objcet")));
				m_bOnceSearch = true;
			}

			if (m_pViewerObject != nullptr)
				m_pViewerObject->Change_Model(pGameInstance->Get_CurLevelIndex(), m_wstrModelName.c_str());

			RELEASE_INSTANCE(CGameInstance);
			m_bModelChange = false;
		}
	}


#pragma endregion ~선택된 오브젝트들 보여주기
}

void CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront(CGameInstance *pGameInstace, CGameObject* pGameObject)
{
	assert(nullptr != pGameObject && "CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront");
	assert(nullptr != pGameInstace && "CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront");
#pragma region  컴포넌트 옵션 불값 받아서 추가하기
	if (true == m_bComOptions[CEnviromentObj::COMPONENTS_CONTROL_MOVE])
	{
		static_cast<CEnviromentObj*>(pGameObject)->
			Add_AdditionalComponent(pGameInstace->Get_CurLevelIndex(), TEXT("Com_CtrlMove"), CEnviromentObj::COMPONENTS_CONTROL_MOVE);
	}

	if (true == m_bComOptions[CEnviromentObj::COMPONENTS_INTERACTION])
	{
		static_cast<CEnviromentObj*>(pGameObject)->
			Add_AdditionalComponent(pGameInstace->Get_CurLevelIndex(), TEXT("Com_Interaction"), CEnviromentObj::COMPONENTS_INTERACTION);
	}
#pragma endregion ~컴포넌트 옵션 불값 받아서 추가하기

#pragma region 카메라 z 앞에서 pos 에서 생성하기
	CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
	assert(pTransform != nullptr && "CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront");

	_float4 vCamPos = pGameInstace->Get_CamPosition();
	_float4 vLook;

	memcpy(&vLook, &pGameInstace->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW).m[2], sizeof(_float4));

	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vCamPos) + XMVector3Normalize(XMLoadFloat4(&vLook)) * 30.f);

	pTransform->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(vPos.x, 0, vPos.z, 1.f));
#pragma endregion 카메라 z 앞에서 pos 에서 생성하기

}

void CImgui_MapEditor::Imgui_CreateEnviromentObj()
{
	ImGui::NewLine();
	ImGui::Text("Create_Button");
	if (ImGui::Button("Create_EnviromentObj"))
	{
		CGameInstance *pGameInstace = GET_INSTANCE(CGameInstance);
		CGameObject* pCreateObject = nullptr;

		CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
		EnviromentDesc.szProtoObjTag = m_wstrProtoName;
		EnviromentDesc.szModelTag = m_wstrModelName;
		EnviromentDesc.szTextureTag = TEXT("");
		EnviromentDesc.iRoomIndex = m_iCreateObjRoom_Option;
		EnviromentDesc.eChapterType = static_cast<CEnviromentObj::CHAPTER>(m_iChapterOption);
		EnviromentDesc.iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();
		string			strCloneTag = m_strCloneTag;
		_tchar *pCloneName = CUtile::StringToWideChar(strCloneTag);
		CGameInstance::GetInstance()->Add_String(pCloneName);

		if (FAILED(pGameInstace->Clone_GameObject(pGameInstace->Get_CurLevelIndex(),
			TEXT("Layer_Enviroment"),
			EnviromentDesc.szProtoObjTag.c_str(),
			pCloneName, &EnviromentDesc, &pCreateObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");
		Imgui_AddComponentOption_CreateCamFront(pGameInstace, pCreateObject);
		Imgui_Create_Option_Reset();
		RELEASE_INSTANCE(CGameInstance);
	}
}


void CImgui_MapEditor::Imgui_Save_Load_Json()
{

	ImGui::Checkbox("Write File ", &m_bSaveWrite);

	if (m_bSaveWrite)
		ImGui::InputText("Save_Name : ", &m_strFileName);

	if (ImGui::Button("Confirm_Save"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Save Folder", "Select Save Folder", ".json", "../Bin/Save Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Save Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Imgui_Save_Func();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}


	ImGui::SameLine();

	if (ImGui::Button("Confirm_Load"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Load Folder", "Select Load Folder", ".json", "../Bin/Load Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Load Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Imgui_Load_Func();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}


	//if (ImGui::Button("Load"))
	//	Imgui_Load_Func();
}

void CImgui_MapEditor::Imgui_SelectObject_Add_TexturePath()
{
	CGameObject*	pSelectEnviObj = CGameInstance::GetInstance()->Get_SelectObjectPtr();

	if (nullptr != dynamic_cast<CEnviromentObj*>(pSelectEnviObj))
	{
		wstring wstrCloneTag = pSelectEnviObj->Get_ObjectCloneName();
		string	strClontTag = CUtile::WstringToString(wstrCloneTag);

		ImGui::Text("Cur Obj Clone Name : %s", strClontTag.c_str());

		Imgui_TexturePathNaming();
		static string	textureFilePath = "";
		ImGui::InputText("Texture_Path_Name", &textureFilePath);
	
		if (ImGui::Button("Add FilePath"))
		{
			_tchar * pFilePath = 		CUtile::StringToWideChar(textureFilePath);
			CGameInstance::GetInstance()->Add_String(pFilePath);
			static_cast<CEnviromentObj*>(pSelectEnviObj)->Add_TexturePath(pFilePath, (aiTextureType)m_iTexturePathNum);
		}

		Imgui_TexturePathViewer(pSelectEnviObj);
		Imgui_Instancing_control(pSelectEnviObj);

	}
}

void CImgui_MapEditor::Imgui_Control_ViewerCamTransform()
{
	ImGui::Separator();

	if (m_pViewerObject != nullptr)
	{
		static float fYPos_Num = 0.1f;
		static float fXAngle_Num = 0.1f;
		static float fZPos_Num = -5.f;

		_float fViewerCamYPos = m_pViewerObject->Get_ViewerCamYPos();
		_float fViewerCamXAngle = m_pViewerObject->Get_ViewerCamXAngle();


		ImGui::Text("ViewerCam ZPos, %f", &fZPos_Num);
		ImGui::Text("ViewerCam YPos, %f", &fViewerCamYPos);
		ImGui::Text("ViewerCam XAngle, %f", fViewerCamXAngle);


		ImGui::InputFloat("ZPosNum", &fZPos_Num); ImGui::SameLine();
		if (ImGui::Button("zPos Chagne"))
			m_pViewerObject->Set_ViewerCamZPos(fZPos_Num);
		ImGui::InputFloat("YPos_Increase&Reduce_Num", &fYPos_Num);
		ImGui::InputFloat("xAngle_Increase&Reduce_Num", &fXAngle_Num);
		
		m_pViewerObject->Set_ViewerCamMoveRatio(fYPos_Num, fXAngle_Num);
	
	
	}
	

}

void CImgui_MapEditor::Imgui_TexturePathNaming()
{
	ImGui::InputInt(strTexturePathNum[m_iTexturePathNum].c_str(), &m_iTexturePathNum); 

	if (m_iTexturePathNum < 0)
		m_iTexturePathNum = 0;

	if (m_iTexturePathNum > _uint(WJTextureType_AMBIENT_OCCLUSION))
		m_iTexturePathNum = _uint(WJTextureType_AMBIENT_OCCLUSION);
}


void CImgui_MapEditor::Imgui_Save_Func()
{
	// 현재 선택한 파일 경로 불러오는 함수
	string      strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
	if (m_bSaveWrite == true)
	{
		char   szDash[128] = "\\";
		strcat_s(szDash, m_strFileName.c_str());
		strSaveDirectory += string(szDash);
		strSaveDirectory += ".json";
	}
	else
	{
		string	   strSaveFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
		char   szDash[128] = "\\";
		strcat_s(szDash, strSaveFileName.c_str());
		strSaveDirectory += string(szDash);
	}

	ofstream      file(strSaveDirectory.c_str());
	Json	jEnviromentObjList;
	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	_float4x4	fWroldMatrix;
	_float		fElement = 0.f;
	string		szLayerTag = "Layer_Enviroment";
	string		szProtoObjTag = "";
	string		szModelTag = "";
	string		szTextureTag = "";
	char*		szCloneTag;

	CEnviromentObj::ENVIROMENT_DESC SaveJson_Desc;
	CEnviromentObj::ENVIROMENT_DESC SaveDataDesc;

	jEnviromentObjList["0_LayerTag"] = szLayerTag;

	for (auto& pObject : pGameInstace->Find_Layer(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Enviroment")->GetGameObjects())
	{
		if (dynamic_cast<CEnviromentObj*>(pObject.second) == nullptr)
			continue;

		Json jChild;

		SaveJson_Desc = static_cast<CEnviromentObj*>(pObject.second)->Get_EnviromentDesc();

		SaveDataDesc.szProtoObjTag = SaveJson_Desc.szProtoObjTag;
		SaveDataDesc.szModelTag = SaveJson_Desc.szModelTag;
		SaveDataDesc.szTextureTag = SaveJson_Desc.szTextureTag;
		SaveDataDesc.iRoomIndex = SaveJson_Desc.iRoomIndex;
		SaveDataDesc.eChapterType = SaveJson_Desc.eChapterType;

		szProtoObjTag = CUtile::WstringToString(SaveDataDesc.szProtoObjTag);
		szModelTag = CUtile::WstringToString(SaveDataDesc.szModelTag);
		szTextureTag = CUtile::WstringToString(SaveDataDesc.szTextureTag);
		szCloneTag = CUtile::WideCharToChar(const_cast<_tchar*>(pObject.second->Get_ObjectCloneName()));

		jChild["0_ProtoTag"] = szProtoObjTag;
		jChild["1_ModelTag"] = szModelTag;
		jChild["2_TextureTag"] = szTextureTag;
		jChild["3_CloneTag"] = szCloneTag;
		jChild["4_RoomIndex"] = (int)(SaveDataDesc.iRoomIndex);
		jChild["5_ChapterType"] = (int)(SaveDataDesc.eChapterType);

		vector<const _tchar*>* vecComTag = static_cast<CEnviromentObj*>(pObject.second)->Get_CurObjectComponentTag();

		for (auto& pComtag : *vecComTag)
		{
			wstring Temp = pComtag;
			string	strSaveTag = CUtile::WstringToString(Temp);
			jChild["6_AddComTag"].push_back(strSaveTag);
		}

		CTransform* pTransform = static_cast<CTransform*>(pObject.second->Find_Component(L"Com_Transform"));
		assert(pTransform != nullptr && "CImgui_MapEditor::Imgui_Save_Func()");
		XMStoreFloat4x4(&fWroldMatrix, pTransform->Get_WorldMatrix());

		for (int i = 0; i < 16; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&fWroldMatrix + i, sizeof(float));
			jChild["7_Transform State"].push_back(fElement);		// 배열 저장. 컨테이너의 구조랑 비슷합니다. 이렇게 하면 Transform State에는 16개의 float 값이 저장됩니다.
		}

		aiTextureType_FilePath* TextureFilePaths = static_cast<CEnviromentObj*>(pObject.second)->Get_TexturePaths();

		string	strDiffuse = CUtile::WstringToString(TextureFilePaths->DIFFUSE_path);
		jChild["8_DIFFUSE_path"] = strDiffuse;

		string	SPECULAR_path = CUtile::WstringToString(TextureFilePaths->SPECULAR_path);
		jChild["9_SPECULAR_path"] = SPECULAR_path;

		string	AMBIENT_path = CUtile::WstringToString(TextureFilePaths->AMBIENT_path);
		jChild["10_AMBIENT_path"] = AMBIENT_path;

		string	EMISSIVE_path = CUtile::WstringToString(TextureFilePaths->EMISSIVE_path);
		jChild["11_EMISSIVE_path"] = EMISSIVE_path;

		string	EMISSIVEMASK_path = CUtile::WstringToString(TextureFilePaths->EMISSIVEMASK_path);
		jChild["12_EMISSIVEMASK_path"] = EMISSIVEMASK_path;

		string	NORMALS_path = CUtile::WstringToString(TextureFilePaths->NORMALS_path);
		jChild["13_NORMALS_path"] = NORMALS_path;

		string	MASK_path = CUtile::WstringToString(TextureFilePaths->MASK_path);
		jChild["14_MASK_path"] = MASK_path;

		string	SSS_MASK_path = CUtile::WstringToString(TextureFilePaths->SSS_MASK_path);
		jChild["15_SSS_MASK_path"] = SSS_MASK_path;

		string	SPRINT_EMISSIVE_path = CUtile::WstringToString(TextureFilePaths->SPRINT_EMISSIVE_path);
		jChild["16_SPRINT_EMISSIVE_path"] = SPRINT_EMISSIVE_path;

		string	HAIR_DEPTH_Path = CUtile::WstringToString(TextureFilePaths->HAIR_DEPTH_Path);
		jChild["17_LIGHTMAP_path"] = HAIR_DEPTH_Path;

		string	HAIR_ALPHA_path = CUtile::WstringToString(TextureFilePaths->HAIR_ALPHA_path);
		jChild["18_REFLECTION_path"] = HAIR_ALPHA_path;

		string	HAIR_ROOT_path = CUtile::WstringToString(TextureFilePaths->HAIR_ROOT_path);
		jChild["19_BASE_COLOR_path"] = HAIR_ROOT_path;

		string	COMP_MSK_CURV_path = CUtile::WstringToString(TextureFilePaths->COMP_MSK_CURV_path);
		jChild["20_NORMAL_CAMERA_path"] = COMP_MSK_CURV_path;

		string	COMP_H_R_AO_path = CUtile::WstringToString(TextureFilePaths->COMP_H_R_AO_path);
		jChild["21_EMISSION_COLOR_path"] = COMP_H_R_AO_path;

		string	METALNESS_path = CUtile::WstringToString(TextureFilePaths->METALNESS_path);
		jChild["22_METALNESS_path"] = METALNESS_path;

		string	COMP_AMBIENT_OCCLUSION_path = CUtile::WstringToString(TextureFilePaths->COMP_AMBIENT_OCCLUSION_path);
		jChild["23_DIFFUSE_ROUGHNESS_path"] = COMP_AMBIENT_OCCLUSION_path;

		string	AMBIENT_OCCLUSION_path = CUtile::WstringToString(TextureFilePaths->AMBIENT_OCCLUSION_path);
		jChild["24_AMBIENT_OCCLUSION_path"] = AMBIENT_OCCLUSION_path;

		CModel* pModel = static_cast<CModel*>(pObject.second->Find_Component(L"Com_Model"));
		
		if (pModel != nullptr &&  true == pModel->Get_IStancingModel())
		{
			vector<_float4x4*> SaveInsPosMatrixVec = (*pModel->Get_InstancePos());
			size_t InstancingPosSize = SaveInsPosMatrixVec.size();
			for (size_t i = 0; i < InstancingPosSize; ++i)
			{
				_float4x4 fInsMaxtrix = *SaveInsPosMatrixVec[i];
				for (int j = 0; j < 16; ++j)
				{
					fElement = 0.f;
					memcpy(&fElement, (float*)&fInsMaxtrix + j, sizeof(float));
					jChild["25_Instancing_Transform_matrix"].push_back(fElement);
				}
			}
		}
	

		jEnviromentObjList["1_Data"].push_back(jChild);
		szProtoObjTag = "";
		szModelTag = "";
		szTextureTag = "";
		Safe_Delete_Array(szCloneTag);
	}


	file << jEnviromentObjList;
	file.close();
	MSG_BOX("Save_jEnviromentObjList");

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CImgui_MapEditor::Imgui_Load_Func()
{
	string      strLoadDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();   // GetCurrentPath F12로 들가면 비슷한 다른 함수 더 있음.
	string	   strLoadFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
	char   szDash[128] = "\\";
	strcat_s(szDash, strLoadFileName.c_str());
	strLoadDirectory += string(szDash);
	//strLoadDirectory += ".json";

	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadEnviromentObjList;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadEnviromentObjList;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		szLayerTag = "";
	string		szProtoObjTag = "";
	string		szModelTag = "";
	string		szTextureTag = "";
	string		szCloneTag = "";
	_tchar*		wszCloneTag = L"";
	int				iLoadRoomIndex = 0;
	int				iLoadChapterType = 0;
	vector<string> StrComponentVec;
	array<string,(_int)WJTextureType_UNKNOWN> strFilePaths_arr;

	strFilePaths_arr.fill("");

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		string	strDiffuse="", SPECULAR_path = "", AMBIENT_path = "", EMISSIVE_path = "",
			EMISSIVEMASK_path = "", NORMALS_path = "", MASK_path = "", SSS_MASK_path = "",
			SPRINT_EMISSIVE_path = "", HAIR_DEPTH_Path = "", HAIR_ALPHA_path = "", HAIR_ROOT_path = "",
			COMP_MSK_CURV_path = "", COMP_H_R_AO_path = "", METALNESS_path = "", COMP_AMBIENT_OCCLUSION_path = "",
			AMBIENT_OCCLUSION_path = "";

		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		jLoadChild["4_RoomIndex"].get_to<int>(iLoadRoomIndex);
		jLoadChild["5_ChapterType"].get_to <int>(iLoadChapterType);

		for (string strTag : jLoadChild["6_AddComTag"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
			StrComponentVec.push_back(strTag);

		float	fElement = 0.f;
		int		k = 0;
		for (float fElement : jLoadChild["7_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
		}
		//for (string strTag : jLoadChild["8_TextureFilePath"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
		//	StrFilePathVec.push_back(strTag);

		//aiTextureType_FilePath* TextureFilePaths = static_cast<CEnviromentObj*>(pObject.second)->Get_TexturePaths();

		jLoadChild["8_DIFFUSE_path"].get_to<string>(strDiffuse);															strFilePaths_arr[WJTextureType_DIFFUSE] = strDiffuse;
		jLoadChild["9_SPECULAR_path"].get_to<string>(SPECULAR_path);											strFilePaths_arr[WJTextureType_SPECULAR] = SPECULAR_path;
		jLoadChild["10_AMBIENT_path"].get_to<string>(AMBIENT_path);												strFilePaths_arr[WJTextureType_AMBIENT] = AMBIENT_path;
		jLoadChild["11_EMISSIVE_path"].get_to<string>(EMISSIVE_path);												strFilePaths_arr[WJTextureType_EMISSIVE] = EMISSIVE_path;
		jLoadChild["12_EMISSIVEMASK_path"].get_to<string>(EMISSIVEMASK_path);							strFilePaths_arr[WJTextureType_EMISSIVEMASK] = EMISSIVEMASK_path;
		jLoadChild["13_NORMALS_path"].get_to<string>(NORMALS_path);											strFilePaths_arr[WJTextureType_NORMALS] = NORMALS_path;
		jLoadChild["14_MASK_path"].get_to<string>(MASK_path);														strFilePaths_arr[WJTextureType_MASK] = MASK_path;
		jLoadChild["15_SSS_MASK_path"].get_to<string>(SSS_MASK_path);											strFilePaths_arr[WJTextureType_SSS_MASK] = SSS_MASK_path;
		jLoadChild["16_SPRINT_EMISSIVE_path"].get_to<string>(SPRINT_EMISSIVE_path);					strFilePaths_arr[WJTextureType_SPRINT_EMISSIVE] = SPRINT_EMISSIVE_path;
		jLoadChild["17_LIGHTMAP_path"].get_to<string>(HAIR_DEPTH_Path);											strFilePaths_arr[WJTextureType_HAIR_DEPTH] = HAIR_DEPTH_Path;
		jLoadChild["18_REFLECTION_path"].get_to<string>(HAIR_ALPHA_path);									strFilePaths_arr[WJTextureType_HAIR_ALPHA] = HAIR_ALPHA_path;
		jLoadChild["19_BASE_COLOR_path"].get_to<string>(HAIR_ROOT_path);								 strFilePaths_arr[WJTextureType_HAIR_ROOT] = HAIR_ROOT_path;
		jLoadChild["20_NORMAL_CAMERA_path"].get_to<string>(COMP_MSK_CURV_path);				strFilePaths_arr[WJTextureType_COMP_MSK_CURV] = COMP_MSK_CURV_path;
		jLoadChild["21_EMISSION_COLOR_path"].get_to<string>(COMP_H_R_AO_path);					strFilePaths_arr[WJTextureType_COMP_H_R_AO] = COMP_H_R_AO_path;
		jLoadChild["22_METALNESS_path"].get_to<string>(METALNESS_path);									strFilePaths_arr[WJTextureType_METALNESS] = METALNESS_path;
		jLoadChild["23_DIFFUSE_ROUGHNESS_path"].get_to<string>(COMP_AMBIENT_OCCLUSION_path);	strFilePaths_arr[WJTextureType_COMP_AMBIENT_OCCLUSION] = COMP_AMBIENT_OCCLUSION_path;
		jLoadChild["24_AMBIENT_OCCLUSION_path"].get_to<string>(AMBIENT_OCCLUSION_path);	strFilePaths_arr[WJTextureType_AMBIENT_OCCLUSION] = AMBIENT_OCCLUSION_path;

	
		vector<_float4x4>	vecInstnaceMatrixVec;
		_int MatrixNumber = 0;
		_float4x4 fInsMaxtrix;
		for (float fElement : jLoadChild["25_Instancing_Transform_matrix"])
		{
			memcpy(((float*)&fInsMaxtrix) + (MatrixNumber++), &fElement, sizeof(float));
		
			if (MatrixNumber >= 16)
			{
				vecInstnaceMatrixVec.push_back(fInsMaxtrix);
				MatrixNumber = 0;
			}
		}
			
		m_wstrProtoName.assign(szProtoObjTag.begin(), szProtoObjTag.end());
		m_wstrModelName.assign(szModelTag.begin(), szModelTag.end());
		m_wstrTexturelName.assign(szTextureTag.begin(), szTextureTag.end());
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		pGameInstance->Add_String(wszCloneTag);

		EnviromentDesc.szProtoObjTag = m_wstrProtoName;
		EnviromentDesc.szModelTag = m_wstrModelName;
		EnviromentDesc.szTextureTag = m_wstrTexturelName;
		EnviromentDesc.iRoomIndex = iLoadRoomIndex;
		EnviromentDesc.eChapterType = CEnviromentObj::CHAPTER(iLoadChapterType);
		Insert_TextureFilePath(pGameInstance, EnviromentDesc, strFilePaths_arr);
		
		if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(),
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		Load_ComTagToCreate(pGameInstance, pLoadObject, StrComponentVec);
		Imgui_Instacing_PosLoad(pLoadObject, vecInstnaceMatrixVec);


		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;
		StrComponentVec.clear();
		strFilePaths_arr.fill("");
		vecInstnaceMatrixVec.clear();
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CImgui_MapEditor::Imgui_Create_Option_Reset()/* 초기화*/
{
	m_bComOptions.fill(false);
	m_iCreateObjRoom_Option = 0;
}

void CImgui_MapEditor::Load_ComTagToCreate(CGameInstance * pGameInstace, CGameObject * pGameObject, vector<string> vecStr)
{
	assert(nullptr != pGameObject && "CImgui_MapEditor::Load_ComTagToCreate");
	assert(nullptr != pGameInstace && "CImgui_MapEditor::Load_ComTagToCreate");

	if (vecStr.size() == 0)
		return;

	for (auto str : vecStr)
	{
		if (str == "Com_CtrlMove")
		{
			static_cast<CEnviromentObj*>(pGameObject)->Add_AdditionalComponent(pGameInstace->Get_CurLevelIndex(),
				TEXT("Com_CtrlMove"), CEnviromentObj::COMPONENTS_CONTROL_MOVE);
		}
		else if (str == "Com_Interaction")
		{
			static_cast<CEnviromentObj*>(pGameObject)->Add_AdditionalComponent(pGameInstace->Get_CurLevelIndex(),
				TEXT("Com_Interaction"), CEnviromentObj::COMPONENTS_INTERACTION);
		}
		else
			continue;
	}

}

void CImgui_MapEditor::Insert_TextureFilePath(CGameInstance * pGameInstace, CEnviromentObj::tagEnviromnetObjectDesc& EnviromentDesc, 
	array<string, (_int)WJTextureType_UNKNOWN> vecStr)
{
	assert(nullptr != pGameInstace && "CImgui_MapEditor::Load_TextureFilePath");

	
	_tchar* pTextureFilePath = L"";
	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_DIFFUSE]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.DIFFUSE_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_SPECULAR]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.SPECULAR_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_AMBIENT]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.AMBIENT_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_EMISSIVE]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.EMISSIVE_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_EMISSIVEMASK]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.EMISSIVEMASK_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_NORMALS]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.NORMALS_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_MASK]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.MASK_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_SSS_MASK]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.SSS_MASK_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_SPRINT_EMISSIVE]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.SPRINT_EMISSIVE_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_HAIR_DEPTH]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.HAIR_DEPTH_Path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_HAIR_ALPHA]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.HAIR_ALPHA_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_HAIR_ROOT]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.HAIR_ROOT_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_COMP_MSK_CURV]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.COMP_MSK_CURV_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_COMP_H_R_AO]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.COMP_H_R_AO_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_METALNESS]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.METALNESS_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_COMP_AMBIENT_OCCLUSION]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.COMP_AMBIENT_OCCLUSION_path = pTextureFilePath;

	pTextureFilePath = CUtile::StringToWideChar(vecStr[WJTextureType_AMBIENT_OCCLUSION]);
	pGameInstace->Add_String(pTextureFilePath);
	EnviromentDesc.AI_textureFilePaths.AMBIENT_OCCLUSION_path = pTextureFilePath;

}

void CImgui_MapEditor::Load_MapObjects(_uint iLevel,  string JsonFileName)
{
	string      strLoadDirectory = "../Bin/Data/EnviromentObj_Json_Dir/";
	strLoadDirectory += JsonFileName;


	ifstream      file(strLoadDirectory);
	Json	jLoadEnviromentObjList;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadEnviromentObjList;
	file.close();

	_float4x4	fWroldMatrix;
	wstring		wstrProtoName = TEXT("");
	wstring		wstrModelName = TEXT("");
	wstring		wstrTexturelName = TEXT("");
	_tchar*		wszLayerTag = L"";
	string		szLayerTag = "";
	string		szProtoObjTag = "";
	string		szModelTag = "";
	string		szTextureTag = "";
	string		szCloneTag = "";
	_tchar*		wszCloneTag = L"";
	int				iLoadRoomIndex = 0;
	int				iLoadChapterType = 0;
	vector<string> StrComTagVec;
	array<string, (_int)WJTextureType_UNKNOWN> strFilePaths_arr;
	strFilePaths_arr.fill("");

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		string	strDiffuse = "", SPECULAR_path = "", AMBIENT_path = "", EMISSIVE_path = "",
			EMISSIVEMASK_path = "", NORMALS_path = "", MASK_path = "", SSS_MASK_path = "",
			SPRINT_EMISSIVE_path = "", HAIR_DEPTH_Path = "", HAIR_ALPHA_path = "", HAIR_ROOT_path = "",
			COMP_MSK_CURV_path = "", COMP_H_R_AO_path = "", METALNESS_path = "", COMP_AMBIENT_OCCLUSION_path = "",
			AMBIENT_OCCLUSION_path = "";

		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		jLoadChild["4_RoomIndex"].get_to<int>(iLoadRoomIndex);
		jLoadChild["5_ChapterType"].get_to <int>(iLoadChapterType);

		for (string strTag : jLoadChild["6_AddComTag"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
			StrComTagVec.push_back(strTag);

		float	fElement = 0.f;
		int k = 0;
		for (float fElement : jLoadChild["7_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
		}

		jLoadChild["8_DIFFUSE_path"].get_to<string>(strDiffuse);															strFilePaths_arr[WJTextureType_DIFFUSE] = strDiffuse;
		jLoadChild["9_SPECULAR_path"].get_to<string>(SPECULAR_path);											strFilePaths_arr[WJTextureType_SPECULAR] = SPECULAR_path;
		jLoadChild["10_AMBIENT_path"].get_to<string>(AMBIENT_path);												strFilePaths_arr[WJTextureType_AMBIENT] = AMBIENT_path;
		jLoadChild["11_EMISSIVE_path"].get_to<string>(EMISSIVE_path);												strFilePaths_arr[WJTextureType_EMISSIVE] = EMISSIVE_path;
		jLoadChild["12_EMISSIVEMASK_path"].get_to<string>(EMISSIVEMASK_path);							strFilePaths_arr[WJTextureType_EMISSIVEMASK] = EMISSIVEMASK_path;
		jLoadChild["13_NORMALS_path"].get_to<string>(NORMALS_path);											strFilePaths_arr[WJTextureType_NORMALS] = NORMALS_path;
		jLoadChild["14_MASK_path"].get_to<string>(MASK_path);														strFilePaths_arr[WJTextureType_MASK] = MASK_path;
		jLoadChild["15_SSS_MASK_path"].get_to<string>(SSS_MASK_path);											strFilePaths_arr[WJTextureType_SSS_MASK] = SSS_MASK_path;
		jLoadChild["16_SPRINT_EMISSIVE_path"].get_to<string>(SPRINT_EMISSIVE_path);					strFilePaths_arr[WJTextureType_SPRINT_EMISSIVE] = SPRINT_EMISSIVE_path;
		jLoadChild["17_LIGHTMAP_path"].get_to<string>(HAIR_DEPTH_Path);											strFilePaths_arr[WJTextureType_HAIR_DEPTH] = HAIR_DEPTH_Path;
		jLoadChild["18_REFLECTION_path"].get_to<string>(HAIR_ALPHA_path);									strFilePaths_arr[WJTextureType_HAIR_ALPHA] = HAIR_ALPHA_path;
		jLoadChild["19_BASE_COLOR_path"].get_to<string>(HAIR_ROOT_path);								strFilePaths_arr[WJTextureType_HAIR_ROOT] = HAIR_ROOT_path;
		jLoadChild["20_NORMAL_CAMERA_path"].get_to<string>(COMP_MSK_CURV_path);				strFilePaths_arr[WJTextureType_COMP_MSK_CURV] = COMP_MSK_CURV_path;
		jLoadChild["21_EMISSION_COLOR_path"].get_to<string>(COMP_H_R_AO_path);					strFilePaths_arr[WJTextureType_COMP_H_R_AO] = COMP_H_R_AO_path;
		jLoadChild["22_METALNESS_path"].get_to<string>(METALNESS_path);									strFilePaths_arr[WJTextureType_METALNESS] = METALNESS_path;
		jLoadChild["23_DIFFUSE_ROUGHNESS_path"].get_to<string>(COMP_AMBIENT_OCCLUSION_path);	strFilePaths_arr[WJTextureType_COMP_AMBIENT_OCCLUSION] = COMP_AMBIENT_OCCLUSION_path;
		jLoadChild["24_AMBIENT_OCCLUSION_path"].get_to<string>(AMBIENT_OCCLUSION_path);	strFilePaths_arr[WJTextureType_AMBIENT_OCCLUSION] = AMBIENT_OCCLUSION_path;

		
		vector<_float4x4>	vecInstnaceMatrixVec;
		_int MatrixNumber = 0;
		_float4x4 fInsMaxtrix;
		for (float fElement : jLoadChild["25_Instancing_Transform_matrix"])
		{
			memcpy(((float*)&fInsMaxtrix) + (MatrixNumber++), &fElement, sizeof(float));

			if (MatrixNumber >= 16)
			{
				vecInstnaceMatrixVec.push_back(fInsMaxtrix);
				MatrixNumber = 0;
			}
		}

		wstrProtoName.assign(szProtoObjTag.begin(), szProtoObjTag.end());
		wstrModelName.assign(szModelTag.begin(), szModelTag.end());
		wstrTexturelName.assign(szTextureTag.begin(), szTextureTag.end());
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		pGameInstance->Add_String(wszCloneTag);

		EnviromentDesc.szProtoObjTag = wstrProtoName;
		EnviromentDesc.szModelTag = wstrModelName;
		EnviromentDesc.szTextureTag = wstrTexturelName;
		EnviromentDesc.iRoomIndex = iLoadRoomIndex;
		EnviromentDesc.eChapterType = CEnviromentObj::CHAPTER(iLoadChapterType);
		EnviromentDesc.iCurLevel = iLevel;
		Insert_TextureFilePath(pGameInstance, EnviromentDesc, strFilePaths_arr);

		if (FAILED(pGameInstance->Clone_GameObject(iLevel,
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		Load_ComTagToCreate(pGameInstance, pLoadObject, StrComTagVec);
		Imgui_Instacing_PosLoad(pLoadObject, vecInstnaceMatrixVec);

		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;
		StrComTagVec.clear();
		strFilePaths_arr.fill("");
	}

	RELEASE_INSTANCE(CGameInstance);

}

CImgui_MapEditor * CImgui_MapEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CImgui_MapEditor*	pInstance = new CImgui_MapEditor(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImgui_MapEditor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_MapEditor::Free()
{
	__super::Free();

	m_wstrProtoName.clear();

}

void CImgui_MapEditor::Imgui_TexturePathViewer(CGameObject*	pSelectEnviObj)
{
	aiTextureType_FilePath* structAiTextureFilePath = static_cast<CEnviromentObj*>(pSelectEnviObj)->Get_TexturePaths();

	wstring  wstrDiffusePath = structAiTextureFilePath->DIFFUSE_path;
	ImGui::Text(strTexturePathNum[1].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrDiffusePath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrSPECULARPath = structAiTextureFilePath->SPECULAR_path;
	ImGui::Text(strTexturePathNum[2].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrSPECULARPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrAMBIENTPath = structAiTextureFilePath->AMBIENT_path;
	ImGui::Text(strTexturePathNum[3].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrAMBIENTPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrEMISSIVEPath = structAiTextureFilePath->EMISSIVE_path;
	ImGui::Text(strTexturePathNum[4].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrEMISSIVEPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrEMISSIVE_MASKPath = structAiTextureFilePath->EMISSIVEMASK_path;
	ImGui::Text(strTexturePathNum[5].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrEMISSIVE_MASKPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrNORMALSPath = structAiTextureFilePath->NORMALS_path;
	ImGui::Text(strTexturePathNum[6].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrNORMALSPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrMASK_Path = structAiTextureFilePath->MASK_path;
	ImGui::Text(strTexturePathNum[7].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrMASK_Path).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrSSS_MASKPath = structAiTextureFilePath->SSS_MASK_path;
	ImGui::Text(strTexturePathNum[8].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrSSS_MASKPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrSPRINT_EMISSIVEPath = structAiTextureFilePath->SPRINT_EMISSIVE_path;
	ImGui::Text(strTexturePathNum[9].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrSPRINT_EMISSIVEPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstrLIGHTMAPPath = structAiTextureFilePath->HAIR_DEPTH_Path;
	ImGui::Text(strTexturePathNum[10].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstrLIGHTMAPPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_REFLECTION_Path = structAiTextureFilePath->HAIR_ALPHA_path;
	ImGui::Text(strTexturePathNum[11].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_REFLECTION_Path).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_BASE_COLORPath = structAiTextureFilePath->HAIR_ROOT_path;
	ImGui::Text(strTexturePathNum[12].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_BASE_COLORPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_NORMAL_CAMERAPath = structAiTextureFilePath->COMP_MSK_CURV_path;
	ImGui::Text(strTexturePathNum[13].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_NORMAL_CAMERAPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_EMISSION_COLORName = structAiTextureFilePath->COMP_H_R_AO_path;
	ImGui::Text(strTexturePathNum[14].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_EMISSION_COLORName).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_METALNESSName = structAiTextureFilePath->METALNESS_path;
	ImGui::Text(strTexturePathNum[15].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_METALNESSName).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_DIFFUSE_ROUGHNESSName = structAiTextureFilePath->COMP_AMBIENT_OCCLUSION_path;
	ImGui::Text(strTexturePathNum[16].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_DIFFUSE_ROUGHNESSName).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END

	wstring  wstr_AMBIENT_OCCLUSIONPath = structAiTextureFilePath->AMBIENT_OCCLUSION_path;
	ImGui::Text(strTexturePathNum[17].c_str()); ImGui::SameLine();
	IMGUI_TEXT_COLOR_CHANGE
	ImGui::Text(CUtile::WstringToString(wstr_AMBIENT_OCCLUSIONPath).c_str());
	IMGUI_TEXT_COLOR_CHANGE_END
}

void CImgui_MapEditor::Imgui_Instancing_control(CGameObject * pSelectEnviObj)
{
#ifdef _DEBUG
	if (pSelectEnviObj == nullptr)
		return;
	
	CModel* pModel =dynamic_cast<CModel*>(pSelectEnviObj->Find_Component(TEXT("Com_Model")));
	if (nullptr == pModel || false == pModel->Get_IStancingModel())
		return;

	ImGui::Begin("Instance Obj PosControl");

	CTransform* pSelectObjTransform = static_cast<CTransform*>(pSelectEnviObj->Find_Component(TEXT("Com_Transform")));

	pModel->Imgui_MeshInstancingPosControl(pSelectObjTransform->Get_WorldMatrix());

	ImGui::End();
#endif
}

void CImgui_MapEditor::Imgui_Instacing_PosLoad(CGameObject * pSelectEnvioObj, vector<_float4x4> vecMatrixVec)
{
	CModel* pModel = dynamic_cast<CModel*>(pSelectEnvioObj->Find_Component(L"Com_Model"));
	assert(nullptr != pModel && "CImgui_MapEditor::Imgui_Instacing_PosLoad");
	
	if (false == pModel->Get_IStancingModel())
		return;

	pModel->Set_InstancePos(vecMatrixVec);

}

