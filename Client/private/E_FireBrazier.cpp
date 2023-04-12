#include "stdafx.h"
#include "..\public\E_FireBrazier.h"
#include "GameInstance.h"

CE_FireBrazier::CE_FireBrazier(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_FireBrazier::CE_FireBrazier(const CE_FireBrazier & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_FireBrazier::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_FireBrazier::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_FireBrazier::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CE_FireBrazier::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CE_FireBrazier::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_Position();
		vPos.y += 3.0f;
		m_pTransformCom->Set_Position(vPos);
	}
}

HRESULT CE_FireBrazier::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	return S_OK;
}

void CE_FireBrazier::Imgui_RenderProperty()
{
}

HRESULT CE_FireBrazier::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_FireBrazier::SetUp_ShaderResources()
{
	return S_OK;
}

void CE_FireBrazier::Reset()
{
	m_eEFfectDesc.bActive = false;
}

CE_FireBrazier * CE_FireBrazier::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_FireBrazier * pInstance = new CE_FireBrazier(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_FireBrazier Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_FireBrazier::Clone(void * pArg)
{
	CE_FireBrazier * pInstance = new CE_FireBrazier(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_FireBrazier Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_FireBrazier::Free()
{
	__super::Free();
}
