#include "stdafx.h"
#include "..\public\Camera_Shaman.h"
#include "GameInstance.h"
#include "PipeLine.h"

CCamera_Shaman::CCamera_Shaman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

CCamera_Shaman::CCamera_Shaman(const CCamera_Shaman& rhs)
	: CCamera(rhs)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CCamera_Shaman::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Shaman::Initialize(void* pArg)
{
	CCamera::CAMERADESC CameraDesc;
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
		CameraDesc.TransformDesc.fSpeedPerSec = 3.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}

	if (FAILED(__super::Initialize(&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Shaman::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CCamera_Shaman::Tick(_float TimeDelta)
{
	switch (m_eState)
	{
	case FIND_SHAMAN:
	{
		Calc_TargetPos();
		m_pTransformCom->Chase(m_vTargetPos, TimeDelta, 0.1f, true);

		if (m_pTransformCom->IsClosed_XYZ(m_vTargetPos))
		{
			m_pTransformCom->Set_Position(m_vTargetPos);
			m_eState = WAIT_SHAMAN_ACTTION;
		}
		break;
	}
	case WAIT_SHAMAN_ACTTION:
	{
		Calc_TargetPos();
		m_pTransformCom->Set_Position(m_vTargetPos);
		
		break;
	}
	}

	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
}

void CCamera_Shaman::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);
}

HRESULT CCamera_Shaman::Render()
{
	return S_OK;
}

void CCamera_Shaman::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
}

CCamera_Shaman* CCamera_Shaman::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Shaman*	pInstance = new CCamera_Shaman(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Shaman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Shaman::Clone(void* pArg)
{
	CCamera_Shaman*	pInstance = new CCamera_Shaman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Shaman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Shaman::Free()
{
	CCamera::Free();
}

void CCamera_Shaman::Calc_TargetPos()
{	
	XMStoreFloat4(&m_vTargetPos, 
		m_pTargetTransform->Get_State(CTransform::STATE_TRANSLATION) + 
		m_pTargetTransform->Get_State(CTransform::STATE_LOOK) * m_fTargetDistance +
		XMLoadFloat4(&m_vTargetPosOffset));
}

void CCamera_Shaman::Start_Action(CGameObject* pShamanPtr, _float4 vKenaPos)
{
	m_pTransformCom->Set_Position(vKenaPos);
	m_pTarget = pShamanPtr;
	m_pTargetTransform = pShamanPtr->Get_TransformCom();
	m_eState = FIND_SHAMAN;
}

void CCamera_Shaman::Clear()
{	
	m_pTarget = nullptr;
	m_pTargetTransform = nullptr;
	m_fTargetDistance = 1.f;
	m_eState = STATE_END;
}




