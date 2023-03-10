#include "stdafx.h"
#include "..\public\Level_EffectTest.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"

#include "Imgui_PropertyEditor.h"
#include "Imgui_Effect.h"
#include "Imgui_ShaderEditor.h"
#include "Tool_Settings.h"
#include "Tool_Animation.h"

CLevel_EffectTest::CLevel_EffectTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_EffectTest::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	// tool 
	pGameInstance->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	pGameInstance->Add_ImguiObject(CImgui_Effect::Create(m_pDevice, m_pContext));
	pGameInstance->Add_ImguiObject(CTool_Settings::Create(m_pDevice, m_pContext));
	pGameInstance->Add_ImguiObject(CImgui_ShaderEditor::Create(m_pDevice, m_pContext));
	pGameInstance->Add_ImguiObject(CTool_Animation::Create(m_pDevice, m_pContext));
	// ~tool 

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

// 	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
// 		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Late_Initialize(LEVEL_EFFECT)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CLevel_EffectTest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_EffectTest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CLevel_EffectTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : TestEffect"));

	return S_OK;

}

HRESULT CLevel_EffectTest::Ready_Lights()
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


HRESULT CLevel_EffectTest::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_EFFECT, pLayerTag, TEXT("Prototype_GameObject_LightCamera"), L"LightCamera")))
		return E_FAIL;

// 	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
// 	CameraDesc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
// 	CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
// 	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
// 	CameraDesc.fFovy = XMConvertToRadians(90.0f);
// 	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
// 	CameraDesc.fNear = 0.1f;
// 	CameraDesc.fFar = 1000.f;
// 	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
// 	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
// 
// 	pCamera = dynamic_cast<CCamera*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Camera_Player", L"Camera_Player", &CameraDesc));
// 	NULL_CHECK_RETURN(pCamera, E_FAIL);
// 	FAILED_CHECK_RETURN(pGameInstance->Add_Camera(L"PLAYER_CAM", pCamera), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_EffectTest::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_EFFECT, pLayerTag, TEXT("Prototype_GameObject_Kena"), L"Kena", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_EFFECT, pGameObject)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_EffectTest::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_EFFECT, pLayerTag, TEXT("Prototype_GameObject_EnemyWisp"), L"EnemyWisp", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_EFFECT, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_EFFECT, pLayerTag, TEXT("Prototype_GameObject_KenaDamage"), L"Damage", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_EFFECT, pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_AnimObject(LEVEL_EFFECT, pLayerTag, TEXT("Prototype_GameObject_KenaHit"), L"Hit", nullptr, &pGameObject)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_ShaderValueObject(LEVEL_EFFECT, pGameObject)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_EffectTest * CLevel_EffectTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_EffectTest*		pInstance = new CLevel_EffectTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EffectTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_EffectTest::Free()
{
	__super::Free();
}
