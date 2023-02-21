#include "stdafx.h"
#include "..\public\UI_Canvas.h"
#include "GameInstance.h"

CUI_Canvas::CUI_Canvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CUI_Canvas::CUI_Canvas(const CUI_Canvas & rhs)
	:CUI(rhs)
{
}

HRESULT CUI_Canvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Canvas::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Canvas::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Canvas::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Canvas::Render()
{
	__super::Render();

	return S_OK;
}

bool	getter_ForCanvas(void* data, int index, const char** output)
{
	vector<string>*	 pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}
void CUI_Canvas::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		vector<wstring>* pTags = pGameInstance->Get_UITextureProtoTagsPtr();
		vector<string>* pNames = pGameInstance->Get_UITextureNamesPtr();
		vector<string>* pPasses = pGameInstance->Get_UIString(L"RenderPass");
		RELEASE_INSTANCE(CGameInstance);
		_uint iNumTextures = (_uint)pTags->size();

		/* Diffuse */
		static int selected_Diffuse = 0;
		if (ImGui::ListBox(" : Diffuse", &selected_Diffuse, getter_ForCanvas, pNames, iNumTextures, 5))
		{
			if (FAILED(Set_Texture(CUI::TEXTURE_DIFFUSE, (*pTags)[selected_Diffuse])))
				MSG_BOX("Failed To Set Diffuse Texture : UIEditor");
		}

		/* Mask */
		static int selected_Mask = 0;
		if (ImGui::ListBox(" : Mask", &selected_Mask, getter_ForCanvas, pNames, iNumTextures, 5))
		{
			if (FAILED(Set_Texture(CUI::TEXTURE_MASK, (*pTags)[selected_Mask])))
				MSG_BOX("Failed To Set Mask Texture : UIEditor");
		}

		/* RenderPass */
		static int selected_Pass = 0;
		_uint iNumPasses = (_uint)pPasses->size();
		if (ImGui::ListBox(" : RenderPass", &selected_Pass, getter_ForCanvas, pPasses, iNumPasses, 5))
		{
			Set_RenderPass(selected_Pass);
		}
	}


	m_pTransformCom->Imgui_RenderProperty();

	/* Node Setting */
	if (ImGui::CollapsingHeader("Nodes"))
	{
		static int selected_Node = 0;
		_uint iNumNodes = (_uint)m_vecNodeCloneTag.size();
		ImGui::ListBox(" : Node", &selected_Node, getter_ForCanvas, &m_vecNodeCloneTag, iNumNodes, 5);

		m_vecNode[selected_Node]->Imgui_RenderProperty();
		
	}

}

HRESULT CUI_Canvas::Add_Node(CUI * pUI)
{
	if (nullptr == pUI)
		return E_FAIL;

	m_vecNode.push_back(pUI);
	// Safe_AddRef(pUI); 

	pUI->Set_Parent(this);

	return S_OK;
}

void CUI_Canvas::Free()
{
	__super::Free();

	//for (auto &pChildUI : m_vecChildUI)
	//	Safe_Release(pChildUI);
	m_vecNodeCloneTag.clear();
	m_vecNode.clear();
}
