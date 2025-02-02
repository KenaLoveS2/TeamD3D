#include "stdafx.h"
#include "..\public\Component.h"

CComponent::CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
	, m_wstrFilePath(rhs.m_wstrFilePath)
	, m_pOwner(nullptr)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void * pArg, CGameObject* pOwner)
{
	if (pOwner != nullptr)
		m_pOwner = pOwner;

	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
