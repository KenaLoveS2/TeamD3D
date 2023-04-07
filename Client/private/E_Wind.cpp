#include "stdafx.h"
#include "..\public\E_Wind.h"
#include "GameInstance.h"

CE_Wind::CE_Wind(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Wind::CE_Wind(const CE_Wind & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_Wind::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Wind::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	/* Set Option */
	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(CUtile::Get_RandomFloat(-100.f, 100.f), CUtile::Get_RandomFloat(2.f, 5.f),
		CUtile::Get_RandomFloat(-100.f, 100.f), 1.f));
	m_eEFfectDesc.fFrame[0] = 20.f;
	m_eEFfectDesc.iPassCnt = 6;
	m_eEFfectDesc.vInitPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_eEFfectDesc.vPixedDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	m_pTransformCom->Set_Scaled(_float3(3.f, 1.f, 1.f));
	/* Set Option */
	return S_OK;
}

void CE_Wind::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_eEFfectDesc.fLife += fTimeDelta;
}

void CE_Wind::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos += XMVector3Normalize(m_eEFfectDesc.vPixedDir) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta;

	if (vPos.x > 100.f)
		vPos = m_eEFfectDesc.vInitPos;

	m_pTransformCom->Set_Position(vPos);
}

HRESULT CE_Wind::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

HRESULT CE_Wind::SetUp_Components()
{	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	_int iModelType = rand() % 2;
	switch (iModelType)
	{
	case 0 :
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Wind"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(-90.f));
		break;

	case 1:
		if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_WindLoop"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	return S_OK;
}

HRESULT CE_Wind::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_eEFfectDesc.fLife, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_Wind * CE_Wind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Wind * pInstance = new CE_Wind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Wind Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Wind::Clone(void * pArg)
{
	CE_Wind * pInstance = new CE_Wind(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Wind Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Wind::Free()
{
	__super::Free();
}