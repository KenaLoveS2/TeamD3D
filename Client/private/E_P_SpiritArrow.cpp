#include "stdafx.h"
#include "..\public\E_P_SpiritArrow.h"
#include "GameInstance.h"

CE_P_SpiritArrow::CE_P_SpiritArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_SpiritArrow::CE_P_SpiritArrow(const CE_P_SpiritArrow & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_SpiritArrow::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_SpiritArrow::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_eEFfectDesc.bActive = true;
	m_pVIInstancingBufferCom->Set_ShapePosition();
	return S_OK;
}

void CE_P_SpiritArrow::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == true)
		__super::Tick(fTimeDelta);
}

void CE_P_SpiritArrow::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_P_SpiritArrow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_P_SpiritArrow * CE_P_SpiritArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_SpiritArrow * pInstance = new CE_P_SpiritArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_SpiritArrow Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_SpiritArrow::Clone(void * pArg)
{
	CE_P_SpiritArrow * pInstance = new CE_P_SpiritArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_SpiritArrow Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_SpiritArrow::Free()
{
	__super::Free();
}
