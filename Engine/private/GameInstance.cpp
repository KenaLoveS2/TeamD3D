#include "..\public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Frustum.h"
#include "Imgui_Manager.h"
#include "String_Manager.h"
#include "Camera_Manager.h"
#include "PostFX.h"

IMPLEMENT_SINGLETON(CGameInstance)

_uint CGameInstance::m_iStaticLevelIndex = 0;
const _tchar* CGameInstance::m_pPrototypeTransformTag = TEXT("Prototype_Component_Transform");

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())
	, m_pImgui_Manager(CImgui_Manager::GetInstance())
	, m_pString_Manager(CString_Manager::GetInstance())
	, m_pCamera_Manager(CCamera_Manager::GetInstance())
	, m_pPostFX(CPostFX::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pImgui_Manager);	
	Safe_AddRef(m_pString_Manager);
	Safe_AddRef(m_pCamera_Manager);
	Safe_AddRef(m_pPostFX);
}

_uint CGameInstance::Get_CurLevelIndex()
{
	if (m_pLevel_Manager != nullptr)
		return m_pLevel_Manager->Get_CurrentLevelIndex();

	return 0;
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pInput_Device || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	/* 그래픽 디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWindowMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	m_pImgui_Manager->Ready_Imgui(GraphicDesc.hWnd, *ppDeviceOut, *ppContextOut);

	/* HDR 초기화 */
	if (FAILED(m_pPostFX->Initialize(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	/* 입력 디바이스 초기화. */
	if (FAILED(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd)))
		return E_FAIL;
	
	/* +1개로 예약하는 이유 : 엔진에서 Level_Static을 추가로 제공하기 위해서. */
	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels + 1)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels + 1)))
		return E_FAIL;

	/* 엔진에서 제공하는 스태틱레벨의 인덱스를 저장해준다. */
	/* 클라이언트 개발자가 스태틱 레벨에 컴포넌트 원형을 추가하고싶은 경우에 스태틱레벨인덱스를 
	클랑리언트에 보여주기 위해서. */
	m_iStaticLevelIndex = iNumLevels;

	/* 엔진에서 제공하는 CGameObject를 상속받는 객체들이 기본적으로 CTransform컴포넌트를 기본으로 가지고 있게 만들어주기위해 
	복제할 수 있는 CTransform의 원형객체를 생성한다. */
	/* 실제 이 원형을 복제하는 루틴 CGameObject의 Initialize함수에서 복제를 담당한다. */
	if (FAILED(m_pComponent_Manager->Add_Prototype(m_iStaticLevelIndex, m_pPrototypeTransformTag, CTransform::Create(*ppDeviceOut, *ppContextOut))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Initialize(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Reserve_Manager(GraphicDesc.pSoundFileTag, GraphicDesc.iNumManualSounds)))
		return E_FAIL;

	m_hClientWnd = GraphicDesc.hWnd;
	
	m_pString_Manager->Initalize(iNumLevels);

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pInput_Device || nullptr == m_pLevel_Manager || nullptr == m_pObject_Manager)
		return;
	
	/* 입력장치의 상태를 갱신받아온다. */
	m_pInput_Device->Invalidate_Input_Device();

	m_pImgui_Manager->Tick_Imgui();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pCamera_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pSound_Manager->Tick(fTimeDelta);

	m_pFrustum->Transform_ToWorldSpace();

	m_pObject_Manager->Late_Tick(fTimeDelta);
	m_pCamera_Manager->Late_Tick(fTimeDelta);
	m_pLevel_Manager->Late_Tick(fTimeDelta);

	
}

void CGameInstance::Clear_Level(_uint iLevelIndex, _bool bCamreaClearFlag)
{
	if (nullptr == m_pObject_Manager)		
		return;

	m_pObject_Manager->Clear(iLevelIndex);
	bCamreaClearFlag && m_pCamera_Manager->Clear();
}

HRESULT CGameInstance::Clear_Graphic_Device(const _float4 * pColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	HRESULT			hr = 0;

	hr = m_pGraphic_Device->Clear_BackBuffer_View(*pColor);	
	hr = m_pGraphic_Device->Clear_DepthStencil_View();

	return hr;
}


HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate)
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Update_SwapChain(hWnd, iWinCX, iWinCY, bIsFullScreen, bNeedUpdate)))
		return E_FAIL;

	return S_OK;
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMoveState);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg, CGameObject** ppObj)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clone_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg, ppObj);

}

void CGameInstance::Imgui_ProtoViewer(_uint iLevel, const _tchar*& szSelectedProto)
{
	if (nullptr == m_pObject_Manager)
		return;
	m_pObject_Manager->Imgui_ProtoViewer(iLevel, szSelectedProto);
}

void CGameInstance::Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject)
{
	if (nullptr == m_pObject_Manager)
		return;
	m_pObject_Manager->Imgui_ObjectViewer(iLevel, pSelectedObject);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();
		
	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix_Inverse(eState);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.0f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPosition();	
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

void CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update_Timer(pTimerTag);
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc, class CLight** ppOut)
{
	if (nullptr == m_pLight_Manager) return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc, ppOut);
}

void CGameInstance::Clear()
{
	if (m_pLight_Manager == nullptr) return;
	m_pLight_Manager->Clear();
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pFontTag, pText, vPos, fRadian, vScale, vColor);
}

_bool CGameInstance::isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_LocalSpace(vLocalPos, fRange);
}

ID3D11ShaderResourceView * CGameInstance::Get_DepthTargetSRV()
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"));	
}

_int CGameInstance::Play_Sound(const _tchar * pSoundKey, _float fVolume, _bool bIsBGM, _int iManualChannelIndex)
{
	if (m_pSound_Manager == nullptr) return -1;
	return m_pSound_Manager->Play_Sound(pSoundKey, fVolume, bIsBGM, iManualChannelIndex);
}

void CGameInstance::Stop_Sound(_uint iManualChannelIndex)
{
	if (m_pSound_Manager == nullptr) return;
	m_pSound_Manager->Stop_Sound(iManualChannelIndex);
}

void CGameInstance::Set_Volume(_uint iManualChannelIndex, _float fVolume)
{
	if (m_pSound_Manager == nullptr) return;
	m_pSound_Manager->Set_Volume(iManualChannelIndex, fVolume);
}

void CGameInstance::Set_MasterVolume(_float fVolume)
{
	if (m_pSound_Manager == nullptr) return;
	m_pSound_Manager->Set_MasterVolume(fVolume);
}

void CGameInstance::Set_SoundDesc(const _tchar * pSoundKey, CSound::SOUND_DESC & SoundDesc)
{
	if (m_pSound_Manager == nullptr) return;
	m_pSound_Manager->Set_SoundDesc(pSoundKey, SoundDesc);
}

_bool CGameInstance::Is_StopSound(_uint iManualChannelIndex)
{
	if (m_pSound_Manager == nullptr) return false;
	return m_pSound_Manager->Is_StopSound(iManualChannelIndex);
}

HRESULT CGameInstance::Copy_Sound(_tchar * pOriginSoundKey, _tchar * pCopySoundKeyOut)
{
	if (m_pSound_Manager == nullptr) return E_FAIL;
	return m_pSound_Manager->Copy_Sound(pOriginSoundKey, pCopySoundKeyOut);
}

void CGameInstance::Stop_All()
{
	if (m_pSound_Manager == nullptr) return;
	m_pSound_Manager->Stop_All();
}

void CGameInstance::Render_ImGui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	m_pImgui_Manager->Render_Imgui();
}

void CGameInstance::Render_Update_ImGui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	m_pImgui_Manager->Render_Update_ImGui();
}

void CGameInstance::Add_ImguiObject(CImguiObject* pImguiObject)
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Add_ImguiObject(pImguiObject);
}

void CGameInstance::Clear_ImguiObjects()
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Clear_ImguiObjects();
}

HRESULT CGameInstance::Add_String(_uint iLevelIndex, _tchar * pStr)
{
	if (m_pString_Manager == nullptr) return E_FAIL;
	return m_pString_Manager->Add_String(iLevelIndex, pStr);
}

HRESULT CGameInstance::Add_String(_tchar * pStr)
{
	if (m_pString_Manager == nullptr) return E_FAIL;
	return m_pString_Manager->Add_String(pStr);
}

_tchar* CGameInstance::Find_String(_uint iLevelIndex, _tchar * pStr)
{
	if (m_pString_Manager == nullptr) return nullptr;
	return m_pString_Manager->Find_String(iLevelIndex, pStr);
}

HRESULT CGameInstance::Add_Camera(const _tchar * pCameraTag, CCamera * pCamrea, _bool bWorkFlag)
{
	if (m_pCamera_Manager == nullptr) return E_FAIL;
	return m_pCamera_Manager->Add_Camera(pCameraTag, pCamrea, bWorkFlag);
}

HRESULT CGameInstance::Work_Camera(const _tchar * pCameraTag)
{
	if (m_pCamera_Manager == nullptr) return E_FAIL;
	return m_pCamera_Manager->Work_Camera(pCameraTag);
}

CCamera * CGameInstance::Find_Camera(const _tchar * pCameraTag)
{
	if (m_pCamera_Manager == nullptr) return nullptr;
	return m_pCamera_Manager->Find_Camera(pCameraTag);
}
void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CImgui_Manager::GetInstance()->DestroyInstance();
	CPostFX::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CCamera_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CLight_Manager::GetInstance()->DestroyInstance();
	CFont_Manager::GetInstance()->DestroyInstance();
	CFrustum::GetInstance()->DestroyInstance();
	CTarget_Manager::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();
	CSound_Manager::GetInstance()->DestroyInstance();
	CString_Manager::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pCamera_Manager);	
	Safe_Release(m_pString_Manager);
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pPostFX);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

