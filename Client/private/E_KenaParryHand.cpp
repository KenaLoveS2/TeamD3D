#include "stdafx.h"
#include "..\public\E_KenaParryHand.h"
#include "GameInstance.h"

CE_KenaParryHand::CE_KenaParryHand(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaParryHand::CE_KenaParryHand(const CE_KenaParryHand & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaParryHand::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaParryHand::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_KenaParryHand::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fDurationTime += fTimeDelta;
		if (m_fDurationTime > 0.5f)
		{
			ResetSprite();
			m_eEFfectDesc.bActive = false;
			m_fDurationTime = 0.0f;
		}
	}
}

void CE_KenaParryHand::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaParryHand::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaParryHand::Imgui_RenderProperty()
{
	if (ImGui::Button("active"))
		m_eEFfectDesc.bActive = true;
}

CE_KenaParryHand * CE_KenaParryHand::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaParryHand * pInstance = new CE_KenaParryHand(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaParryHand Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaParryHand::Clone(void * pArg)
{
	CE_KenaParryHand * pInstance = new CE_KenaParryHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaParryHand Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaParryHand::Free()
{
	__super::Free();
}
