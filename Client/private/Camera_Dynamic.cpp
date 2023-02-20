#include "stdafx.h"
#include "..\public\Camera_Dynamic.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
	, m_fMouseSensitivity(0.1f)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void * pArg)
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

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	if (GetKeyState('W') & 0x8000)
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (GetKeyState('S') & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (GetKeyState('A') & 0x8000)
		m_pTransformCom->Go_Left(fTimeDelta);

	if (GetKeyState('D') & 0x8000)
		m_pTransformCom->Go_Right(fTimeDelta);

	if (GetKeyState(VK_SPACE) & 0x8000)
		m_pTransformCom->Go_AxisY(fTimeDelta);

	if (GetKeyState('C') & 0x8000)
		m_pTransformCom->Go_AxisNegY(fTimeDelta);

	m_pTransformCom->Speed_Boost(_bool(GetKeyState(VK_LSHIFT) & 0x8000), 5.f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIMouseState(DIM_RB) & 0x80)
	{
		/* 카메라 회전 */
		long	MouseMove = 0;
		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensitivity);
		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensitivity);
	}

	if (pGameInstance->Key_Down(DIK_T))
	{
		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}

	if (true == m_bFix)
	{
		/* 마우스 화면 가운데 고정*/
		POINT pt{ (_long)g_iWinSizeX >> 1, (_long)g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
	}
	RELEASE_INSTANCE(CGameInstance);


	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//_long			MouseMove = 0;

	//if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
	//{
	//	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
	//}

	//if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
	//}

	//RELEASE_INSTANCE(CGameInstance);

	__super::Tick(fTimeDelta);
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::SetUp_Components()
{
	return S_OK;
}


CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();


}
