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

using namespace ImGui;

CImgui_UIEditor::CImgui_UIEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_UIEditor::Initialize(void * pArg)
{

	m_pCanvas = nullptr;
	m_pNode = nullptr;


	if (FAILED(Ready_TextureList()))
	{
		MSG_BOX("Failed To Ready Texture List : Imgui_UIEditor");
		return E_FAIL;
	}

	if (FAILED(Ready_CanvasProtoList()))
	{
		MSG_BOX("Failed To Ready CanvasProto List : Imgui_UIEditor");
		return E_FAIL;
	}

	if (FAILED(Ready_CloneCanvasList()))
	{
		MSG_BOX("Failed To Ready CanvasClone List : Imgui_UIEditor");
		return E_FAIL;
	}



	return S_OK;
}

bool	CanvasProto_Getter(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
bool	CanvasDiffuse_Getter(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CImgui_UIEditor::Imgui_FreeRender()
{
	if (Begin("UI Editor"))
	{
		Text("<Canvas>");
		
		/**** Canvas Type *****************************/
		static int selected_canvasType = 0;
		_uint iNumItems = (_uint)m_vecCanvasProtoTag.size();
		if (ListBox(" : Type", &selected_canvasType, CanvasProto_Getter, &m_vecCanvasProtoTag, iNumItems, iNumItems))
		{
			/* Create a Object of the selected type if it doesn't exist. */
			if (m_vecCanvas[selected_canvasType] == nullptr)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				wstring wstr = (*CUI_ClientManager::GetInstance()->Get_CanvasProtoTag())[selected_canvasType].c_str();
				if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), L"Layer_Canvas",
					(*CUI_ClientManager::GetInstance()->Get_CanvasProtoTag())[selected_canvasType].c_str(), nullptr, (CGameObject**)&m_pCanvas)))
					MSG_BOX("Failed To Clone Canvas : UIEditor");

				if (m_pCanvas != nullptr)
					m_vecCanvas.push_back(m_pCanvas);

				RELEASE_INSTANCE(CGameInstance);
			}
			else
				m_pCanvas = m_vecCanvas[selected_canvasType];
		}
		/**** ~Canvas Type *****************************/

		if (m_pCanvas == nullptr)
			goto Exit;

		/* Translation */
		m_pCanvas->Imgui_RenderProperty();

		/* Image */
		if (CollapsingHeader("Image"))
		{
			static int selected_Diffuse = 0;
			_uint iNumTextures = (_uint)m_vecTextureTag.size();
			if (ListBox(" : Diffuse", &selected_Diffuse, CanvasDiffuse_Getter, &m_vecTextureTag, iNumTextures, 5))
			{

			}
		}



	Exit:
		End();
	}
}

HRESULT CImgui_UIEditor::Ready_TextureList()
{
	const vector<wstring>* pTextureTags = CUI_ClientManager::GetInstance()->Get_TextureProtoTag();
	wstring tag = L"Prototype_Component_Texture_";
	size_t tagLength = tag.length();
	size_t length = 0;

	for (auto wstr : *pTextureTags)
	{
		length = wstr.length() - tagLength;
		wstr = wstr.substr(tagLength, length);
		string str; 
		str.assign(wstr.begin(), wstr.end());
		m_vecTextureTag.push_back(str);
	}

	return S_OK;
}

HRESULT CImgui_UIEditor::Ready_CanvasProtoList()
{
	const vector<wstring>* pTags = CUI_ClientManager::GetInstance()->Get_CanvasProtoTag();
	wstring tag = L"Prototype_GameObject_UI_";
	size_t tagLength = tag.length();
	size_t length = 0;

	for (auto wstr : *pTags)
	{
		length = wstr.length() - tagLength;
		wstr = wstr.substr(tagLength, length);
		string str;
		str.assign(wstr.begin(), wstr.end());
		m_vecCanvasProtoTag.push_back(str);
	}

	return S_OK;
}

HRESULT CImgui_UIEditor::Ready_CloneCanvasList()
{
	/* Initialize */
	for (auto wstr : m_vecCanvasProtoTag)
		m_vecCanvas.push_back(nullptr);
	
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

	m_vecTextureTag.clear();
	m_vecCanvasProtoTag.clear();

	for (auto canv : m_vecCanvas)
		Safe_Release(canv);
}