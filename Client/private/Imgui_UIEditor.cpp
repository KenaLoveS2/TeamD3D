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

/* Defines for Imgui */
#define		AND			ImGui::SameLine()
#define		WIDTH(num)	PushItemWidth(num)
#define		ENTER		ImGui::NewLine()
#define		LINE		ImGui::Separator()

using namespace ImGui;

CImgui_UIEditor::CImgui_UIEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_UIEditor::Initialize(void * pArg)
{
	m_pCanvas = nullptr;
	m_pUI = nullptr;

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
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CImgui_UIEditor::Imgui_FreeRender()
{
	//if (Begin("UI Editor"))
	{
		Text("<Canvas>");

		/* Type */
		if (CollapsingHeader("Type"))
		{
			static int selected_canvasType = 0;

			CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
			vector<wstring>*	pCanvasProtoTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_PROTOTAG);
			vector<wstring>*	pCanvasCloneTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_CLONETAG);
			vector<string>*		pCanvasNames = pGameInstance->Get_UIString(CUI_Manager::STRKEY_CANVAS_NAME);

			RELEASE_INSTANCE(CGameInstance);

			_uint iNumItems = (_uint)pCanvasProtoTags->size();
			if (ListBox(" : Type", &selected_canvasType, Editor_Getter, pCanvasNames, iNumItems, iNumItems))
			{
				/* Create a Object of the selected type if it doesn't exist. */
				if (m_vecCanvas[selected_canvasType] == nullptr)
				{
					CUI::tagUIDesc tDesc;
					tDesc.fileName = (*pCanvasCloneTags)[selected_canvasType].c_str();
					if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(), L"Layer_Canvas",
						(*pCanvasProtoTags)[selected_canvasType].c_str(),
						(*pCanvasCloneTags)[selected_canvasType].c_str(), &tDesc, (CGameObject**)&m_pCanvas)))
						MSG_BOX("Failed To Clone Canvas : UIEditor");

					if (m_pCanvas != nullptr)
						m_vecCanvas[selected_canvasType] = m_pCanvas;
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
		End();
	}
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
		}
	}

	if (Button("DeleteEvent"))
		m_pUI->Delete_Event();

}

CImgui_UIEditor * CImgui_UIEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void* pArg)
{
	CImgui_UIEditor*	pInstance = new CImgui_UIEditor(pDevice, pContext);
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

}