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
	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -7.f;
		m_Desc.WorldMatrix._43 = 0.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CMoth::Late_Initialize(void * pArg)
{
	{

	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));

	return S_OK;
}

void CMoth::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;
	__super::Tick(fTimeDelta);
	
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CMoth::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;
	CMonster::Late_Tick(fTimeDelta);
	
	if (m_pRendererCom && m_bSpawn)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CMoth::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
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
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
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

	// shader Value Á¶Àý
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
		.InitState("SLEEP")
		.AddState("SLEEP")
		.OnExit([this]()
	{
		m_pEnemyWisp->IsActiveState();
	})
		.AddTransition("SLEEP to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fSpawnRange);
	})


		.AddState("READY_SPAWN")
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pUIHPBar->Set_Active(true);
		m_bSpawn = true;
	})
		.AddTransition("READY_SPAWN to SPAWN", "SPAWN")
		.Predicator([this]()
	{
		return m_pEnemyWisp->IsActiveState();
	})


		.AddState("SPAWN")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(WISPIN);
		m_pModelCom->Set_AnimIndex(WISPIN);
	})
		.AddTransition("SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(WISPIN);
	})
		

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pTransformCom->LookAt(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(COMBATIDLE);
		m_pModelCom->Set_AnimIndex(COMBATIDLE);
	})
		.Tick([this](_float fTimeDelta)
	{
		
	})
		.OnExit([this]()
	{
		
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("IDLE To CHASE", "CHASE")
		.Predicator([this]()
	{
		return !DistanceTrigger(5.f);
	})
		.AddTransition("IDLE To MELEE_ATTACK", "MELEE_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(2.f);
	})

		.AddState("CHASE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(WALK);
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 1.f);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("CHASE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return DistanceTrigger(1.5f);
	})

		.AddState("MELEE_ATTACK")
		.OnStart([this]()
	{
		m_bRealAttack = true;
		m_pModelCom->ResetAnimIdx_PlayTime(MELEEATTACK);
		m_pModelCom->Set_AnimIndex(MELEEATTACK);
	})		
		.OnExit([this]()
	{	
		m_bRealAttack = false;
		m_pModelCom->Set_AnimIndex(COMBATIDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("MELEE_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{	
		return AnimFinishChecker(MELEEATTACK);
	})


		.AddState("DYING")
		.OnStart([this]()
	{
		Set_Dying(DEATHSIM);
	})
		.AddTransition("DYING to DEATH", "DEATH")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})
		
		.AddState("DEATH")
		.OnStart([this]()
	{
		Clear_Death();
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

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothBody_AOmoth.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothBody_Emissive.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ROUGHNESS, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothBody_Roughness.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Enemy/Moth/T_MothWing_Mask.png")), E_FAIL);
	
	return S_OK;
}

HRESULT CMoth::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDying, sizeof(_bool)), E_FAIL);
	m_bDying && Bind_Dissolove(m_pShaderCom);
	return S_OK;
}

HRESULT CMoth::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;

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
