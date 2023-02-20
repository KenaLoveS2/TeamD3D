#include "stdafx.h"
#include "..\public\Kena_MainOutfit.h"
#include "GameInstance.h"

CKena_MainOutfit::CKena_MainOutfit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CKena_Parts(pDevice, pContext)
{
}

CKena_MainOutfit::CKena_MainOutfit(const CKena_MainOutfit & rhs)
	: CKena_Parts(rhs)
{
}

HRESULT CKena_MainOutfit::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena_MainOutfit::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	return S_OK;
}

void CKena_MainOutfit::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CKena_MainOutfit::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CKena_MainOutfit::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CKena_MainOutfit::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

HRESULT CKena_MainOutfit::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_MainOutfit", L"Com_Renderer", (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

HRESULT CKena_MainOutfit::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);

	return S_OK;
}

CKena_MainOutfit * CKena_MainOutfit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKena_MainOutfit*		pInstance = new CKena_MainOutfit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena_MainOutfit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CKena_MainOutfit::Clone(void * pArg)
{
	CKena_MainOutfit*		pInstance = new CKena_MainOutfit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena_MainOutfit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena_MainOutfit::Free()
{
	__super::Free();
}
