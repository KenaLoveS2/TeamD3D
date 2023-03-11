#include "stdafx.h"
#include "..\public\Imgui_MapEditor.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"

#include "Json/json.hpp"
#include <fstream>

#include "EnviromentObj.h"
#include "ModelViewerObject.h"
#include "Terrain.h"

#include "Imgui_Manager.h"
#include "Imgui_TerrainEditor.h"
#include "Gimmick_EnviObj.h"
#include "Pulse_Plate_Anim.h"
#include "Crystal.h"

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
	m_bComOptions.fill(false);

	return S_OK;
}

void CImgui_MapEditor::Imgui_FreeRender()
{
	ImGui::Text("<MapTool>");
	if (ImGui::CollapsingHeader("Selcte_Option"))
	{
		Imgui_SelectOption();
		Imgui_Maptool_Terrain_Selecte();
		Imgui_CreateEnviromentObj();
		Imgui_Save_Load_Json();

		Imgui_SelectObject_InstancingControl();
		Imgui_Control_ViewerCamTransform();
		imgui_ObjectList_Clear();

		imgui_Gimmic_Class_Viewr();
		Imgui_Crystal_Create_Pulse();
	}

	ImGui::End();
}

void CImgui_MapEditor::Imgui_SelectOption()
{


#pragma region ������ ���Ǵ� ������ Ÿ��
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
						m_wstrProtoName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� ������ Ÿ�� �̸��� ������
				}

			}
			ImGui::EndListBox();
		}
	}
#pragma endregion ~������ ���Ǵ� ������ Ÿ��

#pragma region ������ ���Ǵ� �� �̸�
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
						m_wstrModelName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� �������� Ÿ�� �̸��� ������
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
						m_wstrModelName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� �������� Ÿ�� �̸��� ������
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
						m_wstrModelName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� �������� Ÿ�� �̸��� ������
						m_bModelChange = true;
					}
				}
			}
			ImGui::EndListBox();
		}
	}




#pragma endregion ~������ ���Ǵ� �� �̸�

#pragma region ������ ���Ǵ� Ŭ�� �̸�����
	ImGui::InputText("Clone_Tag ", m_strCloneTag, CLONE_TAG_BUFF_SIZE);

#pragma endregion ~������ ���Ǵ� �� �̸�

#pragma  region		������ ���Ǵ� �ɼ� ����

	if (ImGui::CollapsingHeader("Selcted Obj Option"))
	{
		ImGui::InputInt("Chose_Room_Option", &m_iCreateObjRoom_Option);
		ImGui::Checkbox("Add_MoveCtrlCom", &m_bComOptions[CEnviromentObj::COMPONENTS_CONTROL_MOVE]);  ImGui::SameLine();
		ImGui::Checkbox("Add_InteractionCom", &m_bComOptions[CEnviromentObj::COMPONENTS_INTERACTION]);
	}

#pragma endregion ~������ ���Ǵ� �� �̸�

#pragma region		���õ� ������Ʈ�� �����ֱ�

	char szSelctedObject_Name[256], szSelctedModel_Name[256];
	CUtile::WideCharToChar(m_wstrProtoName.c_str(), szSelctedObject_Name);
	CUtile::WideCharToChar(m_wstrModelName.c_str(), szSelctedModel_Name);

	ImGui::Text("Selected_ProtoObj_Tag : %s", szSelctedObject_Name);
	ImGui::Text("Selected_Model_Tag : %s", szSelctedModel_Name);
	ImGui::Text("Selected_Clone_Tag : %s", m_strCloneTag);

	if(m_bModelChange ==true)
	{
		/* �� ������ Ÿ���� ���� �ƴҶ� �ٲٱ�*/
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


#pragma endregion ~���õ� ������Ʈ�� �����ֱ�
}

void CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront(CGameInstance *pGameInstace, CGameObject* pGameObject)
{
	assert(nullptr != pGameObject && "CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront");
	assert(nullptr != pGameInstace && "CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront");
#pragma region  ������Ʈ �ɼ� �Ұ� �޾Ƽ� �߰��ϱ�
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
#pragma endregion ~������Ʈ �ɼ� �Ұ� �޾Ƽ� �߰��ϱ�

#pragma region ī�޶� z �տ��� pos ���� �����ϱ�
	CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
	assert(pTransform != nullptr && "CImgui_MapEditor::Imgui_AddComponentOption_CreateCamFront");

	_float4 vCamPos = pGameInstace->Get_CamPosition();
	_float4 vLook;

	memcpy(&vLook, &pGameInstace->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW).m[2], sizeof(_float4));

	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vCamPos) + XMVector3Normalize(XMLoadFloat4(&vLook)) * 30.f);

	pTransform->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(vPos.x, 0, vPos.z, 1.f));
#pragma endregion ī�޶� z �տ��� pos ���� �����ϱ�

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

			// �ν��Ͻ� ������Ʈ�϶�


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
		if (ImGuiFileDialog::Instance()->IsOk())        // OK ������ ��
		{
			//JsonTest();
			Imgui_Save_Func();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel ������ ��
			ImGuiFileDialog::Instance()->Close();
	}

	ImGui::SameLine();

	if (ImGui::Button("Confirm_Load"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Load Folder", "Select Load Folder", ".json", "../Bin/Load Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Load Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK ������ ��
		{
			Imgui_Load_Func();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel ������ ��
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
		// Is Picking?
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
	// ���� ������ ���� ��� �ҷ����� �Լ�
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
	//	SaveDataDesc.szTextureTag = SaveJson_Desc.szTextureTag;
		SaveDataDesc.iRoomIndex = SaveJson_Desc.iRoomIndex;
		SaveDataDesc.eChapterType = SaveJson_Desc.eChapterType;

		szProtoObjTag = CUtile::WstringToString(SaveDataDesc.szProtoObjTag);
		szModelTag = CUtile::WstringToString(SaveDataDesc.szModelTag);
		//szTextureTag = CUtile::WstringToString(SaveDataDesc.szTextureTag);
		szCloneTag = CUtile::WideCharToChar(const_cast<_tchar*>(pObject.second->Get_ObjectCloneName()));

		jChild["0_ProtoTag"] = szProtoObjTag;
		jChild["1_ModelTag"] = szModelTag;
		//jChild["2_TextureTag"] = szTextureTag;
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
			jChild["7_Transform State"].push_back(fElement);		// �迭 ����. �����̳��� ������ ����մϴ�. �̷��� �ϸ� Transform State���� 16���� float ���� ����˴ϴ�.
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
	string      strLoadDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();   // GetCurrentPath F12�� �鰡�� ����� �ٸ� �Լ� �� ����.
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


	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{

		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		//jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		jLoadChild["4_RoomIndex"].get_to<int>(iLoadRoomIndex);
		jLoadChild["5_ChapterType"].get_to <int>(iLoadChapterType);

		for (string strTag : jLoadChild["6_AddComTag"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
			StrComponentVec.push_back(strTag);

		float	fElement = 0.f;
		int		k = 0;
		for (float fElement : jLoadChild["7_Transform State"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
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
			
		m_wstrProtoName.assign(szProtoObjTag.begin(), szProtoObjTag.end());
		m_wstrModelName.assign(szModelTag.begin(), szModelTag.end());
		m_wstrTexturelName.assign(szTextureTag.begin(), szTextureTag.end());
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		pGameInstance->Add_String(wszCloneTag);

		EnviromentDesc.szProtoObjTag = m_wstrProtoName;
		EnviromentDesc.szModelTag = m_wstrModelName;
		EnviromentDesc.iRoomIndex = iLoadRoomIndex;
		EnviromentDesc.eChapterType = CEnviromentObj::CHAPTER(iLoadChapterType);
		
		if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(),
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		Load_ComTagToCreate(pGameInstance, pLoadObject, StrComponentVec);
		Imgui_Instacing_PosLoad(pLoadObject, vecInstnaceMatrixVec, EnviromentDesc.eChapterType);
		

		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;
		StrComponentVec.clear();
	
		vecInstnaceMatrixVec.clear();
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CImgui_MapEditor::Imgui_Create_Option_Reset()/* �ʱ�ȭ*/
{
	m_bComOptions.fill(false);
}

void CImgui_MapEditor::Imgui_Maptool_Terrain_Selecte()
{
	const	char* pName = 			typeid(typename CImgui_TerrainEditor).name();
	CImgui_TerrainEditor* pTerrainEditor = 	dynamic_cast<CImgui_TerrainEditor*>(CGameInstance::GetInstance()->Get_ImguiObject(pName));

	if (pTerrainEditor == nullptr)
		return;

	m_pSelectedTerrain = dynamic_cast<CTerrain*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_BackGround", L"Terrain4"));

	if (nullptr == m_pSelectedTerrain)
		return;
}

void CImgui_MapEditor::Imgui_Crystal_Create_Pulse()
{
	if (ImGui::Button("Create_Crystal_Pulse"))
	{

		for (auto &pCrystal : *(CGameInstance::GetInstance()->Find_Layer(g_LEVEL, L"Layer_Enviroment")->Get_CloneObjects()))
		{
			if(dynamic_cast<CCrystal*>(pCrystal.second) == nullptr)
				continue;
		
			static_cast<CCrystal*>(pCrystal.second)->Create_Pulse();
		}


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
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		jLoadChild["4_RoomIndex"].get_to<int>(iLoadRoomIndex);
		jLoadChild["5_ChapterType"].get_to <int>(iLoadChapterType);

		for (string strTag : jLoadChild["6_AddComTag"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
			StrComTagVec.push_back(strTag);

		float	fElement = 0.f;
		int k = 0;
		for (float fElement : jLoadChild["7_Transform State"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
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

		if (FAILED(pGameInstance->Clone_GameObject(iLevel,
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		
		if (dynamic_cast<CPulse_Plate_Anim*>(pLoadObject) != nullptr)
		{
			pGameInstance->Add_AnimObject(g_LEVEL, pLoadObject);
		}
		
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		Load_ComTagToCreate(pGameInstance, pLoadObject, StrComTagVec);
		Imgui_Instacing_PosLoad(pLoadObject, vecInstnaceMatrixVec, EnviromentDesc.eChapterType);
		//pLoadObject->Late_Initialize();
		

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
	_float4 vPickingPos;
	_matrix TerrainMatrix;
	if (m_bIstancingObjPicking == true && m_pSelectedTerrain != nullptr)
	{
		m_bUseTerrainPicking = false;

		if (m_pSelectedTerrain->CreateEnvrObj_PickingPos(vPickingPos))
		{
			TerrainMatrix = m_pSelectedTerrain->Get_TransformCom()->Get_WorldMatrix();
			_matrix 	TerrainMatrixInv = XMMatrixInverse(nullptr, TerrainMatrix);

			_float4 vBasePos;
			XMStoreFloat4(&vBasePos, pSelectObjTransform->Get_State(CTransform::STATE_TRANSLATION));
		
			vPickingPos.x -= vBasePos.x;
			vPickingPos.y -= vBasePos.y;
			vPickingPos.z -= vBasePos.z;
#ifdef	_DEBUG
			pModel->Imgui_MeshInstancingPosControl(pSelectObjTransform->Get_WorldMatrix() , vPickingPos, TerrainMatrix,true);
#endif
		}
	}
	else
	{
#ifdef	_DEBUG
		pModel->Imgui_MeshInstancingPosControl(pSelectObjTransform->Get_WorldMatrix(), vPickingPos, TerrainMatrix, false);
#endif
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
		// ���߿� ���ε��� ���� �����߉�
		pGameInstance->RoomIndex_Object_Clear(g_LEVEL,L"Layer_Enviroment", iDeleteRoomIndex);
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImgui_MapEditor::imgui_Gimmic_Class_Viewr()
{
}

void CImgui_MapEditor::Imgui_Instacing_PosLoad(CGameObject * pSelectEnvioObj, vector<_float4x4> vecMatrixVec,CEnviromentObj::CHAPTER eChapterGimmcik)
{
	CModel* pModel = dynamic_cast<CModel*>(pSelectEnvioObj->Find_Component(L"Com_Model"));
	assert(nullptr != pModel && "CImgui_MapEditor::Imgui_Instacing_PosLoad");
	
	if (false == pModel->Get_IStancingModel())
		return;


	pModel->Set_InstancePos(vecMatrixVec);

	if (dynamic_cast<CGimmick_EnviObj*>(pSelectEnvioObj) != nullptr)
	{
		pModel->Instaincing_GimmkicInit(eChapterGimmcik);
	}

}

