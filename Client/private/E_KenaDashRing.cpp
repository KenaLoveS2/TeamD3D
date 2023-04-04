#include "stdafx.h"
#include "..\public\E_KenaDashRing.h"
#include "GameInstance.h"

CE_KenaDashRing::CE_KenaDashRing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaDashRing::CE_KenaDashRing(const CE_KenaDashRing & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaDashRing::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_KenaDashRing::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_fHDRValue = 2.5f;
	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_pTransformCom->Set_Scaled(_float3(2.f, 2.f, 2.f));
	return S_OK;
}

void CE_KenaDashRing::Tick(_float fTimeDelta)
{
// 	if (m_pParent == nullptr)
// 		ToolOption("CE_KenaDashRing");

	if (m_pParent)
		m_eEFfectDesc.bActive = dynamic_cast<CEffect_Base*>(m_pParent)->Get_Active();

	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
}

void CE_KenaDashRing::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaDashRing::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CE_KenaDashRing::Imgui_RenderProperty()
{
}

CE_KenaDashRing * CE_KenaDashRing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaDashRing * pInstance = new CE_KenaDashRing(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaDashRing Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaDashRing::Clone(void * pArg)
{
	CE_KenaDashRing * pInstance = new CE_KenaDashRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaDashRing Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaDashRing::Free()
{
	__super::Free();
}
