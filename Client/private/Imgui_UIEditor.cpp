#include "stdafx.h"
#include "..\public\Imgui_UIEditor.h"
#include "GameInstance.h"
#include "UI_ClientManager.h"
#include "Utile.h"
#include "UI_Canvas.h"
#include "UI_Node.h"
#include "UI_Event_Guage.h"
#include "UI_Event_ChangeImg.h"
#include "UI_Event_Animation.h"
#include "UI_Event_Fade.h"
#include "Effect_Base_S2.h"

/* Defines for Imgui */
#define		AND			ImGui::SameLine()
#define		WIDTH(num)	PushItemWidth(num)
#define		ENTER		ImGui::NewLine()
#define		LINE		ImGui::Separator()

using namespace ImGui;

CImgui_UIEditor::CImgui_UIEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_UIEditor::Initialize(void* pArg)
{
	m_pCanvas = nullptr;
	m_pUI = nullptr;
	m_pEffect = nullptr;

	if (FAILED(Ready_CloneCanvasList()))
	{
		MSG_BOX("Failed To Ready CanvasClone List : Imgui_UIEditor");
		return E_FAIL;
	}

	m_szFreeRenderName = "UI Editor";

	return S_OK;
}

bool	Editor_Getter(void* data, int index, const char** output)
{
	vector<string>* pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CImgui_UIEditor::Imgui_FreeRender()
{
	Text("<Canvas>");

	/* Type */
	if (CollapsingHeader("Canvas Type"))
	{
		static int selected_canvasType = 0;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		vector<wstring>* pCanvasProtoTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_PROTOTAG);
		vector<wstring>* pCanvasCloneTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_CLONETAG);
		vector<string>* pCanvasNames = pGameInstance->Get_UIString(CUI_Manager::STRKEY_CANVAS_NAME);

		RELEASE_INSTANCE(CGameInstance);

		_uint iNumItems = (_uint)pCanvasProtoTags->size();
		if (ListBox(" : Type", &selected_canvasType, Editor_Getter, pCanvasNames, iNumItems, iNumItems))
		{
			/* Create a Object of the selected type if it doesn't exist. */
			if (m_vecCanvas[selected_canvasType] == nullptr)
			{
				CUI::tagUIDesc tDesc;
				tDesc.fileName = (*pCanvasCloneTags)[selected_canvasType].c_str();
				if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_Canvas",
					(*pCanvasProtoTags)[selected_canvasType].c_str(),
					(*pCanvasCloneTags)[selected_canvasType].c_str(), &tDesc, (CGameObject**)&m_pCanvas)))
					MSG_BOX("Failed To Clone Canvas : UIEditor");

				if (m_pCanvas != nullptr)
				{
					m_vecCanvas[selected_canvasType] = m_pCanvas;
					CUI_ClientManager::GetInstance()->Set_Canvas((CUI_ClientManager::UI_CANVAS)selected_canvasType, m_pCanvas);
				}
			}
			else
				m_pCanvas = m_vecCanvas[selected_canvasType];
		}
	}

	if (m_pCanvas == nullptr)
		goto Exit;

	m_pCanvas->Imgui_RenderProperty();

	/* For. Add Event To UIs */
	EventList();


Exit:
	if (CollapsingHeader("Effect Tool"))
		Effect_Tool();

	End();

}

HRESULT CImgui_UIEditor::Ready_CloneCanvasList()
{
	/* Initialize */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	size_t types = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_PROTOTAG)->size();
	RELEASE_INSTANCE(CGameInstance);

	m_vecCanvas.resize(types);
	for (_uint i = 0; i < types; ++i)
		m_vecCanvas[i] = nullptr;

	/* Todo : File Load */
	/* To Maintain the sequence of the ProtoTags, push_back m_vecCanvas before Dealing with Real Layer */

	return S_OK;
}

void CImgui_UIEditor::EventList()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<string>* pList = pGameInstance->Get_UIString(CUI_Manager::STRKEY_EVENT);
	RELEASE_INSTANCE(CGameInstance);


	_uint iSize = (_uint)pList->size();
	static int selected_Event;
	if (ListBox("EventList", &selected_Event, Editor_Getter, pList, iSize))
	{
		m_pUI = m_pCanvas->Get_SelectedNode();
	}

	if (m_pUI == nullptr)
		return;

	if (Button("Add Event"))
	{
		switch (selected_Event)
		{
		case CUI_ClientManager::EVENT_BARGUAGE:
			m_pUI->Add_Event(CUI_Event_Guage::Create());
			break;
		case CUI_ClientManager::EVENT_CHANGEIMG:
			m_pUI->Add_Event(CUI_Event_ChangeImg::Create());
			break;
		case CUI_ClientManager::EVENT_ANIMATION:
			m_pUI->Add_Event(CUI_Event_Animation::Create(m_pUI));
			break;
		case CUI_ClientManager::EVENT_FADE:
			m_pUI->Add_Event(CUI_Event_Fade::Create());
			break;
		}
	}

	if (Button("DeleteEvent"))
		m_pUI->Delete_Event();

}

void CImgui_UIEditor::Effect_Tool()
{
	Load_List(); AND;
	Save_List();


	static	char szSaveFileName[MAX_PATH] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("##EffectName", "Particle_, Mesh_, Texture_", szSaveFileName, MAX_PATH);
	AND;
	if (Button("Create New Effect"))
	{
		string str = szSaveFileName;
		strcpy_s(szSaveFileName, MAX_PATH, "");
		Create_Effect(str);

		if (m_pEffect != nullptr)
		{
			m_vecEffects.push_back(m_pEffect);
			m_vecEffectTag.push_back(str);
		}
		else
			return;
	}

	static _int iSelectedEffect;
	_int iSelectedBefore = iSelectedEffect;
	if (ListBox("Desc Files", &iSelectedEffect, Editor_Getter, &m_vecEffectTag, (_int)m_vecEffectTag.size(), 5))
	{
		m_pEffect = m_vecEffects[iSelectedEffect];
		m_pEffect->Set_ActiveFlip();
		m_pEffect->Set_EffectTag(m_vecEffectTag[iSelectedEffect]);
	}

	LINE;
	LINE;

	if (nullptr == m_pEffect)
		return;

	if (Button("All off"))
	{
		for (auto eff : m_vecEffects)
			eff->Set_Active(false);
	} AND;
	if (Button("All On"))
	{
		for (auto eff : m_vecEffects)
			eff->Set_Active(true);
	};

	m_pEffect->Imgui_RenderProperty();


	//pEffect->Imgui_RenderProperty();

}

void CImgui_UIEditor::Load_List()
{
	if (Button("Load Effect List"))
		ImGuiFileDialog::Instance()->OpenDialog("Load File", "Select Json", ".json", "../Bin/Data/Effect_UI", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	/* Load Type Data */

	if (ImGuiFileDialog::Instance()->Display("Load File"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			/* Clear All Information */
			if (!m_vecEffectTag.empty())
			{
				for (auto tag : m_vecEffectTag)
				{
					wstring wstr;
					wstr.assign(tag.begin(), tag.end());
					CGameInstance::GetInstance()->Delete_Object(g_LEVEL,
						L"Layer_Effect_S2", wstr.c_str());
				}

				//for (auto effect : m_vecEffects)
				//	Safe_Release(effect);
				m_vecEffectTag.clear();
				m_vecEffects.clear();
			}


			string		strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

			Json	jLoad;
			//string filePath = "../Bin/Data/Effect_UI/00.Effect_List.json";

			ifstream file(strFilePath);
			if (file.fail())
				return;
			file >> jLoad;
			file.close();

			_int iNumEffects;
			jLoad["01. NumEffects"].get_to<_int>(iNumEffects);

			for (auto jSub : jLoad["02. FileName"])
			{
				string strEffect;
				jSub.get_to<string>(strEffect);
				m_vecEffectTag.push_back(strEffect);

				Create_Effect(m_vecEffectTag.back());

				if (m_pEffect != nullptr)
					m_vecEffects.push_back(m_pEffect);
			}
			ImGuiFileDialog::Instance()->Close();
		}
		else
			ImGuiFileDialog::Instance()->Close();
	}
}

void CImgui_UIEditor::Save_List()
{
	if (Button("Save Effect List"))
		ImGuiFileDialog::Instance()->OpenDialog("Save List", "Select Json", ".json", "../Bin/Data/Effect_UI/", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Save List"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			Json json;

			json["01. NumEffects"] = (_int)m_vecEffectTag.size();

			for (auto tag : m_vecEffectTag)
				json["02. FileName"].push_back(tag);


			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

			ofstream file(filePath);
			file << json;
			file.close();

			ImGuiFileDialog::Instance()->Close();
		}
		else
			ImGuiFileDialog::Instance()->Close();
	}


}

void CImgui_UIEditor::Create_Effect(string strEffect)
{
	wstring wstr;
	wstr.assign(strEffect.begin(), strEffect.end());
	_tchar* cloneTag = CUtile::Create_StringAuto(wstr.c_str());

	string type;
	for (auto c : strEffect)
	{
		if (c == '_')
			break;
		type += c;
	}

	m_pEffect = nullptr;

	if (type == "Particle")
		CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL, L"Layer_Effect_S2",
			L"Prototype_GameObject_Effect_Particle_Base", cloneTag, cloneTag, (CGameObject**)&m_pEffect);
	else if (type == "Mesh")
		CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL, L"Layer_Effect_S2",
			L"Prototype_GameObject_Effect_Mesh_Base", cloneTag, cloneTag, (CGameObject**)&m_pEffect);
	else if (type == "Texture")
		CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL, L"Layer_Effect_S2",
			L"Prototype_GameObject_Effect_Texture_Base", cloneTag, cloneTag, (CGameObject**)&m_pEffect);
}

CImgui_UIEditor* CImgui_UIEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CImgui_UIEditor* pInstance = new CImgui_UIEditor(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImgui_UIEditor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_UIEditor::Free()
{
	__super::Free();

	m_vecCanvas.clear();

	/*  Effect */
	//for (auto effect : m_vecEffects)
	//	Safe_Release(effect);
	m_vecEffects.clear();

	m_vecEffectTag.clear();
	/* ~Effect */
}