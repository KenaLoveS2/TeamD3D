#include "stdafx.h"
#include "..\public\Camera_Photo.h"
#include "GameInstance.h"
#include "PipeLine.h"

#include "Kena.h"
#include "Rot.h"
#include "Saiya.h"


CCamera_Photo::CCamera_Photo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

CCamera_Photo::CCamera_Photo(const CCamera_Photo& rhs)
	: CCamera(rhs)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CCamera_Photo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Photo::Initialize(void* pArg)
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

	if (FAILED(__super::Initialize(&CameraDesc))) return E_FAIL;
	if (FAILED(Setup_Component())) return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Photo::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CCamera_Photo::Tick(_float TimeDelta)
{
	switch (m_eState)
	{
	case MOVE:
	{	
		if (m_bEyeComplete == false)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_vector vDir = XMVector3Normalize(XMLoadFloat4(&m_vTargetEye) - vPos);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos + vDir * m_fSpeedPerSec * TimeDelta);
			
			if (m_pTransformCom->IsClosed_XYZ(m_vTargetEye))
			{
				m_pTransformCom->Set_Position(m_vTargetEye);
				m_bEyeComplete = true;
			}
		}

		if (m_bAtComplete == false)
		{
			m_vOldAt = m_vOldAt + m_vTurnDir * m_fSpeedPerSec * TimeDelta;
			m_pTransformCom->LookAt(m_vOldAt);
			
			if (XMVectorGetX(XMVector3Length(m_vOldAt - m_vTargetAt)) <= 0.1f)
			{
				m_pTransformCom->LookAt(m_vTargetAt);
				m_bAtComplete = true;
			}
		}

		if (m_bEyeComplete && m_bAtComplete)
		{
			m_eState = WAIT;

			m_pFirstRot->Execute_Photo();
			m_pKena->Execute_Photo();
		}
			
		
		break;
	}
	case WAIT:
	{
		_bool bCompleteKena = m_pKena->Is_PhotoAnimEnd();
		_bool bCompleteRot = m_pFirstRot && m_pFirstRot->Is_PhotoAnimEnd();
		_bool bCompleteSaiya = false;

		if (bCompleteKena && bCompleteRot && bCompleteSaiya)
			m_eState = PHOTO;

		break;
	}
	case PHOTO:
	{

	}
	}

	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
}

void CCamera_Photo::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);
}

HRESULT CCamera_Photo::Render()
{
	return S_OK;
}

void CCamera_Photo::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
}

CCamera_Photo* CCamera_Photo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Photo*	pInstance = new CCamera_Photo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Photo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Photo::Clone(void* pArg)
{
	CCamera_Photo*	pInstance = new CCamera_Photo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Photo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Photo::Free()
{
	CCamera::Free();

	Safe_Release(m_pRendererCom);
}

HRESULT CCamera_Photo::Setup_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	return S_OK;
}

_bool CCamera_Photo::Is_Work()
{
	return m_pGameInstance->Get_WorkCameraPtr() == this;
}

void CCamera_Photo::Execute_Move(_float4 vInitEye, _float4 vInitAt)
{
	// юс╫ц
	m_vTargetAt = CRot::Get_RotUseKenaPos() + _float4(0.f, 0.5f, 0.f, 0.f);
	m_vTargetEye = m_vTargetAt + _float4(5.f, 1.f, 5.f, 0.f);
	

	m_eState = MOVE;
	
	m_pTransformCom->Set_Position(vInitEye);
	m_pTransformCom->LookAt(vInitAt);
	
	m_vTurnDir = XMVector3Normalize(m_vTargetAt - vInitAt);
	m_vOldAt = vInitAt;

	m_bAtComplete = m_bEyeComplete = false;
}


