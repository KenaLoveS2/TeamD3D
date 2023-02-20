#pragma once
#include "Base.h"

BEGIN(Engine)
class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	map<const _tchar*, class CCamera*> m_Cameras;

	class CCamera* m_pWorkCamera = nullptr;

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Add_Camera(const _tchar* pCameraTag, class CCamera* pCamrea, _bool bWorkFlag = false);
	HRESULT Work_Camera(const _tchar* pCameraTag);
	class CCamera* Find_Camera(const _tchar* pCameraTag);
	
	virtual void Free() override;

	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	HRESULT Render();

	HRESULT Clear();
};
END