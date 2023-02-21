#include "stdafx.h"
#include "..\public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

#include "Imgui_ShaderEditor.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_UIEditor.h"
#include "Imgui_Effect.h"

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
		p_game_instance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));
		p_game_instance->Add_ImguiObject(CImgui_Effect::Create(m_pDevice, m_pContext));

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
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

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
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCamera *pCamera = (CCamera *)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc);
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
	
	pCamera = (CCamera *)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc);
	if (pCamera == nullptr) return E_FAIL;
	if (FAILED(pGameInstance->Add_Camera(TEXT("DEBUG_CAM_2"), pCamera))) return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_LightCamera"), L"LightCamera")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Kena"), L"Kena")))
		return E_FAIL;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Kena_MainOutfit"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_ForkLift"), L"ForkLift_Test")))
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
}