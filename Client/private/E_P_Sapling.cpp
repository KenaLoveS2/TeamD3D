#include "stdafx.h"
#include "..\public\E_P_Sapling.h"
#include "GameInstance.h"

CE_P_Sapling::CE_P_Sapling(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Sapling::CE_P_Sapling(const CE_P_Sapling & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Sapling::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Sapling::Initialize(void * pArg)
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

void CE_P_Sapling::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_P_Sapling::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_P_Sapling::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_P_Sapling * CE_P_Sapling::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Sapling * pInstance = new CE_P_Sapling(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Sapling Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Sapling::Clone(void * pArg)
{
	CE_P_Sapling * pInstance = new CE_P_Sapling(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Sapling Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Sapling::Free()
{
	__super::Free();
}
