#include "stdafx.h"
#include "..\public\E_Chest.h"
#include "GameInstance.h"

CE_Chest::CE_Chest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_Chest::CE_Chest(const CE_Chest & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_Chest::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_Chest::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_Chest::Late_Initialize(void* pArg)
{
	m_bTimer = true;
	return S_OK;
}

void CE_Chest::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	if (m_fShaderBindTime > 1.f)
		Reset();
}

void CE_Chest::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent)	Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_Chest::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CE_Chest::Imgui_RenderProperty()
{
}

HRESULT CE_Chest::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_Chest::SetUp_ShaderResources()
{
	return S_OK;
}

void CE_Chest::Reset()
{
	m_fTimeDelta = 0.0f;
	m_fShaderBindTime = 0.0f;
	m_fHDRValue = 2.f;
	m_bTimer = false;
}

CE_Chest * CE_Chest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Chest * pInstance = new CE_Chest(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Chest Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Chest::Clone(void * pArg)
{
	CE_Chest * pInstance = new CE_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Chest Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Chest::Free()
{
	__super::Free();
}
