#include "stdafx.h"
#include "..\public\E_KenaJump.h"
#include "GameInstance.h"

CE_KenaJump::CE_KenaJump(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaJump::CE_KenaJump(const CE_KenaJump & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaJump::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaJump::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);

		_matrix matrchildworld = XMMatrixIdentity();
		_matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(90.0f));

		_float3	vScale = m_eEFfectDesc.vScale;

		_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y; 
		_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

		matrchildworld.r[0] = XMVector4Transform(vRight, RotationMatrix);
		matrchildworld.r[1] = XMVector4Transform(vUp, RotationMatrix);
		matrchildworld.r[2] = XMVector4Transform(vLook, RotationMatrix);

		pChild->Set_InitMatrix(matrchildworld);
	}

	// m_eEFfectDesc.vScale = _float4(0.4f, 0.4f, 1.f, 1.f);
	m_fDurationTime = 0.35f;
	return S_OK;
}

void CE_KenaJump::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

    	__super::Tick(fTimeDelta);

 	for (auto& pChild : m_vecChild)
		pChild->Set_Active(true);

	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta > m_fDurationTime)
	{
		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0f;
		for (auto& pChild : m_vecChild)
			pChild->ResetSprite();
	}
}

void CE_KenaJump::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return ;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaJump::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaJump * CE_KenaJump::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaJump * pInstance = new CE_KenaJump(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaJump Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaJump::Clone(void * pArg)
{
	CE_KenaJump * pInstance = new CE_KenaJump(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaJump Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaJump::Free()
{
	__super::Free();
}
