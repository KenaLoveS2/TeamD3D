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

	return S_OK;
}

void CImgui_ShaderEditor::Imgui_FreeRender()
{
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if(ImGui::Button("Use Shadow"))
	{
		m_pGameInstance->SwitchOnOff_Shadow(true);
	}

	if(ImGui::Button("Don't Use Shadow"))
	{
		m_pGameInstance->SwitchOnOff_Shadow(false);
	}

	CPostFX::GetInstance()->Imgui_Render();

	ImGui::End();
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
