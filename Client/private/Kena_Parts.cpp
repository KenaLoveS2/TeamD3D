#include "stdafx.h"
#include "..\public\Kena_Parts.h"
#include "GameInstance.h"
#include "Kena.h"

CKena_Parts::CKena_Parts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_tPartsInfo, sizeof(KENAPARTS_DESC));
}

CKena_Parts::CKena_Parts(const CKena_Parts & rhs)
	: CGameObject(rhs)
	, m_tPartsInfo(rhs.m_tPartsInfo)
{
}

HRESULT CKena_Parts::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena_Parts::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		KENAPARTS_DESC	tKenaPartsDesc;
		memcpy(&tKenaPartsDesc, pArg, sizeof(KENAPARTS_DESC));

		m_tPartsInfo.pPlayer = tKenaPartsDesc.pPlayer;
		m_tPartsInfo.eType = tKenaPartsDesc.eType;
		m_pPlayer = m_tPartsInfo.pPlayer;

		FAILED_CHECK_RETURN(__super::Initialize(&tKenaPartsDesc), E_FAIL);
	}
	else
		FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	return S_OK;
}

void CKena_Parts::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CKena_Parts::Late_Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Set_WorldMatrix(m_pPlayer->Get_WorldMatrix());
}

HRESULT CKena_Parts::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CKena_Parts::RenderShadow()
{
	FAILED_CHECK_RETURN(__super::RenderShadow(), E_FAIL);

	return S_OK;
}

void CKena_Parts::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CKena_Parts::ImGui_ShaderValueProperty()
{
	__super::ImGui_ShaderValueProperty();
}

void CKena_Parts::Model_Synchronization(_bool bPausePlay)
{
	m_pModelCom->Set_AnimIndex(m_pPlayer->Get_AnimationIndex());
	m_pModelCom->Reset_Animation();
	m_pModelCom->Set_PausePlay(bPausePlay);
}

void CKena_Parts::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
