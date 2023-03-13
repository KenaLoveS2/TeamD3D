#include "stdafx.h"
#include "..\public\E_EnemyWispGround.h"
#include "GameInstance.h"

CE_EnemyWispGround::CE_EnemyWispGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_EnemyWispGround::CE_EnemyWispGround(const CE_EnemyWispGround & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_EnemyWispGround::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_EnemyWispGround::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_eEFfectDesc.vScale = _float4(1.f, 1.f, 1.f, 1.f);
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(90.f));
	return S_OK;
}

void CE_EnemyWispGround::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fDissolveTime = 0.0f;
		return;
	}
	__super::Tick(fTimeDelta);
	m_fDissolveTime += fTimeDelta;
}

void CE_EnemyWispGround::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		m_pTransformCom->Set_Position(m_pParent->Get_WorldMatrix().r[3]);
}

HRESULT CE_EnemyWispGround::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_EnemyWispGround::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_eEFfectDesc.bActive, sizeof(_bool))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_EnemyWispGround * CE_EnemyWispGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_EnemyWispGround * pInstance = new CE_EnemyWispGround(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_EnemyWispGround Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_EnemyWispGround::Clone(void * pArg)
{
	CE_EnemyWispGround * pInstance = new CE_EnemyWispGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_EnemyWispGround Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_EnemyWispGround::Free()
{
	__super::Free();
}
