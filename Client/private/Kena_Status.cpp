#include "stdafx.h"
#include "..\public\Kena_Status.h"
#include "GameInstance.h"

CKena_Status::CKena_Status(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CStatus(pDevice, pContext)
{
}

CKena_Status::CKena_Status(const CKena_Status & rhs)
	: CStatus(rhs)
{
}

HRESULT CKena_Status::Initialize_Prototype(const wstring & wstrFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(wstrFilePath), E_FAIL);

	return S_OK;
}

HRESULT CKena_Status::Initialize(void * pArg, CGameObject * pOwner)
{
	return S_OK;
}

void CKena_Status::Tick(_float fTimeDelta)
{
}

void CKena_Status::Imgui_RenderProperty()
{
}

HRESULT CKena_Status::Save_Status(const _tchar * pFilePath)
{
	return S_OK;
}

HRESULT CKena_Status::Load_Status(const _tchar * pFilePath)
{
	return S_OK;
}

CKena_Status * CKena_Status::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & wstrFilePath)
{
	CKena_Status*	pInstance = new CKena_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrFilePath)))
	{
		MSG_BOX("Failed to Create : CKena_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CKena_Status::Clone(void * pArg, CGameObject * pOwner)
{
	CKena_Status*	pInstance = new CKena_Status(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Clone : CKena_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena_Status::Free()
{
	__super::Free();
}
