#include "stdafx.h"
#include "..\public\E_KenaPulseDot.h"
#include "GameInstance.h"

CE_KenaPulseDot::CE_KenaPulseDot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_KenaPulseDot::CE_KenaPulseDot(const CE_KenaPulseDot & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_KenaPulseDot::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulseDot::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	//m_pVIInstancingBufferCom->Set_PSize(_float2(0.05f, 0.05f));
	return S_OK;
}

void CE_KenaPulseDot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == true)
		m_fLife += fTimeDelta * 0.8f;
	else
		m_fLife = 0.0f;
}

void CE_KenaPulseDot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_KenaPulseDot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaPulseDot * CE_KenaPulseDot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaPulseDot * pInstance = new CE_KenaPulseDot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulseDot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulseDot::Clone(void * pArg)
{
	CE_KenaPulseDot * pInstance = new CE_KenaPulseDot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulseDot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulseDot::Free()
{
	__super::Free();
}
