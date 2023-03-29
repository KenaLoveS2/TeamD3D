#include "stdafx.h"
#include "..\public\E_SpiritArrowHit.h"
#include "GameInstance.h"

CE_SpiritArrowHit::CE_SpiritArrowHit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_SpiritArrowHit::CE_SpiritArrowHit(const CE_SpiritArrowHit & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_SpiritArrowHit::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_SpiritArrowHit::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	return S_OK;
}

HRESULT CE_SpiritArrowHit::Late_Initialize(void * pArg)
{
	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);

		wstring strCloneTag = this->m_szCloneObjectTag;
		strCloneTag += L"_";
		strCloneTag += to_wstring(m_iHaveChildCnt);
		_tchar* pChildCloneTag = CUtile::Create_StringAuto(strCloneTag.c_str());
		pChild->Set_CloneTag(pChildCloneTag);
		m_iHaveChildCnt++;
	}

	return S_OK;
}

void CE_SpiritArrowHit::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;
	else // true
	{
		m_fTimeDelta += fTimeDelta;
		m_eEFfectDesc.vScale *= (2.0f + fTimeDelta * 2.f);

		if (m_fTimeDelta > 0.5f)
		{
			m_eEFfectDesc.fWidthFrame = 0.0;
			m_eEFfectDesc.fHeightFrame = 0.0;
			m_eEFfectDesc.vScale = _float3(2.f, 2.f, 1.f);
		    m_eEFfectDesc.bActive = false;
			m_fTimeDelta = 0.0f;
		}
	}
}

void CE_SpiritArrowHit::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_SpiritArrowHit::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_SpiritArrowHit * CE_SpiritArrowHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_SpiritArrowHit * pInstance = new CE_SpiritArrowHit(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_SpiritArrowHit Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_SpiritArrowHit::Clone(void * pArg)
{
	CE_SpiritArrowHit * pInstance = new CE_SpiritArrowHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_SpiritArrowHit Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_SpiritArrowHit::Free()
{
	__super::Free();
}
