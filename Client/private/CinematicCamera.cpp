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

void CCinematicCamera::Tick(_float fTimeDelta)
{
	if (m_bPlay)
	{
		m_fDeltaTime += fTimeDelta;
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 InterPolatePos = _float3(vPos.x, vPos.y, vPos.z);
		_float3 InterPolateLook = _float3(vLook.x, vLook.y, vLook.z);
		Interpolate(m_fDeltaTime, InterPolatePos, InterPolateLook);
		vPos = _float4(InterPolatePos.x, InterPolatePos.y, InterPolatePos.z, 1.f);
		vLook = _float4(InterPolateLook.x, InterPolateLook.y, InterPolateLook.z, 0.f);
		m_pTransformCom->Set_Position(vPos);
		m_pTransformCom->Set_Look(vLook);
	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(fTimeDelta);

		if (pGameInstance->Key_Pressing(DIK_S))
			m_pTransformCom->Go_Backward(fTimeDelta);

		if (pGameInstance->Key_Pressing(DIK_A))
			m_pTransformCom->Go_Left(fTimeDelta);

		if (pGameInstance->Key_Pressing(DIK_D))
			m_pTransformCom->Go_Right(fTimeDelta);

		if (pGameInstance->Get_DIMouseState(DIM_RB) & 0x80)
		{
			long	MouseMove = 0;
			if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
			if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	CCamera::Tick(fTimeDelta);
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
	ImGui::InputFloat("InputTime", &m_fInputTime);
	if(ImGui::Button("Add KeyFrame"))
	{
		CAMERAKEYFRAME keyFrame;
		ZeroMemory(&keyFrame, sizeof(CAMERAKEYFRAME));
		if(m_fInputTime != 0.f)
		{
			keyFrame.fTime = (m_keyframes.size() * m_fInputTime) + 0.5f;
			keyFrame.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			keyFrame.vLookAt = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_keyframes.push_back(keyFrame);
		}
	}

	if (ImGui::Button("Clear") && m_bPlay)
	{
		m_fDeltaTime = 0.f;
		m_keyframes.clear();
	}
		

	ImGui::Checkbox("Play", &m_bPlay);
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

	if (numKeyframes > i + 2)
	{
		// Calculate the parameter t for the interpolation
		float t = 0.f;
		if(m_keyframes[i].fTime - m_keyframes[i - 1].fTime != 0)
			 t = (time - m_keyframes[i - 1].fTime) / (m_keyframes[i].fTime - m_keyframes[i - 1].fTime);
		else
			t = time - m_keyframes[i - 1].fTime;

		// Calculate the interpolated position and look-at direction
		position = CatmullRomInterpolation(m_keyframes[i - 1].vPos, m_keyframes[i].vPos,
			m_keyframes[i + 1].vPos, m_keyframes[i + 2].vPos, t);
		lookAt = CatmullRomInterpolation(m_keyframes[i - 1].vLookAt, m_keyframes[i].vLookAt,
			m_keyframes[i + 1].vLookAt, m_keyframes[i + 2].vLookAt, t);
	}
}

XMFLOAT3 CCinematicCamera::CatmullRomInterpolation(_float3 p0, _float3 p1, _float3 p2, _float3 p3, float t)
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
