#include "stdafx.h"
#include "..\public\ImGui_Monster.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"
#include "Json/json.hpp"
#include <fstream>
#include "Monster.h"

CImGui_Monster::CImGui_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CImguiObject(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Monster::Initialize(void* pArg)
{
	m_szFreeRenderName = "Monster_Editor";
	return S_OK;
}

void CImGui_Monster::Imgui_FreeRender()
{
	ImGui::BulletText("Monster");

	MonsterList();

	ImGui::Checkbox("MonsterWriteFileOn", &m_bSaveWrite);

	if (m_bSaveWrite)
		ImGui::InputText("MonsterSave_Name : ", &m_strFileName);

	if (ImGui::Button("Confirm_MonsterSave"))
		ImGuiFileDialog::Instance()->OpenDialog("Monster Save Folder", "Select Monster Save Folder", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Monster Save Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Save();
			ImGuiFileDialog::Instance()->Close();
		}

		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("Confirm_MonsterLoad"))
		ImGuiFileDialog::Instance()->OpenDialog("Monster Load Folder", "Select Monster Load Folder", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Monster Load Folder"))
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

void CImGui_Monster::MonsterList()
{
	if (ImGui::CollapsingHeader("ProtoType_Monster"))
	{
		static string FindProtoObjectlTag = "";		// ! == No Find
		ImGui::InputText("Find_ProtoObjectlTag", &FindProtoObjectlTag);

		if (ImGui::BeginListBox("#GameObject_Proto#"))
		{
			const bool bObjectSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ProtoTypeObjects())
			{
				if (dynamic_cast<CMonster*>(ProtoPair.second) == nullptr)
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
							m_wstrSelectedProtoName = ProtoPair.first;
					}
				}
				else
				{
					CUtile::WideCharToChar(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bObjectSelected))
						m_wstrSelectedProtoName = ProtoPair.first;			// 리스트 박스를 누르면 현재 프로토 타입 이름을 가져옴
				}
			}
			ImGui::EndListBox();
		}
	}

	if (!m_wstrSelectedProtoName.empty())
	{
		ImGui::Text("ProtoName : "); ImGui::SameLine(); ImGui::Text(CUtile::WstringToString(m_wstrSelectedProtoName).c_str());

		CLayer* pLayer = m_pGameInstance->Find_Layer(g_LEVEL, L"Layer_Monster");

		_int iSize = 0;
		if (pLayer == nullptr)
			iSize = 0;
		else
		{
			iSize = (_int)pLayer->GetGameObjects().size();
		}
			

		if (ImGui::Button("CLONE_MONSTER"))
		{
			_tchar szCloneMonsterTag[MAX_PATH] = { 0, };
			swprintf_s(szCloneMonsterTag, L"Monster_%d", iSize);

			_tchar wszProtoTag[MAX_PATH] = { 0, };
			swprintf_s(wszProtoTag, m_wstrSelectedProtoName.c_str());

			CGameObject* p_game_object = nullptr;
			m_pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_Monster", CUtile::Create_StringAuto(wszProtoTag), CUtile::Create_StringAuto(szCloneMonsterTag), nullptr, &p_game_object);
			p_game_object->Late_Initialize();
		}
	}

	CLayer* pLayer = m_pGameInstance->Find_Layer(g_LEVEL, L"Layer_Monster");

	if (pLayer == nullptr)
		return;

	if (ImGui::CollapsingHeader("Monster"))
	{
		bool bFound = false;
		if (ImGui::BeginListBox("MonsterListBox"))
		{
			const bool bObjectSelected = false;
			for (auto& ProtoPair : pLayer->GetGameObjects())
			{
				if (dynamic_cast<CMonster*>(ProtoPair.second) == nullptr)
					continue;

				char  szCloneName[256];
				CUtile::WideCharToChar(ProtoPair.second->Get_ObjectCloneName(), szCloneName);
				if (ImGui::Selectable(szCloneName, bObjectSelected))
				{
					m_pMonster = static_cast<CMonster*>(ProtoPair.second);
				}
			}
			ImGui::EndListBox();
		}

		if (m_pMonster != nullptr)
		{
			m_pMonster->Imgui_RenderProperty();
			m_pMonster->Get_TransformCom()->Imgui_RenderProperty();
		}
	}
}

void CImGui_Monster::Load_MonsterObjects(_uint iLevel, string JsonFileName, _bool isDynamic)
{
	list<CGameObject*> gameobjectList;

	string      strLoadDirectory = "../Bin/Data/Monster/";
	strLoadDirectory += JsonFileName;

	ifstream      file(strLoadDirectory);
	Json	jLoadMonsterObjList;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CMonster::DESC MonsterDesc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadMonsterObjList;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		strLayerTag = "Layer_Monster";
	string		strProtoObjTag = "";
	string		strCloneTag = "";
	string		strGroupName = "";

	jLoadMonsterObjList["0_LayerTag"].get_to<string>(strLayerTag);
	wszLayerTag = CUtile::StringToWideChar(strLayerTag);
	pGameInstance->Add_String(wszLayerTag);
	

	for (auto jLoadChild : jLoadMonsterObjList["1_Data"])
	{
		jLoadChild["2_Group Name"].get_to<string>(strGroupName);
		jLoadChild["3_ProtoObjTag"].get_to<string>(strProtoObjTag);
		jLoadChild["4_CloneObjTag"].get_to<string>(strCloneTag);

		int k = 0;
		for (float fElement : jLoadChild["1_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));

		ZeroMemory(&MonsterDesc, sizeof(CMonster::DESC));
		MonsterDesc.WorldMatrix = fWroldMatrix;
		MonsterDesc.pGroupName = CUtile::Create_StringAuto(strGroupName.c_str());

		_tchar*		wszProtoObjTag = L"";
		wszProtoObjTag = CUtile::StringToWideChar(strProtoObjTag);
		pGameInstance->Add_String(wszProtoObjTag);

		_tchar*		wszCloneTag = L"";
		wszCloneTag = CUtile::StringToWideChar(strCloneTag);
		pGameInstance->Add_String(wszCloneTag);

		pGameInstance->Clone_GameObject(g_LEVEL, wszLayerTag, wszProtoObjTag, wszCloneTag, &MonsterDesc, &pLoadObject);

		strProtoObjTag = "";
		strCloneTag = "";

		if (isDynamic)
			gameobjectList.push_back(pLoadObject);
	}

	if(isDynamic)
	{
		for (auto& iter : gameobjectList)
			iter->Late_Initialize();
	}

	gameobjectList.clear();

	RELEASE_INSTANCE(CGameInstance);
}

void CImGui_Monster::Save()
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
	string		szLayerTag = "Layer_Monster";
	wstring		wstrProtoObjTag;
	wstring		wstrCloneObjTag;
	string		strProtoObjTag;
	string		strCloneObjTag;

	jMonsterObjList["0_LayerTag"] = szLayerTag;

	for (auto& pObject : m_pGameInstance->Find_Layer(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_Monster")->GetGameObjects())
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pObject.second);

		if(pMonster == nullptr)
			continue;

		Json jChild;

		CTransform* pTransform = static_cast<CTransform*>(pObject.second->Find_Component(L"Com_Transform"));
		assert(pTransform != nullptr && "CImGui_Monster:Save()");
		XMStoreFloat4x4(&fWroldMatrix, pTransform->Get_WorldMatrix());

		for (int i = 0; i < 16; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&fWroldMatrix + i, sizeof(float));
			jChild["1_Transform State"].push_back(fElement);		
		}

 		jChild["2_Group Name"] = pMonster->Get_Desc().pGroupName;

		wstrProtoObjTag = pObject.second->Get_ProtoObjectName();
		strProtoObjTag = CUtile::WstringToString(wstrProtoObjTag);
		jChild["3_ProtoObjTag"] = strProtoObjTag;

		wstrCloneObjTag = pObject.second->Get_ObjectCloneName();
		strCloneObjTag = CUtile::WstringToString(wstrCloneObjTag);
		jChild["4_CloneObjTag"] = strCloneObjTag;

		jMonsterObjList["1_Data"].push_back(jChild);

		wstrProtoObjTag = L"";
		wstrCloneObjTag = L"";
		strProtoObjTag = "";
		strCloneObjTag = "";
	}

	file << jMonsterObjList;
	file.close();

	MSG_BOX("Save_jMonsterObjList");
}

HRESULT CImGui_Monster::Load()
{
	list<CGameObject*> gameobjectList;

	string      strLoadDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();   // GetCurrentPath F12로 들가면 비슷한 다른 함수 더 있음.
	string	   strLoadFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
	char   szDash[128] = "\\";
	strcat_s(szDash, strLoadFileName.c_str());
	strLoadDirectory += string(szDash);

	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadMonsterObjList;

	CMonster::DESC monster_desc;
	CGameObject*	pLoadObject = nullptr;

	file >> jLoadMonsterObjList;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		strLayerTag = "Layer_Monster";
	string		strProtoObjTag = "";
	string		strCloneTag = "";
	string		strGroupName = "";

	jLoadMonsterObjList["0_LayerTag"].get_to<string>(strLayerTag);
	wszLayerTag = CUtile::StringToWideChar(strLayerTag);
	m_pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadMonsterObjList["1_Data"])
	{
		jLoadChild["2_Group Name"].get_to<string>(strGroupName);
		jLoadChild["3_ProtoObjTag"].get_to<string>(strProtoObjTag);
		jLoadChild["4_CloneObjTag"].get_to<string>(strCloneTag);

		int k = 0;
		for (float fElement : jLoadChild["1_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));

		ZeroMemory(&monster_desc, sizeof(CMonster::DESC));
		monster_desc.WorldMatrix = fWroldMatrix;
		monster_desc.pGroupName = CUtile::Create_StringAuto(strGroupName.c_str());

		_tchar*		wszProtoObjTag = L"";
		wszProtoObjTag = CUtile::StringToWideChar(strProtoObjTag);
		m_pGameInstance->Add_String(wszProtoObjTag);

		_tchar*		wszCloneTag = L"";
		wszCloneTag = CUtile::StringToWideChar(strCloneTag);
		m_pGameInstance->Add_String(wszCloneTag);

		m_pGameInstance->Clone_AnimObject(g_LEVEL, wszLayerTag, wszProtoObjTag, wszCloneTag, &monster_desc, &pLoadObject);
		gameobjectList.push_back(pLoadObject);

		strProtoObjTag = "";
		strCloneTag = "";
	}

	for (auto& iter : gameobjectList)
		iter->Late_Initialize();

	gameobjectList.clear();

	return S_OK;
}

CImGui_Monster* CImGui_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CImGui_Monster*	 pInstance = new CImGui_Monster(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImGui_Monster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_Monster::Free()
{
	CImguiObject::Free();
	Safe_Release(m_pGameInstance);
}
