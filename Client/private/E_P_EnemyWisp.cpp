#include "stdafx.h"
#include "..\public\E_P_EnemyWisp.h"
#include "GameInstance.h"

CE_P_EnemyWisp::CE_P_EnemyWisp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_EnemyWisp::CE_P_EnemyWisp(const CE_P_EnemyWisp & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_EnemyWisp::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_EnemyWisp::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_P_EnemyWisp::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
	m_fLifeTime += fTimeDelta;
}

void CE_P_EnemyWisp::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_EnemyWisp::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_EnemyWisp::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fLife", &m_fLifeTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_P_EnemyWisp * CE_P_EnemyWisp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_EnemyWisp * pInstance = new CE_P_EnemyWisp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_EnemyWisp Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_EnemyWisp::Clone(void * pArg)
{
	CE_P_EnemyWisp * pInstance = new CE_P_EnemyWisp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_EnemyWisp Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_EnemyWisp::Free()
{
	__super::Free();
}
