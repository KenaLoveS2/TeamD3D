#include "stdafx.h"
#include "..\public\E_KenaPulse.h"
#include "GameInstance.h"

CE_KenaPulse::CE_KenaPulse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_KenaPulse::CE_KenaPulse(const CE_KenaPulse & rhs)
	:CEffect_Mesh(rhs)
{
	
}

void CE_KenaPulse::Set_InitMatrixScaled(_float3 vScale)
{
	_float4 vRight, vUp, vLook;

	XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[0]) * vScale.x);
	XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])  * vScale.y);
	XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])  *vScale.z);

	memcpy(&m_InitWorldMatrix.m[0][0], &vRight, sizeof vRight);
	memcpy(&m_InitWorldMatrix.m[1][0], &vUp, sizeof vUp);
	memcpy(&m_InitWorldMatrix.m[2][0], &vLook, sizeof vLook);
}

_float3 CE_KenaPulse::Get_InitMatrixScaled()
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[0])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])));
}

void CE_KenaPulse::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulseCloud", L"KenaPulseCloud"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	pEffectBase->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CE_KenaPulse::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulse::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"), L"Com_DissolveTexture", (CComponent**)&m_pDissolveTexture, this)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));

	Set_Child();

	return S_OK;
}

void CE_KenaPulse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bNoActive == true) // Pulse ³¡
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);

		m_fDissolveTime += fTimeDelta;

		_float3 vScale = Get_InitMatrixScaled();
		Set_InitMatrixScaled(vScale * 1.3f);
		if (m_fDissolveTime > 1.f)
		{
			m_eEFfectDesc.bActive = false;
			m_bNoActive = false;
			m_bDesolve = true;
			memcpy(&m_InitWorldMatrix, &m_SaveInitWorldMatrix, sizeof(_float4x4));
			m_fDissolveTime = 0.0f;
		}
	}

	if (m_bNoActive == false && m_eEFfectDesc.bActive == true)
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(true);

		m_fDissolveTime += fTimeDelta;
		if (m_fDissolveTime > 1.f)
		{
			m_bDesolve = false;
			m_fDissolveTime = 0.0f;
		}
	}
}

void CE_KenaPulse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaPulse::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, 1);

	return S_OK;
}

HRESULT CE_KenaPulse::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDesolve, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
		return E_FAIL;

	if(m_bDesolve)
	{
		if (FAILED(m_pDissolveTexture->Bind_ShaderResources(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CE_KenaPulse * CE_KenaPulse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulse Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulse::Clone(void * pArg)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulse Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulse::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
}
