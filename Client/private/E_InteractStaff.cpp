#include "stdafx.h"
#include "..\public\E_InteractStaff.h"
#include "GameInstance.h"

CE_InteractStaff::CE_InteractStaff(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_InteractStaff::CE_InteractStaff(const CE_InteractStaff & rhs)
	: CEffect(rhs)
{

}

HRESULT CE_InteractStaff::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_InteractStaff::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_fDurationTime = 0.3f;

	Set_Child();
	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	return S_OK;
}

void CE_InteractStaff::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	if(m_eType == CE_InteractStaff::TYPE_KENASTAFF)
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(m_eEFfectDesc.bActive);

		m_eEFfectDesc.vScale *= 1.1f + fTimeDelta;
	}
	else
	{
		m_eEFfectDesc.vScale *= 1.2f + fTimeDelta;
		m_fDurationTime = 0.5f;
	}

	if (m_fTimeDelta > m_fDurationTime)
	{
		m_eEFfectDesc.fWidthFrame = 0.0;
		m_eEFfectDesc.fHeightFrame = 0.0;
		m_eEFfectDesc.vScale = _float3(0.05f, 0.05f, 0.05f);
		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0f;
	}
}

void CE_InteractStaff::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_InteractStaff::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_InteractStaff::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_InteractStaff_P", L"InteractStaff_P"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
}

CE_InteractStaff * CE_InteractStaff::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_InteractStaff * pInstance = new CE_InteractStaff(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_InteractStaff Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_InteractStaff::Clone(void * pArg)
{
	CE_InteractStaff * pInstance = new CE_InteractStaff(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_InteractStaff Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_InteractStaff::Free()
{
	__super::Free();
}
