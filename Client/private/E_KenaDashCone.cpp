#include "stdafx.h"
#include "..\public\E_KenaDashCone.h"
#include "GameInstance.h"

CE_KenaDashCone::CE_KenaDashCone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_KenaDashCone::CE_KenaDashCone(const CE_KenaDashCone & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CE_KenaDashCone::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_KenaDashCone::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);		

	m_fHDRValue = 1.5f; 
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_pTransformCom->Set_Scaled(_float3(10.f, 5.f, 10.f));
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_KenaDashCone::Tick(_float fTimeDelta)
{
  	//if (m_pParent == nullptr)
  	//	ToolOption("CE_KenaDashCone");

	if (m_pParent)
		m_eEFfectDesc.bActive = dynamic_cast<CEffect_Base*>(m_pParent)->Get_Active();

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CE_KenaDashCone::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		return;
	}

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_KenaDashCone::Render()
{	
 	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

HRESULT CE_KenaDashCone::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	Set_ModelCom(m_eEFfectDesc.eMeshType);

	return S_OK;
}

HRESULT CE_KenaDashCone::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

CE_KenaDashCone * CE_KenaDashCone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaDashCone * pInstance = new CE_KenaDashCone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaDashCone Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaDashCone::Clone(void * pArg)
{
	CE_KenaDashCone * pInstance = new CE_KenaDashCone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaDashCone Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaDashCone::Free()
{
	__super::Free();
}

