#include "stdafx.h"
#include "..\public\E_P_Chest.h"
#include "GameInstance.h"

CE_P_Chest::CE_P_Chest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Chest::CE_P_Chest(const CE_P_Chest & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Chest::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Chest::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_fHDRValue = 3.f;
	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_Chest::Late_Initialize(void* pArg)
{
	m_ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	m_iNumInstance = m_pVIInstancingBufferCom->Get_InstanceNum();

//	Reset();
	return S_OK;
}

void CE_P_Chest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bTurnState)
		Update_Particle(fTimeDelta);
}

void CE_P_Chest::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.1f, 0.4f));
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent)
		m_pTransformCom->Set_Position(m_pParent->Get_TransformCom()->Get_Position());

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Chest::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Chest::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_Chest::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

void CE_P_Chest::Update_Particle(_float fTimeDelta)
{
	m_fOverInstanceCnt = m_pVIInstancingBufferCom->Get_InstanceNum();

	m_fMinusTime += fTimeDelta;
	if (m_fMinusTime > 1.f)
	{
		m_fOverInstanceCnt -= 10 * (_int)fTimeDelta;
		m_pVIInstancingBufferCom->Set_InstanceNum(m_fOverInstanceCnt);

		if (m_fOverInstanceCnt < 1.f)
			Reset();
		m_fMinusTime = 0.0f;
	}
}

void CE_P_Chest::Reset()
{
	m_bTurnState = false;
	m_fDurationTime = 0.0f;
	m_fMinusTime = 0.0f;
	m_fOverInstanceCnt = 0;

	if(m_bTurnOnfirst)
	{
		m_eEFfectDesc.bActive = false;
		m_pVIInstancingBufferCom->Set_InstanceNum(m_iNumInstance);
	}

	Set_ShapePosition();
	m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 5.f);
	m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.1f, 0.4f));
}

CE_P_Chest * CE_P_Chest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Chest * pInstance = new CE_P_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Chest Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Chest::Clone(void * pArg)
{
	CE_P_Chest * pInstance = new CE_P_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Chest Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Chest::Free()
{
	__super::Free();
}
