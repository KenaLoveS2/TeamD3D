#include "stdafx.h"
#include "..\public\Monster_Status.h"
#include "GameInstance.h"

CMonster_Status::CMonster_Status(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CStatus(pDevice, pContext)
{
}

CMonster_Status::CMonster_Status(const CMonster_Status & rhs)
	: CStatus(rhs)
{
}

HRESULT CMonster_Status::Initialize_Prototype(const wstring & wstrFilePath)
{
	return S_OK;
}

HRESULT CMonster_Status::Initialize(void * pArg, CGameObject * pOwner)
{
	return S_OK;
}

void CMonster_Status::Tick(_float fTimeDelta)
{
}

void CMonster_Status::Imgui_RenderProperty()
{
}

HRESULT CMonster_Status::Save_Status(const _tchar * pFilePath)
{
	return S_OK;
}

HRESULT CMonster_Status::Load_Status(const _tchar * pFilePath)
{
	return S_OK;
}

CMonster_Status * CMonster_Status::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & wstrFilePath)
{
	CMonster_Status*	pInstance = new CMonster_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrFilePath)))
	{
		MSG_BOX("Failed to Create : CMonster_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMonster_Status::Clone(void * pArg, CGameObject * pOwner)
{
	CMonster_Status*	pInstance = new CMonster_Status(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Clone : CMonster_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Status::Free()
{
	__super::Free();
}
