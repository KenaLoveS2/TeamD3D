#include "stdafx.h"
#include "..\public\Camera_Player.h"
#include "GameInstance.h"
#include "Kena.h"

CCamera_Player::CCamera_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CCamera_Player::Initialize(void * pArg)
{
	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	if (pArg != nullptr)
		memcpy(&CameraDesc, pArg, sizeof(CCamera::CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(&CameraDesc), E_FAIL);

	return S_OK;
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_F1))
		m_bMouseFix = !m_bMouseFix;
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_LSHIFT))
		m_bAim = true;
	else
		m_bAim = false;

	if (m_pKena == nullptr || m_pKenaTransform == nullptr)
	{
		m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(LEVEL_GAMEPLAY, L"Layer_Player", L"Kena"));
		m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Find_Component(L"Com_Transform"));
		Initialize_Position();

		__super::Tick(fTimeDelta);

		return;
	}

	m_CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;

	_vector	vKenaPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vKenaRight = XMVector3Normalize(m_pKenaTransform->Get_State(CTransform::STATE_RIGHT));
	vKenaPos = XMVectorSetY(vKenaPos, XMVectorGetY(vKenaPos) + m_fCurCamHeight);

	m_MouseMoveX = 0;
	m_MouseMoveY = 0;
	_float		fBackupAngle = m_fVerticalAngle;

	/* Limit Up&Down Movement */
	_vector	vKenaLook = m_pKenaTransform->Get_State(CTransform::STATE_LOOK);
	m_fVerticalAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	if (isnan(m_fVerticalAngle))
		m_fVerticalAngle = fBackupAngle;

	if (m_bAim == false)
	{
		m_fDistanceFromTarget = m_fInitDistance;
		m_fCurCamHeight = m_fInitCamHeight;

		if ((m_MouseMoveX = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_X)) || m_fCurMouseSensitivityX != 0.f)
		{
			if (m_MouseMoveX != 0)
			{
				m_LastMoveX = m_MouseMoveX;

				if (m_fCurMouseSensitivityX < m_fInitMouseSensitivity)
					m_fCurMouseSensitivityX += fTimeDelta * 0.15f;
				else
					m_fCurMouseSensitivityX = m_fInitMouseSensitivity;
			}
			else
			{
				if (m_fCurMouseSensitivityX > 0.f)
					m_fCurMouseSensitivityX -= fTimeDelta * 0.15f;
				else
					m_fCurMouseSensitivityX = 0.f;
			}

			if (m_MouseMoveX != 0)
				m_pTransformCom->Orbit(vKenaPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fDistanceFromTarget, fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX);
			else
			{
				if (m_LastMoveX < 0)
					m_pTransformCom->Orbit(vKenaPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fDistanceFromTarget, fTimeDelta * m_fCurMouseSensitivityX * -1.f);
				else if (m_LastMoveX > 0)
					m_pTransformCom->Orbit(vKenaPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fDistanceFromTarget, fTimeDelta * m_fCurMouseSensitivityX);
			}
		}
		if (m_MouseMoveY = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
		{
			_bool	bPossible = true;

			if (m_fVerticalAngle < XMConvertToRadians(70.f) && m_MouseMoveY < 0)
			{
				bPossible = false;

				if (m_fVerticalAngle < XMConvertToRadians(40.f))
					m_fCurMouseSensitivityY = 0.f;
			}
			if (m_fVerticalAngle > XMConvertToRadians(80.f) && m_MouseMoveY > 0)
			{
				bPossible = false;

				if (m_fVerticalAngle > XMConvertToRadians(120.f))
					m_fCurMouseSensitivityY = 0.f;
			}

			if (bPossible == false)
			{
				if (m_fCurMouseSensitivityY > 0.f)
					m_fCurMouseSensitivityY -= fTimeDelta * 0.05f;
				else
					m_fCurMouseSensitivityY = 0.f;
			}
			else
			{
				if (m_fCurMouseSensitivityY < m_fInitMouseSensitivity)
					m_fCurMouseSensitivityY += fTimeDelta * 0.1f;
				else
					m_fCurMouseSensitivityY = m_fInitMouseSensitivity;
			}

			m_pTransformCom->Orbit(vKenaPos, m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fDistanceFromTarget, fTimeDelta * m_MouseMoveY * m_fCurMouseSensitivityY);
		}
		if (m_MouseMoveX == 0 && m_MouseMoveY == 0)
			m_pTransformCom->Orbit(vKenaPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fDistanceFromTarget, 0.f);
	}
	else
	{
		m_fDistanceFromTarget = 0.7f;
		m_fCurCamHeight = 1.2f;

		if ((m_MouseMoveX = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_X)) || m_fCurMouseSensitivityX != 0.f)
		{
			if (m_MouseMoveX != 0)
			{
				m_LastMoveX = m_MouseMoveX;

				if (m_fCurMouseSensitivityX < m_fInitMouseSensitivity)
					m_fCurMouseSensitivityX += fTimeDelta * 0.15f;
				else
					m_fCurMouseSensitivityX = m_fInitMouseSensitivity;
			}
			else
			{
				if (m_fCurMouseSensitivityX > 0.f)
					m_fCurMouseSensitivityX -= fTimeDelta * 0.15f;
				else
					m_fCurMouseSensitivityX = 0.f;
			}

			if (m_MouseMoveX != 0)
			{
				//m_pKenaTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * m_pKenaTransform->Get_TransformDesc().fRotationPerSec);
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * m_pKenaTransform->Get_TransformDesc().fRotationPerSec);
			}
			else
			{
				if (m_LastMoveX < 0)
				{
					//m_pKenaTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * m_pKenaTransform->Get_TransformDesc().fRotationPerSec * -1.f);
					m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * m_pKenaTransform->Get_TransformDesc().fRotationPerSec * -1.f);
				}
				else if (m_LastMoveX > 0)
				{
					//m_pKenaTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * m_pKenaTransform->Get_TransformDesc().fRotationPerSec);
					m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * m_pKenaTransform->Get_TransformDesc().fRotationPerSec);
				}
			}
			
		}
		if (m_MouseMoveY = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
		{

		}
	}

	__super::Tick(fTimeDelta);
}

void CCamera_Player::Late_Tick(_float fTimeDelta)
{
	if (m_bMouseFix)
	{
		CUtile::SetClientCursorPos(g_hWnd, g_iWinSizeX >> 1, g_iWinSizeY >> 1);

		::SetCursor(NULL);
	}
	else
		::SetCursor(::LoadCursor(nullptr, IDC_ARROW));

	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Player::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CCamera_Player::Initialize_Position()
{
	_vector	vKenaPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vKenaLook = m_pKenaTransform->Get_State(CTransform::STATE_LOOK);

	vKenaPos = XMVectorSetY(vKenaPos, XMVectorGetY(vKenaPos) + 2.f);

	m_CameraDesc.vEye = vKenaPos + XMVector3Normalize(vKenaLook) * m_fDistanceFromTarget * -1.f;
	m_CameraDesc.vAt = vKenaPos;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_CameraDesc.vEye);
	m_pTransformCom->LookAt(m_CameraDesc.vAt);
}

CCamera_Player * CCamera_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Player*	pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Player::Clone(void * pArg)
{
	CCamera_Player*	pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();
}
