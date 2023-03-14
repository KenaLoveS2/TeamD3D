#include "stdafx.h"
#include "..\public\E_P_KenaHeavyAttack_Into.h"
#include "GameInstance.h"

CE_P_KenaHeavyAttack_Into::CE_P_KenaHeavyAttack_Into(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_KenaHeavyAttack_Into::CE_P_KenaHeavyAttack_Into(const CE_P_KenaHeavyAttack_Into & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_KenaHeavyAttack_Into::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_KenaHeavyAttack_Into::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_P_KenaHeavyAttack_Into::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("HeavyAttack Into");
	if (ImGui::Button("Recompile"))
		m_pShaderCom->ReCompile();
	ImGui::End();

}

void CE_P_KenaHeavyAttack_Into::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr)
		Set_Matrix();
}

HRESULT CE_P_KenaHeavyAttack_Into::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_KenaHeavyAttack_Into::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vPosition", &m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), sizeof _float4)))
		return S_OK;

	return S_OK;
}

CE_P_KenaHeavyAttack_Into * CE_P_KenaHeavyAttack_Into::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_KenaHeavyAttack_Into * pInstance = new CE_P_KenaHeavyAttack_Into(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_KenaHeavyAttack_Into Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_KenaHeavyAttack_Into::Clone(void * pArg)
{
	CE_P_KenaHeavyAttack_Into * pInstance = new CE_P_KenaHeavyAttack_Into(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_KenaHeavyAttack_Into Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_KenaHeavyAttack_Into::Free()
{
	__super::Free();
}
