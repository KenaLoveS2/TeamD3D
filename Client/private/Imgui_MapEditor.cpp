#include "stdafx.h"
#include "..\public\Imgui_MapEditor.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"

#include "Json/json.hpp"
#include <fstream>

#include "EnviromentObj.h"
#include "Terrain.h"

#include "Imgui_Manager.h"
#include "Imgui_TerrainEditor.h"
#include "Gimmick_EnviObj.h"
#include "Pulse_Plate_Anim.h"
#include "ControlRoom.h"
#include "DeadZoneObj.h"
#include "Dynamic_Stone.h"

#define IMGUI_TEXT_COLOR_CHANGE				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
#define IMGUI_TEXT_COLOR_CHANGE_END		ImGui::PopStyleColor();

static string strTexturePathNum[WJTextureType_UNKNOWN] =
{
	"NONE_path",					"DIFFUSE_path",							"SPECULAR_path",					"AMBIENT_path",							"EMISSIVE_path",
	"EMISSIVEMASK_path",		"NORMALS_path",						"MASK_path",							"SSS_MASK_path",						"SPRINT_EMISSIVE_path",
	"HAIR_DEPTH_Path",			"ALPHA_path",						"HAIR_ROOT_path",				"COMP_MSK_CURV_path",			"COMP_H_R_AO_path",
	"COMP_E_R_AO_path",			"ROUGHNESS_path",		"AMBIENT_OCCLUSION_path"
};

CImgui_MapEditor::CImgui_MapEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_MapEditor::Initialize(void * pArg)
{
#ifdef _DEBUG
	m_bComOptions.fill(false);
#endif
	return S_OK;
}


void CImgui_MapEditor::Imgui_FreeRender()
{
#ifdef _DEBUG
	ImGui::Text("<MapTool>");
	if (ImGui::CollapsingHeader("Selcte_Option"))
	{
		Imgui_SelectOption();
		Imgui_Maptool_Terrain_Selecte();
		Imgui_CreateEnviromentObj();
		Imgui_Save_Load_Json();

		Imgui_SelectObject_InstancingControl();
		imgui_ObjectList_Clear();

		Imgui_Instance_Edit_Collider();
		Imgui_DZ_objClear();
	}

	ImGui::End();
#endif
}

#ifdef _DEBUG
void CImgui_MapEditor::Imgui_SelectOption()
{


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
							m_wstrProtoName = ProtoPair.first;
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


#pragma endregion 생성 클래스, 모델, 클론이름 정보 
	char szSelctedObject_Name[256], szSelctedModel_Name[256];
	CUtile::WideCharToChar(m_wstrProtoName.c_str(), szSelctedObject_Name);
	CUtile::WideCharToChar(m_wstrModelName.c_str(), szSelctedModel_Name);

	ImGui::Text("Selected_ProtoObj_Tag : %s", szSelctedObject_Name);
	ImGui::Text("Selected_Model_Tag : %s", szSelctedModel_Name);
	ImGui::Text("Selected_Clone_Tag : %s", m_strCloneTag);

#pragma region ~생성 클래스, 모델, 클론이름 정보 

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

	ImGui::Checkbox("Picking Create Obj", &m_bUseTerrainPicking);

	if (m_wstrProtoName == L"" || m_wstrModelName == L"")
		return;

	if (m_bUseTerrainPicking && m_pSelectedTerrain != nullptr)
	{
		m_bIstancingObjPicking = false;
		_float4 fCreatePos;

		if (m_pSelectedTerrain->CreateEnvrObj_PickingPos(fCreatePos))
		{
			CGameInstance *pGameInstace = GET_INSTANCE(CGameInstance);
			CGameObject* pCreateObject = nullptr;

			CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
			EnviromentDesc.szProtoObjTag = m_wstrProtoName;
			EnviromentDesc.szModelTag = m_wstrModelName;
			//EnviromentDesc.szTextureTag = TEXT("");
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

			CTransform* pTransform = dynamic_cast<CTransform*>(pCreateObject->Find_Component(L"Com_Transform"));
			pTransform->Set_State(CTransform::STATE_TRANSLATION,XMLoadFloat4(&fCreatePos));

			// 인스턴싱 오브젝트일때


			RELEASE_INSTANCE(CGameInstance);
		}

	}



	if (ImGui::Button("Create_EnviromentObj"))
	{
		CGameInstance *pGameInstace = GET_INSTANCE(CGameInstance);
		CGameObject* pCreateObject = nullptr;

		CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
		EnviromentDesc.szProtoObjTag =   m_wstrProtoName;
		EnviromentDesc.szModelTag =		m_wstrModelName;
		//EnviromentDesc.szTextureTag = TEXT("");
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

void CImgui_MapEditor::Imgui_ViewMeshOption(CGameObject* pSelecteObj)
{
	CModel* pModel = dynamic_cast<CModel*>(pSelecteObj->Find_Component(L"Com_Model"));
	
	_uint iNumMesh = pModel->Get_NumMeshes();
	 vector<_uint> vecMeshIndex;

	for (_uint i = 0; i < iNumMesh; ++i)
	{
		vecMeshIndex.push_back(i);
	}

	if (ImGui::BeginListBox("#Select_Mesh_Index#"))
	{
		_int iIndex = 0;
		for (auto& ProtoPair : vecMeshIndex)
		{
			const bool bSelected = false;
			char szViewName[512];

			sprintf_s(szViewName, sizeof(szViewName), " Mesh %d _ Index ", iIndex);

			if (ImGui::Selectable(szViewName, bSelected))
			{
				m_iSelectMeshIndex = iIndex;
			}
			++iIndex;
		}

		ImGui::EndListBox();
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
			//JsonTest();
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
}

void CImgui_MapEditor::Imgui_SelectObject_InstancingControl()
{
	CGameObject*	pSelectEnviObj = CGameInstance::GetInstance()->Get_SelectObjectPtr();

	if (nullptr != dynamic_cast<CEnviromentObj*>(pSelectEnviObj))
	{
		Imgui_Instancing_control(pSelectEnviObj);
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
		
		SaveDataDesc.iRoomIndex = SaveJson_Desc.iRoomIndex;
		SaveDataDesc.eChapterType = SaveJson_Desc.eChapterType;
		SaveDataDesc.iShaderPass = SaveJson_Desc.iShaderPass;

		szProtoObjTag = CUtile::WstringToString(SaveDataDesc.szProtoObjTag);
		szModelTag = CUtile::WstringToString(SaveDataDesc.szModelTag);
		//szTextureTag = CUtile::WstringToString(SaveDataDesc.szTextureTag);
		szCloneTag = CUtile::WideCharToChar(const_cast<_tchar*>(pObject.second->Get_ObjectCloneName()));

		jChild["0_ProtoTag"] = szProtoObjTag;
		jChild["1_ModelTag"] = szModelTag;
		jChild["2_ShaderOption"] = (int)SaveDataDesc.iShaderPass;
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
			
			vector<_float3>* SaveInstColiderSize = pModel->Get_ColiderSize();
			
			if(SaveInstColiderSize->size() >0)
			{ 
				for (size_t i = 0; i < InstancingPosSize; ++i)
				{
					_float3 fColiderSize = (*SaveInstColiderSize)[i];
					for (int j = 0; j < 3; ++j)
					{
						fElement = 0.f;
						memcpy(&fElement, (float*)&fColiderSize + j, sizeof(float));
						jChild["10_Instancing_ColiderSize"].push_back(fElement);
					}
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
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
	int				 iShaderPath = 0;

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{

		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_ShaderOption"].get_to<int>(iShaderPath);
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
		
		vector<_float3> SaveInstColiderSize;
		MatrixNumber = 0;
		_float3 fColiderSize;
		for (float fElement : jLoadChild["10_Instancing_ColiderSize"])
		{
			fElement = 0.f;
			memcpy(((float*)&fColiderSize) + (MatrixNumber++), &fElement, sizeof(float));
			
			if (MatrixNumber >= 3)
			{
				SaveInstColiderSize.push_back(fColiderSize);
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
		EnviromentDesc.iRoomIndex = iLoadRoomIndex;
		EnviromentDesc.eChapterType = CEnviromentObj::CHAPTER(iLoadChapterType);
		EnviromentDesc.iShaderPass = iShaderPath;




		if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(),
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		CTransform* pLoadObjTrnasform = nullptr;
		pLoadObjTrnasform = static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"));
		pLoadObjTrnasform->Set_WorldMatrix_float4x4(fWroldMatrix);

		Load_ComTagToCreate(pGameInstance, pLoadObject, StrComponentVec);
		Imgui_Instacing_PosLoad(pLoadObject, pLoadObjTrnasform, vecInstnaceMatrixVec, SaveInstColiderSize,EnviromentDesc.eChapterType);
		pGameInstance->Add_ShaderValueObject(g_LEVEL, pLoadObject); // 추가

		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;		iShaderPath = 0;
		StrComponentVec.clear();
		SaveInstColiderSize.clear();
		vecInstnaceMatrixVec.clear();
	}


	return S_OK;
}

void CImgui_MapEditor::Imgui_Create_Option_Reset()/* 초기화*/
{
	m_bComOptions.fill(false);
}

void CImgui_MapEditor::Imgui_Maptool_Terrain_Selecte()
{
	//const	char* pName = 			typeid(typename CImgui_TerrainEditor).name();
	//CImgui_TerrainEditor* pTerrainEditor = 	dynamic_cast<CImgui_TerrainEditor*>(CGameInstance::GetInstance()->Get_ImguiObject(pName));

	//if (pTerrainEditor == nullptr)
	//	return;

	m_pSelectedTerrain = dynamic_cast<CTerrain*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_BackGround", L"Terrain3"));

	if (nullptr == m_pSelectedTerrain)
		return;
}

#endif

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
}

#ifdef _DEBUG
void CImgui_MapEditor::Imgui_Instancing_control(CGameObject * pSelectEnviObj)
{
	if (pSelectEnviObj == nullptr)
		return;
	
	CModel* pModel =dynamic_cast<CModel*>(pSelectEnviObj->Find_Component(TEXT("Com_Model")));
	if (nullptr == pModel || false == pModel->Get_IStancingModel())
		return;

	ImGui::Begin("Instance Obj PosControl");

	CTransform* pSelectObjTransform = static_cast<CTransform*>(pSelectEnviObj->Find_Component(TEXT("Com_Transform")));	
	ImGui::Checkbox("Picking Terrain", &m_bIstancingObjPicking);
	
	static _int			m_iNum_GroundCoverObj = 0;
	static _float			m_fBetween = 0.f;
	static _float			m_fRadius = 0.f;
	static	_bool			bIstancingObj_RectAngle_Create = false;
	ImGui::Checkbox("Multiple Obj Create", &bIstancingObj_RectAngle_Create);
	ImGui::DragInt("iNum GroundCover", &m_iNum_GroundCoverObj, 1, 1, 200);
	ImGui::DragFloat("Between", &m_fBetween, 0.1f, 0.1f, 10.f);
	ImGui::DragFloat("Radius", &m_fRadius, 0.5f, 1.f, 10.f);


	pModel->Imgui_Mesh_InstancingSort_EditOrCreate();

	_float4 vPickingPos;
	_matrix TerrainMatrix = XMMatrixIdentity();
	if (m_bIstancingObjPicking == true && m_pSelectedTerrain != nullptr)
	{
		if (m_pSelectedTerrain->CreateEnvrObj_PickingPos(vPickingPos))
		{		
			m_bUseTerrainPicking = false;
			TerrainMatrix = m_pSelectedTerrain->Get_TransformCom()->Get_WorldMatrix();
			_matrix 	TerrainMatrixInv = XMMatrixInverse(nullptr, TerrainMatrix);

			_float4 vBasePos;
			XMStoreFloat4(&vBasePos, pSelectObjTransform->Get_State(CTransform::STATE_TRANSLATION));
			vPickingPos.x -= vBasePos.x;
			vPickingPos.y -= vBasePos.y;
			vPickingPos.z -= vBasePos.z;

			pModel->Imgui_MeshInstancingPosControl(pSelectObjTransform->Get_WorldMatrix() , 
				vPickingPos, TerrainMatrix,true, m_iNum_GroundCoverObj, m_fBetween, bIstancingObj_RectAngle_Create, m_fRadius);
		}

	}
	else
	{
		pModel->Imgui_MeshInstancingPosControl(pSelectObjTransform->Get_WorldMatrix(), vPickingPos,
			TerrainMatrix, false);
	}

	ImGui::End();

}

void CImgui_MapEditor::imgui_ObjectList_Clear()
{
	static int iDeleteRoomIndex = 0;

	ImGui::InputInt("Delete RoomIndex", &iDeleteRoomIndex);


	if (ImGui::Button("Object_List_Clear"))
	{
		CGameInstance *pGameInstance = GET_INSTANCE(CGameInstance);
		// 나중에 룸인덱스 조정 만들어야됌
		pGameInstance->RoomIndex_Object_Clear(g_LEVEL,L"Layer_Enviroment", iDeleteRoomIndex);
		
		RELEASE_INSTANCE(CGameInstance);
	}
}


void CImgui_MapEditor::Imgui_Instance_Edit_Collider()
{
	m_pSelectedObj = dynamic_cast<CEnviromentObj*>(CGameInstance::GetInstance()->Get_SelectObjectPtr());

	if (nullptr == m_pSelectedObj)
		return;

	CModel* pModel = static_cast<CModel*>(m_pSelectedObj->Find_Component(L"Com_Model"));
	if (m_pSelectedObj != m_pOldSelectedObj)
	{
		m_pOldSelectedObj = m_pSelectedObj;
		pModel->InitPhysxData();
	}

	if (pModel == nullptr)
		return;

	ImGui::Begin("Instance Obj Colider_Control");

	pModel->Edit_InstModel_Collider(m_pSelectedObj->Get_ObjectCloneName());

	ImGui::End();

}

void CImgui_MapEditor::Imgui_DZ_objClear()
{

	if (ImGui::Button("FIrst_Gimmick On"))
	{
		CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		pCtrlRoom->PulsePlate_Down_Active(1,true);
		
	}
	if (ImGui::Button("Third Gimmick On"))
	{
		CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		pCtrlRoom->Trigger_Active(4, CEnviromentObj::Gimmick_TYPE_GO_UP,true);
	}
	if (ImGui::Button("DZ_Portal On"))
	{
		CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		pCtrlRoom->Boss_HunterDeadGimmick();
	}

	if(ImGui::Button("DZ_Obj Clear"))
	{
		CControlRoom* pCtrlRoom =static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom",L"ControlRoom"));
		pCtrlRoom->DeadZoneObject_Change(true);
	}

	if (ImGui::Button("DZ_Obj Revert"))
	{
		CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		pCtrlRoom->DeadZoneObject_Change(false);
	}
}

#endif




void CImgui_MapEditor::Imgui_Instacing_PosLoad(CGameObject * pSelectEnvioObj, CTransform* pTransform,vector<_float4x4> & vecMatrixVec, vector<_float3> & vecColiderSize, CEnviromentObj::CHAPTER eChapterGimmcik)
{
	if (dynamic_cast<CDynamic_Stone*>(pSelectEnvioObj) != nullptr)
		return;

	CModel* pModel = dynamic_cast<CModel*>(pSelectEnvioObj->Find_Component(L"Com_Model"));

	assert(nullptr != pModel && "CImgui_MapEditor::Imgui_Instacing_PosLoad");

	if (false == pModel->Get_IStancingModel())
		return;

	pModel->Set_InstancePos(vecMatrixVec);

	//pModel->InstanceModelPosInit(pTransform->Get_WorldMatrix());

	if (dynamic_cast<CGimmick_EnviObj*>(pSelectEnvioObj) != nullptr)
	{
		dynamic_cast<CGimmick_EnviObj*>(pSelectEnvioObj)->SetUp_GimmicKEffect_Pos();
		pModel->Instaincing_GimmkicInit(eChapterGimmcik);
	}
	
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

void CImgui_MapEditor::Load_MapObjects(_uint iLevel, string JsonFileName)
{
	string      strLoadDirectory = "../Bin/Data/EnviromentObj_Json_Dir/";
	strLoadDirectory += JsonFileName;

	ifstream      file(strLoadDirectory);
	Json	jLoadEnviromentObjList;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadEnviromentObjList;
	file.close();

	_float4x4	fWroldMatrix;
	wstring			wstrProtoName = TEXT("");
	wstring			wstrModelName = TEXT("");
	wstring			wstrTexturelName = TEXT("");
	_tchar*			wszLayerTag = L"";
	string			szLayerTag = "";
	string			szProtoObjTag = "";
	string			szModelTag = "";
	string			szTextureTag = "";
	string			szCloneTag = "";
	_tchar*			wszCloneTag = L"";
	int				iLoadRoomIndex = 0;
	int				iLoadChapterType = 0;
	int				iShaderPass = 0;
	int				iDeadZone_ID = -1;
	vector<string> StrComTagVec;
	array<string, (_int)WJTextureType_UNKNOWN> strFilePaths_arr;
	strFilePaths_arr.fill("");

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_ShaderOption"].get_to<int>(iShaderPass);
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

		vector<_float3> SaveInstColiderSize;
		MatrixNumber = 0;
		_float3 fColiderSize;
		for (float fElement : jLoadChild["10_Instancing_ColiderSize"])
		{
			memcpy(((float*)&fColiderSize) + (MatrixNumber++), &fElement, sizeof(float));

			if (MatrixNumber >= 3)
			{
				SaveInstColiderSize.push_back(fColiderSize);
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
		EnviromentDesc.iRoomIndex = iLoadRoomIndex;
		EnviromentDesc.eChapterType = CEnviromentObj::CHAPTER(iLoadChapterType);
		EnviromentDesc.iCurLevel = iLevel;
		EnviromentDesc.iShaderPass = iShaderPass;


		if (FAILED(pGameInstance->Clone_GameObject(iLevel,
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");

		/*if (dynamic_cast<CPulse_Plate_Anim*>(pLoadObject) != nullptr)
		{
			pGameInstance->Add_AnimObject(g_LEVEL, pLoadObject);
		}*/

		CTransform* pLoadObjTrnasform = nullptr;
		pLoadObjTrnasform = static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"));
		pLoadObjTrnasform->Set_WorldMatrix_float4x4(fWroldMatrix);
		//Load_ComTagToCreate(pGameInstance, pLoadObject, StrComTagVec);
		Imgui_Instacing_PosLoad(pLoadObject, pLoadObjTrnasform,vecInstnaceMatrixVec, SaveInstColiderSize, EnviromentDesc.eChapterType);

		for (_int iModelId : jLoadChild["8_DeadZoneModel_ID"])
		{
			memcpy((&iDeadZone_ID), &iModelId, sizeof(_int));
			static_cast<CDeadZoneObj*>(pLoadObject)->Set_DeadZoneModel_ChangeID(iDeadZone_ID);

		}

		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;		iShaderPass = 0;
		StrComTagVec.clear();
		strFilePaths_arr.fill("");
		iDeadZone_ID = -1;
	}




}


