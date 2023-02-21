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
#pragma region ������ ���Ǵ� é��
	ImGui::RadioButton("CHAPTER_ONE_CAVE", &m_iChapterOption, _int(CEnviromentObj::CHAPTER_ONE_CAVE));
	ImGui::RadioButton("CHAPTER_TWO_FOREST", &m_iChapterOption, _int(CEnviromentObj::CHAPTER_TWO_FOREST));
#pragma endregion  ~������ ���Ǵ� é��

#pragma region ������ ���Ǵ� ������ Ÿ��
	if (ImGui::CollapsingHeader("ProtoType"))
	{	
		if (ImGui::BeginListBox("#GameObject_Proto#"))
		{
			const bool bObjectSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ProtoTypeObjects())
			{
				if(dynamic_cast<CEnviromentObj*>(ProtoPair.second) ==nullptr )
					continue;

				char szViewName[512], szProtoName[256];
				CUtile::WideCharToChar(ProtoPair.first, szProtoName);
				sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
				if (ImGui::Selectable(szViewName, bObjectSelected))
					m_wstrProtoName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� ������ Ÿ�� �̸��� ������
			}
			ImGui::EndListBox();
		}
	}
#pragma endregion ~������ ���Ǵ� ������ Ÿ��
	
#pragma region ������ ���Ǵ� �� �̸�
	if (ImGui::CollapsingHeader("Model_Proto"))
	{
		if (ImGui::BeginListBox("#Model_Proto#"))
		{
			const bool bModelSelected = false;
			for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ComponentProtoType()[CGameInstance::GetInstance()->Get_CurLevelIndex()])
			{
				if(dynamic_cast<CModel*>(ProtoPair.second) == nullptr)
						continue;
				char szViewName[512], szProtoName[256];
				CUtile::WideCharToChar(ProtoPair.first, szProtoName);
				sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
				if (ImGui::Selectable(szViewName, bModelSelected))
					m_wstrModelName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� �������� Ÿ�� �̸��� ������
			}
			ImGui::EndListBox();
		}
	}
#pragma endregion ~������ ���Ǵ� �� �̸�

#pragma region ������ ���Ǵ� Ŭ�� �̸�����
	ImGui::InputText("Clone_Tag ", m_strCloneTag, CLONE_TAG_BUFF_SIZE);
	
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

void CImgui_MapEditor::Imgui_CreateEnviromentObj()
{
	CGameInstance *pGameInstace = GET_INSTANCE(CGameInstance);

	if (ImGui::Button("Create_EnviromentObj"))
	{
		CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
		lstrcpy(EnviromentDesc.szProtoObjTag, m_wstrProtoName.c_str());
		lstrcpy(EnviromentDesc.szModelTag, m_wstrModelName.c_str());
		//EnviromentDesc.szTextureTag = TEXT("");		// ���߿� ä��
		
		m_wstrCloneName = CUtile::CharToWideChar(m_strCloneTag);

		if (FAILED(pGameInstace->Clone_GameObject(LEVEL_MAPTOOL, 
			TEXT("Layer_Enviroment"), 
			m_wstrProtoName.c_str(),
			m_wstrCloneName.c_str(), &EnviromentDesc)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");
	}

	RELEASE_INSTANCE(CGameInstance);
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
	//	jTest["Transform State"].push_back(fElement);		// �迭 ����. �����̳��� ������ ����մϴ�. �̷��� �ϸ� Transform State���� 16���� float ���� ����˴ϴ�.
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
	char*		szLayerTag = "LayerEnviroment";
	char*		szProtoObjTag = "";
	char*		szModelTag = "";
	char*		szTextureTag = "";
	string		szCloneTag = "";

	jEnviromentObjList["0_LayerTag"] = szLayerTag;
	
	for (auto& pObject : pGameInstace->Find_Layer(LEVEL_MAPTOOL, L"Layer_Enviroment")->GetGameObjects())
	{
		if(dynamic_cast<CEnviromentObj*>(pObject.second) == nullptr)
			continue;

		Json jChild;
		m_wstrCloneName = const_cast<_tchar*>(pObject.second->Get_ObjectCloneName());
		
		CEnviromentObj::ENVIROMENT_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		memcpy(&Desc, &static_cast<CEnviromentObj*>(pObject.second)->Get_EnviromentDesc(), sizeof(Desc));

		szProtoObjTag =CUtile::WideCharToChar(Desc.szProtoObjTag);
		szModelTag = CUtile::WideCharToChar(Desc.szModelTag);
		szTextureTag = CUtile::WideCharToChar(Desc.szTextureTag);
		szCloneTag.assign(m_wstrCloneName.begin(),m_wstrCloneName.end());

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
			jChild["4_Transform State"].push_back(fElement);		// �迭 ����. �����̳��� ������ ����մϴ�. �̷��� �ϸ� Transform State���� 16���� float ���� ����˴ϴ�.
		}

		jEnviromentObjList["1_Data"].push_back(jChild);
		Safe_Delete_Array(szProtoObjTag);
		Safe_Delete_Array(szModelTag);
		Safe_Delete_Array(szTextureTag);
	}


	file << jEnviromentObjList;
	file.close();
	MSG_BOX("Save_jEnviromentObjList");

	RELEASE_INSTANCE(CGameInstance);
}

void CImgui_MapEditor::Imgui_Load_Func()
{
	ifstream		file("../Bin/Data/EnviromentObj_Json/EnviroMent.json");
	Json	jLoadEnviromentObjList;

	file >> jLoadEnviromentObjList;
	file.close();


	_float4x4	fWroldMatrix;
	string		szLayerTag = "";
	string		szProtoObjTag = "";
	string		szModelTag = "";
	string		szTextureTag = "";
	
	jLoadEnviromentObjList["0_LayerTag"].get_to<string>(szLayerTag);

	for (auto jLoadChild : jLoadEnviromentObjList["1_Data"])
	{
		jLoadChild["0_ProtoTag"].get_to<string>(szProtoObjTag);
		jLoadChild["1_ModelTag"].get_to<string>(szModelTag);
		jLoadChild["2_TextureTag"].get_to<string>(szTextureTag);
		
		float	fElement = 0.f;
		int k = 0;
	
		for (float fElement : jLoadChild["3_Transform State"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
		}
	}

	_bool b = false;

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