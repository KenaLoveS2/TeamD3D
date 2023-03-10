#include "stdafx.h"
#include "..\public\Level_TestPlay.h"
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

CLevel_TestPlay::CLevel_TestPlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_TestPlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)
		p_game_instance->Clear_ImguiObjects();
	p_game_instance->Add_ImguiObject(CTool_Settings::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext),true);
	p_game_instance->Add_ImguiObject(CImgui_MapEditor::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CTool_Animation::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_UIEditor::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));
	p_game_instance->Add_ImguiObject(CImgui_Effect::Create(m_pDevice, m_pContext));
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

	if (FAILED(p_game_instance->Late_Initialize(LEVEL_TESTPLAY)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CLevel_TestPlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if(!m_bEnviromentInputShaderValue)
	//{
	//	CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)

	//	CLayer* pLayer =	p_game_instance->Find_Layer(LEVEL_TESTPLAY, L"Layer_Enviroment");

	//	for(auto& pGameObject : pLayer->GetGameObjects())
	//	{
	//		if(pGameObject.second != nullptr)
	//			p_game_instance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject.second);
	//	}

	//	RELEASE_INSTANCE(CGameInstance)

	//	m_bEnviromentInputShaderValue = true;
	//}
}

void CLevel_TestPlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CLevel_TestPlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : TestPlay"));

	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, 1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vAmbient = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vPosition = _float4(100.f, 100.f, 100.f, 1.f);
	strcpy_s(LightDesc.szLightName, MAX_PATH, "DIRECTIONAL");

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Sky"), TEXT("Clone_Sky"))))
		return E_FAIL;

	CImgui_TerrainEditor::LoadFilterData("0_Terrain.json");
	CImgui_TerrainEditor::LoadFilterData("1_Terrain.json");
	CImgui_TerrainEditor::LoadFilterData("2_Terrain.json");
	CImgui_TerrainEditor::LoadFilterData("3_Terrain.json");
	CImgui_TerrainEditor::LoadFilterData("4_Terrain.json");

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_Enviroment(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_0.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_1.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_2.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_3.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_4.json");
	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "Instancing_Forest_map_5.json");

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
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

	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(90.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Camera_Player", L"Camera_Player", &CameraDesc));
	NULL_CHECK_RETURN(pCamera, E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"PLAYER_CAM", pCamera), E_FAIL);
	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Kena"), L"Kena", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	//if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Moth"), L"Moth_0", nullptr, &pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_RockGolem"), L"RockGolem_0", nullptr, &pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_RotEater"), L"RotEater_0", nullptr, &pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Sticks01"), L"Sticks01_0", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Sticks01"), L"Sticks01_1", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Sticks01"), L"Sticks01_2", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_VillageGuard"), L"VillageGuard_0", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_WoodKnight"), L"WoodKnight_0", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Sapling"), L"Sapling_0", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;
		
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_CRope_RotRock"), L"Rope_RotRock", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_Rot(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	_uint iCreateRotCount = 1;
	_tchar szCloneRotTag[32] = { 0, };
	for (_uint i = 0; i < iCreateRotCount; i++)
	{
		swprintf_s(szCloneRotTag, L"Rot_%d", i);
		if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_Rot"), CUtile::Create_StringAuto(szCloneRotTag), nullptr, &pGameObject)))
			return E_FAIL;
	}
		
	//if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject))) return E_FAIL;
	
	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_LiftRot_Master"), TEXT("LiftRot_Master"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_EnemyWisp"), L"EnemyWisp", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
		return E_FAIL;

	//if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_KenaDamage"), L"Damage", nullptr, &pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
	//	return E_FAIL;
	
	//if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_TESTPLAY, pLayerTag, TEXT("Prototype_GameObject_KenaHit"), L"Hit", nullptr, &pGameObject)))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_TESTPLAY, pGameObject)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_TestPlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	vector<wstring>*	pCanvasProtoTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_PROTOTAG);
	vector<wstring>*	pCanvasCloneTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_CANVAS_CLONETAG);
	vector<string>*		pCanvasNames = pGameInstance->Get_UIString(CUI_Manager::STRKEY_CANVAS_NAME);

	for (_uint i = 0; i < CUI_ClientManager::CANVAS_END; ++i)
	{
		CUI::tagUIDesc tDesc;
		tDesc.fileName = (*pCanvasCloneTags)[i].c_str();

		CUI_Canvas* pCanvas = nullptr;
		if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_Canvas",
			(*pCanvasProtoTags)[i].c_str(), (*pCanvasCloneTags)[i].c_str(), &tDesc, (CGameObject**)&pCanvas)))
			MSG_BOX("Failed To Clone Canvas : UIEditor");

		if (pCanvas != nullptr)
			CUI_ClientManager::GetInstance()->Set_Canvas((CUI_ClientManager::UI_CANVAS)i, pCanvas);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_TestPlay * CLevel_TestPlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_TestPlay*		pInstance = new CLevel_TestPlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_TestPlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_TestPlay::Free()
{
	__super::Free();
}
