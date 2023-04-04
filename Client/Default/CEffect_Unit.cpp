#include "stdafx.h"
#include "CEffect_Unit.h"
#include "gameinstance.h"

CEffect_Unit::CEffect_Unit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base_S2(pDevice, pContext)
{
}

CEffect_Unit::CEffect_Unit(const CEffect_Unit& rhs)
	: CEffect_Base_S2(rhs)
{
}

HRESULT CEffect_Unit::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Unit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pfileName = (_tchar*)pArg;
		if (FAILED(Load_Data(m_pfileName)))
			return E_FAIL;
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Unit::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Unit::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	for (auto& effect : m_vecEffects)
		effect->Tick(fTimeDelta);
}

void CEffect_Unit::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	for (auto& effect : m_vecEffects)
		effect->Late_Tick(fTimeDelta);
}

HRESULT CEffect_Unit::Render()
{
	return S_OK;
}

void CEffect_Unit::Imgui_RenderProperty()
{
}

HRESULT CEffect_Unit::Save_Data()
{
	return S_OK;
}

HRESULT CEffect_Unit::Load_Data(_tchar* fileName)
{
	return S_OK;
}

void CEffect_Unit::Activate(_float4 vPos)
{
}

void CEffect_Unit::Activate(CGameObject* pTarget)
{
}

void CEffect_Unit::DeActivate()
{
}

CEffect_Unit* CEffect_Unit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Unit* pInstance = new CEffect_Unit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create: CEffect_Unit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Unit::Clone(void* pArg)
{
	CEffect_Unit* pInstance = new CEffect_Unit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CEffect_Unit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Unit::Free()
{
	__super::Free();

	for (auto& effect : m_vecEffects)
		Safe_Release(effect);
}
