#include "stdafx.h"
#include "..\public\E_HunterTrail.h"
#include "GameInstance.h"
#include "E_RectTrail.h"

CE_HunterTrail::CE_HunterTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_HunterTrail::CE_HunterTrail(const CE_HunterTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_HunterTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_HunterTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iTotalDTextureComCnt = 0; 	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	// FAILED_CHECK_RETURN(SetUp_MovementTrail(), E_FAIL);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.7f;
	m_eEFfectDesc.fLife = 0.4f; 
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.001f;
	m_eEFfectDesc.vColor = XMVectorSet(255.f, 129.f, 0.0f, 255.f) / 255.f;
	/* ~Trail Option */

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_HunterTrail::Late_Initialize(void * pArg)
{
	return S_OK;
}

void CE_HunterTrail::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);
}

void CE_HunterTrail::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		ResetInfo();
		return;
	}
	else
		m_fTimeDelta += fTimeDelta;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_HunterTrail::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

HRESULT CE_HunterTrail::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_BossTrailInfoMatrix"), E_FAIL);

	FAILED_CHECK_RETURN(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_FlowTexture", m_iTrailFlowTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pTrailTypeTexture->Bind_ShaderResource(m_pShaderCom, "g_TypeTexture", m_iTrailTypeTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float)), E_FAIL);

	return S_OK;
}

HRESULT CE_HunterTrail::SetUp_Components()
{	
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailType"), L"Com_typeTexture", (CComponent**)&m_pTrailTypeTexture, this), E_FAIL);

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	Delete_Component(L"Com_Shader");
	Safe_Release(m_pShaderCom);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffect_BossTrail"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CE_HunterTrail::SetUp_MovementTrail()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pMovementTrail = dynamic_cast<CE_RectTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", L"S_MovementParticle"));
	NULL_CHECK_RETURN(m_pMovementTrail, E_FAIL);
	m_pMovementTrail->Set_Parent(this);
	m_pMovementTrail->SetUp_Option(CE_RectTrail::OBJ_B_HUNTER);
	// m_pMovementTrail->Set_TexRandomPrint(); /* Sprite Random Print */

	return S_OK;
}

void CE_HunterTrail::Update_Trail()
{
	if (m_eEFfectDesc.bActive == true)
		m_pMovementTrail->Trail_InputRandomPos(m_pTransformCom->Get_Position());
}

CE_HunterTrail * CE_HunterTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_HunterTrail * pInstance = new CE_HunterTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_HunterTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_HunterTrail::Clone(void * pArg)
{
	CE_HunterTrail * pInstance = new CE_HunterTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_HunterTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_HunterTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
	Safe_Release(m_pTrailTypeTexture);

	// Safe_Release(m_pMovementTrail);
}
