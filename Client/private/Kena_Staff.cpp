#include "stdafx.h"
#include "..\public\Kena_Staff.h"
#include "GameInstance.h"
#include "Kena.h"

CKena_Staff::CKena_Staff(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CKena_Parts(pDevice, pContext)
{
}

CKena_Staff::CKena_Staff(const CKena_Staff & rhs)
	: CKena_Parts(rhs)
{
}

HRESULT CKena_Staff::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena_Staff::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	CModel*	pParentModel = dynamic_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pModelCom->Animation_Synchronization(pParentModel, "SK_Staff.ao");
	m_vMulAmbientColor = _float4(2.f,2.f, 2.f,1.f);
	return S_OK;
}

void CKena_Staff::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CModel*	pKenaModel = dynamic_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pModelCom->Set_AnimIndex(pKenaModel->Get_AnimIndex(), pKenaModel->Get_BlendAnimIndex());
	m_pModelCom->Set_PlayTime(m_pPlayer->Get_AnimationPlayTime());
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CKena_Staff::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CKena_Staff::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i > 1)
			continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		/********************* For. Kena PostProcess By WJ*****************/
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
		/******************************************************************/
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 5);
	}

	return S_OK;
}

HRESULT CKena_Staff::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CKena_Staff::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CKena_Staff::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Kena Staff Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CKena_Staff::ImGui_ShaderValueProperty()
{
	{
		_float fColor[3] = { m_vMulAmbientColor.x, m_vMulAmbientColor.y, m_vMulAmbientColor.z };
		static _float2 maMinMax{ 0.f, 255.f };
		ImGui::InputFloat2("Staff_MAMinMax", (float*)&maMinMax);
		ImGui::DragFloat3("Staff_MAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
		m_vMulAmbientColor.x = fColor[0];
		m_vMulAmbientColor.y = fColor[1];
		m_vMulAmbientColor.z = fColor[2];
	}
}

HRESULT CKena_Staff::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_Staff", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/********************* For. Kena PostProcess By WJ*****************/
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for(int i = 0; i<2; ++i)
	{
		// AO_R_M
		m_pModelCom->SetUp_Material(i, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_AO_R_M.png"));
		// EMISSIVE
		m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_EMISSIVE.png"));
		// SPRINT_EMISSIVE
		m_pModelCom->SetUp_Material(i, WJTextureType_SPRINT_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_sprint_EMISSIVE.png"));
	}
	/******************************************************************/

	return S_OK;
}

HRESULT CKena_Staff::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	m_pShaderCom->Set_RawValue("g_fSSSAmount", &m_fSSSAmount, sizeof(float));
	m_pShaderCom->Set_RawValue("g_vSSSColor", &m_vSSSColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vAmbientColor", &m_vMulAmbientColor, sizeof(_float4));

	return S_OK;
}

HRESULT CKena_Staff::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CKena_Staff * CKena_Staff::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKena_Staff*		pInstance = new CKena_Staff(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena_Staff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CKena_Staff::Clone(void * pArg)
{
	CKena_Staff*		pInstance = new CKena_Staff(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena_Staff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena_Staff::Free()
{
	__super::Free();
}