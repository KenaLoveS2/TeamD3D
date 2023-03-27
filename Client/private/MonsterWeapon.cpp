#include "stdafx.h"
#include "..\public\MonsterWeapon.h"
#include "GameInstance.h"
#include "Bone.h"

Client::CMonsterWeapon::CMonsterWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

Client::CMonsterWeapon::CMonsterWeapon(const CMonsterWeapon& rhs)
	:CGameObject(rhs)
	, m_SocketMatrix(g_IdentityFloat4x4)
{
	
}

HRESULT Client::CMonsterWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
		
	return S_OK;
}

HRESULT Client::CMonsterWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	/* For.Com_Dissolve_Texture */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Dissolve_Texture"),
		(CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT Client::CMonsterWeapon::Late_Initialize(void* pArg)
{
	return CGameObject::Late_Initialize(pArg);
}

void Client::CMonsterWeapon::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);
}

void Client::CMonsterWeapon::Late_Tick(_float fTimeDelta)
{
	CGameObject::Late_Tick(fTimeDelta);
}

HRESULT Client::CMonsterWeapon::Render()
{
	return CGameObject::Render();
}

HRESULT Client::CMonsterWeapon::RenderShadow()
{
	return CGameObject::RenderShadow();
}

void Client::CMonsterWeapon::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void Client::CMonsterWeapon::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void Client::CMonsterWeapon::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

void Client::CMonsterWeapon::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_WeaponDesc.pSocket);
		Safe_Release(m_WeaponDesc.pTargetTransform);
	}

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDissolveTextureCom);
}
