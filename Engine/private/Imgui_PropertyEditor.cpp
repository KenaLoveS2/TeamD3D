#include "stdafx.h"
#include "..\public\Imgui_PropertyEditor.h"
#include "GameObject.h"
#include "Level_Manager.h"
#include "GameInstance.h"

CImgui_PropertyEditor::CImgui_PropertyEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_PropertyEditor::Initialize(void* pArg)
{
	m_szTabname = "ObjectEditor";
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	return S_OK;
}

void CImgui_PropertyEditor::Imgui_RenderTab()
{
	static const _tchar* test2 = L"";
	m_pGameInstance->Imgui_ProtoViewer(CLevel_Manager::GetInstance()->Get_CurrentLevelIndex(), test2);
	m_pGameInstance->Imgui_ObjectViewer(CLevel_Manager::GetInstance()->Get_CurrentLevelIndex(), m_pSelectedObject);
	m_pGameInstance->Imgui_Push_Group(m_pSelectedObject);
	m_pGameInstance->Imgui_DeleteComponent(m_pSelectedObject);

	// todo: 마우스 피킹으로 오브젝트 선택하는 기능 추가
	

	if (m_pSelectedObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pSelectedObject).name());
		m_pSelectedObject->Imgui_RenderProperty();
		m_pSelectedObject->Imgui_RenderComponentProperties();
	}
}

CImgui_PropertyEditor* CImgui_PropertyEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_PropertyEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_PropertyEditor::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
