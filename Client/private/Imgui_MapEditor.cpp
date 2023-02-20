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

	}
	ImGui::End();
}

HRESULT CImgui_MapEditor::Ready_For_MapString()
{

	return S_OK;
}

void CImgui_MapEditor::Imgui_SelectOption()
{
	ImGui::RadioButton("CHAPTER_ONE_CAVE",	&m_iChapterOption, _int(CEnviromentObj::CHAPTER_ONE_CAVE));
	ImGui::RadioButton("CHAPTER_TWO_FOREST", &m_iChapterOption , _int(CEnviromentObj::CHAPTER_TWO_FOREST));

	if (ImGui::BeginListBox("##"))
	{
		const bool bSelected = false;
		for (auto& ProtoPair : CGameInstance::GetInstance()->Get_ProtoTypeObjects())
		{
			char szViewName[512], szProtoName[256];
			CUtile::WideCharToChar(ProtoPair.first, szProtoName);
			sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
			if (ImGui::Selectable(szViewName, bSelected))
				_bool b = false;
		}

		ImGui::EndListBox();
	}

	
	
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
}