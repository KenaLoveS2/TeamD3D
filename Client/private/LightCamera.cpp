#include "stdafx.h"
#include "..\Public\LightCamera.h"
#include "GameInstance.h"
#include "PipeLine.h"

CLightCamera::CLightCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCamera(pDevice, pContext)
{
}

CLightCamera::CLightCamera(const CLightCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CLightCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightCamera::Initialize(void * pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else 
	{
		CameraDesc.vEye = _float4(-30.f, 200.f, -50.f, 1.f);
		CameraDesc.vAt = _float4(128.f, 0.f, 128.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fFovy = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 1000.f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}

	if (FAILED(__super::Initialize(&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CLightCamera::Tick(_float TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	if(m_bSwitch)
	{
		pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
		pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
	}

	pGameInstance->Set_Transform(CPipeLine::D3DTS_LIGHTVIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	RELEASE_INSTANCE(CGameInstance)
}

void CLightCamera::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLightCamera::Render()
{
	return S_OK;
}

void CLightCamera::Imgui_RenderProperty()
{
	ImGui::Checkbox("Switching Cam", &m_bSwitch);
	float fAt[3] = { m_CameraDesc.vAt.x,m_CameraDesc.vAt.y, m_CameraDesc.vAt.z};
	ImGui::DragFloat3("At", fAt, 0.1f, -1000.f, 2000.f);
	m_CameraDesc.vAt.x = fAt[0];
	m_CameraDesc.vAt.y = fAt[1];
	m_CameraDesc.vAt.z = fAt[2];
	m_pTransformCom->Imgui_RenderProperty();
}

CLightCamera * CLightCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLightCamera*	pInstance = new CLightCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLightCamera::Clone(void* pArg)
{
	CLightCamera*	pInstance = new CLightCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLightCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightCamera::Free()
{
	__super::Free();
}
