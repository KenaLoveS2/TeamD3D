#include "stdafx.h"
#include "..\public\Moth.h"
#include "GameInstance.h"

CMoth::CMoth(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CMoth::CMoth(const CMoth & rhs)
	: CMonster(rhs)
{
}

HRESULT CMoth::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CMoth::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	// SetUp_Component(); Monster가 불러줌
	//	Push_EventFunctions();

	m_pTransformCom->Set_Translation(_float4(0.f, 0.f, 5.f, 1.f), _float4());

	m_pModelCom->Set_AllAnimCommonType();

	return S_OK;
}

void CMoth::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CMoth::Late_Tick(_float fTimeDelta)
{
	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CMoth::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if(i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ROUGHNESS, "g_NormalTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SEPARATE_AO_R_M_E);
		}
		else
		{
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT);
		}
	}
	return S_OK;
}

HRESULT CMoth::RenderShadow()
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

void CMoth::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
}

void CMoth::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Moth Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("State"))
	{
		m_pFSM->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CMoth::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();

	// shader Value 조절
}

HRESULT CMoth::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CMoth::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CMoth::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.Build();

	return S_OK;
}

HRESULT CMoth::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Moth", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_Moth.json");


	//0 : body
	//1 : wing

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothBody_AOmoth.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothBody_Emissive.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ROUGHNESS, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothBody_Roughness.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothWing_Mask.png")), E_FAIL);
	
	return S_OK;
}

HRESULT CMoth::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CMoth::SetUp_ShadowShaderResources()
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

void CMoth::Update_Collider(_float fTimeDelta)
{
}

CMoth* CMoth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoth*	pInstance = new CMoth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMoth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoth::Clone(void* pArg)
{
	CMoth*	pInstance = new CMoth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMoth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoth::Free()
{
	CMonster::Free();
}
