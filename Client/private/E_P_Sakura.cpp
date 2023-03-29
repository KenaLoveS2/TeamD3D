#include "stdafx.h"
#include "..\public\E_P_Sakura.h"
#include "GameInstance.h"
#include "Kena.h"

CE_P_Sakura::CE_P_Sakura(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Sakura::CE_P_Sakura(const CE_P_Sakura & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Sakura::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	//if (FAILED(Load_E_Desc(pFilePath)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Sakura::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = true;
	return S_OK;
}

void CE_P_Sakura::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
}

void CE_P_Sakura::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Sakura::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_P_Sakura * CE_P_Sakura::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Sakura * pInstance = new CE_P_Sakura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Sakura Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Sakura::Clone(void * pArg)
{
	CE_P_Sakura * pInstance = new CE_P_Sakura(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Sakura Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Sakura::Free()
{
	__super::Free();
}
