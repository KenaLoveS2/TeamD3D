#include "stdafx.h"
#include "..\public\E_KenaPulseDot.h"
#include "GameInstance.h"

CE_KenaPulseDot::CE_KenaPulseDot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_KenaPulseDot::CE_KenaPulseDot(const CE_KenaPulseDot & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_KenaPulseDot::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulseDot::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	Set_Option();
	return S_OK;
}

void CE_KenaPulseDot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == true)
		m_fLife += fTimeDelta;
	else
		m_fLife = 0.0f;
}

void CE_KenaPulseDot::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaPulseDot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaPulseDot::Set_Option()
{
	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	//CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	//_float3 fMin = _float3(-1.f, -1.f, -1.f);
	//_float3 fMax = _float3(1.f, 1.f, 1.f);

	//m_eEFfectDesc.fFrame[0] = 53.f;
	//m_eEFfectDesc.iPassCnt = 1;
	//m_eEFfectDesc.vColor = XMVectorSet(114.f, 227.f, 255.f, 255.f) / 255.f;

	///* Point Instance Option */
	//m_pVIInstancingBufferCom->Set_Speeds(0.1f);
	//ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_GATHER;
	//ePointDesc->bSpread = true;

	//ePointDesc->fCreateRange = 1.f;
	//ePointDesc->fRange = 1.f;
	//ePointDesc->fTerm = 1.f;
	//ePointDesc->bSetDir = false;

	//m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
	//m_pVIInstancingBufferCom->Set_PSize(_float2(0.3f, 0.3f));
}

CE_KenaPulseDot * CE_KenaPulseDot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaPulseDot * pInstance = new CE_KenaPulseDot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulseDot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulseDot::Clone(void * pArg)
{
	CE_KenaPulseDot * pInstance = new CE_KenaPulseDot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulseDot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulseDot::Free()
{
	__super::Free();
}
