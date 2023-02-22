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
	if (FAILED(Ready_For_MapString()))
	{
		MSG_BOX("Failed To Ready : Ready_For_MapString");
		return E_FAIL;
	}

	m_bComponets.fill(false);
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

HRESULT CImgui_MapEditor::Ready_For_MapString()
{
	return S_OK;
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
						m_wstrModelName = ProtoPair.first;			// 리스트 박스를 누르면 현재 모델프로토 타입 이름을 가져옴
				}


			}
			ImGui::EndListBox();
		}
	}
#pragma endregion ~생성시 사용되는 모델 이름


#pragma region 생성시 사용되는 클론 이름짓기
	if (ImGui::CollapsingHeader("Create_Clone_Tag"))
		ImGui::InputText("Clone_Tag ", m_strCloneTag, CLONE_TAG_BUFF_SIZE);

#pragma endregion ~생성시 사용되는 모델 이름

	ImGui::Checkbox("Is_Have_Wall", &m_bComponets[COMPONENTS_WALL]);
	ImGui::Checkbox("Is_Have_Interaction", &m_bComponets[COMPONENTS_INTERACTION]);
	

#pragma region		선택된 오브젝트들 보여주기
	if (ImGui::CollapsingHeader("Selcted_Object_Data"))
	{
		char szSelctedObject_Name[256], szSelctedModel_Name[256];
		CUtile::WideCharToChar(m_wstrProtoName.c_str(), szSelctedObject_Name);
		CUtile::WideCharToChar(m_wstrModelName.c_str(), szSelctedModel_Name);

		ImGui::Text("Selected_ProtoObj_Tag : %s", szSelctedObject_Name);
		ImGui::Text("Selected_Model_Tag : %s", szSelctedModel_Name);
		ImGui::Text("Selected_Clone_Tag : %s", m_strCloneTag);
	}
#pragma endregion ~선택된 오브젝트들 보여주기
}

void CImgui_MapEditor::Imgui_CreateEnviromentObj()
{
	if (ImGui::CollapsingHeader("Create_Object"))
	{
		CGameInstance *pGameInstace = GET_INSTANCE(CGameInstance);

		if (ImGui::Button("Create_EnviromentObj"))
		{
			CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
			lstrcpy(EnviromentDesc.szProtoObjTag, m_wstrProtoName.c_str());
			lstrcpy(EnviromentDesc.szModelTag, m_wstrModelName.c_str());
			//EnviromentDesc.szTextureTag = TEXT("");		// 나중에 채워
			string			strCloneTag = m_strCloneTag;
			_tchar *pCloneName = CUtile::StringToWideChar(strCloneTag);
			CGameInstance::GetInstance()->Add_String(pCloneName);

			if (FAILED(pGameInstace->Clone_GameObject(LEVEL_MAPTOOL,
				TEXT("Layer_Enviroment"),
				EnviromentDesc.szProtoObjTag,
				pCloneName, &EnviromentDesc)))
				assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}


void CImgui_MapEditor::Imgui_Save_Load_Json()
{
	if (ImGui::Button("Save"))
	{
		Imgui_Save_Func();
	}
	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		Imgui_Load_Func();
	}
}

void CImgui_MapEditor::Imgui_ModelProtoFinder()
{
}

void CImgui_MapEditor::Imgui_Save_Func()
{
	//ofstream	file("../Bin/Data/EnviromentObj_Json/Chil.json");
	//Json		jTest;
	//int a = 27;
	//string str = "choi";
	//_float4x4	matWorld;
	//matWorld.m[0][0] = 0.f;
	//matWorld.m[0][1] = 0.f;
	//matWorld.m[0][2] = 0.f;
	//matWorld.m[0][3] = 0.f;
	//matWorld.m[1][0] = 0.f;
	//matWorld.m[1][1] = 0.f;
	//matWorld.m[1][2] = 0.f;
	//matWorld.m[1][3] = 0.f;
	//matWorld.m[2][0] = 0.f;
	//matWorld.m[2][1] = 0.f;
	//matWorld.m[2][2] = 0.f;
	//matWorld.m[2][3] = 0.f;
	//matWorld.m[3][0] = 0.f;
	//matWorld.m[3][1] = 0.f;
	//matWorld.m[3][2] = 0.f;
	//matWorld.m[3][3] = 0.f;
	//jTest["Age"] = a;
	//jTest["Name"] = str;
	//float	fElement = 0.f;
	//for (int i = 0; i < 16; ++i)
	//{
	//	fElement = 0.f;
	//	memcpy(&fElement, (float*)&matWorld + i , sizeof(float));
	//	jTest["Transform State"].push_back(fElement);		// 배열 저장. 컨테이너의 구조랑 비슷합니다. 이렇게 하면 Transform State에는 16개의 float 값이 저장됩니다.
	//}
	//
	//Json	jChild;
	//jChild["Math"] = "A";
	//jChild["English"] = "A";
	//jTest["Grade"].push_back(jChild);
	//file << jTest;
	//file.close();

	ofstream	file("../Bin/Data/EnviromentObj_Json/EnviroMent.json");
	Json	jEnviromentObjList;

	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	_float4x4	fWroldMatrix;
	_float		fElement = 0.f;
	char*		szLayerTag = "Layer_Enviroment";
	char*		szProtoObjTag = "";
	char*		szModelTag = "";
	char*		szTextureTag = "";
	char*		szCloneTag = "";

	jEnviromentObjList["0_LayerTag"] = szLayerTag;

	for (auto& pObject : pGameInstace->Find_Layer(LEVEL_MAPTOOL, L"Layer_Enviroment")->GetGameObjects())
	{
		if (dynamic_cast<CEnviromentObj*>(pObject.second) == nullptr)
			continue;

		Json jChild;

		CEnviromentObj::ENVIROMENT_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		memcpy(&Desc, &static_cast<CEnviromentObj*>(pObject.second)->Get_EnviromentDesc(), sizeof(Desc));

		szProtoObjTag = CUtile::WideCharToChar(Desc.szProtoObjTag);
		szModelTag = CUtile::WideCharToChar(Desc.szModelTag);
		szTextureTag = CUtile::WideCharToChar(Desc.szTextureTag);
		szCloneTag = CUtile::WideCharToChar(const_cast<_tchar*>(pObject.second->Get_ObjectCloneName()));

		jChild["0_ProtoTag"] = szProtoObjTag;
		jChild["1_ModelTag"] = szModelTag;
		jChild["2_TextureTag"] = szTextureTag;
		jChild["3_CloneTag"] = szCloneTag;

		CTransform* pTransform = static_cast<CTransform*>(pObject.second->Find_Component(L"Com_Transform"));
		assert(pTransform != nullptr && "CImgui_MapEditor::Imgui_Save_Func()");
		XMStoreFloat4x4(&fWroldMatrix, pTransform->Get_WorldMatrix());

		for (int i = 0; i < 16; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&fWroldMatrix + i, sizeof(float));
			jChild["4_Transform State"].push_back(fElement);		// 배열 저장. 컨테이너의 구조랑 비슷합니다. 이렇게 하면 Transform State에는 16개의 float 값이 저장됩니다.
		}

		jEnviromentObjList["1_Data"].push_back(jChild);
		Safe_Delete_Array(szProtoObjTag);
		Safe_Delete_Array(szModelTag);
		Safe_Delete_Array(szTextureTag);
		Safe_Delete_Array(szCloneTag);
	}


	file << jEnviromentObjList;
	file.close();
	MSG_BOX("Save_jEnviromentObjList");

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CImgui_MapEditor::Imgui_Load_Func()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
	CGameObject*	pLoadObject = nullptr;
	ifstream		file("../Bin/Data/EnviromentObj_Json/EnviroMent.json");
	Json	jLoadEnviromentObjList;

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

	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	pGameInstance->Add_String(wszLayerTag);


	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		jLoadChild["3_CloneTag"].get_to<string>(szCloneTag);
		float	fElement = 0.f;
		int k = 0;

		for (float fElement : jLoadChild["4_Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
		}

		ZeroMemory(&EnviromentDesc, sizeof(EnviromentDesc));
		m_wstrProtoName.assign(szProtoObjTag.begin(), szProtoObjTag.end());
		m_wstrModelName.assign(szModelTag.begin(), szModelTag.end());
		m_wstrTexturelName.assign(szTextureTag.begin(), szTextureTag.end());
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		pGameInstance->Add_String(wszCloneTag);

		lstrcpy(EnviromentDesc.szProtoObjTag, m_wstrProtoName.c_str());
		lstrcpy(EnviromentDesc.szModelTag, m_wstrModelName.c_str());
		lstrcpy(EnviromentDesc.szTextureTag, m_wstrTexturelName.c_str());

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MAPTOOL,
			wszLayerTag,
			EnviromentDesc.szProtoObjTag,
			wszCloneTag, &EnviromentDesc, &pLoadObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		assert(pLoadObject != nullptr && "pLoadObject Issue");
		static_cast<CTransform*>(pLoadObject->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);

	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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