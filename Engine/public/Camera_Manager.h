#pragma once
#include "Base.h"
#include "Camera.h"

BEGIN(Engine)
class ENGINE_DLL CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	map<const _tchar*, class CCamera*> m_Cameras;
	class CCamera* m_pWorkCamera = nullptr;

	map<const _tchar*, class CCamera*> m_LightCameras;
	class CCamera* m_pLightWorkCamera = nullptr;

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT						Add_Camera(const _tchar* pCameraTag, class CCamera* pCamrea, _bool bWorkFlag = false);
	HRESULT						Work_Camera(const _tchar* pCameraTag);
	class CCamera*			Find_Camera(const _tchar* pCameraTag);

	HRESULT						Add_LightCamera(const _tchar* pCameraTag, class CCamera* pCamrea, _bool bWorkFlag = false);
	HRESULT						Work_LightCamera(const _tchar* pCameraTag);
	class CCamera*			Find_LightCamera(const _tchar* pCameraTag);

public:
	void		Scene_Change_Clear();
	_float* Get_CameraNear();
	_float* Get_CameraFar();
	_float* Get_CameraFov();

	map<const _tchar*, class CCamera*>*	Get_CameraContainer() { return &m_Cameras; }

	virtual void Free() override;

	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	HRESULT Render();

	HRESULT Clear();

	class CCamera* Get_WorkCameraPtr() { return m_pWorkCamera; }
	_bool IsWorkCamera(const _tchar* pCameraTag);
};
END