#include "stdafx.h"
#include "..\public\E_TeleportRot.h"
#include "GameInstance.h"

CE_TeleportRot::CE_TeleportRot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_TeleportRot::CE_TeleportRot(const CE_TeleportRot & rhs)
	: CEffect(rhs)
{

}

HRESULT CE_TeleportRot::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_TeleportRot::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_fInitSpriteCnt = { 0.0f ,0.0f };

	return S_OK;
}

void CE_TeleportRot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

	m_fDurationTime += fTimeDelta;
	if (m_fDurationTime > 0.2f)
	{
		ResetSprite();
		m_eEFfectDesc.bActive = false;
		m_fDurationTime = 0.0f;
	}
}

void CE_TeleportRot::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_TeleportRot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_TeleportRot * CE_TeleportRot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_TeleportRot * pInstance = new CE_TeleportRot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_TeleportRot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_TeleportRot::Clone(void * pArg)
{
	CE_TeleportRot * pInstance = new CE_TeleportRot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_TeleportRot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_TeleportRot::Free()
{
	__super::Free();
}
