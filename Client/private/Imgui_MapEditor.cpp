#include "stdafx.h"
#include "..\public\Imgui_MapEditor.h"
#include "GameInstance.h"
#include "EnviromentObj.h"
#include "Layer.h"


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
					m_strProtoName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� ������ Ÿ�� �̸��� ������
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
					m_strModelName = ProtoPair.first;			// ����Ʈ �ڽ��� ������ ���� �������� Ÿ�� �̸��� ������
			}
			ImGui::EndListBox();
		}
	}
#pragma endregion ~������ ���Ǵ� �� �̸�

#pragma region		���õ� ������Ʈ�� �����ֱ�
	char szSelctedObject_Name[256], szSelctedModel_Name[256];
	CUtile::WideCharToChar(m_strProtoName.c_str(), szSelctedObject_Name);
	CUtile::WideCharToChar(m_strModelName.c_str(), szSelctedModel_Name);

	ImGui::Text("Selected_ProtoObj_Tag : %s", szSelctedObject_Name);
	ImGui::Text("Selected_Model_Tag : %s", szSelctedModel_Name);

#pragma endregion ~���õ� ������Ʈ�� �����ֱ�
}

void CImgui_MapEditor::Imgui_CreateEnviromentObj()
{
	CGameInstance *pGameInstace = GET_INSTANCE(CGameInstance);

	if (ImGui::Button("Create_EnviromentObj"))
	{
		CEnviromentObj::tagEnviromnetObjectDesc EnviromentDesc;
		lstrcpy(EnviromentDesc.szProtoObjTag, m_strProtoName.c_str());
		lstrcpy(EnviromentDesc.szModelTag, m_strModelName.c_str());
		//EnviromentDesc.szTextureTag = TEXT("");		// ���߿� ä��


		if (FAILED(pGameInstace->Clone_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Enviroment"), m_strProtoName.c_str(),&EnviromentDesc)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

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

	m_strProtoName.clear();
}