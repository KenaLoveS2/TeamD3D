#include "stdafx.h"
#include "..\public\E_KenaPulse.h"

CE_KenaPulse::CE_KenaPulse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
	// ZeroMemory(&m_eEFfectDesc, sizeof(EFFECTDESC));
}

CE_KenaPulse::CE_KenaPulse(const CE_KenaPulse & rhs)
	:CEffect_Mesh(rhs)
{
	// memcpy(&m_eEFfectDesc, &rhs.m_eEFfectDesc, sizeof(EFFECTDESC));
}

HRESULT CE_KenaPulse::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulse::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (m_pParent != nullptr)
		this->Set_InitMatrix(m_pParent->Get_TransformCom()->Get_WorldMatrix());

	return S_OK;
}

void CE_KenaPulse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_KenaPulse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaPulse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaPulse * CE_KenaPulse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulse Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulse::Clone(void * pArg)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulse Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulse::Free()
{
	__super::Free();
}
