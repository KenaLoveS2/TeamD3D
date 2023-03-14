#include "stdafx.h"
#include "..\public\E_KenaDust.h"
#include "GameInstance.h"

CE_KenaDust::CE_KenaDust(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaDust::CE_KenaDust(const CE_KenaDust & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaDust::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaDust::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

void CE_KenaDust::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

 	__super::Tick(fTimeDelta);
}

void CE_KenaDust::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return ;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaDust::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaDust * CE_KenaDust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaDust * pInstance = new CE_KenaDust(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaDust Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaDust::Clone(void * pArg)
{
	CE_KenaDust * pInstance = new CE_KenaDust(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaDust Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaDust::Free()
{
	__super::Free();
}
