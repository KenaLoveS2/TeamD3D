#include "stdafx.h"
#include "..\public\E_P_ShamanTeleport.h"
#include "GameInstance.h"

CE_P_ShamanTeleport::CE_P_ShamanTeleport(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_ShamanTeleport::CE_P_ShamanTeleport(const CE_P_ShamanTeleport & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_ShamanTeleport::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_ShamanTeleport::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.05f, 1.0f);
	return S_OK;
}

void CE_P_ShamanTeleport::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);

	TurnOffSystem(m_fDurationTime, 8.f, fTimeDelta);
}

void CE_P_ShamanTeleport::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_ShamanTeleport::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_P_ShamanTeleport::Imgui_RenderProperty()
{
}

void CE_P_ShamanTeleport::Reset()
{
	m_pVIInstancingBufferCom->Set_ShapePosition();
}

CE_P_ShamanTeleport * CE_P_ShamanTeleport::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_ShamanTeleport * pInstance = new CE_P_ShamanTeleport(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_ShamanTeleport Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_ShamanTeleport::Clone(void * pArg)
{
	CE_P_ShamanTeleport * pInstance = new CE_P_ShamanTeleport(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_ShamanTeleport Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_ShamanTeleport::Free()
{
	__super::Free();
}
