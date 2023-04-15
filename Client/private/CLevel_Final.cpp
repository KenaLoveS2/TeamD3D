#include "stdafx.h"
#include "CLevel_Final.h"
#include "GameInstance.h"

#include "Camera_Dynamic.h"
#include "CinematicCamera.h"
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
#include "ImGui_Monster.h"
#include "ImGui_Rot.h"

#include "UI_ClientManager.h"
#include "UI.h"
#include "Level_Loading.h"
#include "Kena.h"
#include "BGM_Manager.h"

CLevel_Final::CLevel_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Final::Initialize()
{
	CGameInstance* p_game_instance = CGameInstance::GetInstance();

	p_game_instance->Clear();				// Light_Manager
	p_game_instance->Camera_SceneClear();	// Camera_Manager

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	p_game_instance->Clear_ImguiObjects();
	p_game_instance->Add_ImguiObject(CTool_Settings::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext), true);
	p_game_instance->Add_ImguiObject(CTool_Animation::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImGui_Monster::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImGui_Rot::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImGui_PhysX::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		MSG_BOX("Layer_BackGround");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Enviroment(TEXT("Layer_Enviroment"))))
	{
		MSG_BOX("Layer_Enviroment");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		MSG_BOX("Layer_Camera");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_CineCamera(TEXT("CinemaCam"))))
	{
		MSG_BOX("CinemaCam");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		MSG_BOX("Layer_Player");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	{
		MSG_BOX("Layer_Monster");
		return E_FAIL;
	}

	/*if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
	{
		MSG_BOX("Layer_NPC");
		return E_FAIL;
	}*/

	if (FAILED(Ready_Layer_Rot(TEXT("Layer_Rot"))))
	{
		MSG_BOX("Layer_Rot");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		MSG_BOX("Layer_Effect");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI(TEXT("Layer_Canvas"))))
	{
		MSG_BOX("Layer_Canvas");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_ControlRoom(TEXT("Layer_ControlRoom"))))
	{
		MSG_BOX("Layer_ControlRoom");
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Trigger(L"Layer_Trigger")))
	{
		MSG_BOX("Layer_Trigger");
		return E_FAIL;
	}

	if (FAILED(p_game_instance->Late_Initialize(LEVEL_FINAL)))
		return E_FAIL;

	CBGM_Manager::GetInstance()->Change_FieldState(CBGM_Manager::FIELD_IDLE);

	return S_OK;
}

void CLevel_Final::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_Final::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CLevel_Final::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : Final"));

	return S_OK;
}

HRESULT CLevel_Final::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC         LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vPosition = _float4(-100.f, 100.f, -100.f, 1.f);
	LightDesc.szLightName = "DIRECTIONAL";

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_Sky"), TEXT("Clone_Sky"))))
		return E_FAIL;

	_uint iCreateWindCount = 10;
	_tchar szCloneRotTag[32] = { 0, };
	for (_uint i = 0; i < iCreateWindCount; i++)
	{
		swprintf_s(szCloneRotTag, L"Wind_%d", i);
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_Wind"), CUtile::Create_StringAuto(szCloneRotTag))))
			return E_FAIL;
	}

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_EffectFlower"), L"flower")))
		return E_FAIL;

	CImgui_TerrainEditor::LoadFilterData("2_Terrain.json");
	CImgui_TerrainEditor::LoadFilterData("3_Terrain.json");
	CImgui_TerrainEditor::LoadFilterData("4_Terrain.json");

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Enviroment(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CImgui_MapEditor::Load_MapObjects(LEVEL_FINAL, "Instancing_Forest_map_3.json");
	CImgui_MapEditor::Load_MapObjects(LEVEL_FINAL, "Instancing_Forest_map_4.json");
	CImgui_MapEditor::Load_MapObjects(LEVEL_FINAL, "Instancing_Forest_map_5.json");
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_CRope_RotRock"), L"Rope_RotRock", nullptr), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CCamera::CAMERADESC			CameraDesc;
	CCamera* pCamera = nullptr;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float4(0.f, 7.f, 500.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 500.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCamera = (CCamera*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), nullptr, &CameraDesc);
	if (pCamera == nullptr) return E_FAIL;
	if (FAILED(pGameInstance->Add_Camera(TEXT("DEBUG_CAM"), pCamera, true))) return E_FAIL;
	pGameInstance->Work_Camera(TEXT("DEBUG_CAM"));

	///* For. DynamicShadow */
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_DynamicLightCamera"), L"DynamicLightCamera")))
		return E_FAIL;

	{
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
		CameraDesc.vEye = _float4(-30.f, 120.f, -50.f, 1.f);
		CameraDesc.vAt = _float4(60.f, 0.f, 60.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(75.0f);
		CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		pCamera = (CCamera*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LightCamera"), nullptr, &CameraDesc);
		if (pCamera == nullptr) return E_FAIL;
		if (FAILED(pGameInstance->Add_LightCamera(TEXT("LIGHT_CAM_0"), pCamera))) return E_FAIL;

		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
		CameraDesc.vEye = _float4(-10.f, 180.f, 250.f, 1.f);
		CameraDesc.vAt = _float4(100.f, 0.f, 384.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(75.0f);
		CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		pCamera = (CCamera*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LightCamera"), nullptr, &CameraDesc);
		if (pCamera == nullptr) return E_FAIL;
		if (FAILED(pGameInstance->Add_LightCamera(TEXT("LIGHT_CAM_1"), pCamera))) return E_FAIL;

		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
		CameraDesc.vEye = _float4(30.f, 150.f, 430.f, 1.f);
		CameraDesc.vAt = _float4(120.f, 0.f, 580.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(75.0f);
		CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		pCamera = (CCamera*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LightCamera"), nullptr, &CameraDesc);
		if (pCamera == nullptr) return E_FAIL;
		if (FAILED(pGameInstance->Add_LightCamera(TEXT("LIGHT_CAM_2"), pCamera))) return E_FAIL;

		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
		CameraDesc.vEye = _float4(-20.f, 150.f, 750.f, 1.f);
		CameraDesc.vAt = _float4(106.f, -20.f, 864.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(75.0f);
		CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		pCamera = (CCamera*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LightCamera"), nullptr, &CameraDesc);
		if (pCamera == nullptr) return E_FAIL;
		if (FAILED(pGameInstance->Add_LightCamera(TEXT("LIGHT_CAM_3"), pCamera))) return E_FAIL;

		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
		CameraDesc.vEye = _float4(-20.f, 140.f, 890.f, 1.f);
		CameraDesc.vAt = _float4(170.f, -80.f, 1153.8f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(75.0f);
		CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		pCamera = (CCamera*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LightCamera"), nullptr, &CameraDesc);
		if (pCamera == nullptr) return E_FAIL;
		if (FAILED(pGameInstance->Add_LightCamera(TEXT("LIGHT_CAM_4"), pCamera))) return E_FAIL;
	}

	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(75.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 500.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Camera_Player", L"Camera_Player", &CameraDesc));
	NULL_CHECK_RETURN(pCamera, E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"PLAYER_CAM", pCamera), E_FAIL);

	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(75.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 500.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CameraForRot", L"CameraForRot", &CameraDesc));
	NULL_CHECK_RETURN(pCamera, E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"ROT_CAM", pCamera), E_FAIL);

	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(75.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 500.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CameraForNpc", L"CameraForNpc", &CameraDesc));
	NULL_CHECK_RETURN(pCamera, E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"NPC_CAM", pCamera), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_CineCamera(const _tchar* pLayerTag)
{
	{
		vector<CCinematicCamera::CAMERAKEYFRAME> v;
		string chatFileName;
		CCinematicCamera::Clone_Load_Data("BossWarrior_Start.json", v, chatFileName);
		CGameObject* p_game_object = nullptr;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		CCamera * pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CinematicCamera", L"BOSSWARRIOR_START", &v));
		//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_CinematicCamera"), L"BOSSWARRIOR_START", &v, &p_game_object))) return E_FAIL;
		//CCamera* pCamera = dynamic_cast<CCamera*>(p_game_object);
		NULL_CHECK_RETURN(pCamera, E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"BOSSWARRIOR_START", pCamera), E_FAIL);
		static_cast<CCinematicCamera*>(pCamera)->Load_ChatData(chatFileName);
		RELEASE_INSTANCE(CGameInstance)
		v.clear();
	}

	{
		vector<CCinematicCamera::CAMERAKEYFRAME> v;
		string chatFileName;
		CCinematicCamera::Clone_Load_Data("BossWarrior_End.json", v, chatFileName);
		CGameObject* p_game_object = nullptr;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			CCamera * pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CinematicCamera", L"BOSSWARRIOR_END", &v));
		//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_CinematicCamera"), L"BOSSWARRIOR_END", &v, &p_game_object))) return E_FAIL;
		//CCamera* pCamera = dynamic_cast<CCamera*>(p_game_object);
		NULL_CHECK_RETURN(pCamera, E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"BOSSWARRIOR_END", pCamera), E_FAIL);
		static_cast<CCinematicCamera*>(pCamera)->Load_ChatData(chatFileName);
		RELEASE_INSTANCE(CGameInstance)
			v.clear();
	}

	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		CCamera* pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CameraShaman", L"Camera_Shaman"));
		NULL_CHECK_RETURN(pCamera, E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Camera(CAMERA_SHAMAN_TAG, pCamera), E_FAIL);
	}

	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_Kena"), L"Kena", RUNTIME_STATUS_FILEPATH, &pGameObject)))
		return E_FAIL;

	CGameInstance::GetInstance()->Set_PlayerPtr(pGameObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	CImGui_Monster::Load_MonsterObjects(g_LEVEL, "Level1_Chap0_Monster.json");
	//CImGui_Monster::Load_MonsterObjects(g_LEVEL, "Level1_Test.json");
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Rot(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_LiftRot_Master"), L"LiftRot_Master", nullptr), E_FAIL);
	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_P_DeadZoneDust"), L"Dust", nullptr), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_P_DeadZoneDust"), L"Dust2", nullptr), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	vector<wstring>* pCanvasProtoTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_PROTOTAG);
	vector<wstring>* pCanvasCloneTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_CLONETAG);
	vector<string>* pCanvasNames = pGameInstance->Get_UIString(CUI_Manager::STRKEY_CANVAS_NAME);

	for (_uint i = 0; i < CUI_ClientManager::CANVAS_END; ++i)
	{
		CUI::tagUIDesc tDesc;
		tDesc.fileName = (*pCanvasCloneTags)[i].c_str();

		CUI_Canvas* pCanvas = nullptr;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_FINAL, L"Layer_Canvas",
			(*pCanvasProtoTags)[i].c_str(), (*pCanvasCloneTags)[i].c_str(), &tDesc, (CGameObject**)&pCanvas)))
			MSG_BOX("Failed To Clone Canvas : UIEditor");

		if (pCanvas != nullptr)
			CUI_ClientManager::GetInstance()->Set_Canvas((CUI_ClientManager::UI_CANVAS)i, pCanvas);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_Beni"), L"Beni");
	pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_Saiya"), L"Saiya");
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_ControlRoom(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Clone_GameObject(LEVEL_FINAL, pLayerTag, TEXT("Prototype_GameObject_ControlRoom"), L"ControlRoom");
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Final::Ready_Layer_Trigger(const _tchar* pLayerTag)
{
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_FINAL, pLayerTag, L"Prototype_GameObject_BowTarget_Trigger", L"BowTarget_Trigger"), E_FAIL);

	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_FINAL, pLayerTag, L"Prototype_GameObject_Village_Trigger", L"IntoVillage_Trigger"), E_FAIL);

	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_FINAL, pLayerTag, L"Prototype_GameObject_Village_Trigger", L"FromVillage_Trigger"), E_FAIL);

	return S_OK;
}

CLevel_Final* CLevel_Final::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Final* pInstance = new CLevel_Final(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Final");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Final::Free()
{
	__super::Free();
}
