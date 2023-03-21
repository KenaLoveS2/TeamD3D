#include "stdafx.h"
#include "..\public\HatCart.h"
#include "GameInstance.h"

CHatCart::CHatCart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CHatCart::CHatCart(const CHatCart & rhs)
	:CGameObject(rhs)
{
}

HRESULT CHatCart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHatCart::Initialize(void * pArg)
{
	return S_OK;
}

void CHatCart::Tick(_float fTimeDelta)
{
}

void CHatCart::Late_Tick(_float fTimeDelta)
{
}

HRESULT CHatCart::Render()
{
	return S_OK;
}

HRESULT CHatCart::SetUp_Components()
{
	return S_OK;
}

HRESULT CHatCart::SetUp_ShaderResources()
{
	return S_OK;
}

CHatCart * CHatCart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHatCart*		pInstance = new CHatCart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHatCart");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHatCart::Clone(void * pArg)
{
	CHatCart*		pInstance = new CHatCart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHatCart");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHatCart::Free()
{
	__super::Free();
}
