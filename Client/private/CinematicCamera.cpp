#include "stdafx.h"
#include "..\public\CinematicCamera.h"
#include "GameInstance.h"
#include "PipeLine.h"

CCinematicCamera::CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& rhs)
	:CCamera(rhs)
{
}

HRESULT CCinematicCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCinematicCamera::Initialize(void* pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(CCamera::Initialize(&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CCinematicCamera::Tick(_float TimeDelta)
{
	CCamera::Tick(TimeDelta);
}

void CCinematicCamera::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);
}

HRESULT CCinematicCamera::Render()
{
	return CCamera::Render();
}

void CCinematicCamera::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
}

void CCinematicCamera::AddKeyFrame(CAMERAKEYFRAME keyFrame)
{
	m_keyframes.push_back(keyFrame);
}

void CCinematicCamera::Interpolate(float time, _float3& position, _float3& lookAt)
{
	int numKeyframes = m_keyframes.size();
	if (numKeyframes < 4) {
		// Not enough keyframes to interpolate, just return the first keyframe
		position = m_keyframes[0].vPos;
		lookAt = m_keyframes[0].vLookAt;
		return;
	}

	// Find the two keyframes that surround the given time
	int i = 1;
	while (i < numKeyframes - 2 && m_keyframes[i].fTime < time) {
		i++;
	}

	// Calculate the parameter t for the interpolation
	float t = (time - m_keyframes[i - 1].fTime) / (m_keyframes[i].fTime - m_keyframes[i - 1].fTime);

	// Calculate the interpolated position and look-at direction
	position = CatmullRomInterpolation(m_keyframes[i - 1].vPos, m_keyframes[i].vPos,
		m_keyframes[i + 1].vPos, m_keyframes[i + 2].vPos, t);
	lookAt = CatmullRomInterpolation(m_keyframes[i - 1].vLookAt, m_keyframes[i].vLookAt,
		m_keyframes[i + 1].vLookAt, m_keyframes[i + 2].vLookAt, t);
}

_float3 CCinematicCamera::CatmullRomInterpolation(_float3 p0, _float3 p1, _float3 p2, _float3 p3, float t)
{
	XMFLOAT3 v;
	v.x = 0.5f * ((2.0f * p1.x) +	(-p0.x + p2.x) * t +	(2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t * t +	(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t * t * t);
	v.y = 0.5f * ((2.0f * p1.y) +	(-p0.y + p2.y) * t +	(2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t * t +	(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t * t * t);
	v.z = 0.5f * ((2.0f * p1.z) +	(-p0.z + p2.z) * t +	(2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t * t +	(-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t * t * t);
	return v;
}

CCinematicCamera* CCinematicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicCamera*		pInstance = new CCinematicCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCinematicCamera::Clone(void* pArg)
{
	CCinematicCamera*		pInstance = new CCinematicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCinematicCamera::Free()
{
	CCamera::Free();
}
