#include "stdafx.h"
#include "..\public\CameraForNpc.h"
#include "GameInstance.h"
#include "PipeLine.h"

CCameraForNpc::CCameraForNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCameraForNpc::CCameraForNpc(const CCameraForNpc& rhs)
	: CCamera(rhs)
{
}

HRESULT CCameraForNpc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraForNpc::Initialize(void* pArg)
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

HRESULT CCameraForNpc::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CCameraForNpc::Tick(_float TimeDelta)
{
	if (m_bInitSet)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		m_pPlayerCam = pGameInstance->Find_Camera(L"PLAYER_CAM");
		RELEASE_INSTANCE(CGameInstance)
		m_pTransformCom->Set_WorldMatrix_float4x4(m_pPlayerCam->Get_TransformCom()->Get_WorldMatrixFloat4x4());
		m_bInitSet = false;
	}

	if (m_pTarget != nullptr)
	{
		if (m_iOffsetType == OFFSET_FRONT_LERP)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
				m_vTargetPos = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

			/* For. At */
			_float3 vDir = XMVector3Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)) * m_fOffsetMulLook;
			vDir.y += m_fOffsetY;
			_float3 vLerpLook = _float3::Lerp(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vDir, TimeDelta * m_fLerpTime);
			_float4 vCalculatedLook = _float4(vLerpLook.x, vLerpLook.y, vLerpLook.z, 0.f);
			m_pTransformCom->Set_Look(vCalculatedLook);

			/* For. Pos*/
			_vector vTargetPos = XMVectorSet(m_vTargetPos.x, m_vTargetPos.y + 0.5f, m_vTargetPos.z, 1.f) + m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
			_float3 vLerpPos = _float3::Lerp(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vTargetPos, TimeDelta * m_fLerpTime);
			_float4 vCalculatedCamPos = _float4(vLerpPos.x, vLerpPos.y, vLerpPos.z, 1.f);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCalculatedCamPos);

			pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
			RELEASE_INSTANCE(CGameInstance)
		}
		else if(m_iOffsetType == OFFSET_FRONT)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
				m_vTargetPos = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

			_float3 vDir = XMVector3Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)) * m_fOffsetMulLook;
			vDir.y += m_fOffsetY;
			m_pTransformCom->Set_Look(vDir);
			_vector vTargetPos = XMVectorSet(m_vTargetPos.x, m_vTargetPos.y + 0.5f, m_vTargetPos.z, 1.f) + m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
			pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
			RELEASE_INSTANCE(CGameInstance)
		}
	}
}

void CCameraForNpc::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);
}

HRESULT CCameraForNpc::Render()
{
	return S_OK;
}

void CCameraForNpc::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
}

CCameraForNpc* CCameraForNpc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraForNpc*	pInstance = new CCameraForNpc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCameraForNpc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCameraForNpc::Clone(void* pArg)
{
	CCameraForNpc*	pInstance = new CCameraForNpc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCameraForNpc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraForNpc::Free()
{
	CCamera::Free();
}
