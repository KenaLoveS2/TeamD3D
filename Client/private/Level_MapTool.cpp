#include "stdafx.h"
#include "..\public\Level_MapTool.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

#include "Imgui_PropertyEditor.h"
#include "Imgui_MapEditor.h"
#include "Imgui_TerrainEditor.h"
#include "Imgui_ShaderEditor.h"
#include "Imgui_Effect.h"
#include "Layer.h"
#include "GameObject.h"
#include "Tool_Settings.h"
#include "Tool_Animation.h"
#include "Imgui_UIEditor.h"
#include "ImGui_PhysX.h"

#include "UI_ClientManager.h"
#include "UI.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MapTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CGameInstance* p_game_instance = CGameInstance::GetInstance();
	p_game_instance->Clear_ImguiObjects();
	
	p_game_instance->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext), true);
	p_game_instance->Add_ImguiObject(CImgui_MapEditor::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_TerrainEditor::Create(m_pDevice, m_pContext));

	p_game_instance->Add_ImguiObject(CTool_Animation::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CTool_Settings::Create(m_pDevice, m_pContext));

	p_game_instance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));	
	
	p_game_instance->Add_ImguiObject(CImGui_PhysX::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Enviroment(TEXT("Layer_Enviroment"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_ControlRoom(TEXT("Layer_ControlRoom"))))
	{
		MSG_BOX("Layer_ControlRoom");
		return E_FAIL;
	}

	if (FAILED(p_game_instance->Late_Initialize(LEVEL_MAPTOOL)))
		return E_FAIL;

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_MapTool::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CLevel_MapTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : MapTool"));

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	strcpy_s(LightDesc.szLightName, MAX_PATH, "DIRECTIONAL");

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CImgui_TerrainEditor::LoadFilterData("0_Terrain.json");

	//CImgui_TerrainEditor::LoadFilterData("1_Terrain.json");
	//CImgui_TerrainEditor::LoadFilterData("2_Terrain.json");
	//CImgui_TerrainEditor::LoadFilterData("3_Terrain.json");
	//CImgui_TerrainEditor::LoadFilterData("4_Terrain.json");
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Enviroment(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	//CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Test_InstGimmick_2.json");

	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_0.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_1.json");
	//CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_2.json");

#ifdef FOR_MAP_GIMMICK
	
	/*CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_1.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_2.json");*/
#else 
	//CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_0.json");
	//CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_1.json");
	//CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_2.json");

#endif

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCamera *pCamera = (CCamera *)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), nullptr, &CameraDesc);
	if (pCamera == nullptr) return E_FAIL;
	if (FAILED(pGameInstance->Add_Camera(TEXT("DEBUG_CAM_1"), pCamera, true))) return E_FAIL;
	
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(30.f, 7.f, 30.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(40.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCamera = (CCamera *)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), nullptr, &CameraDesc);
	if (pCamera == nullptr) return E_FAIL;
	if (FAILED(pGameInstance->Add_Camera(TEXT("DEBUG_CAM_2"), pCamera))) return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, pLayerTag, TEXT("Prototype_GameObject_LightCamera"), L"LightCamera")))
		return E_FAIL;

#ifdef FOR_MAP_GIMMICK
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(75.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Camera_Player", L"Camera_Player", &CameraDesc));
	NULL_CHECK_RETURN(pCamera, E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"PLAYER_CAM", pCamera), E_FAIL);
#endif
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

#ifdef FOR_MAP_GIMMICK
	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_MAPTOOL, pLayerTag, TEXT("Prototype_GameObject_Kena"), L"Kena", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_MAPTOOL, pGameObject)))
		return E_FAIL;
#endif
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

HRESULT CLevel_MapTool::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_ControlRoom(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Clone_GameObject(LEVEL_MAPTOOL, pLayerTag, TEXT("Prototype_GameObject_ControlRoom"), L"ControlRoom");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CLevel_MapTool * CLevel_MapTool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_MapTool*		pInstance = new CLevel_MapTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_MapTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();
}

