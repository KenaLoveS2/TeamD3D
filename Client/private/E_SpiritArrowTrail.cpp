#include "stdafx.h"
#include "..\public\E_SpiritArrowTrail.h"
#include "GameInstance.h"
#include "Camera.h"

CE_SpiritArrowTrail::CE_SpiritArrowTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_SpiritArrowTrail::CE_SpiritArrowTrail(const CE_SpiritArrowTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_SpiritArrowTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_SpiritArrowTrail::Initialize(void * pArg)
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
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailFlow"), L"Com_flowTexture", (CComponent**)&m_pTrailflowTexture, this)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_TrailType"), L"Com_typeTexture", (CComponent**)&m_pTrailTypeTexture, this)))
		return E_FAIL;
	/* Trail Texture */

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.5f; //5.f
	m_eEFfectDesc.fLife = 1.5f; //1.f
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.03f; // 0.5f
	m_eEFfectDesc.vColor = XMVectorSet(160.f, 231.f, 255.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_iTrailFlowTexture = 5;
	m_iTrailTypeTexture = 7;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_SpiritArrowTrail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CE_SpiritArrowTrail::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		ResetInfo();
		return;
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_SpiritArrowTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

HRESULT CE_SpiritArrowTrail::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_SpiritArrowTrail * CE_SpiritArrowTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_SpiritArrowTrail * pInstance = new CE_SpiritArrowTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_SpiritArrowTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_SpiritArrowTrail::Clone(void * pArg)
{
	CE_SpiritArrowTrail * pInstance = new CE_SpiritArrowTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_SpiritArrowTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_SpiritArrowTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
	Safe_Release(m_pTrailTypeTexture);
}
