#include "stdafx.h"
#include "..\public\E_WarriorWeaponAcc.h"
#include "GameInstance.h"
#include "BossWarrior.h"

CE_WarriorWeaponAcc::CE_WarriorWeaponAcc(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_WarriorWeaponAcc::CE_WarriorWeaponAcc(const CE_WarriorWeaponAcc & rhs)
	: CEffect_Point_Instancing(rhs)
{
	
}

HRESULT CE_WarriorWeaponAcc::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(__super::Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_WarriorWeaponAcc::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = true;
	return S_OK;
}

HRESULT CE_WarriorWeaponAcc::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_WarriorWeaponAcc::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);
}

void CE_WarriorWeaponAcc::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.3f, 0.5f));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(0.5f, 1.f);

		m_bTurnOnfirst = true;
	}

	if (dynamic_cast<CBossWarrior*>(m_pParent)->Get_MonsterStatusPtr()->Get_HP() < 1.0f && m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_WarriorWeaponAcc::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

HRESULT CE_WarriorWeaponAcc::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_WarriorWeaponAcc::SetUp_Components()
{	
	return S_OK;
}

void CE_WarriorWeaponAcc::Imgui_RenderProperty()
{
}

CE_WarriorWeaponAcc * CE_WarriorWeaponAcc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_WarriorWeaponAcc * pInstance = new CE_WarriorWeaponAcc(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_WarriorWeaponAcc Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_WarriorWeaponAcc::Clone(void * pArg)
{
	CE_WarriorWeaponAcc * pInstance = new CE_WarriorWeaponAcc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_WarriorWeaponAcc Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_WarriorWeaponAcc::Free()
{
	__super::Free();
}
