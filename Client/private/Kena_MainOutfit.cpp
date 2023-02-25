#include "stdafx.h"
#include "..\public\Kena_MainOutfit.h"
#include "GameInstance.h"
#include "Kena.h"

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

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	CModel*	pParentModel = dynamic_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pModelCom->Animation_Synchronization(pParentModel, "SK_Kena_Clothing.ao");

	m_vMulAmbientColor = _float4(2.f, 2.f, 2.f, 1.f);
	m_fSSSAmount = 0.09f;
	m_vSSSColor = _float4(0.2f, 0.2f, 0.2f, 1.f);
	return S_OK;
}

void CKena_MainOutfit::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Set_AnimIndex(m_pPlayer->Get_AnimationIndex());
	m_pModelCom->Set_PlayTime(m_pPlayer->Get_AnimationPlayTime());
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pModelCom->Imgui_RenderProperty();
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

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if (i == 0)
		{
			// Real Cloth			
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveMaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SSS_MASK, "g_SSSMaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		}
		else	if(i==1 || i ==2)
		{
			// Shoes & Bag
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",5);
		}
		else if(i == 3 || i == 4)
		{
			// Boots & Hair
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_HAIR_DEPTH, "g_HairDepthTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_HAIR_ALPHA, "g_HairAlphaTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_HAIR_ROOT, "g_HairRootTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices" , 6);
		}
	}

	return S_OK;
}

void CKena_MainOutfit::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CKena_MainOutfit::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Kena MainOutfit Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CKena_MainOutfit::ImGui_ShaderValueProperty()
{
	{
		static _float2 AmountMinMax{ -10.f, 10.f };
		ImGui::InputFloat2("OutFit_SSSAmountMinMax", (float*)&AmountMinMax);
		ImGui::DragFloat("OutFit_SSSAmount", &m_fSSSAmount, 0.001f, AmountMinMax.x, AmountMinMax.y);
		_float fColor[3] = { m_vSSSColor.x, m_vSSSColor.y, m_vSSSColor.z };
		static _float2 sssMinMax{ -1.f, 1.f };
		ImGui::InputFloat2("OutFit_SSSMinMax", (float*)&sssMinMax);
		ImGui::DragFloat3("OutFit_SSSAColor", fColor, 0.001f, sssMinMax.x, sssMinMax.y);
		m_vSSSColor.x = fColor[0];
		m_vSSSColor.y = fColor[1];
		m_vSSSColor.z = fColor[2];
	}

	{
		_float fColor[3] = { m_vMulAmbientColor.x, m_vMulAmbientColor.y, m_vMulAmbientColor.z };
		static _float2 maMinMax{ 0.f, 255.f };
		ImGui::InputFloat2("OutFit_MAMinMax", (float*)&maMinMax);
		ImGui::DragFloat3("OutFit_MAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
		m_vMulAmbientColor.x = fColor[0];
		m_vMulAmbientColor.y = fColor[1];
		m_vMulAmbientColor.z = fColor[2];
	}

	{
		static _float2 HairLengthMinMax{ -10.f, 10.f };
		ImGui::InputFloat2("g_fHairLengthMinMax", (float*)&HairLengthMinMax);
		ImGui::DragFloat("g_fHairLength", &m_fHairLength, 0.001f, HairLengthMinMax.x, HairLengthMinMax.y);
	}

	{
		static _float2 HairThicknessMinMax{ -10.f, 10.f };
		ImGui::InputFloat2("g_fHairThicknessMinMax", (float*)&HairThicknessMinMax);
		ImGui::DragFloat("g_fHairThickness", &m_fHairThickness, 0.001f, HairThicknessMinMax.x, HairThicknessMinMax.y);
	}
}

HRESULT CKena_MainOutfit::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_MainOutfit", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/********************* For. Kena PostProcess By WJ*****************/
	//For.Cloth
	// AO_R_M
	m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_AO_R_M.png"));
	// EMISSIVE
	m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_EMISSIVE.png"));
	// EMISSIVE_MASK
	m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVEMASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_EMISSIVE_MASK.png"));
	// MASK
	m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_MASK.png"));
	// SPRINT_EMISSIVE
	m_pModelCom->SetUp_Material(0, WJTextureType_SPRINT_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_sprint_EMISSIVE.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(0, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_SSS_MASK.png"));

	// 1~2 For. props
	for (_uint i = 1; i < 3; ++i)
	{
		// AO_R_M
		m_pModelCom->SetUp_Material(i, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_AO_R_M.png"));
		// EMISSIVE
		m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_EMISSIVE.png"));
		// SPRINT_EMISSIVE
		m_pModelCom->SetUp_Material(i, WJTextureType_SPRINT_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_sprint_EMISSIVE.png"));
	}
	/******************************************************************/

	for(_uint i = 3; i<5; ++i)
	{
		// Depth
		m_pModelCom->SetUp_Material(i, WJTextureType_HAIR_DEPTH, TEXT("../Bin/Resources/Anim/Kena/hair_DEPTH.png"));
		// Alpha
		m_pModelCom->SetUp_Material(i, WJTextureType_HAIR_ALPHA, TEXT("../Bin/Resources/Anim/Kena/hair_ALPHA.png"));
		// Root
		m_pModelCom->SetUp_Material(i, WJTextureType_HAIR_ROOT, TEXT("../Bin/Resources/Anim/Kena/hair_ROOT.png"));
	}

	return S_OK;
}

HRESULT CKena_MainOutfit::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	
	m_pShaderCom->Set_RawValue("g_fSSSAmount", &m_fSSSAmount, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_vSSSColor", &m_vSSSColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vAmbientColor", &m_vMulAmbientColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_fHairLength", &m_fHairLength, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fHairThickness", &m_fHairThickness, sizeof(_float));

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
