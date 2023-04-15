#include "stdafx.h"
#include "..\public\E_P_Rain.h"
#include "GameInstance.h"

CE_P_Rain::CE_P_Rain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Rain::CE_P_Rain(const CE_P_Rain& rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Rain::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Rain::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_Rain::Late_Initialize(void* pArg)
{
	m_ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	
	return S_OK;
}

void CE_P_Rain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	// m_fLife += fTimeDelta;

	m_pTransformCom->Set_Position(CGameInstance::GetInstance()->Get_CamPosition());
}

void CE_P_Rain::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(1.f, 3.f));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 10.f);
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Rain::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Rain::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_Rain::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

void CE_P_Rain::Reset()
{
	m_fLife = 0.0f;
	m_eEFfectDesc.bActive = false;
}

CE_P_Rain* CE_P_Rain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_P_Rain* pInstance = new CE_P_Rain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Rain Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CE_P_Rain::Clone(void* pArg)
{
	CE_P_Rain* pInstance = new CE_P_Rain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Rain Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Rain::Free()
{
	__super::Free();
}
