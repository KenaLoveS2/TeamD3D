#include "stdafx.h"
#include "..\public\RotBomb.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Kena_State.h"
#include "Bone.h"

CRotBomb::CRotBomb(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CRotBomb::CRotBomb(const CRotBomb & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CRotBomb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRotBomb::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRotBomb::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CRotBomb::Tick(_float fTimeDelta)
{
}

void CRotBomb::Late_Tick(_float fTimeDelta)
{
}

HRESULT CRotBomb::Render()
{
	return S_OK;
}

HRESULT CRotBomb::RenderShadow()
{
	return S_OK;
}

void CRotBomb::Imgui_RenderProperty()
{
}

void CRotBomb::ImGui_ShaderValueProperty()
{
}

void CRotBomb::ImGui_PhysXValueProperty()
{
}

HRESULT CRotBomb::SetUp_Components()
{
	return S_OK;
}

HRESULT CRotBomb::SetUp_ShaderResources()
{
	return S_OK;
}

HRESULT CRotBomb::SetUp_ShadowShaderResources()
{
	return S_OK;
}

_int CRotBomb::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

CRotBomb * CRotBomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRotBomb*	pInstance = new CRotBomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRotBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotBomb::Clone(void * pArg)
{
	CRotBomb*	pInstance = new CRotBomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRotBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotBomb::Free()
{
	__super::Free();
}
