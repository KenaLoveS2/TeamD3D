#include "stdafx.h"
#include "..\public\Imgui_UIEditor.h"
#include "GameInstance.h"
#include "UI_ClientManager.h"
#include "Utile.h"
#include "UI_Canvas.h"
#include "UI_Node.h"

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


	//if (FAILED(Ready_TextureList()))
	//{
	//	MSG_BOX("Failed To Ready Texture List : Imgui_UIEditor");
	//	return E_FAIL;
	//}

	//if (FAILED(Ready_CanvasProtoList()))
	//{
	//	MSG_BOX("Failed To Ready CanvasProto List : Imgui_UIEditor");
	//	return E_FAIL;
	//}

	if (FAILED(Ready_CloneCanvasList()))
	{
		MSG_BOX("Failed To Ready CanvasClone List : Imgui_UIEditor");
		return E_FAIL;
	}

	m_szFreeRenderName = "UI Editor";

	return S_OK;
}

bool	CanvasProto_Getter(void* data, int index, const char** output)
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
			if (ListBox(" : Type", &selected_canvasType, CanvasProto_Getter, pCanvasNames, iNumItems, iNumItems))
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