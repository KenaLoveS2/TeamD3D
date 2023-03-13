#include "stdafx.h"
#include "..\public\Camera_Manager.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{

}

HRESULT CCamera_Manager::Add_Camera(const _tchar * pCameraTag, CCamera * pCamrea, _bool bWorkFlag)
{
	if (nullptr != Find_Camera(pCameraTag))
		return E_FAIL;

	m_Cameras.emplace(pCameraTag, pCamrea);

	bWorkFlag && (m_pWorkCamera = pCamrea);

	return S_OK;
}

HRESULT CCamera_Manager::Work_Camera(const _tchar * pCameraTag)
{	
	auto Pair = find_if(m_Cameras.begin(), m_Cameras.end(), CTag_Finder(pCameraTag));
	if (Pair == m_Cameras.end()) return E_FAIL;

	m_pWorkCamera = Pair->second;

	return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(const _tchar * pCameraTag)
{
	auto Pair = find_if(m_Cameras.begin(), m_Cameras.end(), CTag_Finder(pCameraTag));

	if (Pair == m_Cameras.end())
		return nullptr;

	return Pair->second;
}

_float * CCamera_Manager::Get_CameraNear()
{
	if (m_pWorkCamera == nullptr)		return nullptr;

	return m_pWorkCamera->Get_Near();
}

_float* CCamera_Manager::Get_CameraFar()
{
	if (m_pWorkCamera == nullptr)		return nullptr;

	return m_pWorkCamera->Get_Far();
}

_float * CCamera_Manager::Get_CameraFov()
{
	if (m_pWorkCamera == nullptr)		return nullptr;

	return m_pWorkCamera->Get_Fov();
}

void CCamera_Manager::Free()
{
	Clear();
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
	if (m_pWorkCamera == nullptr) return;

	m_pWorkCamera->Tick(fTimeDelta);
}

void CCamera_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_pWorkCamera == nullptr) return;

	m_pWorkCamera->Late_Tick(fTimeDelta);
}

HRESULT CCamera_Manager::Render()
{
	m_pWorkCamera && m_pWorkCamera->Render();

	return S_OK;
}

HRESULT CCamera_Manager::Clear()
{
	m_pWorkCamera = nullptr;

	for (auto& Pair : m_Cameras)
		Safe_Release(Pair.second);

	m_Cameras.clear();

	return S_OK;
}

_bool CCamera_Manager::IsWorkCamera(const _tchar * pCameraTag)
{	
	return m_pWorkCamera == Find_Camera(pCameraTag);
}

