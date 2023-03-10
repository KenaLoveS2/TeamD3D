#include "stdafx.h"
#include "..\public\E_KenaChargeImpact.h"
#include "GameInstance.h"

CE_KenaChargeImpact::CE_KenaChargeImpact(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaChargeImpact::CE_KenaChargeImpact(const CE_KenaChargeImpact & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaChargeImpact::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaChargeImpact::Initialize(void * pArg)
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

void CE_KenaChargeImpact::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_KenaChargeImpact::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_KenaChargeImpact::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaChargeImpact * CE_KenaChargeImpact::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaChargeImpact * pInstance = new CE_KenaChargeImpact(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaChargeImpact Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaChargeImpact::Clone(void * pArg)
{
	CE_KenaChargeImpact * pInstance = new CE_KenaChargeImpact(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaChargeImpact Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaChargeImpact::Free()
{
	__super::Free();
}
