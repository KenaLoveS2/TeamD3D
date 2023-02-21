#include "stdafx.h"
#include "..\private\Imgui_ShaderEditor.h"

CImgui_ShaderEditor::CImgui_ShaderEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_ShaderEditor::Initialize(void * pArg)
{
	m_szFreeRenderName = "Shader_Editor";

	return S_OK;
}

void CImgui_ShaderEditor::Imgui_FreeRender()
{
	ImGui::Text("Shader");
	
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
}
