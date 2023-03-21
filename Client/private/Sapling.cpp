#include "stdafx.h"
#include "..\public\Sapling.h"
#include "GameInstance.h"
#include "Bone.h"
#include "E_Sapling.h"

CSapling::CSapling(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CSapling::CSapling(const CSapling & rhs)
	: CMonster(rhs)
{
}

HRESULT CSapling::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CSapling::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = 14.f;
		m_Desc.WorldMatrix._43 = 5.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	return S_OK;
}

HRESULT CSapling::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(0.25f, 0.1f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.3f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = vPos;
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 20.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.bCCD = true;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_MONSTER));

		// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
		m_pRendererCom->Set_PhysXRender(true);
		m_pTransformCom->Set_PxPivotScale(vPivotScale);
		m_pTransformCom->Set_PxPivot(vPivotPos);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));

	return S_OK;
}

void CSapling::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pEffects) m_pEffects->Tick(fTimeDelta);
}

void CSapling::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bSpawn)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	if (m_pEffects) m_pEffects->Late_Tick(fTimeDelta);

}

HRESULT CSapling::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 3 && !m_bBombUp) 
			continue;

		if (i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_NoiseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_ReamTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT, "g_LineTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_SmoothTexture");

			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_DissolveTexture");

			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SAPLING_BOMBUP);
		}

		if (i == 1 || i == 2 || i == 4)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");

			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_DissolveTexture");

			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
		}
	}

	// 0 : 뒤에 달려있는 폭탄입니다. 제대로된 애니메이션 일 때 만 렌더를 켜야합니다.
	// 1, 2, 4: 
	// 3 : 눈도 빛이 납니다

	return S_OK;
}

HRESULT CSapling::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CSapling::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	if (ImGui::Button("SPAWN"))
		m_bSpawn = true;
}

void CSapling::ImGui_AnimationProperty()
{
	if (ImGui::CollapsingHeader("Sapling"))
	{
		ImGui::BeginTabBar("Sapling Animation & State");

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
}

void CSapling::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CSapling::ImGui_PhysXValueProperty()
{
	CMonster::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();
	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();
	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	m_pTransformCom->Set_PxPivot(vPxPivot);
}

HRESULT CSapling::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CSapling::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CSapling::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("NONE")
		.AddState("NONE")
		.OnExit([this]()
	{
		m_pEnemyWisp->IsActiveState();
	})
		.AddTransition("NONE to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{
		m_fSpawnRange = 10.f;
		return DistanceTrigger(m_fSpawnRange);
	})


		.AddState("READY_SPAWN")		
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pUIHPBar->Set_Active(true);
		m_bSpawn = true;
	})
		.AddTransition("READY_SPAWN to AWAKE", "WISPOUT")
		.Predicator([this]()
	{
		return m_pEnemyWisp->IsActiveState();
	})


		.AddState("WISPOUT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(WISPOUT);
		m_pModelCom->Set_AnimIndex(WISPOUT);
	})
		.AddTransition("WISPOUT to IDLE" , "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(WISPOUT);
	})

		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_fIdletoAttack += fTimeDelta;
		m_pModelCom->Set_AnimIndex(COMBATIDLE);
	})
		.AddTransition("IDLE to ALERTED", "ALERTED")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttack, 3.f);
	})
		.AddTransition("IDLE to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bWeaklyHit || m_bStronglyHit;
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("TAKEDAMAGE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGE);
		m_pModelCom->Set_AnimIndex(TAKEDAMAGE);
	})
		.OnExit([this]()
	{
		m_bWeaklyHit = false;
		m_bStronglyHit = false;
	})
		.AddTransition("TAKEDAMAGE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TAKEDAMAGE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("ALERTED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ALERTED);
		m_pModelCom->Set_AnimIndex(ALERTED);
	})
		.AddTransition("ALERTED to BOMBCHARGEUP" , "BOMBCHARGEUP")
		.Predicator([this]()
	{
		return AnimFinishChecker(ALERTED);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("BOMBCHARGEUP")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BOMBCHARGEUP);
		m_pModelCom->Set_AnimIndex(BOMBCHARGEUP);
		m_bBombUp = true;
	})
		.AddTransition("BOMBCHARGEUP to CHARGE", "CHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BOMBCHARGEUP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("CHARGE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(CHARGE);
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta);
	})
		.AddTransition("CHARGE to CHARGEATTACK", "CHARGEATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("CHARGEATTACK")
		.OnStart([this]()
	{
		m_bRealAttack = true;
		m_pModelCom->ResetAnimIdx_PlayTime(CHARGEATTACK);
		m_pModelCom->Set_AnimIndex(CHARGEATTACK);

		/* Effect */
		m_pEffects->Set_Active(true);
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pEffects->Set_Position(vPos);
	})
		.OnExit([this]()
	{
		m_bRealAttack = false;
	})

		.AddTransition("CHARGEATTACK to IDLE", "DYING") // 폭발 처리가 있어야 할듯
		.Predicator([this]()
	{
		return AnimFinishChecker(CHARGEATTACK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("DYING")
		.OnStart([this]()
	{
		Set_Dying(WISPOUT);

		/* Effect */
		m_pEffects->Set_Active(true);
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pEffects->Set_Position(vPos);
	})
		.AddTransition("DYING to DEATH", "DEATH")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.AddState("DEATH")
		.OnStart([this]()
	{
		m_bDeath = true;
	})	
		.Build();

	return S_OK;
}

HRESULT CSapling::SetUp_Components()
{
	__super::SetUp_Components();
	
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sapling", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_Sapling.json");


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 3)
			continue;

		if (i == 0) // Boom
		{
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/Noise_cloudsmed.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Deadzone_REAM.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Black_Linear.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_GR_Noise_Smooth_A.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_105.png")), E_FAIL);
		}

		if (i == 1 || i == 2 || i == 4) // Body
		{
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sapling/stick_02_AO_R_M.png")), E_FAIL);

			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_105.png")), E_FAIL);
		}
	}

	m_pModelCom->Set_RootBone("Sticks_02_RIG");

	return S_OK;
}

HRESULT CSapling::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDying, sizeof(_bool)), E_FAIL);
	m_bDying && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CSapling::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CSapling::SetUp_Effects()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pEffects = dynamic_cast<CE_Sapling*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionSapling", L"Sapling_Effect"));
	NULL_CHECK_RETURN(m_pEffects, E_FAIL);
	m_pEffects->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CSapling::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

void CSapling::AdditiveAnim(_float fTimeDelta)
{
}

CSapling* CSapling::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSapling*	pInstance = new CSapling(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSapling");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSapling::Clone(void* pArg)
{
	CSapling*	pInstance = new CSapling(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSapling");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSapling::Free()
{
	CMonster::Free();

	Safe_Release(m_pEffects);
}
