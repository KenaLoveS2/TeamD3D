#include "stdafx.h"
#include "..\public\E_EnemyWispBackground.h"
#include "GameInstance.h"
#include "Effect_Trail.h"

CE_EnemyWispBackground::CE_EnemyWispBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_EnemyWispBackground::CE_EnemyWispBackground(const CE_EnemyWispBackground & rhs)
	:CEffect_Mesh(rhs)
{
	
}

void CE_EnemyWispBackground::Set_InitMatrixScaled(_float3 vScale)
{
	_float4 vRight, vUp, vLook;

	XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[0]) * vScale.x);
	XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])  * vScale.y);
	XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])  *vScale.z);

	memcpy(&m_InitWorldMatrix.m[0][0], &vRight, sizeof vRight);
	memcpy(&m_InitWorldMatrix.m[1][0], &vUp, sizeof vUp);
	memcpy(&m_InitWorldMatrix.m[2][0], &vLook, sizeof vLook);
}

_float3 CE_EnemyWispBackground::Get_InitMatrixScaled()
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[0])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])));
}

HRESULT CE_EnemyWispBackground::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_EnemyWispBackground::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Component */
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_Scaled(_float3(2.f, 1.f, 3.f));
	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));
	return S_OK;
}

void CE_EnemyWispBackground::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == true)
		m_fLife += fTimeDelta * 0.5f;
	else
		m_fLife = 0.0f;

	m_pTransformCom->LookAt(CGameInstance::GetInstance()->Get_CamPosition());
	m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
}

void CE_EnemyWispBackground::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_EnemyWispBackground::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

HRESULT CE_EnemyWispBackground::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_eEFfectDesc.bActive, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fLife, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_EnemyWispBackground * CE_EnemyWispBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_EnemyWispBackground * pInstance = new CE_EnemyWispBackground(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_EnemyWispBackground Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_EnemyWispBackground::Clone(void * pArg)
{
	CE_EnemyWispBackground * pInstance = new CE_EnemyWispBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_EnemyWispBackground Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_EnemyWispBackground::Free()
{
	__super::Free();

}
