#include "stdafx.h"
#include "..\public\E_P_KenaHeavyAttack_end.h"

CE_P_KenaHeavyAttack_end::CE_P_KenaHeavyAttack_end(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_KenaHeavyAttack_end::CE_P_KenaHeavyAttack_end(const CE_P_KenaHeavyAttack_end & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_KenaHeavyAttack_end::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_KenaHeavyAttack_end::Initialize(void * pArg)
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

void CE_P_KenaHeavyAttack_end::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_P_KenaHeavyAttack_end::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_P_KenaHeavyAttack_end::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_P_KenaHeavyAttack_end * CE_P_KenaHeavyAttack_end::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_KenaHeavyAttack_end * pInstance = new CE_P_KenaHeavyAttack_end(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_KenaHeavyAttack_end Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_KenaHeavyAttack_end::Clone(void * pArg)
{
	CE_P_KenaHeavyAttack_end * pInstance = new CE_P_KenaHeavyAttack_end(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_KenaHeavyAttack_end Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_KenaHeavyAttack_end::Free()
{
	__super::Free();
}
