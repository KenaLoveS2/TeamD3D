#include "stdafx.h"
#include "..\public\Kena_Staff.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Bone.h"
#include "Effect_Base.h"
#include "E_KenaTrail.h"

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
	FAILED_CHECK_RETURN(Ready_Effects(), E_FAIL);

	m_vMulAmbientColor = _float4(2.f, 2.f, 2.f, 1.f);

	return S_OK;
}

HRESULT CKena_Staff::Late_Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CKena_Staff::Ready_Effects()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* KenaStaffTrail */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaStaffTrail", L"KenaStaffTrail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("KenaTrail", pEffectBase);

	/* KenaChargeEffect */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaCharge", L"KenaChargeEffect"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("KenaCharge", pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CKena_Staff::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
 	m_fTimeDelta += fTimeDelta;
 
 	if (m_pPlayer->Is_Bow())
 		m_fBowDurationTime += fTimeDelta ;
 	else
 		m_fBowDurationTime = 0.5f;
 
 	m_mapEffect["KenaTrail"]->Set_Active(m_pPlayer->Is_Attack());
 	m_mapEffect["KenaCharge"]->Set_Active(m_pPlayer->Is_ChargeLight());
 
 	for (auto& pEffect : m_mapEffect)
 		pEffect.second->Tick(fTimeDelta);
}

void CKena_Staff::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/* Weapon Update */
	CBone*	pStaffBonePtr = m_pModelCom->Get_BonePtr("staff_skin8_jnt");
	_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	for (auto& Effect : m_mapEffect)
	{
		if (Effect.first == "KenaCharge")
		{
			_matrix matChargeEffect = Effect.second->Get_TransformCom()->Get_WorldMatrix();
			matChargeEffect.r[3] = matWorldSocket.r[3];

			Effect.second->Get_TransformCom()->Set_WorldMatrix(matChargeEffect);
		}
		else
			Effect.second->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	}
	/* ~Weapon Update */

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
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
		if(i <= 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			/********************* For. Kena PostProcess By WJ*****************/
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			/******************************************************************/
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 5);
		}

		if(m_pPlayer->Is_Bow() == true)
		{
			if (i == 2)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_NoiseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_SwipeTexture");
				/******************************************************************/
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 9);
			}

			if (i == 3) // M_bowTrails == 3
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
				/******************************************************************/
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 8);
			}

			if (i == 4)
			{
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_NoiseTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_SwipeTexture");
				m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_GradientTexture");
				/******************************************************************/
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 10);
			}
		}
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
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 11);

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
	_float fColor[3] = { m_vMulAmbientColor.x, m_vMulAmbientColor.y, m_vMulAmbientColor.z };
	static _float2 maMinMax{ 0.f, 255.f };
	ImGui::InputFloat2("Staff_MAMinMax", (float*)&maMinMax);
	ImGui::DragFloat3("Staff_MAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
	m_vMulAmbientColor.x = fColor[0];
	m_vMulAmbientColor.y = fColor[1];
	m_vMulAmbientColor.z = fColor[2];
}

HRESULT CKena_Staff::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Kena_Staff", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/********************* For. Kena PostProcess By WJ*****************/
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i <= 1)
		{
			// AO_R_M
			m_pModelCom->SetUp_Material(i, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_AO_R_M.png"));
			// EMISSIVE
			m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_EMISSIVE.png"));
			// SPRINT_EMISSIVE
			m_pModelCom->SetUp_Material(i, WJTextureType_SPRINT_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_props_sprint_EMISSIVE.png"));
		}

		if (i == 2 || i == 4)
		{
			// Diffuse
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/Trail/Bow_String/T_customNoise.png"));
			// Mask
			m_pModelCom->SetUp_Material(i, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/Trail/Bow_String/T_stringMask.png"));
			// Noise Texture
			m_pModelCom->SetUp_Material(i, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Textures/Effect/Trail/Bow_String/T_swipe02.png"));
			// Gradient
			m_pModelCom->SetUp_Material(i, WJTextureType_ALPHA, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_4.png"));
		}

		if (i == 3)
		{
			// Diffuse
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_54.png"));
			// Diffuse2
			m_pModelCom->SetUp_Material(i, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/Trail/flow/E_Flow_6.png"));
		}
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
	m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_BowDurationTime", &m_fBowDurationTime, sizeof(_float));
	return S_OK;
}

HRESULT CKena_Staff::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

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

	for (auto& Pair : m_mapEffect)
		Safe_Release(Pair.second);
	m_mapEffect.clear();
}