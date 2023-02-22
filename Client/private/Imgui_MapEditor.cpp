#include "stdafx.h"
#include "..\public\Imgui_MapEditor.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"

#include "Json/json.hpp"
#include <fstream>

#include "EnviromentObj.h"

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
	}
	ImGui::End();
}

void CImgui_MapEditor::Imgui_SelectOption()
{
#pragma region ������ ���Ǵ� é��
	ImGui::RadioButton("CHAPTER_ONE_CAVE", &m_iChapterOption, _int(CEnviromentObj::CHAPTER_ONE_CAVE));
	ImGui::RadioButton("CHAPTER_TWO_FOREST", &m_iChapterOption, _int(CEnviromentObj::CHAPTER_TWO_FOREST));
#pragma endregion  ~������ ���Ǵ� é��

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
							m_wstrModelName = ProtoPair.first;
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

		if (ImGui::BeginListBox("#Model_Proto#"))
		{
			char szViewName[512], szProtoName[256];
			const bool bModelSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ComponentProtoType()[CGameInstance::GetInstance()->Get_CurLevelIndex()])
			{
				if (dynamic_cast<CModel*>(ProtoPair.second) == nullptr)
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
							m_wstrModelName = ProtoPair.first;
					}
				}
				else
				{
					CUtile::WideCharToChar(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bModelSelected))
						m_wstrModelName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� �������� Ÿ�� �̸��� ������
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
		if (ImGuiFileDialog::Instance()->IsOk())        // OK ������ ��
		{
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


	//if (ImGui::Button("Load"))
	//	Imgui_Load_Func();
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
		strSaveFileName += string(szDash);
		//strSaveFileName += ".json";
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
			jChild["7_Transform State"].push_back(fElement);		// �迭 ����. �����̳��� ������ ����մϴ�. �̷��� �ϸ� Transform State���� 16���� float ���� ����˴ϴ�.
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
	vector<string> testStr;

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		jLoadChild["4_RoomIndex"].get_to<int>(iLoadRoomIndex);
		jLoadChild["5_ChapterType"].get_to <int>(iLoadChapterType);

		for (string strTag : jLoadChild["6_AddComTag"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
			testStr.push_back(strTag);

		float	fElement = 0.f;
		int k = 0;
		for (float fElement : jLoadChild["7_Transform State"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
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

		if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(),
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		Load_ComTagToCreate(pGameInstance, pLoadObject, testStr);

		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;
		testStr.clear();
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CImgui_MapEditor::Imgui_Create_Option_Reset()/* �ʱ�ȭ*/
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

void CImgui_MapEditor::Load_MapObjects(_uint iLevel)
{
	ifstream      file("../Bin/Data/EnviromentObj_Json_Dir/Test_Map.json");
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
	vector<string> testStr;

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		jLoadChild["4_RoomIndex"].get_to<int>(iLoadRoomIndex);
		jLoadChild["5_ChapterType"].get_to <int>(iLoadChapterType);

		for (string strTag : jLoadChild["6_AddComTag"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
			testStr.push_back(strTag);

		float	fElement = 0.f;
		int k = 0;
		for (float fElement : jLoadChild["7_Transform State"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
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

		if (FAILED(pGameInstance->Clone_GameObject(iLevel,
			wszLayerTag,
			EnviromentDesc.szProtoObjTag.c_str(),
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		Load_ComTagToCreate(pGameInstance, pLoadObject, testStr);

		szProtoObjTag = "";			szModelTag = "";			szTextureTag = "";
		szCloneTag = "";				wszCloneTag = L""; 		iLoadRoomIndex = 0;
		iLoadChapterType = 0;		pLoadObject = nullptr;
		testStr.clear();
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