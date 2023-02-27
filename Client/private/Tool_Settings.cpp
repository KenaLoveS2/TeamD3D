#include "stdafx.h"
#include "..\public\Tool_Settings.h"
#include "GameInstance.h"
#include "Utile.h"

CTool_Settings::CTool_Settings(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImguiObject(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CTool_Settings::Initialize(void * pArg)
{
	m_szWindowName = "Public Settings";

	m_fFrameRate = &ImGui::GetIO().Framerate;
	m_TextColor = { 0.f, 0.f, 0.f, 1.f };

	return S_OK;
}

void CTool_Settings::Imgui_RenderWindow()
{
	FAILED_CHECK_RETURN(FrameRate(), );
	ImGui::Separator();
	FAILED_CHECK_RETURN(Camera_Setting(), );
	ImGui::Separator();
}

HRESULT CTool_Settings::FrameRate()
{
	if (*m_fFrameRate >= 60.f)
		m_TextColor = { 0.f, 1.f, 0.f, 1.f };
	else if (*m_fFrameRate >= 30.f && *m_fFrameRate < 60.f)
		m_TextColor = { 1.f, 0.7f, 0.f, 1.f };
	else
		m_TextColor = { 1.f, 0.f, 0.f, 1.f };

	ImGui::TextColored(m_TextColor, "FPS : %.1f", ImGui::GetIO().Framerate);

	return S_OK;
}

HRESULT CTool_Settings::Camera_Setting()
{
	ImGui::Text("Change Camera");
	m_mapCamera = m_pGameInstance->Get_CameraContainer();
	NULL_CHECK_RETURN(m_mapCamera, E_FAIL);

	_uint	iCameraCount = (_uint)m_mapCamera->size();

	char**		ppCameraTag = new char*[iCameraCount];

	_uint	iTagLength = 0;
	_uint	iCurCameraIndex = 0;
	_uint	k = 0;
	for (auto& Pair : *m_mapCamera)
	{
		if (Pair.second == m_pGameInstance->Get_WorkCameraPtr())
			iCurCameraIndex = k;

		ppCameraTag[k++] = CUtile::WideCharToChar(const_cast<_tchar*>(Pair.first));
	}

	ImGui::BulletText("Current Camera : %s", ppCameraTag[iCurCameraIndex]);

	for (_uint i = 0; i < iCameraCount; ++i)
	{
		if (ImGui::RadioButton(ppCameraTag[i], (_int*)&iCurCameraIndex, (_int)i))
		{
			iCurCameraIndex = i;
			_tchar*	pWorkCameraTag = CUtile::CharToWideChar(ppCameraTag[iCurCameraIndex]);
			m_pGameInstance->Work_Camera(pWorkCameraTag);
			Safe_Delete_Array(pWorkCameraTag);
		}
	}

	for (_uint i = 0; i < iCameraCount; ++i)
		Safe_Delete_Array(ppCameraTag[i]);
	Safe_Delete_Array(ppCameraTag);

	return S_OK;
}

CTool_Settings * CTool_Settings::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CTool_Settings*	pInstance = new CTool_Settings(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_Settings");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Settings::Free()
{
	__super::Free();
}
