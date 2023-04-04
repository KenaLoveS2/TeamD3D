#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Imgui_ShaderEditor.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_UIEditor.h"
#include "Tool_Animation.h"
#include "Tool_Settings.h"
#include "ImGui_PhysX.h"
#include "ImGui_Monster.h"
#include "ImGui_Rot.h"
#include "UI_ClientManager.h"
#include "UI.h"
#include "Imgui_MapEditor.h"
#include "Imgui_Effect.h"
#include "CinematicCamera.h"
#include "Imgui_TerrainEditor.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)
		p_game_instance->Clear_ImguiObjects();
		p_game_instance->Add_ImguiObject(CTool_Settings::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice,m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_UIEditor::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CTool_Animation::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CImGui_PhysX::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_Effect::Create(m_pDevice, m_pContext));

	RELEASE_INSTANCE(CGameInstance)
		
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment(L"Layer_Environment")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

 	if (FAILED(Ready_Layer_Rot(TEXT("Layer_Rot"))))
 		return E_FAIL;

 	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
 		return E_FAIL;

	// Ready_Thread_CloneObject(TEXT(""));
	
	if (FAILED(p_game_instance->Late_Initialize(LEVEL_GAMEPLAY)))
		return E_FAIL;


	//{
	//	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	//	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	//	BoxDesc.eType = BOX_STATIC;
	//	BoxDesc.pActortag = L"RayTrigger";
	//	BoxDesc.vSize = _float3(2.f, 1.f, 1.f);
	//	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	//	BoxDesc.isTrigger = true;

	//	pPhysX->Create_Box(BoxDesc, nullptr);
	//}

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	//{
	//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	//	_float3 vRayPos = pGameInstance->Get_CamPosition();
	//	_float3 vRayDir = pGameInstance->Get_CamLook_Float3();
	//	_float3 vOut;
	//	CGameObject* pOut = nullptr;
	//	if (pPhysX->Raycast_CollisionExceptTrigger(vRayPos, vRayDir, 10.f, &vOut, &pOut))
	//	// if (pPhysX->Raycast_Collision(vRayPos, vRayDir, 10.f, &vOut, &pOut))
	//	{
	//		int i = 0;
	//	}
	//}

	__super::Tick(fTimeDelta);
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC         LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 1.f);
	LightDesc.vPosition = _float4(100.f, 100.f, 100.f, 1.f);
	LightDesc.szLightName = "DIRECTIONAL";

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		
	CImgui_TerrainEditor::LoadFilterData("0_Terrain.json");
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sky"), TEXT("Clone_Sky"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.02f;
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
	CameraDesc.fNear = 0.02f;
	CameraDesc.fFar = 300.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	
	pCamera = (CCamera *)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), nullptr, &CameraDesc);
	if (pCamera == nullptr) return E_FAIL;
	if (FAILED(pGameInstance->Add_Camera(TEXT("DEBUG_CAM_2"), pCamera))) return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_LightCamera"), L"LightCamera")))
		return E_FAIL;

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

	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(75.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;
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
	CameraDesc.fFar = 300.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_CameraForNpc", L"CameraForNpc", &CameraDesc));
	NULL_CHECK_RETURN(pCamera, E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"NPC_CAM", pCamera), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	//CImgui_MapEditor::Load_MapObjects(LEVEL_GAMEPLAY, "Test_TelePortFlower.json");

	//CImgui_MapEditor::Load_MapObjects(LEVEL_GAMEPLAY, "Chest.json");

	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, L"Prototype_GameObject_HealthFlower_Anim", L"HealthFlower", nullptr, nullptr), E_FAIL);

	CImgui_MapEditor::Load_MapObjects(LEVEL_GAMEPLAY, "Portal_Test.json");

	//FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_PortalPlane"), L"Portal_0"), E_FAIL);

	//FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_PortalPlane"), L"Portal_1"), E_FAIL);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Kena"), L"Kena", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	// CImGui_Monster::Load_MonsterObjects(g_LEVEL, "Test_Chap1.json"); return S_OK;
		
	CGameInstance *pGameInstance = CGameInstance::GetInstance();
	CGameObject *pGameObject = nullptr;

	CImgui_MapEditor::Load_MapObjects(g_LEVEL, "HatCart_.json");
	
	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BranchTosser"), L"BranchTosser_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;
	
	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Moth"), L"Moth_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;
	
	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_RockGolem"), L"RockGolem_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;

	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_RotEater"), L"RotEater_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;
	
	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sticks01"), L"Sticks01_0", nullptr, &pGameObject))) return E_FAIL;
	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;
	
	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_VillageGuard"), L"VillageGuard_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;f

	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_WoodKnight"), L"WoodKnight_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;

	/*if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sapling"), L"Sapling_0", nullptr, &pGameObject))) return E_FAIL;
	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;*/

	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Mage"), L"Mage_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;

	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_ShieldStick"), L"SheildStick_0", nullptr, &pGameObject))) return E_FAIL;
	// if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;

	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BossWarrior"), L"BossWarrior_0", nullptr, &pGameObject))) return E_FAIL;
	
	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BossShaman"), L"BossShaman_0", nullptr, &pGameObject))) return E_FAIL;	 
	// if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_ShamanTrapHex"), L"ShamanTrapHex_0", nullptr, &pGameObject))) return E_FAIL;

	 if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_BossHunter"), L"BossHunter_0", nullptr, &pGameObject))) return E_FAIL;
		
	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_CRope_RotRock"), L"Rope_RotRock", nullptr, &pGameObject))) return E_FAIL;
	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_GAMEPLAY, pGameObject))) return E_FAIL;*/
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_ExplosionGravity"), L"Test", nullptr, &pGameObject), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Swipes_Charged"), L"Swipes_Charged", nullptr, &pGameObject), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Warrior_FireSwipe"), L"Swipe", nullptr, &pGameObject), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_FireBullet"), L"FireBullet", nullptr, &pGameObject), E_FAIL);

	//Prototype_GameObject_RotBombExplosion
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_RotBombExplosion"), L"RotBombExplosion", nullptr, &pGameObject), E_FAIL);

	//Prototype_GameObject_Rot_Bomb
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Rot_Bomb"), L"RotBomb", nullptr, &pGameObject), E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_DashCloud"), L"DashCloud", nullptr, &pGameObject), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_DashCone"), L"DashCone", nullptr, &pGameObject), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_DashRing"), L"DashRing", nullptr, &pGameObject), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_uint APIENTRY Clone_GameObjects(void* pArg)
{
	CLevel_GamePlay::THREAD_DATA* pThreadData = (CLevel_GamePlay::THREAD_DATA*)pArg;

	EnterCriticalSection(pThreadData->pCriticalSection);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	vector<map<const _tchar*, CGameObject*>> &rCopyPrototypes = pGameInstance->Get_CopyPrototypes();
	map<const _tchar*, CGameObject*> &CopyPrototype = rCopyPrototypes[pThreadData->iIndex];

	Safe_Release(pGameInstance);
	LeaveCriticalSection(pThreadData->pCriticalSection);

	CLevel_GamePlay::CloneObjectDesc* p = nullptr;
	
	for (_uint i = 0; i < pThreadData->iNumCopyObjects; i++)
	{
		p = &pThreadData->pCloneObjectDesc[i];

		auto Pair = find_if(CopyPrototype.begin(), CopyPrototype.end(), CTag_Finder(p->pPrototypeTag));
		if (Pair == CopyPrototype.end()) break;

		CGameObject* pPrototype = Pair->second;		 
		CGameObject* pCloneObject = pPrototype->Clone(p->pArg);
		
		_float4 vPos;
		vPos.x = CUtile::Get_RandomFloat(1.f, 5.f);
		vPos.y = 1.f;
		vPos.z = CUtile::Get_RandomFloat(1.f, 5.f);
		vPos.w = 1.f;

		pCloneObject->Set_Position(vPos);
		
		p->pObjectOut = pCloneObject;
	}

	return 0;
}

HRESULT CLevel_GamePlay::Ready_Thread_CloneObject(const _tchar * pLayerTag)
{	
	Initialize_ThreadData();

	Clone_GameObjectsByThreadData();

	Clear_ThreadData();

	return S_OK;
}

HRESULT CLevel_GamePlay::Initialize_ThreadData()
{
	CGameInstance *pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	InitializeCriticalSection(&m_Critical_Section);

	_tchar szTempObject[2][32] = {
		TEXT("Prototype_GameObject_Player") ,
	};
	_tchar szTempClone[32] = { 0, };
	
	
	THREAD_DATA *p = nullptr;
	for (_uint i = 0; i < THREAD_COUNT; i++)
	{
		p = &m_ThreadData[i];
		p->pCriticalSection = &m_Critical_Section;
		p->iIndex = i;		
		p->iNumCopyObjects = 10;
		p->pCloneObjectDesc = new CloneObjectDesc[p->iNumCopyObjects];

		CloneObjectDesc* pDesc = nullptr;		
		for (_uint j = 0; j < p->iNumCopyObjects; j++)
		{
			pDesc = &p->pCloneObjectDesc[j];
			pDesc->pLayerTag = CUtile::Create_String(TEXT("Layer_Player"));			
			pDesc->pPrototypeTag = CUtile::Create_String(TEXT("Prototype_GameObject_Player"));

			swprintf_s(szTempClone, TEXT("Clone_%d"), j);
			pDesc->pCloneTag = CUtile::Create_String(szTempClone);
			pDesc->pArg = nullptr;
		}
	}
	
	for (_uint i = 0; i < THREAD_COUNT; i++)
	{
		m_ThreadData[i].hThread = (HANDLE)_beginthreadex(nullptr, 0, Clone_GameObjects, &m_ThreadData[i], 0, nullptr);
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Clone_GameObjectsByThreadData()
{
	for (_uint i = 0; i < THREAD_COUNT; i++)
	{
		WaitForSingleObject(m_ThreadData[i].hThread, INFINITE);
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < THREAD_COUNT; i++)
	{		
		for (_uint j = 0; j < m_ThreadData[i].iNumCopyObjects; j++)
		{
			CloneObjectDesc *p = &m_ThreadData[i].pCloneObjectDesc[j];
			pGameInstance->Add_ClonedGameObject(LEVEL_GAMEPLAY, p->pLayerTag, p->pCloneTag, p->pObjectOut);
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CLevel_GamePlay::Clear_ThreadData()
{
	for (_uint i = 0; i < THREAD_COUNT; i++)
	{
		WaitForSingleObject(m_ThreadData[i].hThread, INFINITE);
		CloseHandle(m_ThreadData[i].hThread);
		DeleteObject(m_ThreadData[i].hThread);
	}

	DeleteCriticalSection(&m_Critical_Section);
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	/*
	for (_uint i = 0; i < THREAD_COUNT; i++)
	{
	Safe_Delete_Array(m_ThreadData[i].pCloneObjectDesc->pLayerTag);
	Safe_Delete_Array(m_ThreadData[i].pCloneObjectDesc->pPrototypeTag);
	Safe_Delete_Array(m_ThreadData[i].pCloneObjectDesc->pCloneTag);
	Safe_Delete_Array(m_ThreadData[i].pCloneObjectDesc);
	}
	*/
}

HRESULT CLevel_GamePlay::Ready_Layer_Rot(const _tchar* pLayerTag)
{	
	//CImGui_Rot::Load_RotObjects(g_LEVEL, "Test_Chap1.json");
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Rot"), TEXT("Rot_JH")))) return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;

	/*
	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Rot"), CUtile::Create_DummyString(TEXT("TEST_Rot"), i))))
			return E_FAIL;
	}
	*/
	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_LiftRot_Master"), TEXT("LiftRot_Master"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* UIs */
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

	
	/* 3D UI */
	//if (FAILED(pGameInstance->Clone_GameObject(g_LEVEL, L"Layer_3DUI", TEXT("Prototype_GameObject_Effect_Particle_Base"), TEXT("Clone_EffectParticleBase"), "Effect_Haze_For_Test" )))
	//	return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

