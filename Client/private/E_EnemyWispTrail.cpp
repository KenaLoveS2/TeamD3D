#include "stdafx.h"
#include "..\public\E_EnemyWispTrail.h"
#include "GameInstance.h"

CE_EnemyWispTrail::CE_EnemyWispTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_EnemyWispTrail::CE_EnemyWispTrail(const CE_EnemyWispTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_EnemyWispTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_EnemyWispTrail::Initialize(void * pArg)
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
	/* Trail Texture */

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.08f;
	m_eEFfectDesc.fLife = 0.5f; 
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.001f; 
	m_eEFfectDesc.vColor = XMVectorSet(160.f, 231.f, 255.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_iTrailFlowTexture = 0;
	m_eEFfectDesc.bActive = true;
	return S_OK;
}

void CE_EnemyWispTrail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CE_EnemyWispTrail::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_EnemyWispTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

HRESULT CE_EnemyWispTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/* Flow */
	if (FAILED(m_pTrailflowTexture->Bind_ShaderResource(m_pShaderCom, "g_TrailflowTexture", m_iTrailFlowTexture)))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_UV", &m_fUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_EnemyWispTrail * CE_EnemyWispTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_EnemyWispTrail * pInstance = new CE_EnemyWispTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_EnemyWispTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_EnemyWispTrail::Clone(void * pArg)
{
	CE_EnemyWispTrail * pInstance = new CE_EnemyWispTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_EnemyWispTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_EnemyWispTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTrailflowTexture);
}
