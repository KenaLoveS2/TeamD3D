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
			_float3 vDir = XMVector3Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vDir += m_vLookOffset;
			_float3 vLerpLook = _float3::Lerp(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vDir, TimeDelta * m_fLerpTime);
			_float4 vCalculatedLook = _float4(vLerpLook.x, vLerpLook.y, vLerpLook.z, 0.f);
			m_pTransformCom->Set_Look(vCalculatedLook);

			/* For. Pos*/
			_vector vTargetPos = XMVectorSet(m_vTargetPos.x + m_vOffset.x, m_vTargetPos.y + m_vOffset.y, m_vTargetPos.z + m_vOffset.z, 1.f) + m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) * m_fMulLook;
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

			_float3 vDir = XMVector3Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vDir += m_vLookOffset;
			m_pTransformCom->Set_Look(vDir);
			_vector vTargetPos = XMVectorSet(m_vTargetPos.x + m_vOffset.x, m_vTargetPos.y + m_vOffset.y, m_vTargetPos.z + m_vOffset.z, 1.f) + m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
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
	float fPos[3] = { m_vOffset.x,m_vOffset.y, m_vOffset.z };
	ImGui::DragFloat3("PosOffset", fPos, 0.01f, -100.f, 100.f);
	m_vOffset.x = fPos[0]; m_vOffset.y = fPos[1]; m_vOffset.z = fPos[2];
	float fLook[3] = { m_vLookOffset.x,m_vLookOffset.y ,m_vLookOffset.z };
	ImGui::DragFloat3("LookOffset", fLook, 0.01f, -100.f, 100.f);
	m_vLookOffset.x = fLook[0]; m_vLookOffset.y = fLook[1]; m_vLookOffset.z = fLook[2];
	ImGui::DragFloat("MulLook", &m_fMulLook, 0.01f, 1.f, 10.f);
}

void CCameraForNpc::Set_Target(CGameObject* pTarget, _uint iOffsetType, _float3 vOffset, _float3 vLookOffset, _float fTime, _float fMulLook)
{
	m_bInitSet = true;
	m_pTarget = pTarget;
	m_iOffsetType = iOffsetType;
	m_vOffset = vOffset;
	m_vLookOffset = vLookOffset;
	m_fLerpTime = fTime;
	m_fMulLook = fMulLook;
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
