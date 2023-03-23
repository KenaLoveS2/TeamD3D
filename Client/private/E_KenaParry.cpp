#include "stdafx.h"
#include "..\public\E_KenaParry.h"
#include "GameInstance.h"

CE_KenaParry::CE_KenaParry(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaParry::CE_KenaParry(const CE_KenaParry & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaParry::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaParry::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Set Option */
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);
	/* ~Set Option */

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_KenaParry::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fTimeDelta += fTimeDelta;
		m_eEFfectDesc.vScale *= 1.1f + fTimeDelta;

		for (auto& pChild : m_vecChild)
			pChild->Set_AddScale(1.1f + fTimeDelta);

		if (m_fTimeDelta > 0.3f)
		{
			ResetSprite();
			m_eEFfectDesc.vScale = _float3(0.5f, 0.5f, 1.f);

			for (auto& pChild : m_vecChild)
				pChild->Set_Scale(_float3(0.7f, 0.7f, 1.f));

			m_eEFfectDesc.bActive = false;
			m_fTimeDelta = 0.0f;
		}
	}
}

void CE_KenaParry::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaParry::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaParry::Imgui_RenderProperty()
{
	if (ImGui::Button("active"))
		m_eEFfectDesc.bActive = true;
}

void CE_KenaParry::Set_Child()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base* pEffectBase = nullptr;

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Explosion_p", L"Parry_Explosion_P"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

CE_KenaParry * CE_KenaParry::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaParry * pInstance = new CE_KenaParry(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaParry Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaParry::Clone(void * pArg)
{
	CE_KenaParry * pInstance = new CE_KenaParry(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaParry Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaParry::Free()
{
	__super::Free();
}
