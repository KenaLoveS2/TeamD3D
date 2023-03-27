#include "stdafx.h"
#include "..\public\E_P_Explosion.h"
#include "GameInstance.h"

CE_P_Explosion::CE_P_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Explosion::CE_P_Explosion(const CE_P_Explosion & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Explosion::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Explosion::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	//m_pVIInstancingBufferCom->Set_PSize(_float2(0.5f, 0.5f));
	return S_OK;
}

void CE_P_Explosion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fLife += fTimeDelta;
		if (m_fLife > 0.1f)
		{
			m_eEFfectDesc.bActive = false;
			m_fLife = 0.0f;
		}
	}
}

void CE_P_Explosion::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	//if (m_pParent != nullptr)
	//	Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Explosion::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Explosion::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

CE_P_Explosion * CE_P_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Explosion * pInstance = new CE_P_Explosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Explosion Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Explosion::Clone(void * pArg)
{
	CE_P_Explosion * pInstance = new CE_P_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Explosion Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Explosion::Free()
{
	__super::Free();
}
