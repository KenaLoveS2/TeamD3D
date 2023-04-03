#include "stdafx.h"
#include "..\public\ImGui_Rot.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"
#include "Json/json.hpp"
#include <fstream>
#include  "Rot.h"

CImGui_Rot::CImGui_Rot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CImguiObject(pDevice, pContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Rot::Initialize(void* pArg)
{
	m_szFreeRenderName = "Rot_Editor";
	return S_OK;
}

void CImGui_Rot::Imgui_FreeRender()
{
	ImGui::BulletText("Rot");

	RotList();

	ImGui::Checkbox("RotWriteFileOn", &m_bSaveWrite);

	if (m_bSaveWrite)
		ImGui::InputText("RotSave_Name : ", &m_strFileName);

	if (ImGui::Button("Confirm_RotSave"))
		ImGuiFileDialog::Instance()->OpenDialog("Rot Save Folder", "Select Rot Save Folder", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Rot Save Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Save();
			ImGuiFileDialog::Instance()->Close();
		}

		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("Confirm_RotLoad"))
		ImGuiFileDialog::Instance()->OpenDialog("Rot Load Folder", "Select Rot Load Folder", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Rot Load Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Load();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}

	ImGui::End();
}

void CImGui_Rot::RotList()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(g_LEVEL, L"Layer_Rot");

	_int iSize = 0;
	if (pLayer == nullptr)
		iSize = 0;
	else
		iSize = (_int)pLayer->GetGameObjects().size();

	char szCount[32] = { 0, };
	sprintf_s(szCount, "%d", iSize);
	ImGui::Text("Rot Count :");  ImGui::SameLine(); ImGui::Text(szCount);

	_tchar szCloneRotTag[32] = { 0, };
	swprintf_s(szCloneRotTag, L"Rot_%d", iSize);

	if(ImGui::Button("CLONE_ROT"))
	{
		CGameObject* p_game_object = nullptr;
		m_pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_Rot", L"Prototype_GameObject_Rot", CUtile::Create_StringAuto(szCloneRotTag), nullptr, &p_game_object);
		p_game_object->Late_Initialize();
	}

	if (ImGui::CollapsingHeader("RotList"))
	{
		bool bFound = false;
		if (ImGui::BeginListBox("RotListBox"))
		{
			const bool bObjectSelected = false;
			for (auto& ProtoPair : pLayer->GetGameObjects())
			{
				if (dynamic_cast<CRot*>(ProtoPair.second) == nullptr)
					continue;

				char  szCloneName[256];
				CUtile::WideCharToChar(ProtoPair.second->Get_ObjectCloneName(), szCloneName);
				if (ImGui::Selectable(szCloneName, bObjectSelected))
				{
					m_pRot = static_cast<CRot*>(ProtoPair.second);
				}
			}
			ImGui::EndListBox();
		}

		if (m_pRot != nullptr)
		{
			m_pRot->Imgui_RenderProperty();
			m_pRot->Get_TransformCom()->Imgui_RenderProperty();
		}
	}
}

void CImGui_Rot::Load_RotObjects(_uint iLevel, string JsonFileName, _bool isDynamic)
{
	list<CGameObject*> gameobjectList;

	string      strLoadDirectory = "../Bin/Data/Rot/";
	strLoadDirectory += JsonFileName;

	ifstream      file(strLoadDirectory);
	Json	jLoadMonsterObjList;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CRot::DESC RotDesc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadMonsterObjList;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		szLayerTag = "Layer_Rot";
	string		szCloneTag = "";
	int			iLoadRoomIndex = 0;

	jLoadMonsterObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);


	for (auto jLoadChild : jLoadMonsterObjList["1_Data"])
	{
		jLoadChild["2_Room Index"].get_to<int>(iLoadRoomIndex);
		jLoadChild["3_CloneObjTag"].get_to<string>(szCloneTag);

		int k = 0;
		for (float fElement : jLoadChild["1_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));

		ZeroMemory(&RotDesc, sizeof(CRot::DESC));
		RotDesc.WorldMatrix = fWroldMatrix;
		RotDesc.iRoomIndex = iLoadRoomIndex;

		_tchar*		wszCloneTag = L"";
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		pGameInstance->Add_String(wszCloneTag);

		pGameInstance->Clone_GameObject(g_LEVEL, wszLayerTag, L"Prototype_GameObject_Rot", wszCloneTag, &RotDesc, &pLoadObject);

		if (isDynamic)
			gameobjectList.push_back(pLoadObject);
	}

	if (isDynamic)
	{
		for (auto& iter : gameobjectList)
			iter->Late_Initialize();
	}
	gameobjectList.clear();

	RELEASE_INSTANCE(CGameInstance);
}

void CImGui_Rot::Save()
{
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
	Json	jMonsterObjList;

	_float4x4	fWroldMatrix;
	_float		fElement = 0.f;
	string		strLayerTag = "Layer_Rot";
	char*		pCloneObjTag;

	jMonsterObjList["0_LayerTag"] = strLayerTag;

	for (auto& pObject : m_pGameInstance->Find_Layer(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Rot")->GetGameObjects())
	{
		CRot* pRot = dynamic_cast<CRot*>(pObject.second);

		if (pRot == nullptr)
			continue;

		Json jChild;

		CTransform* pTransform = static_cast<CTransform*>(pObject.second->Find_Component(L"Com_Transform"));
		assert(pTransform != nullptr && "CImGui_Rot:Save()");
		XMStoreFloat4x4(&fWroldMatrix, pTransform->Get_WorldMatrix());

		for (int i = 0; i < 16; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&fWroldMatrix + i, sizeof(float));
			jChild["1_Transform State"].push_back(fElement);		// 배열 저장. 컨테이너의 구조랑 비슷합니다. 이렇게 하면 Transform State에는 16개의 float 값이 저장됩니다.
		}

		jChild["2_Room Index"] = pRot->Get_Desc().iRoomIndex;

		pCloneObjTag = CUtile::WideCharToChar(const_cast<_tchar*>(pRot->Get_ObjectCloneName()));

		jChild["3_CloneObjTag"] = pCloneObjTag;

		Safe_Delete_Array(pCloneObjTag);

		jMonsterObjList["1_Data"].push_back(jChild);
	}

	file << jMonsterObjList;
	file.close();
	MSG_BOX("Save_jRotObjList");
}

HRESULT CImGui_Rot::Load()
{
	list<CGameObject*> gameobjectList;

	string      strLoadDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();   // GetCurrentPath F12로 들가면 비슷한 다른 함수 더 있음.
	string	   strLoadFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
	char   szDash[128] = "\\";
	strcat_s(szDash, strLoadFileName.c_str());
	strLoadDirectory += string(szDash);

	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadRotObjList;

	CRot::DESC rot_desc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadRotObjList;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		szLayerTag = "Layer_Rot";
	string		szCloneTag = "";
	int			iLoadRoomIndex = 0;

	jLoadRotObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	m_pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadRotObjList["1_Data"])
	{
		jLoadChild["2_Room Index"].get_to<int>(iLoadRoomIndex);
		jLoadChild["3_CloneObjTag"].get_to<string>(szCloneTag);

		int k = 0;
		for (float fElement : jLoadChild["1_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));

		ZeroMemory(&rot_desc, sizeof(CRot::DESC));
		rot_desc.WorldMatrix = fWroldMatrix;
		rot_desc.iRoomIndex = iLoadRoomIndex;

		_tchar*		wszCloneTag = L"";
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		m_pGameInstance->Add_String(wszCloneTag);

		m_pGameInstance->Clone_AnimObject(g_LEVEL, wszLayerTag, L"Prototype_GameObject_Rot", wszCloneTag, &rot_desc, &pLoadObject);
		gameobjectList.push_back(pLoadObject);
	}

	for (auto& iter : gameobjectList)
		iter->Late_Initialize();

	gameobjectList.clear();

	return S_OK;
}

CImGui_Rot* CImGui_Rot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CImGui_Rot*	 pInstance = new CImGui_Rot(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImGui_Monster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_Rot::Free()
{
	CImguiObject::Free();
	Safe_Release(m_pGameInstance);
}
