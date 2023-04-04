#include "stdafx.h"
#include "..\public\E_RotTrail.h"
#include "GameInstance.h"

CE_RotTrail::CE_RotTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CE_RotTrail::CE_RotTrail(const CE_RotTrail & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CE_RotTrail::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_RotTrail::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_pVITrailBufferCom = CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300);

	/* Trail Option */
	m_eEFfectDesc.IsTrail = true;
	m_eEFfectDesc.fWidth = 0.3f;
	m_eEFfectDesc.fLife = 2.0f; 
	m_eEFfectDesc.bAlpha = false;
	m_eEFfectDesc.fAlpha = 0.6f;
	m_eEFfectDesc.fSegmentSize = 0.001f; 
	m_eEFfectDesc.vColor = XMVectorSet(255.f, 97.f, 0.f, 255.f) / 255.f;
	/* ~Trail Option */

	m_eEFfectDesc.fFrame[0] = 83.f;
	m_eEFfectDesc.bActive = false;

	return S_OK;
}

void CE_RotTrail::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		m_pVITrailBufferCom->Get_InstanceInfo()->clear();

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CE_RotTrail::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_RotTrail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(15);
	m_pVITrailBufferCom->Render();

	return S_OK;
}

HRESULT CE_RotTrail::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/* Instance Buffer */
	if (FAILED(m_pVITrailBufferCom->Bind_ShaderResouce(m_pShaderCom, "g_RotInfoMatrix")))
		return E_FAIL;
	/* Instance Buffer */

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CE_RotTrail * CE_RotTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_RotTrail * pInstance = new CE_RotTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_RotTrail Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_RotTrail::Clone(void * pArg)
{
	CE_RotTrail * pInstance = new CE_RotTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_RotTrail Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_RotTrail::Free()
{
	__super::Free();
}
