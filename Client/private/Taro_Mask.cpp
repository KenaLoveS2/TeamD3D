#include "stdafx.h"
#include "..\public\Taro_Mask.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"

CTaro_Mask::CTaro_Mask(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CKena_Parts(pDevice, pContext)
{
}

CTaro_Mask::CTaro_Mask(const CTaro_Mask & rhs)
	: CKena_Parts(rhs)
{
}

HRESULT CTaro_Mask::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTaro_Mask::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	return S_OK;
}

void CTaro_Mask::Tick(_float fTimeDelta)
{
	if (m_bActive == false)
		return;

	__super::Tick(fTimeDelta);
}

void CTaro_Mask::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == false)
		return;

	//__super::Late_Tick(fTimeDelta);

	CModel*	pModel = dynamic_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	CBone*	pHand = pModel->Get_BonePtr("lf_hand_socket_jnt");
	_matrix	matSocket = pHand->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pPlayer->Get_WorldMatrix();

	m_pTransformCom->Set_WorldMatrix(matSocket);

	if (m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CTaro_Mask::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");

		m_pModelCom->Render(m_pShaderCom, i, nullptr, 8);
	}

	return S_OK;
}

HRESULT CTaro_Mask::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);

	return S_OK;
}

void CTaro_Mask::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CTaro_Mask::ImGui_AnimationProperty()
{
}

void CTaro_Mask::ImGui_ShaderValueProperty()
{
}

HRESULT CTaro_Mask::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Taro_Mask", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	// AO_R_M
	m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, L"../Bin/Resources/NonAnim/Taro_Mask/taro_props_tools_AO_R_M.png");
	// EMISSIVE
	m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, L"../Bin/Resources/NonAnim/Taro_Mask/taro_props_tools_EMISSIVE.png");

	return S_OK;
}

HRESULT CTaro_Mask::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	//FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_matSocket), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CTaro_Mask::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTaro_Mask * CTaro_Mask::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTaro_Mask*		pInstance = new CTaro_Mask(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTaro_Mask");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTaro_Mask::Clone(void * pArg)
{
	CTaro_Mask*		pInstance = new CTaro_Mask(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTaro_Mask");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTaro_Mask::Free()
{
	__super::Free();
}
