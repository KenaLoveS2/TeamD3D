#include "stdafx.h"
#include "..\public\E_KenaTrail.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Kena.h"

CE_KenaTrail::CE_KenaTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_KenaTrail::CE_KenaTrail(const CE_KenaTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_KenaTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_iTotalDTextureComCnt = 0;
	m_iTotalMTextureComCnt = 0;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Trail Texture */
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailType"), L"Com_typeTexture", (CComponent**)&m_pTrailTypeTexture, this), E_FAIL);
	/* Trail Texture */

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.8f; //5.f
	m_eEFfectDesc.fLife = 0.25f; //1.f
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.01f; // 0.5f
	m_eEFfectDesc.vColor = XMVectorSet(160.f, 231.f, 255.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_iTrailFlowTexture = 5;
	m_iTrailTypeTexture = 7;

	m_eEFfectDesc.bActive = false;

	m_vInitColor = XMVectorSet(0.0f, 195.f, 255.f, 255.f) / 255.f;
	m_vHeavyAttacktColor = XMVectorSet(255.f, 89.0f, 0.0f, 255.f) / 255.f;
	return S_OK;
}

HRESULT CE_KenaTrail::Late_Initialize(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CE_KenaTrail::Tick(_float fTimeDelta)
{
	if (m_pKena == nullptr)
		m_pKena = (CKena*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	if (m_pKena->Get_State(CKena::STATE_PERFECTATTACK) == true)
	{
		m_eEFfectDesc.vColor = m_vHeavyAttacktColor= XMVectorSet(255.f, 89.0f, 0.0f, 255.f) / 255.f;
		m_eEFfectDesc.fWidth = 1.3f; //5.f
	}
	else
	{
		m_eEFfectDesc.vColor = m_vInitColor;
		m_eEFfectDesc.fWidth = 0.8f; //5.f
	}
}

void CE_KenaTrail::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		ResetInfo();
		return;
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

HRESULT CE_KenaTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/* Instance Buffer */
	if (FAILED(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_KenaInfoMatrix")))
		return E_FAIL;
	/* Instance Buffer */

	/* Flow */
	if (FAILED(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_FlowTexture", m_iTrailFlowTexture)))
		return E_FAIL;

	/* Type */
	if (FAILED(m_pTrailTypeTexture->Bind_ShaderResource(m_pShaderCom, "g_TypeTexture", m_iTrailTypeTexture)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UV", &m_fUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_KenaTrail * CE_KenaTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaTrail * pInstance = new CE_KenaTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaTrail::Clone(void * pArg)
{
	CE_KenaTrail * pInstance = new CE_KenaTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
	Safe_Release(m_pTrailTypeTexture);
}
