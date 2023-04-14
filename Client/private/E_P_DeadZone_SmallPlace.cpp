#include "stdafx.h"
#include "..\public\E_P_DeadZone_SmallPlace.h"
#include "GameInstance.h"

CE_P_DeadZone_SmallPlace::CE_P_DeadZone_SmallPlace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_DeadZone_SmallPlace::CE_P_DeadZone_SmallPlace(const CE_P_DeadZone_SmallPlace& rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_DeadZone_SmallPlace::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_DeadZone_SmallPlace::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_DeadZone_SmallPlace::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CE_P_DeadZone_SmallPlace::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_P_DeadZone_SmallPlace::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(1.f, 2.f));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 3.f);
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_DeadZone_SmallPlace::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_DeadZone_SmallPlace::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_DeadZone_SmallPlace::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

void CE_P_DeadZone_SmallPlace::Reset()
{
	m_fLife = 0.0f;
	m_eEFfectDesc.bActive = false;
}

CE_P_DeadZone_SmallPlace* CE_P_DeadZone_SmallPlace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_P_DeadZone_SmallPlace* pInstance = new CE_P_DeadZone_SmallPlace(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_DeadZone_SmallPlace Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CE_P_DeadZone_SmallPlace::Clone(void* pArg)
{
	CE_P_DeadZone_SmallPlace* pInstance = new CE_P_DeadZone_SmallPlace(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_DeadZone_SmallPlace Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_DeadZone_SmallPlace::Free()
{
	__super::Free();
}
