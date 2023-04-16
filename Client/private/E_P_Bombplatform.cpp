#include "stdafx.h"
#include "..\public\E_P_Bombplatform.h"
#include "GameInstance.h"
#include "E_P_CommonBox.h"

CE_P_Bombplatform::CE_P_Bombplatform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Bombplatform::CE_P_Bombplatform(const CE_P_Bombplatform & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Bombplatform::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Bombplatform::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_bTimer = true;
	m_bDissolve = false;
	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_Bombplatform::Late_Initialize(void* pArg)
{
	m_pCommonBox = (CE_P_CommonBox*)(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_P_CommonBox", CUtile::Create_DummyString()));
	NULL_CHECK_RETURN(m_pCommonBox, E_FAIL);
	m_pCommonBox->Set_Parent(this);
	m_pCommonBox->Late_Initialize(nullptr);

	return S_OK;
}

void CE_P_Bombplatform::Tick(_float fTimeDelta)
{
	if (m_pCommonBox)
		m_pCommonBox->Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	m_fLife += fTimeDelta;

	if (m_bDissolve)
		Update_Particle(fTimeDelta);
}

void CE_P_Bombplatform::Late_Tick(_float fTimeDelta)
{
	if (m_pCommonBox)m_pCommonBox->Late_Tick(fTimeDelta);

	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(2.f, 5.f));
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_Position();
		vPos.y -= 2.f;
		m_pTransformCom->Set_Position(vPos);
		vPos.y -= 2.f;
		m_pCommonBox->Set_Effect(vPos, true);
	}


	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Bombplatform::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Bombplatform::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_Bombplatform::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);

	return S_OK;
}

void CE_P_Bombplatform::Update_Particle(_float fTimeDelta)
{
	m_pCommonBox->TurnSystem_CommonBox(true);

	if (m_fLife > 6.f)
		Reset();
}

void CE_P_Bombplatform::Reset()
{
	m_fLife = 0.0f;

	m_bTimer = true;
	m_bDissolve = false;

	m_eEFfectDesc.bActive = false;
}

CE_P_Bombplatform * CE_P_Bombplatform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Bombplatform * pInstance = new CE_P_Bombplatform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Bombplatform Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Bombplatform::Clone(void * pArg)
{
	CE_P_Bombplatform * pInstance = new CE_P_Bombplatform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Bombplatform Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Bombplatform::Free()
{
	__super::Free();

	Safe_Release(m_pCommonBox);
}
