#include "stdafx.h"
#include "..\public\E_KenaCharge.h"
#include "GameInstance.h"

CE_KenaCharge::CE_KenaCharge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaCharge::CE_KenaCharge(const CE_KenaCharge & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaCharge::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaCharge::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;

	Set_Child();
	m_InitScale = m_eEFfectDesc.vScale;
	m_pTransformCom->Set_Scaled(m_InitScale);
	m_InitColor = m_eEFfectDesc.vColor;
	return S_OK;
}

void CE_KenaCharge::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	_vector fCurScale = m_eEFfectDesc.vScale;

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	if (m_eEFfectDesc.bActive == true)
	{
		m_fScale += 0.6f;
		m_eEFfectDesc.vScale *= m_fScale;

		if (XMVectorGetX(m_eEFfectDesc.vScale) > 3.f)
			m_eEFfectDesc.bActive = false;
	}
	else
	{
		m_fScale = 0.0f;
		fCurScale = m_eEFfectDesc.vScale = _float3(1.f, 1.f, 1.f);
	}
}

void CE_KenaCharge::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaCharge::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaCharge::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaChargeImpact", L"KenaChargeEffect_Impact"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

void CE_KenaCharge::Set_InitMatrixScaled(_float3 vScale)
{
	_float4 vRight, vUp, vLook;

	XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[0]) * vScale.x);
	XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])  * vScale.y);
	XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])  *vScale.z);

	memcpy(&m_InitWorldMatrix.m[0][0], &vRight, sizeof vRight);
	memcpy(&m_InitWorldMatrix.m[1][0], &vUp, sizeof vUp);
	memcpy(&m_InitWorldMatrix.m[2][0], &vLook, sizeof vLook);
}

HRESULT CE_KenaCharge::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

CE_KenaCharge * CE_KenaCharge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaCharge * pInstance = new CE_KenaCharge(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaCharge Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaCharge::Clone(void * pArg)
{
	CE_KenaCharge * pInstance = new CE_KenaCharge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaCharge Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaCharge::Free()
{
	__super::Free();
}
