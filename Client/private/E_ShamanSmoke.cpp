#include "stdafx.h"
#include "..\public\E_ShamanSmoke.h"
#include "GameInstance.h"

CE_ShamanSmoke::CE_ShamanSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_ShamanSmoke::CE_ShamanSmoke(const CE_ShamanSmoke & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_ShamanSmoke::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanSmoke::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_fInitSpriteCnt = _float2(0.0f, 0.0f);
	return S_OK;
}

void CE_ShamanSmoke::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	m_fDurationTime += fTimeDelta;
	if (m_fDurationTime > 2.f)
	{
		m_eEFfectDesc.bActive = false;
		m_fDurationTime = 0.0f;
	}
}

void CE_ShamanSmoke::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_ShamanSmoke::Render()
{	
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_ShamanSmoke::Imgui_RenderProperty()
{

}

HRESULT CE_ShamanSmoke::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_ShamanSmoke::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

CE_ShamanSmoke * CE_ShamanSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,  const _tchar* pFilePath )
{
	CE_ShamanSmoke * pInstance = new CE_ShamanSmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanSmoke Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_ShamanSmoke::Clone(void * pArg)
{
	CE_ShamanSmoke * pInstance = new CE_ShamanSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanSmoke Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanSmoke::Free()
{
	__super::Free();
}

