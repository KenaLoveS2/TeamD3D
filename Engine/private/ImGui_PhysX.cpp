#include "stdafx.h"
#include "..\public\ImGui_PhysX.h"

#include "GameInstance.h"
#include "PhysX_Manager.h"

CImGui_PhysX::CImGui_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CImguiObject(pDevice, pContext)
	,m_pGameInstance(CGameInstance::GetInstance())
	,m_pPhysXManager(CPhysX_Manager::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_PhysX::Initialize(void* pArg)
{
	m_szFreeRenderName = "PhysX_Editor";
	return S_OK;
}

void CImGui_PhysX::Imgui_FreeRender()
{
	m_pPhysXManager->Imgui_Render();

	ImGui::End();
}

CImGui_PhysX* CImGui_PhysX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CImGui_PhysX*	pInstance = new CImGui_PhysX(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImGui_PhysX");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_PhysX::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pPhysXManager);
}
