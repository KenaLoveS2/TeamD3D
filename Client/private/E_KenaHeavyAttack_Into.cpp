#include "stdafx.h"
#include "..\public\E_KenaHeavyAttack_into.h"
#include "GameInstance.h"

CE_KenaHeavyAttack_Into::CE_KenaHeavyAttack_Into(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaHeavyAttack_Into::CE_KenaHeavyAttack_Into(const CE_KenaHeavyAttack_Into & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaHeavyAttack_Into::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaHeavyAttack_Into::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Set Option */
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	Set_Child();

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);
	/* ~Set Option */

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_KenaHeavyAttack_Into::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fTimeDelta += fTimeDelta;
	//	m_eEFfectDesc.vScale *= 1.2f + fTimeDelta;
		for (auto& pChild : m_vecChild)
			pChild->Set_AddScale(fTimeDelta + 1.1f);

		if (m_fTimeDelta > 0.2f)
		{
			m_eEFfectDesc.fWidthFrame = 0.0;
			m_eEFfectDesc.fHeightFrame = 0.0;
	//		m_eEFfectDesc.vScale = _float3(0.5f, 0.5f, 1.f);

			for (auto& pChild : m_vecChild)
				pChild->Set_Scale(_float3(1.f, 1.f, 1.f));

			m_eEFfectDesc.bActive = false;
			m_fTimeDelta = 0.0f;
		}
	}
}

void CE_KenaHeavyAttack_Into::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaHeavyAttack_Into::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaHeavyAttack_Into::Imgui_RenderProperty()
{
	if (ImGui::Button("active"))
		m_eEFfectDesc.bActive = true;
}

void CE_KenaHeavyAttack_Into::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaHeavyAttackInto_P", L"KenaHeavyAttackInto_P"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
}

CE_KenaHeavyAttack_Into * CE_KenaHeavyAttack_Into::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaHeavyAttack_Into * pInstance = new CE_KenaHeavyAttack_Into(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaHeavyAttack_Into Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaHeavyAttack_Into::Clone(void * pArg)
{
	CE_KenaHeavyAttack_Into * pInstance = new CE_KenaHeavyAttack_Into(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaHeavyAttack_Into Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaHeavyAttack_Into::Free()
{
	__super::Free();
}
