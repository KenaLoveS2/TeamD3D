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
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
#ifdef  FOR_MAPTOOL
	if (pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);
#else
	if (pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);
	if (pGameInstance->Key_Pressing(DIK_UP))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_DOWN))
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pTransformCom->Go_Right(fTimeDelta);
#endif
	if (pGameInstance->Key_Pressing(DIK_SPACE))
		m_pTransformCom->Go_AxisY(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_C))
		m_pTransformCom->Go_AxisNegY(fTimeDelta);

	if (pGameInstance->Key_Pressing(DIK_F4))
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 950.f,1.f));

	if (CGameInstance::GetInstance()->Key_Down(DIK_F1))
		m_bMouseFix = !m_bMouseFix;

	m_pTransformCom->Speed_Boost(pGameInstance->Key_Pressing(DIK_LSHIFT), 5.f);
	m_pTransformCom->Speed_Down(pGameInstance->Key_Pressing(DIK_LCONTROL), 5.f);

	if (!pGameInstance->Key_Pressing(DIK_LSHIFT) && !pGameInstance->Key_Pressing(DIK_LCONTROL))
		m_pTransformCom->Set_Speed(10.f);

	if ((pGameInstance->Get_DIMouseState(DIM_RB) & 0x80) && !m_bAutoTurn)
	{
		long	MouseMove = 0;
		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensitivity);
		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensitivity);
	}

	if (pGameInstance->Key_Down(DIK_DELETE))
		m_bAutoTurn = !m_bAutoTurn;
	if (pGameInstance->Key_Down(DIK_HOME))
		m_fTurnSpeed += 1.f;
	if (pGameInstance->Key_Down(DIK_END))
		m_fTurnSpeed -= 1.f;
	if (pGameInstance->Key_Down(DIK_BACKSPACE))
		m_fTurnSpeed = 20.f;

	if (m_bAutoTurn)
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fTurnSpeed * m_fMouseSensitivity);

	RELEASE_INSTANCE(CGameInstance);

	__super::Tick(fTimeDelta);
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//if (m_bMouseFix)
	//{
	//	CUtile::SetClientCursorPos(g_hWnd, g_iWinSizeX >> 1, g_iWinSizeY >> 1);
	//	::ShowCursor(false);
	//	::SetCursor(NULL);
	//}
	//else
	//{
	//	::ShowCursor(true);
	//	::SetCursor(NULL);
	//}
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
