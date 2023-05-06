#include "stdafx.h"
#include "..\public\Imgui_ShaderEditor.h"

#include "GameInstance.h"
#include "PostFX.h"

CImgui_ShaderEditor::CImgui_ShaderEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CImguiObject(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_ShaderEditor::Initialize(void * pArg)
{
	m_szFreeRenderName = "Shader_Editor";
	m_iCurrentLevel = m_pGameInstance->Get_CurLevelIndex();
	m_mapShaderValueObject = m_pGameInstance->Get_ShaderValueObjects(m_iCurrentLevel);
	return S_OK;
}

void CImgui_ShaderEditor::Imgui_FreeRender()
{
	Update_Level();

	if (m_iCurrentLevel == 0)
	{
		ImGui::Text("Wait for Loading...");
		return;
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Checkbox("PhysXRender", &m_bPhysXRender);
	if (CGameInstance::GetInstance()->Key_Down(DIK_HOME))
		m_bPhysXRender = true;
	if (CGameInstance::GetInstance()->Key_Down(DIK_END))
		m_bPhysXRender = false;

	if(m_bPhysXRender)
		m_pRendererCom->Set_PhysXRender(true);
	else
		m_pRendererCom->Set_PhysXRender(false);

	if (ImGui::CollapsingHeader("HDR"))
		CPostFX::GetInstance()->Imgui_Render();

	if (ImGui::CollapsingHeader("Light"))
		m_pGameInstance->Imgui_LightManagerRender();

	if (ImGui::CollapsingHeader("RENDERER"))
	{
		if(m_pRendererCom)
			m_pRendererCom->Imgui_Render();
	}

	if(m_mapShaderValueObject->empty())
	{
		ImGui::Text("There is No object to change shader variables");
		ImGui::End();
		return;
	}

	_uint	 nObjectCount = static_cast<_uint>(m_mapShaderValueObject->size());

	ImGui::BulletText("Object to change shader variables");

	static _int iSelectObject = -1;
	char** ppObjectTag = new char*[nObjectCount];

	_uint iTagLength = 0;
	_uint i = 0;

	for (auto& Pair : *m_mapShaderValueObject)
		ppObjectTag[i++] = CUtile::WideCharToChar(const_cast<_tchar*>(Pair.first));

	ImGui::ListBox("Object List", &iSelectObject, ppObjectTag, nObjectCount);

	if(iSelectObject != -1)
	{
		CGameObject*	pGameObject = Find_GameObject(iSelectObject);

		ImGui::BulletText("Current Object : ");
		ImGui::SameLine();
		ImGui::Text(ppObjectTag[iSelectObject]);
		pGameObject->ImGui_ShaderValueProperty();
	}

	for (_uint i = 0; i < nObjectCount; ++i)
		Safe_Delete_Array(ppObjectTag[i]);
	Safe_Delete_Array(ppObjectTag);

	ImGui::End();
}

CGameObject * CImgui_ShaderEditor::Find_GameObject(_int iIndex)
{
	NULL_CHECK_RETURN(m_mapShaderValueObject, nullptr);

	auto	iter = m_mapShaderValueObject->begin();

	for (_int i = 0; i < iIndex; ++i)
		++iter;

	return iter->second;
}

void CImgui_ShaderEditor::Update_Level()
{
	m_iCurrentLevel = m_pGameInstance->Get_CurLevelIndex();
	m_mapShaderValueObject = m_pGameInstance->Get_ShaderValueObjects(m_iCurrentLevel);

	if(!m_bInit)
	{
		CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance);
		m_pRendererCom = static_cast<CRenderer*>(p_game_instance->Get_ComponentPtr(m_iCurrentLevel, L"Layer_Player", L"Kena", L"Com_Renderer"));
		RELEASE_INSTANCE(CGameInstance);

		if (m_pRendererCom)
			m_bInit = true;
	}
}

CImgui_ShaderEditor * CImgui_ShaderEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CImgui_ShaderEditor*	pInstance = new CImgui_ShaderEditor(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImgui_ShaderEditor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_ShaderEditor::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
