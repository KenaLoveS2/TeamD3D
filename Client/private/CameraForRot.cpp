#include "stdafx.h"
#include "..\public\CameraForRot.h"
#include "GameInstance.h"
#include "PipeLine.h"

CCameraForRot::CCameraForRot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCameraForRot::CCameraForRot(const CCameraForRot& rhs)
	:CCamera(rhs)
{
}

HRESULT CCameraForRot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraForRot::Initialize(void* pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(-30.f, 30.f, -50.f, 1.f);
		CameraDesc.vAt = _float4(60.f, 0.f, 60.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 500.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}

	if (FAILED(__super::Initialize(&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraForRot::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CCameraForRot::Tick(_float TimeDelta)
{
	if(m_bInitSet)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		m_pPlayerCam = pGameInstance->Find_Camera(L"PLAYER_CAM");
		RELEASE_INSTANCE(CGameInstance)
		m_pTransformCom->Set_WorldMatrix_float4x4(m_pPlayerCam->Get_TransformCom()->Get_WorldMatrixFloat4x4());
		m_bInitSet = false;
	}

	if(m_pTarget != nullptr)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

		if (m_bCloseflag == false)
		{	
			m_vTargetPos = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

			/* For. At */
			_float3 vDir = XMVector3Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vDir.y += 0.4f;
			_float3 vLerpLook = _float3::Lerp(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vDir, TimeDelta);
			_float4 vCalculatedLook = _float4(vLerpLook.x, vLerpLook.y, vLerpLook.z, 0.f);
			m_pTransformCom->Set_Look(vCalculatedLook);

			/* For. Pos*/
			_vector vTargetPos = XMVectorSet(m_vTargetPos.x, m_vTargetPos.y + 0.3f, m_vTargetPos.z, 1.f) + m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) * 0.5f;
			_float3 vLerpPos = _float3::Lerp(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vTargetPos, TimeDelta);
			_float4 vCalculatedCamPos = _float4(vLerpPos.x, vLerpPos.y, vLerpPos.z, 1.f);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCalculatedCamPos);

			if (m_pTransformCom->IsClosed_XYZ(_float4(vTargetPos), 0.1f))
				m_bCloseflag = true;
		}

		pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
		RELEASE_INSTANCE(CGameInstance)
	}
}

void CCameraForRot::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);
}

HRESULT CCameraForRot::Render()
{
	return S_OK;
}

void CCameraForRot::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
}

CCameraForRot* CCameraForRot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraForRot*	pInstance = new CCameraForRot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCameraForRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCameraForRot::Clone(void* pArg)
{
	CCameraForRot*	pInstance = new CCameraForRot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCameraForRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraForRot::Free()
{
	CCamera::Free();
}

void CCameraForRot::Clear()
{
	m_bInitSet = true;
	m_bCloseflag = false;
}