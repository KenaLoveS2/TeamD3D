#include "stdafx.h"
#include "..\public\E_KenaHeavyAttack_end.h"
#include "GameInstance.h"

CE_KenaHeavyAttack_end::CE_KenaHeavyAttack_end(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaHeavyAttack_end::CE_KenaHeavyAttack_end(const CE_KenaHeavyAttack_end & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaHeavyAttack_end::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaHeavyAttack_end::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = true;
	/* Set Option */
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	Set_Child();
	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);
	/* ~Set Option */
	return S_OK;
}

void CE_KenaHeavyAttack_end::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);
}

void CE_KenaHeavyAttack_end::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaHeavyAttack_end::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaHeavyAttack_end::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaHeavyAttackEnd_P", L"KenaHeavyAttackEnd_P"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
}

CE_KenaHeavyAttack_end * CE_KenaHeavyAttack_end::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaHeavyAttack_end * pInstance = new CE_KenaHeavyAttack_end(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaHeavyAttack_end Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaHeavyAttack_end::Clone(void * pArg)
{
	CE_KenaHeavyAttack_end * pInstance = new CE_KenaHeavyAttack_end(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaHeavyAttack_end Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaHeavyAttack_end::Free()
{
	__super::Free();
}
