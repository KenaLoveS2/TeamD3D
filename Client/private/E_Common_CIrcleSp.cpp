#include "stdafx.h"
#include "..\public\E_Common_CircleSp.h"
#include "GameInstance.h"

CE_Common_CircleSp::CE_Common_CircleSp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_Common_CircleSp::CE_Common_CircleSp(const CE_Common_CircleSp & rhs)
	:CEffect(rhs)
{
	
}

HRESULT CE_Common_CircleSp::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(__super::Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_Common_CircleSp::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.fTimeDelta = 0.4f;
	m_eEFfectDesc.vScale = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	return S_OK;
}

HRESULT CE_Common_CircleSp::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_Common_CircleSp::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);

	TurnOffSystem(m_fDurationTime, 0.3f, fTimeDelta);
}

void CE_Common_CircleSp::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_Common_CircleSp::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

HRESULT CE_Common_CircleSp::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Common_CircleSp::SetUp_Components()
{	
	return S_OK;
}

void CE_Common_CircleSp::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
}

CE_Common_CircleSp * CE_Common_CircleSp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Common_CircleSp * pInstance = new CE_Common_CircleSp(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Common_CircleSp Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Common_CircleSp::Clone(void * pArg)
{
	CE_Common_CircleSp * pInstance = new CE_Common_CircleSp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Common_CircleSp Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Common_CircleSp::Free()
{
	__super::Free();
}
