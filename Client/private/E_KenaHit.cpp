#include "stdafx.h"
#include "..\public\E_KenaHit.h"
#include "GameInstance.h"

CE_KenaHit::CE_KenaHit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_KenaHit::CE_KenaHit(const CE_KenaHit & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_KenaHit::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaHit::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Component */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	m_eEFfectDesc.bActive = false;
	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 0.2f));
	return  S_OK;
}

void CE_KenaHit::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Hit");
	ImGui::Checkbox("Avtice", &m_eEFfectDesc.bActive);
	ImGui::End();

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fScaleTime += fTimeDelta;
		m_fAddValue += 0.3f;

		if (m_fScaleTime > 0.6f)
		{
			m_eEFfectDesc.vScale *= m_fAddValue;
			m_eEFfectDesc.bActive = false;
		}
	}
	else
	{
		for (auto& pChild : m_vecChild)
			pChild->ResetSprite();

		m_fAddValue = 0.0f;
		m_fScaleTime = 0.0f;
		m_eEFfectDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	}
}

void CE_KenaHit::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaHit::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

CE_KenaHit * CE_KenaHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaHit * pInstance = new CE_KenaHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaHit Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaHit::Clone(void * pArg)
{
	CE_KenaHit * pInstance = new CE_KenaHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaHit Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaHit::Free()
{
	__super::Free();
}
