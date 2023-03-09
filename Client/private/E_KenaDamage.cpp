#include "stdafx.h"
#include "..\public\E_KenaDamage.h"
#include "GameInstance.h"

CE_KenaDamage::CE_KenaDamage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaDamage::CE_KenaDamage(const CE_KenaDamage & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_KenaDamage::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaDamage::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;

	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);

		if (!lstrcmp(pChild->Get_ObjectCloneName(), L"KenaDamageCircleR_2"))
			pChild->Get_TransformCom()->Rotation(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), 90.f);
	}

	m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 0.3f));
	return S_OK;
}

void CE_KenaDamage::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	ImGui::Begin("Damage");
	ImGui::Checkbox("Avtice", &m_eEFfectDesc.bActive);
	ImGui::End();
	if (m_eEFfectDesc.bActive == true)
	{
// 		m_fScale += 0.3f;
// 		m_pTransformCom->Set_Scaled(m_CurScale * m_fScale);
	}
	else
	{
// 		m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 0.3f));
// 		m_fScale = 0.0f;
	}

	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Active(m_eEFfectDesc.bActive);

	}
}

void CE_KenaDamage::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaDamage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_KenaDamage * CE_KenaDamage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaDamage * pInstance = new CE_KenaDamage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaDamage Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaDamage::Clone(void * pArg)
{
	CE_KenaDamage * pInstance = new CE_KenaDamage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaDamage Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaDamage::Free()
{
	__super::Free();
}
