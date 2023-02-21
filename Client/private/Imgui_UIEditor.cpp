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

	m_szFreeRenderName = "UI Editor";

	return S_OK;
}

bool	CanvasProto_Getter(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
bool	CanvasTexture_Getter(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
bool	RenderPass_Getter(void* data, int index, const char** output)
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
			_uint iNumItems = (_uint)m_vecCanvasProtoTag.size();
			if (ListBox(" : Type", &selected_canvasType, CanvasProto_Getter, &m_vecCanvasProtoTag, iNumItems, iNumItems))
			{
				/* Create a Object of the selected type if it doesn't exist. */
				if (m_vecCanvas[selected_canvasType] == nullptr)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					wstring wstr = (*CUI_ClientManager::GetInstance()->Get_CanvasProtoTag())[selected_canvasType].c_str();
					wstring tag = L"Prototype_GameObject_UI_";
					size_t tagLength = tag.length();
					size_t length = 0;
					length = wstr.length() - tagLength;

					wstring wstt = (*CUI_ClientManager::GetInstance()->Get_CanvasProtoTag())[selected_canvasType].substr(tagLength, length).c_str();

					if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->Get_CurLevelIndex(), L"Layer_Canvas",
						(*CUI_ClientManager::GetInstance()->Get_CanvasProtoTag())[selected_canvasType].c_str(), 
						(*CUI_ClientManager::GetInstance()->Get_CanvasProtoTag())[selected_canvasType].substr(tagLength, length).c_str(), nullptr, (CGameObject**)&m_pCanvas)))
						MSG_BOX("Failed To Clone Canvas : UIEditor");

					if (m_pCanvas != nullptr)
						m_vecCanvas.push_back(m_pCanvas);

					RELEASE_INSTANCE(CGameInstance);
				}
				else
					m_pCanvas = m_vecCanvas[selected_canvasType];
			}
		}

		if (m_pCanvas == nullptr)
			goto Exit;

		/* Texture */
		if (CollapsingHeader("Texture"))
		{
			/* Diffuse */
			static int selected_Diffuse = 0;
			_uint iNumTextures = (_uint)m_vecTextureTag.size();
			if (ListBox(" : Diffuse", &selected_Diffuse, CanvasTexture_Getter, &m_vecTextureTag, iNumTextures, 5))
			{
				wstring wstr = (*CUI_ClientManager::GetInstance()->Get_TextureProtoTag())[selected_Diffuse].c_str();
				if (FAILED(m_pCanvas->Set_Texture(CUI::TEXTURE_DIFFUSE, wstr)))
				{
					MSG_BOX("Failed To Set Diffuse Texture : UIEditor");
				}
			}

			LINE;
			
			/* Mask */
			static int selected_Mask = 0;
			if (ListBox(" : Mask", &selected_Mask, CanvasTexture_Getter, &m_vecTextureTag, iNumTextures, 5))
			{
				wstring wstr = (*CUI_ClientManager::GetInstance()->Get_TextureProtoTag())[selected_Mask].c_str();
				if (FAILED(m_pCanvas->Set_Texture(CUI::TEXTURE_MASK, wstr)))
				{
					MSG_BOX("Failed To Set Mask Texture : UIEditor");
				}
			}

			LINE;

			/* Shader Pass */ //RenderPass_Getter
			static int selected_Pass = 0;
			_uint iNumPass = (_uint)m_vecRenderPass.size();
			if (ListBox(" : Shader Pass", &selected_Pass, RenderPass_Getter, &m_vecRenderPass, iNumPass, 5))
			{
				m_pCanvas->Set_RenderPass(selected_Pass);
			}

		}

		/* Translation */
		m_pCanvas->Imgui_RenderProperty();



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

	/* RenderPass List */
	m_vecRenderPass.push_back("Default");
	m_vecRenderPass.push_back("x(Effect)");
	m_vecRenderPass.push_back("DiffuseAlphaBlend");
	m_vecRenderPass.push_back("MaskMap");

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
	m_vecCanvas.clear();

	m_vecRenderPass.clear();
}