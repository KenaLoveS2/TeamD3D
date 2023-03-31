#include "stdafx.h"
#include "..\public\LiftRot_Master.h"
#include "FSMComponent.h"
#include "Kena.h"
#include "Rope_RotRock.h"
#include "LiftRot.h"

CLiftRot_Master::CLiftRot_Master(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CLiftRot_Master::CLiftRot_Master(const CLiftRot_Master& rhs)
	:CGameObject(rhs)
{
}

HRESULT CLiftRot_Master::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CLiftRot_Master::Initialize(void* pArg)
{
	m_pGameInstacne = CGameInstance::GetInstance();

	return S_OK;
}

HRESULT CLiftRot_Master::Late_Initialize(void * pArg)
{	
	_tchar szCloneRotTag[32] = { 0, };

	CLiftRot::DESC Desc;
	Desc.eType = CLiftRot::LIFT;
	Desc.vInitPos = _float4(-50.f, 0.f, -50.f, 1.f);

	for (_uint i = 0; i < LIFT_ROT_COUNT; i++)
	{
		swprintf_s(szCloneRotTag, L"LiftRot_%d", i);
		m_pLiftRotArr[i] = (CLiftRot*)m_pGameInstacne->Clone_GameObject(TEXT("Prototype_GameObject_LiftRot"), CUtile::Create_StringAuto(szCloneRotTag), &Desc);
		assert(m_pLiftRotArr[i] != nullptr && "CLiftRot_Master::Late_Initialize");		
		m_pLiftRotArr[i]->Set_OwnerLiftRotMasterPtr(this);
	}

	return S_OK;
}

void CLiftRot_Master::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pRopeRotRock)
	{
		m_vRopeRotRockPos = m_pRopeRotRock->Get_TransformCom()->Get_Position();		
	}
	
	for (auto &pLiftRot : m_pLiftRotArr)
	{
		pLiftRot->Tick(fTimeDelta);
	}
}

void CLiftRot_Master::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto &pLiftRot : m_pLiftRotArr)
	{
		pLiftRot->Late_Tick(fTimeDelta);
	}
}

CLiftRot_Master* CLiftRot_Master::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLiftRot_Master* pInstance = new CLiftRot_Master(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLiftRot_Master");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLiftRot_Master::Clone(void* pArg)
{
	CLiftRot_Master* pInstance = new CLiftRot_Master(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLiftRot_Master");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiftRot_Master::Free()
{
	__super::Free();

	for (auto& pLiftRot : m_pLiftRotArr)
		Safe_Release(pLiftRot);
}

void CLiftRot_Master::Execute_WakeUp(_float4 vCenterPos, _float3* pCreatePosOffsetArr, _float3* pLiftPosOffsetArr)
{
	_float4 vCreate, vLift;
	for (_uint i = 0; i < LIFT_ROT_COUNT; i++)
	{
		vCreate = vCenterPos + pCreatePosOffsetArr[i];
		vLift = vCenterPos + pLiftPosOffsetArr[i];
		m_pLiftRotArr[i]->Execute_WakeUp(vCreate, vLift);
	}		
}

void CLiftRot_Master::Execute_WakeUp(_fmatrix ParentMatrix, _float3* pCreateLocalPos, _float3* pLiftLocalPos)
{
	_float4 vCreate, vLift;
	for (_uint i = 0; i < LIFT_ROT_COUNT; i++)
	{
		vCreate = XMVectorSetW(XMVector3TransformCoord(pCreateLocalPos[i], ParentMatrix), 1.f);
		vLift = XMVectorSetW(XMVector3TransformCoord(pLiftLocalPos[i], ParentMatrix), 1.f);		
		m_pLiftRotArr[i]->Execute_WakeUp(vCreate, vLift);
	}
}

void CLiftRot_Master::Execute_LiftStart()
{
	for (auto& pLiftRot : m_pLiftRotArr)
		pLiftRot->Execute_LiftStart();
}

void CLiftRot_Master::Execute_LiftMoveStart()
{
	for (auto& pLiftRot : m_pLiftRotArr)
		pLiftRot->Execute_LiftMoveStart();
}

void CLiftRot_Master::Execute_Move(_float4 vCenterPos, _float3 *pOffsetPosArr)
{
	for (_uint i = 0; i < LIFT_ROT_COUNT; i++)
	{
		m_pLiftRotArr[i]->Set_NewPosition(vCenterPos + pOffsetPosArr[i], vCenterPos);
	}	
}

void CLiftRot_Master::Execute_Move(_fmatrix ParentMatrix, _float3* pLocalPosArr)
{
	_float4 vNewPos;
	for (_uint i = 0; i < LIFT_ROT_COUNT; i++)
	{
		vNewPos = XMVectorSetW(XMVector3TransformCoord(pLocalPosArr[i], ParentMatrix), 1.f);		
		m_pLiftRotArr[i]->Set_NewPosition(vNewPos, ParentMatrix.r[3]);
	}
}

void CLiftRot_Master::Execute_LiftMoveEnd()
{
	for (auto& pLiftRot : m_pLiftRotArr)
		pLiftRot->Execute_LiftMoveEnd();
}

_bool CLiftRot_Master::Is_LiftReady()
{
	_bool bRet = true;
	for (auto& pLiftRot : m_pLiftRotArr)
	{
		bRet = pLiftRot->Get_LiftReady();
		if (bRet == false) break;
	}

	return bRet;
}

_bool CLiftRot_Master::Is_LiftEnd()
{
	_bool bRet = true;
	for (auto& pLiftRot : m_pLiftRotArr)
	{
		bRet = pLiftRot->Get_LiftEnd();
		if (bRet == false) break;
	}

	return bRet;
}
