#include "stdafx.h"
#include "..\public\E_ShamanLazer.h"
#include "GameInstance.h"
#include "E_P_ExplosionGravity.h"

CE_ShamanLazer::CE_ShamanLazer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_ShamanLazer::CE_ShamanLazer(const CE_ShamanLazer & rhs)
	:CEffect(rhs)
{
	
}

HRESULT CE_ShamanLazer::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(__super::Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanLazer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.vScale = XMVectorSet(0.3f, 0.3f, 0.3f, 1.f);
	Reset();

	return S_OK;
}

HRESULT CE_ShamanLazer::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_ShamanLazer::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);
	TurnOnLazer(fTimeDelta);
}

void CE_ShamanLazer::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_ShamanLazer::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

void CE_ShamanLazer::Reset()
{
	for (auto& pChild : m_vecChild)
		pChild->Set_Active(false);

	m_vecChild[CHILD_LINE]->Set_Scale(XMVectorSet(5.f, 1.f, 1.f, 1.f));
	m_fDurationTime = 0.0f;
	m_fTimeDelta = 0.0f;
}

_int CE_ShamanLazer::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_ShamanLazer::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_ShamanLazer::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanLazer::SetUp_Components()
{	
	return S_OK;
}

HRESULT CE_ShamanLazer::SetUp_Effects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base*  pEffectBase = nullptr;

	// TYPE_BOSS_GATHER
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"Lazer_Gravity"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	dynamic_cast<CE_P_ExplosionGravity*>(pEffectBase)->Set_Option(CE_P_ExplosionGravity::TYPE_BOSS_GATHER);
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	return S_OK;
}

void CE_ShamanLazer::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
}

void CE_ShamanLazer::TurnOnLazer(_float fTimeDelta)
{
	/* CHILD_LINE, CHILD_SP, CHILD_P */
	m_vecChild[CHILD_P]->Set_Active(m_eEFfectDesc.bActive);

	if (m_vecChild[CHILD_LINE]->Get_vScale().x <= 0.0f)
	{
		m_vecChild[CHILD_SP]->Set_Active(true);
		m_vecChild[CHILD_LINE]->Set_Active(false);
	}
	else
		m_vecChild[CHILD_LINE]->Set_AddScale(fTimeDelta * -2.f);

	_bool bResult = TurnOffSystem(m_fDurationTime, 1.f, fTimeDelta);
	if (bResult == true) Reset();
}

CE_ShamanLazer * CE_ShamanLazer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_ShamanLazer * pInstance = new CE_ShamanLazer(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanLazer Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_ShamanLazer::Clone(void * pArg)
{
	CE_ShamanLazer * pInstance = new CE_ShamanLazer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanLazer Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanLazer::Free()
{
	__super::Free();
}
