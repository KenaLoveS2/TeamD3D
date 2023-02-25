#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Imgui_ShaderEditor.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_UIEditor.h"
#include "Tool_Animation.h"

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
		p_game_instance->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice,m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_UIEditor::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CTool_Animation::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));

	RELEASE_INSTANCE(CGameInstance)
		
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	// Ready_Thread_CloneObject(TEXT(""));

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
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
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	strcpy_s(LightDesc.szLightName, MAX_PATH, "DIRECTIONAL");

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"), TEXT("Clone_Terrain"))))
		return E_FAIL;
	
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
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

