#include "stdafx.h"
#include "..\public\E_Sapling.h"
#include "GameInstance.h"

CE_Sapling::CE_Sapling(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_Sapling::CE_Sapling(const CE_Sapling & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_Sapling::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Sapling::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	Set_Child();

	/*Set Option */
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_Sapling::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	m_fTimeDelta += fTimeDelta;
	m_eEFfectDesc.vScale *= 1.3f + fTimeDelta;

	if (m_fTimeDelta > 0.2f)
	{
		m_eEFfectDesc.fWidthFrame = 0.0;
		m_eEFfectDesc.fHeightFrame = 0.0;
		m_eEFfectDesc.vScale = _float3(0.5f, 0.5f, 1.f);

		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0f;
	}
}

void CE_Sapling::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_Sapling::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_Sapling::Imgui_RenderProperty()
{
	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;
}

void CE_Sapling::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Sapling_P", L"Sapling_Particle"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

CE_Sapling * CE_Sapling::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Sapling * pInstance = new CE_Sapling(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Sapling Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Sapling::Clone(void * pArg)
{
	CE_Sapling * pInstance = new CE_Sapling(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Sapling Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Sapling::Free()
{
	__super::Free();
}
