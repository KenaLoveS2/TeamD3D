#include "stdafx.h"
#include "..\public\Camera_Player.h"
#include "GameInstance.h"
#include "Kena.h"
#include "UI_MousePointer.h"

CCamera_Player::CCamera_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs)
	: CCamera(rhs)
{
}

_long CCamera_Player::Get_MouseMove(MOUSEMOVESTATE eState)
{
	if (eState == DIMS_X)
		return m_MouseMoveX;
	else if (eState == DIMS_Y)
		return m_MouseMoveY;

	return 0;
}

void CCamera_Player::Set_CamOffset(CAMOFFSET eOffset)
{
	if (eOffset == CCamera_Player::CAMOFFSET_END)
		return;

	if (m_pCurOffset == m_mapCamOffset[eOffset])
		return;
	
	m_pPreOffset = m_pCurOffset;
	m_pPreOffset->vLastPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_pCurOffset = m_mapCamOffset[eOffset];
	m_fCurLerpTime = 0.f;

	if (m_pCurOffset->bPlayerControl == true && eOffset != CCamera_Player::CAMOFFSET_PARRY)
	{
		_matrix   matKena = m_pKenaTransform->Get_WorldMatrix();
		_float3   vScale = m_pKenaTransform->Get_Scaled();
		
		_vector   vLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)) * vScale.z;
		_vector   vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
		_vector   vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
		
		m_pKenaTransform->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pKenaTransform->Set_State(CTransform::STATE_UP, vUp);
		m_pKenaTransform->Set_State(CTransform::STATE_LOOK, vLook);
	}
}

void CCamera_Player::Set_MouseFix(_bool bFix)
{
	m_bMouseFix = bFix;

	CUI_MousePointer* pMouse = static_cast<CUI_MousePointer*>(CGameInstance::GetInstance()->Get_GameObjectPtr(
		g_LEVEL, L"Layer_Canvas", L"Clone_MousePointer"));
	if (pMouse != nullptr)
		pMouse->Set_Active(!m_bMouseFix);
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CCamera_Player::Initialize(void * pArg)
{
	CCamera::CAMERADESC      CameraDesc;
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

	m_mapCamOffset.emplace(CAMOFFSET_DEFAULT		, new CCamOffset(1.2f, 2.f, 0.f, 0.4f, false));
	m_mapCamOffset.emplace(CAMOFFSET_BOSSBATTLE		, new CCamOffset(1.9f, 3.2f, 0.f, 0.4f, false));
	m_mapCamOffset.emplace(CAMOFFSET_GRAB_WARRIOR	, new CCamOffset(1.1f, 0.6f, -0.35f, 1.f, true));
	m_mapCamOffset.emplace(CAMOFFSET_MASK			, new CCamOffset(1.2f, 2.f, 0.f, 0.4f, true));
	m_mapCamOffset.emplace(CAMOFFSET_AIM			, new CCamOffset(1.2f, 0.85f, 0.5f, 0.3f, true));
	m_mapCamOffset.emplace(CAMOFFSET_AIR_AIM		, new CCamOffset(1.2f, 0.85f, 0.5f, 0.3f, true));
	m_mapCamOffset.emplace(CAMOFFSET_INJECTBOW		, new CCamOffset(1.25f, 0.55f, 0.55f, 0.7f, true));
	m_mapCamOffset.emplace(CAMOFFSET_PULSE			, new CCamOffset(1.2f, 1.7f, 0.f, 0.15f, true));
	m_mapCamOffset.emplace(CAMOFFSET_PARRY			, new CCamOffset(1.2f, 0.85f, 0.5f, 0.6f, true));
	m_mapCamOffset.emplace(CAMOFFSET_HEAVYATTACK	, new CCamOffset(1.2f, 1.7f, 0.f, 0.15f, false));

	m_pCurOffset = m_mapCamOffset[CAMOFFSET_DEFAULT];
	m_pPreOffset = m_pCurOffset;

	return S_OK;
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	ImGui::Begin("Camera");
	if(ImGui::Button("XMMatrixIdentity"))
	{
		m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	}
	ImGui::End();

	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
	fTimeDelta /= fTimeRate;

	if (CGameInstance::GetInstance()->Key_Down(DIK_F1))
		m_bMouseFix = !m_bMouseFix;

	if (m_pKena == nullptr || m_pKenaTransform == nullptr)
	{
		m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
		m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Find_Component(L"Com_Transform"));
		Initialize_Position();

		__super::Tick(fTimeDelta);
		return;
	}

	if (m_pKena->Get_State(CKena::STATE_AIM) == true && m_pKena->Get_State(CKena::STATE_INJECTBOW) == false)
	{
		if (m_pKena->Get_State(CKena::STATE_JUMP) == true)
			Set_CamOffset(CCamera_Player::CAMOFFSET_AIR_AIM);
		else
			Set_CamOffset(CCamera_Player::CAMOFFSET_AIM);
	}
	if (m_pKena->Get_State(CKena::STATE_INJECTBOW) == true)
		Set_CamOffset(CCamera_Player::CAMOFFSET_INJECTBOW);
	if (m_pKena->Get_State(CKena::STATE_PULSE) == true)
		Set_CamOffset(CCamera_Player::CAMOFFSET_PULSE);
	if (m_pKena->Get_State(CKena::STATE_PARRY) == true)
		Set_CamOffset(CCamera_Player::CAMOFFSET_PARRY);
	if (m_pKena->Get_State(CKena::STATE_HEAVYATTACK) == true)
		Set_CamOffset(CCamera_Player::CAMOFFSET_HEAVYATTACK);
	if (m_pKena->Get_State(CKena::STATE_GRAB_WARRIOR) == true)
		Set_CamOffset(CCamera_Player::CAMOFFSET_GRAB_WARRIOR);
	if (m_pKena->Get_State(CKena::STATERETURN_END) == true)
	{
		if (m_pKena->Get_State(CKena::STATE_BOSSBATTLE) == true)
			Set_CamOffset(CCamera_Player::CAMOFFSET_BOSSBATTLE);
		else
			Set_CamOffset(CCamera_Player::CAMOFFSET_DEFAULT);
	}

	_vector   vKenaPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector   vKenaRight = m_pKenaTransform->Get_State(CTransform::STATE_RIGHT);

	m_MouseMoveX = 0;
	m_MouseMoveY = 0;
	_float      fBackupAngle = m_fVerticalAngle;

	/* Limit Up&Down Movement */
	_vector   vKenaLook = m_pKenaTransform->Get_State(CTransform::STATE_LOOK);
	m_fVerticalAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	if (isnan(m_fVerticalAngle))
		m_fVerticalAngle = fBackupAngle;

	_float		fCamHeight = 0.f;
	_float		fCamDistance = 0.f;
	_float		fCamRightDist = 0.f;
	_bool		bPlayerControl = m_pCurOffset->bPlayerControl;

	if (m_fCurLerpTime < m_pCurOffset->fLerpDuration)
	{
		_float		fRatio = m_fCurLerpTime / m_pCurOffset->fLerpDuration;
		fCamHeight = CUtile::FloatLerp(m_pPreOffset->fCamHeight, m_pCurOffset->fCamHeight, fRatio);
		fCamDistance = CUtile::FloatLerp(m_pPreOffset->fCamDistance, m_pCurOffset->fCamDistance, fRatio);
		fCamRightDist = CUtile::FloatLerp(m_pPreOffset->fCamRightDist, m_pCurOffset->fCamRightDist, fRatio);

		m_fCurLerpTime += fTimeDelta;
	}
	else
	{
		m_pPreOffset = m_pCurOffset;

		fCamHeight = m_pCurOffset->fCamHeight;
		fCamDistance = m_pCurOffset->fCamDistance;
		fCamRightDist = m_pCurOffset->fCamRightDist;
	}

	vKenaPos = XMVectorSetY(vKenaPos, XMVectorGetY(vKenaPos) + fCamHeight);
	_vector	vCamPos = vKenaPos - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fCamDistance + XMVector3Normalize(vKenaRight) * fCamRightDist;

	if (m_pCurOffset->bPlayerControl == false)
	{
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
				m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVectorSet(0.f, 1.f, 0.f, 0.f), fCamDistance, fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX);
			else
			{
				if (m_LastMoveX < 0)
					m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVectorSet(0.f, 1.f, 0.f, 0.f), fCamDistance, fTimeDelta * m_fCurMouseSensitivityX * -1.f);
				else if (m_LastMoveX > 0)
					m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVectorSet(0.f, 1.f, 0.f, 0.f), fCamDistance, fTimeDelta * m_fCurMouseSensitivityX);
			}
		}
		if (m_MouseMoveY = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
		{
			_bool   bPossible = true;

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

			m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fCamDistance, fTimeDelta * m_MouseMoveY * m_fCurMouseSensitivityY);
		}
		if (m_MouseMoveX == 0 && m_MouseMoveY == 0)
			m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVectorSet(0.f, 1.f, 0.f, 0.f), fCamDistance, 0.f);
	}
	else
	{
		if (m_pCurOffset != m_mapCamOffset[CCamera_Player::CAMOFFSET_PARRY])
		{
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);

			_float3   vScale = m_pTransformCom->Get_Scaled();
			_vector   vRight = XMVector3Normalize(XMVector3TransformNormal(vKenaRight, XMMatrixRotationY(XMConvertToRadians(-2.f)))) * vScale.x;
			_vector   vUp = XMVector3Normalize(m_pKenaTransform->Get_State(CTransform::STATE_UP)) * vScale.y;
			_vector   vLook = XMVector3Normalize(XMVector3TransformNormal(vKenaLook, XMMatrixRotationY(XMConvertToRadians(-2.f)))) * vScale.z;
			_vector   vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
			m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

			_float      fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCamLook), XMVector3Normalize(XMVectorSetY(vCamLook, 0.f)))));
			if (m_fVerticalAngle < XMConvertToRadians(90.f))
				fAngle *= -1.f;
			else if (fabs(m_fVerticalAngle - XMConvertToRadians(90.f)) < EPSILON)
				fAngle = 0.f;

			if (fAngle != 0.f && !isnan(fAngle))
			{
				m_pTransformCom->RotationFromNow(XMVector3Normalize(vRight), fAngle);
				vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			}

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
					m_pKenaTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX);
					m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX);
				}
				else
				{
					if (m_LastMoveX < 0)
					{
						m_pKenaTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * -1.f);
						m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX * -1.f);
					}
					else if (m_LastMoveX > 0)
					{
						m_pKenaTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX);
						m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_MouseMoveX * m_fCurMouseSensitivityX);
					}
				}
				vKenaRight = m_pKenaTransform->Get_State(CTransform::STATE_RIGHT);
				//vShoulderPos = vKenaPos - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fCamDistance + XMVector3Normalize(vKenaRight) * 0.6f;
				vCamPos = vKenaPos - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fCamDistance + XMVector3Normalize(vKenaRight) * fCamRightDist;
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);
			}
			if (m_MouseMoveY = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
			{
				_bool   bPossible = true;

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

				if (m_MouseMoveX == 0)
					m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), fCamDistance, fTimeDelta * m_MouseMoveY * m_fCurMouseSensitivityY);
				else
				{
					vRight = XMVector3Normalize(XMVector3TransformNormal(m_pKenaTransform->Get_State(CTransform::STATE_RIGHT), XMMatrixRotationY(XMConvertToRadians(-2.f)))) * vScale.x;
					m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVector3Normalize(vRight), fCamDistance, fTimeDelta * m_MouseMoveY * m_fCurMouseSensitivityY);
				}
			}
			if (m_MouseMoveX == 0 && m_MouseMoveY == 0)
				m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVectorSet(0.f, 1.f, 0.f, 0.f), fCamDistance, 0.f);
		}
		else
		{
			_float	fRatio = m_fCurLerpTime / m_pCurOffset->fLerpDuration;
			CUtile::Saturate<_float>(fRatio, 0.f, 1.f);
			vCamPos = _float4::Lerp(m_pPreOffset->vLastPos, vCamPos, fRatio);
			_vector   vCamLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vKenaLook, 0.f)), vCamLook)));
			fAngle = XMConvertToDegrees(fAngle);
			if (fAngle < 180.f)
				fAngle *= -1.f;
			else
				fAngle = 180.f - fAngle;

			m_pTransformCom->Orbit(vKenaPos + XMVector3Normalize(vKenaRight) * fCamRightDist, XMVectorSet(0.f, 1.f, 0.f, 0.f), fCamDistance, XMConvertToRadians(fAngle) * fTimeDelta);
		}
	}

	_matrix	matWorld = m_pTransformCom->Get_WorldMatrix();

	if (m_DirShakeList.empty() == false)
	{
		_vector	vShakeAxis = m_DirShakeList.front();

		_float		fAngle = XMVectorGetX(XMVector3Length(vShakeAxis));
		vShakeAxis = XMVector3Normalize(vShakeAxis);

		if (m_bShakeReturn == false)
		{
			m_fShakeRatio += 0.25f;
			CUtile::Saturate<_float>(m_fShakeRatio, 0.f, 1.f);

			vShakeAxis = XMVector3TransformNormal(vShakeAxis, m_pTransformCom->Get_WorldMatrix());

			_matrix	matRotation = XMMatrixRotationAxis(vShakeAxis, CUtile::FloatLerp(0.f, fAngle, m_fShakeRatio));

			matWorld.r[0] = XMVector4Transform(matWorld.r[0], matRotation);
			matWorld.r[1] = XMVector4Transform(matWorld.r[1], matRotation);
			matWorld.r[2] = XMVector4Transform(matWorld.r[2], matRotation);

			if (isnan(XMVectorGetX(matWorld.r[0])) || isnan(XMVectorGetX(matWorld.r[1])) || isnan(XMVectorGetX(matWorld.r[2])))
				return;

			if (m_fShakeRatio >= 1.f)
				m_bShakeReturn = true;
		}
		else
		{
			m_fShakeRatio -= 0.2f;
			CUtile::Saturate<_float>(m_fShakeRatio, 0.f, 1.f);

			vShakeAxis = XMVector3TransformNormal(vShakeAxis, m_pTransformCom->Get_WorldMatrix());

			_matrix	matRotation = XMMatrixRotationAxis(vShakeAxis, CUtile::FloatLerp(0.f, fAngle, m_fShakeRatio));

			matWorld.r[0] = XMVector4Transform(matWorld.r[0], matRotation);
			matWorld.r[1] = XMVector4Transform(matWorld.r[1], matRotation);
			matWorld.r[2] = XMVector4Transform(matWorld.r[2], matRotation);

			if (isnan(XMVectorGetX(matWorld.r[0])) || isnan(XMVectorGetX(matWorld.r[1])) || isnan(XMVectorGetX(matWorld.r[2])))
				return;

			if (m_fShakeRatio <= 0.f)
			{
				m_DirShakeList.pop_front();
				m_bShakeReturn = false;
			}
		}

		if (m_RandomShakeList.empty() == false)
		{
			_vector	vShakeDir = m_RandomShakeList.front();

			_vector	vEye = matWorld.r[3];
			_vector	vAt = vEye + XMVector3Normalize(matWorld.r[2]);

			_vector	vShakeEye = vEye - vShakeDir;
			_vector	vShakeAt = vAt + vShakeDir;

			matWorld.r[3] = vShakeEye;
			_float3		vScale = _float3(XMVectorGetX(XMVector3Length(matWorld.r[0])), XMVectorGetX(XMVector3Length(matWorld.r[1])), XMVectorGetX(XMVector3Length(matWorld.r[2])));

			_vector		vLook = XMVector3Normalize(vShakeAt - matWorld.r[3]) * vScale.z;
			_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
			_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

			if (isnan(XMVectorGetX(vLook)) || isnan(XMVectorGetX(vRight)) || isnan(XMVectorGetX(vUp)))
				return;

			matWorld.r[0] = vRight;
			matWorld.r[1] = vUp;
			matWorld.r[2] = vLook;

			m_RandomShakeList.pop_front();
		}

		CGameInstance::GetInstance()->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, matWorld));
		return;
	}

	if (m_RandomShakeList.empty() == false)
	{
		_vector	vShakeDir = m_RandomShakeList.front();
		
		_vector	vEye = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector	vAt = vEye + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_vector	vShakeEye = vEye - vShakeDir;
		_vector	vShakeAt = vAt + vShakeDir;

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vShakeEye);
		m_pTransformCom->LookAt(vShakeAt);

		m_RandomShakeList.pop_front();
	}
	
		__super::Tick(fTimeDelta);
		//Update_ReflectWorld();
}

void CCamera_Player::Late_Tick(_float fTimeDelta)
{
	if (m_bMouseFix)
	{
		CUtile::SetClientCursorPos(g_hWnd, g_iWinSizeX >> 1, g_iWinSizeY >> 1);
		::SetCursor(NULL);

		CUI_MousePointer* pMouse = static_cast<CUI_MousePointer*>(CGameInstance::GetInstance()->Get_GameObjectPtr(
			g_LEVEL, L"Layer_Canvas", L"Clone_MousePointer"));
		if (pMouse != nullptr)
			pMouse->Set_Active(false);
	}
	else
	{
		::SetCursor(NULL);
		CUI_MousePointer* pMouse = static_cast<CUI_MousePointer*>(CGameInstance::GetInstance()->Get_GameObjectPtr(
			g_LEVEL, L"Layer_Canvas", L"Clone_MousePointer"));
		if (pMouse != nullptr)
			pMouse->Set_Active(true);
	}
	//::SetCursor(::LoadCursor(nullptr, IDC_ARROW));

	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Player::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CCamera_Player::Imgui_RenderProperty()
{
	CAMOFFSET	eOffset = CCamera_Player::CAMOFFSET_END;
	const auto	iter = find_if(m_mapCamOffset.begin(), m_mapCamOffset.end(), [&](const pair<CAMOFFSET, CCamOffset*>& Pair) {
		if (m_pCurOffset == Pair.second)
		{
			eOffset = Pair.first;
			return true;
		}

		return false;
	});

	char	szOffset[16] = "";
	switch (eOffset)
	{
	case CCamera_Player::CAMOFFSET_DEFAULT:
		strcpy_s(szOffset, "DEFAULT");
		break;

	case CCamera_Player::CAMOFFSET_BOSSBATTLE:
		strcpy_s(szOffset, "BOSSBATTLE");
		break;

	case CCamera_Player::CAMOFFSET_GRAB_WARRIOR:
		strcpy_s(szOffset, "GRAB_WARRIOR");
		break;

	case CCamera_Player::CAMOFFSET_MASK:
		strcpy_s(szOffset, "MASK");
		break;

	case CCamera_Player::CAMOFFSET_AIM:
		strcpy_s(szOffset, "AIM");
		break;

	case CCamera_Player::CAMOFFSET_AIR_AIM:
		strcpy_s(szOffset, "AIR_AIM");
		break;

	case CCamera_Player::CAMOFFSET_INJECTBOW:
		strcpy_s(szOffset, "INJECT_BOW");
		break;

	case CCamera_Player::CAMOFFSET_PULSE:
		strcpy_s(szOffset, "PULSE");
		break;

	case CCamera_Player::CAMOFFSET_PARRY:
		strcpy_s(szOffset, "PARRY");
		break;

	case CCamera_Player::CAMOFFSET_HEAVYATTACK:
		strcpy_s(szOffset, "HEAVY_ATTACK");
		break;
	}
	
	ImGui::BulletText("Current Offset : %s", szOffset);
	ImGui::DragFloat("Cam Height", &m_pCurOffset->fCamHeight, 0.01f, 0.f);
	ImGui::DragFloat("Cam Distance", &m_pCurOffset->fCamDistance, 0.01f, 0.f);
	ImGui::DragFloat("Cam Right", &m_pCurOffset->fCamRightDist, 0.01f, 0.f);
	ImGui::DragFloat("Lerp Duration", &m_pCurOffset->fLerpDuration, 0.01f, 0.f);
	ImGui::Text("Player Control");
	ImGui::SameLine();
	if (ImGui::RadioButton("True", m_pCurOffset->bPlayerControl))
		m_pCurOffset->bPlayerControl = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("False", !m_pCurOffset->bPlayerControl))
		m_pCurOffset->bPlayerControl = false;
	ImGui::InputFloat4("Last Position", (_float*)&m_pCurOffset->vLastPos, "%.3f", ImGuiInputTextFlags_ReadOnly);

	char*	pOffsetTag[CAMOFFSET_END] = { "DEFAULT", "BOSS_BATTLE", "GRAB_WARRIOR", "MASK", "AIM", "AIR_AIM", "INJECT_BOW", "PULSE", "PARRY", "HEAVY_ATTACK" };
	static _int	iSelectOffset = -1;
	ImGui::ListBox("Offset", &iSelectOffset, pOffsetTag, (_int)CAMOFFSET_END);

	if (iSelectOffset != -1)
	{
		CAMOFFSET	eOffset = (CAMOFFSET)iSelectOffset;
		ImGui::BulletText("Current Offset : %s", pOffsetTag[iSelectOffset]);
		ImGui::DragFloat("Cam Height ", &m_mapCamOffset[eOffset]->fCamHeight, 0.01f, 0.f);
		ImGui::DragFloat("Cam Distance ", &m_mapCamOffset[eOffset]->fCamDistance, 0.01f, 0.f);
		ImGui::DragFloat("Cam Right ", &m_mapCamOffset[eOffset]->fCamRightDist, 0.01f, 0.f);
		ImGui::DragFloat("Lerp Duration ", &m_mapCamOffset[eOffset]->fLerpDuration, 0.01f, 0.f);
		ImGui::Text("Player Control");
		ImGui::SameLine();
		if (ImGui::RadioButton("True ", m_mapCamOffset[eOffset]->bPlayerControl))
			m_pCurOffset->bPlayerControl = true;
		ImGui::SameLine();
		if (ImGui::RadioButton("False ", !m_mapCamOffset[eOffset]->bPlayerControl))
			m_pCurOffset->bPlayerControl = false;
		ImGui::InputFloat4("Last Position ", (_float*)&m_mapCamOffset[eOffset]->vLastPos, "%.3f", ImGuiInputTextFlags_ReadOnly);
	}
}

void CCamera_Player::Initialize_Position()
{
	_vector   vKenaPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector   vKenaLook = m_pKenaTransform->Get_State(CTransform::STATE_LOOK);

	vKenaPos = XMVectorSetY(vKenaPos, XMVectorGetY(vKenaPos) + m_pCurOffset->fCamHeight);

	m_CameraDesc.vEye = vKenaPos + XMVector3Normalize(vKenaLook) * m_pCurOffset->fCamDistance * -1.f;
	m_CameraDesc.vAt = vKenaPos;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_CameraDesc.vEye);
	m_pTransformCom->LookAt(m_CameraDesc.vAt);
}

void CCamera_Player::Camera_Shake(_float fPower, _uint iCount)
{
	_vector	vRandom;
	
	for (_uint i = 0; i < iCount; ++i)
	{
		vRandom = XMVector3Normalize(CUtile::Get_RandomVector(_float3(-1.f, -1.f, 0.f), _float3(1.f, 1.f, 0.f))) * fPower;

		m_RandomShakeList.push_back(vRandom);
		m_RandomShakeList.push_back(vRandom * -1.f);
	}
}

void CCamera_Player::Camera_Shake(_float4 vDir, _float fAngle)
{
	if (m_DirShakeList.empty() == false)
		return;

	_vector	vAxis = XMVector3Normalize(XMVector3Cross(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMVector3Normalize(vDir)));
	m_DirShakeList.push_back(vAxis * fAngle);
}

void CCamera_Player::TimeSleep(_float fDuration)
{
	CGameInstance::GetInstance()->Set_TimeSleep(L"Timer_60", fDuration);
}

_float4 CCamera_Player::Calculate_CamPosition(CAMOFFSET eOffset, _fvector vTargetPos)
{
	_float4	vResultPos = vTargetPos;

	switch (eOffset)
	{
	case CCamera_Player::CAMOFFSET_DEFAULT:
		break;

	case CCamera_Player::CAMOFFSET_AIM:
		break;

	case CCamera_Player::CAMOFFSET_AIR_AIM:
		break;

	case CCamera_Player::CAMOFFSET_INJECTBOW:
		break;

	case CCamera_Player::CAMOFFSET_PULSE:
		break;

	case CCamera_Player::CAMOFFSET_HEAVYATTACK:
		break;

	default:
		break;
	}

	return vResultPos;
}

//void CCamera_Player::Update_ReflectWorld()
//{
//	/*_vector   vTargetPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
//	_matrix   matWorld = m_pTransformCom->Get_WorldMatrix();
//	_float3   vScale = m_pTransformCom->Get_Scaled();
//	_vector   vEye = matWorld.r[3];
//	_vector   vLook = XMVector3Normalize(vTargetPos - vEye) * vScale.z;
//	_vector   vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
//	_vector   vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;
//	matWorld.r[0] = vRight;	matWorld.r[1] = vUp;	matWorld.r[2] = vLook; matWorld.r[3] = vEye;
//	CGameInstance::GetInstance()->Set_Transform(CPipeLine::D3DTS_REFLECTVIEW, XMMatrixInverse(nullptr,matWorld));*/
//
//	_vector   vCamPos, vCamLook, vTargetPos;
//	_vector   vCamRight;
//
//	vCamPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//	vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
//	vCamRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
//	vTargetPos = vCamPos + vCamLook;
//	_float      fReflectionCamYCoord = -XMVectorGetY(vCamPos) + 2.f * 0.2f;
//	_vector   vReflectionCamPos = XMVectorSet(XMVectorGetX(vCamPos), fReflectionCamYCoord, XMVectorGetZ(vCamPos), 1.f);
//	_float      fReflectionTargetYCoord = -XMVectorGetY(vTargetPos) + 2.f * 0.2f;
//	_vector   vReflectionCamTarget = XMVectorSet(XMVectorGetX(vTargetPos), fReflectionTargetYCoord, XMVectorGetZ(vTargetPos), 1.f);
//	_vector   vForward = vReflectionCamTarget - vReflectionCamPos;
//	_vector   vReflectionCamUp = XMVector3Cross(vCamRight, vForward);
//	CGameInstance::GetInstance()->Set_Transform(CPipeLine::D3DTS_REFLECTVIEW, XMMatrixLookAtLH(vReflectionCamPos, vReflectionCamTarget, vReflectionCamUp));
//}

CCamera_Player * CCamera_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Player*   pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Player::Clone(void * pArg)
{
	CCamera_Player*   pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	for (auto& Pair : m_mapCamOffset)
		Safe_Release(Pair.second);
	m_mapCamOffset.clear();

	__super::Free();
}