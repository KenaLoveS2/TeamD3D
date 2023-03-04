#include "stdafx.h"
#include "..\public\E_KenaPulse.h"

CE_KenaPulse::CE_KenaPulse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_KenaPulse::CE_KenaPulse(const CE_KenaPulse & rhs)
	:CEffect_Mesh(rhs)
{
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

	return S_OK;
}

void CE_KenaPulse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_KenaPulse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pOwner != nullptr)
	{
		// Kena == m_pOwner
		_matrix  matParent = m_pOwner->Get_TransformCom()->Get_WorldMatrix();
		_matrix  matScaleSet = XMMatrixIdentity();
		_vector vRight, vUp, vLook;

		memcpy(&vRight, &matParent.r[0], sizeof(_vector));
		memcpy(&vUp, &matParent.r[1], sizeof(_vector));
		memcpy(&vLook, &matParent.r[2], sizeof(_vector));

		memcpy(&matScaleSet.r[0], &XMVector3Normalize(vRight), sizeof(_vector));
		memcpy(&matScaleSet.r[1], &XMVector3Normalize(vUp), sizeof(_vector));
		memcpy(&matScaleSet.r[2], &XMVector3Normalize(vLook), sizeof(_vector));
		memcpy(&matScaleSet.r[3], &matParent.r[3], sizeof(_vector));

		m_WorldWithParentMatrix = m_InitWorldMatrix * matScaleSet;
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldWithParentMatrix));
	}
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
