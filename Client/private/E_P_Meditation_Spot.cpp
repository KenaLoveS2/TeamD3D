#include "stdafx.h"
#include "..\public\E_P_Meditation_Spot.h"
#include "GameInstance.h"

CE_P_Meditation_Spot::CE_P_Meditation_Spot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Meditation_Spot::CE_P_Meditation_Spot(const CE_P_Meditation_Spot & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Meditation_Spot::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Meditation_Spot::Initialize(void * pArg)
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

HRESULT CE_P_Meditation_Spot::Late_Initialize(void* pArg)
{
	m_ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();

	//m_ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_PLANECIRCLE;
	//m_ePointDesc->bRotation = true;
	//Set_ShapePosition();

	//m_ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_BOX;
	//m_ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::tagPointDesc::DIR_Y;

	//Reset();
	return S_OK;
}

void CE_P_Meditation_Spot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// m_fLife += fTimeDelta;
	if (m_bTurnoff)	Update_Particle(fTimeDelta);
}

void CE_P_Meditation_Spot::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.1f, 0.2f));
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
		m_pTransformCom->Set_Position(m_pParent->Get_TransformCom()->Get_Position());

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Meditation_Spot::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Meditation_Spot::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_Meditation_Spot::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

void CE_P_Meditation_Spot::Update_Particle(_float fTimeDelta)
{
	m_fOverInstanceCnt = m_pVIInstancingBufferCom->Get_InstanceNum();

	m_fMinusTime += fTimeDelta;
	if (m_fMinusTime > 1.f)
	{
		m_fOverInstanceCnt--;
		m_pVIInstancingBufferCom->Set_InstanceNum(m_fOverInstanceCnt);

		if (m_fOverInstanceCnt < 1.f)
			Reset();
		m_fMinusTime = 0.0f;
	}
}

void CE_P_Meditation_Spot::Reset()
{
	m_fLife = 0.0f;
	m_bTurnoff = false;

	m_eEFfectDesc.bActive = false;
	m_pVIInstancingBufferCom->Set_InstanceNum(m_iNumInstance);

	Set_ShapePosition();
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.1f, 1.0f);
	m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.1f));
}

void CE_P_Meditation_Spot::TurnOff_Meditation(_bool bTuroff)
{
	m_ePointDesc->fMax.y = 6.f;
	m_bTurnoff = bTuroff;
}

CE_P_Meditation_Spot * CE_P_Meditation_Spot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Meditation_Spot * pInstance = new CE_P_Meditation_Spot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Meditation_Spot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Meditation_Spot::Clone(void * pArg)
{
	CE_P_Meditation_Spot * pInstance = new CE_P_Meditation_Spot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Meditation_Spot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Meditation_Spot::Free()
{
	__super::Free();
}
