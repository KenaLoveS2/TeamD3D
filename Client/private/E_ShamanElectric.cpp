#include "stdafx.h"
#include "..\public\E_ShamanElectric.h"
#include "GameInstance.h"

CE_ShamanElectric::CE_ShamanElectric(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_ShamanElectric::CE_ShamanElectric(const CE_ShamanElectric & rhs)
	:CEffect(rhs)
{
	
}

HRESULT CE_ShamanElectric::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(__super::Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_ShamanElectric::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = true;
	m_eEFfectDesc.vScale = XMVectorSetY(m_eEFfectDesc.vScale, 4.0f);
	return S_OK;
}

HRESULT CE_ShamanElectric::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_ShamanElectric::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
	{
		m_bFixPos = false;
		return;
	}

	__super::Tick(fTimeDelta);

	if (m_pParent && m_bFixPos == false)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_Position();
		_float fRange = 1.5f;
		vPos.y += 1.f;

		_float3 vMin = _float3(vPos.x - fRange, vPos.y, vPos.z - fRange);
		_float3 vMax = _float3(vPos.x + fRange, vPos.y, vPos.z + fRange);

		m_eEFfectDesc.fTimeDelta = CUtile::Get_RandomFloat(0.1f, 0.3f);
	//	m_eEFfectDesc.vScale = XMVectorSetY(m_eEFfectDesc.vScale, CUtile::Get_RandomFloat(1.f, 4.0f));

		m_pTransformCom->Set_Position(CUtile::Get_RandomVector(vMin, vMax));

		m_bFixPos = true;
	}
}

void CE_ShamanElectric::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		Reset();
		return;
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_ShamanElectric::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

void CE_ShamanElectric::Reset()
{
	m_eEFfectDesc.bActive = false;
	m_fDurationTime = 0.0f;
	m_fTimeDelta = 0.0f;
}

HRESULT CE_ShamanElectric::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_ShamanElectric::SetUp_Components()
{	
	return S_OK;
}

void CE_ShamanElectric::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
}

CE_ShamanElectric * CE_ShamanElectric::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_ShamanElectric * pInstance = new CE_ShamanElectric(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_ShamanElectric Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_ShamanElectric::Clone(void * pArg)
{
	CE_ShamanElectric * pInstance = new CE_ShamanElectric(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_ShamanElectric Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_ShamanElectric::Free()
{
	__super::Free();
}
