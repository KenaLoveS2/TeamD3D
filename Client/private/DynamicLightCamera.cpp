#include "stdafx.h"
#include "..\Public\DynamicLightCamera.h"
#include "GameInstance.h"
#include "PipeLine.h"

CDynamicLightCamera::CDynamicLightCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCamera(pDevice, pContext)
{
}

CDynamicLightCamera::CDynamicLightCamera(const CDynamicLightCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CDynamicLightCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamicLightCamera::Initialize(void * pArg)
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

	m_vPivotPos = _float4(-10.f, 10.f, -10.f, 1.f);

	return S_OK;
}

HRESULT CDynamicLightCamera::Late_Initialize(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_pKena = pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	RELEASE_INSTANCE(CGameInstance)
	return S_OK;
}

void CDynamicLightCamera::Tick(_float TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_vKenaPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vPos =	_float4(m_vPivotPos.x + m_vKenaPos.x, m_vPivotPos.y + m_vKenaPos.y, m_vPivotPos.z + m_vKenaPos.z, 1.f);
	m_pTransformCom->LookAt(m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	pGameInstance->Set_Transform(CPipeLine::D3DTS_DYNAMICLIGHTVEIW, m_pTransformCom->Get_WorldMatrix_Inverse());
	RELEASE_INSTANCE(CGameInstance)
}

void CDynamicLightCamera::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CDynamicLightCamera::Render()
{
	return S_OK;
}

void CDynamicLightCamera::Imgui_RenderProperty()
{
	float fAt[3] = { m_vPivotPos.x,m_vPivotPos.y,m_vPivotPos.z };
	ImGui::DragFloat3("At", fAt, 0.1f, -1000.f, 1000.f);
	m_vPivotPos.x = fAt[0];
	m_vPivotPos.y = fAt[1];
	m_vPivotPos.z = fAt[2];
}

CDynamicLightCamera * CDynamicLightCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDynamicLightCamera*	pInstance = new CDynamicLightCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDynamicLightCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDynamicLightCamera::Clone(void* pArg)
{
	CDynamicLightCamera*	pInstance = new CDynamicLightCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDynamicLightCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicLightCamera::Free()
{
	__super::Free();
}
