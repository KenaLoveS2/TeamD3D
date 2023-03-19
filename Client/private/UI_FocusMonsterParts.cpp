#include "stdafx.h"
#include "..\public\UI_FocusMonsterParts.h"
#include "GameInstance.h"

CUI_FocusMonsterParts::CUI_FocusMonsterParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Billboard(pDevice, pContext)
{
}

CUI_FocusMonsterParts::CUI_FocusMonsterParts(const CUI_FocusMonsterParts & rhs)
	:CUI_Billboard(rhs)
{
}

HRESULT CUI_FocusMonsterParts::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUI_FocusMonsterParts::Initialize(void * pArg)
{
	return E_NOTIMPL;
}

void CUI_FocusMonsterParts::Tick(_float fTimeDelta)
{
}

void CUI_FocusMonsterParts::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUI_FocusMonsterParts::Render()
{
	return E_NOTIMPL;
}

HRESULT CUI_FocusMonsterParts::SetUp_Components()
{
	return E_NOTIMPL;
}

HRESULT CUI_FocusMonsterParts::SetUp_ShaderResources()
{
	return E_NOTIMPL;
}

CUI_FocusMonsterParts * CUI_FocusMonsterParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CGameObject * CUI_FocusMonsterParts::Clone(void * pArg)
{
	return nullptr;
}

void CUI_FocusMonsterParts::Free()
{
	__super::Free();
}
