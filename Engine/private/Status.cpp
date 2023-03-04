#include "stdafx.h"
#include "..\public\Status.h"
#include "GameInstance.h"

CStatus::CStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus & rhs)
	: CComponent(rhs)
{
}

HRESULT CStatus::Initialize_Prototype(const wstring & wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;

	return S_OK;
}

HRESULT CStatus::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

void CStatus::Tick(_float fTimeDelta)
{
}

void CStatus::Imgui_RenderProperty()
{
}

HRESULT CStatus::Save_Status(const _tchar * pFilePath)
{
	return S_OK;
}

void CStatus::Free()
{
	__super::Free();
}
