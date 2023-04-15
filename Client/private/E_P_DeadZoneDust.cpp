#include "stdafx.h"
#include "..\public\E_P_DeadZoneDust.h"
#include "GameInstance.h"
#include "BossShaman.h"

CE_P_DeadZoneDust::CE_P_DeadZoneDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_DeadZoneDust::CE_P_DeadZoneDust(const CE_P_DeadZoneDust& rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_DeadZoneDust::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_DeadZoneDust::Initialize(void* pArg)
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

HRESULT CE_P_DeadZoneDust::Late_Initialize(void* pArg)
{
	m_pShaman = (CBossShaman*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Monster", L"BossShaman_0");
	NULL_CHECK_RETURN(m_pShaman, E_FAIL);

	return S_OK;
}

void CE_P_DeadZoneDust::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pShaman == nullptr){
		m_pShaman = (CBossShaman*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Monster", L"BossShaman_0");
	//	m_pTransformCom->Set_Position(m_pShaman->Get_TransformCom()->Get_Position());
	}
	__super::Tick(fTimeDelta);

	if (m_pShaman && m_pShaman->Get_MonsterStatusPtr()->Get_HP() < 1.0f)
	{
		m_bTimer = true;
		m_bDissolve = true;
		m_fLife += fTimeDelta;

		TurnOffSystem(m_fLife, 1.f, fTimeDelta);
	}
}

void CE_P_DeadZoneDust::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		if (!lstrcmp(m_szCloneObjectTag, L"Dust2"))
			Set_Type(CE_P_DeadZoneDust::TYPE_DUST_B);
		else
			Set_Type(CE_P_DeadZoneDust::TYPE_DUST_A);

		// m_pTransformCom->Set_Position(m_pShaman->Get_TransformCom()->Get_Position());
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_DeadZoneDust::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_DeadZoneDust::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_DeadZoneDust::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);

	return S_OK;
}

void CE_P_DeadZoneDust::Set_Type(TYPE eType)
{
	m_eType = eType;
	switch (eType)
	{
	case Client::CE_P_DeadZoneDust::TYPE_DUST_A:
		m_eEFfectDesc.iPassCnt = 12;
		m_eEFfectDesc.vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		m_eEFfectDesc.fFrame[0] = 114.0f;

		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(1.f, 2.f));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 3.f);

		m_pTransformCom->Set_Position(XMVectorSet(10.f, 15.f, 1170.f, 1.f));
		break;

	case Client::CE_P_DeadZoneDust::TYPE_DUST_B:
		//m_eEFfectDesc.iPassCnt = 22;
		m_eEFfectDesc.iPassCnt = 20;
		m_eEFfectDesc.vColor = XMVectorSet(0.25f, 0.25f, 0.25f, 0.7f);
		m_eEFfectDesc.fFrame[0] = 90.0f;

		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.2f, 1.f));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 5.f);
		m_pTransformCom->Set_Position(XMVectorSet(7.f, 14.f, 1150.f, 1.f));
		break;
	}
}

void CE_P_DeadZoneDust::Reset()
{
	m_fLife = 0.0f;
	m_eEFfectDesc.bActive = false;
}

CE_P_DeadZoneDust* CE_P_DeadZoneDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_P_DeadZoneDust* pInstance = new CE_P_DeadZoneDust(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_DeadZoneDust Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CE_P_DeadZoneDust::Clone(void* pArg)
{
	CE_P_DeadZoneDust* pInstance = new CE_P_DeadZoneDust(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_DeadZoneDust Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_DeadZoneDust::Free()
{
	__super::Free();
}
