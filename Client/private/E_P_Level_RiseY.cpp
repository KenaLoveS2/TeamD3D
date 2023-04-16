#include "stdafx.h"
#include "..\public\E_P_Level_RiseY.h"
#include "GameInstance.h"

CE_P_Level_RiseY::CE_P_Level_RiseY(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Level_RiseY::CE_P_Level_RiseY(const CE_P_Level_RiseY & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Level_RiseY::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Level_RiseY::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 6.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_fHDRValue = 2.5f;
	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_Level_RiseY::Late_Initialize(void* pArg)
{
	m_ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	return S_OK;
}

void CE_P_Level_RiseY::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bReset)
	{
		m_fLife += fTimeDelta;
		if (m_fLife > 2.f)
		{
			Reset();
			m_fLife = 0.0f;
		}
	}
}

void CE_P_Level_RiseY::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_fHDRValue = 2.f;
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 2.f);
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.2f));
		m_bTurnOnfirst = true;
	}

 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Level_RiseY::Render()
{
	FAILED_CHECK_RETURN(__super::Render(),E_FAIL);

	return S_OK;
}

void CE_P_Level_RiseY::Reset()
{
	m_bReset = false;
	m_bTimer = false;
	m_eEFfectDesc.bActive = false;

	m_fLife = 0.0f;
	m_fDurationTime = 0.0f;
}

CE_P_Level_RiseY * CE_P_Level_RiseY::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Level_RiseY * pInstance = new CE_P_Level_RiseY(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Level_RiseY Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Level_RiseY::Clone(void * pArg)
{
	CE_P_Level_RiseY * pInstance = new CE_P_Level_RiseY(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Level_RiseY Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Level_RiseY::Free()
{
	__super::Free();
}
