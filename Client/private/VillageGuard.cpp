#include "stdafx.h"
#include "..\public\VillageGuard.h"
#include "GameInstance.h"
#include "Bone.h"

CVillageGuard::CVillageGuard(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CVillageGuard::CVillageGuard(const CVillageGuard & rhs)
	: CMonster(rhs)
{
}

HRESULT CVillageGuard::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CVillageGuard::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(1.f), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -8.f;
		m_Desc.WorldMatrix._43 = -0.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();	
	m_bRotable = true;
	
	m_pWeaponBone = m_pModelCom->Get_BonePtr("Weapon_Root_Jnt");

	return S_OK;
}

HRESULT CVillageGuard::Late_Initialize(void * pArg)
{
	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();
	
	{
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = { 0.f, 0.f, 0.f };
		PxCapsuleDesc.fRadius = 0.5f;
		PxCapsuleDesc.fHalfHeight = 0.6f;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 10.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_MONSTER));
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.6f, 0.f));
	}

	{
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = { 0.f, 0.f, 0.f };
		PxCapsuleDesc.fRadius = 0.12f;
		PxCapsuleDesc.fHalfHeight = 0.6f;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 10.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));

		CBone* pBone = m_pModelCom->Get_BonePtr("Weapon_Root_Jnt");

		_matrix	SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		// SocketMatrix = XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z) * SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(m_szCloneObjectTag, mat);

		m_pTransformCom->Add_Collider(m_szCloneObjectTag, g_IdentityFloat4x4);
	}

	_matrix matPivot = XMMatrixTranslation(-0.44f, -1.27f, 0.89f) * XMMatrixRotationX(XMConvertToRadians(91.f)) * XMMatrixRotationZ(XMConvertToRadians(116.510f));
	XMStoreFloat4x4(&m_vPivotMatrix, matPivot);
	
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));
	
	return S_OK;
}

void CVillageGuard::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);
	
	Update_Collider(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CVillageGuard::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	
	if (m_pRendererCom && m_bSpawn)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CVillageGuard::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if(i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_OpacityTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",AO_R_M_O);
		}
		else
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
		}

	}
	return S_OK;
}

HRESULT CVillageGuard::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CVillageGuard::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
}

void CVillageGuard::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("VillageGuard Animation & State");

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

void CVillageGuard::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();

	// shader Value ����
}

HRESULT CVillageGuard::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CVillageGuard::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CVillageGuard::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.Tick([this](_float fTimeDelta)
	{
		
	})
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
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(WISPIN);
		m_pModelCom->Set_AnimIndex(WISPIN);
	})
		.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pUIHPBar->Set_Active(true);
		m_bSpawn = true;
	})
		.AddTransition("READY_SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pEnemyWisp->IsActiveState();
	})

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(IDLE);
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("IDLE to JUMP_BACK", "JUMP_BACK")
		.Predicator([this]()
	{
		return DistanceTrigger(0.8f);
	})
		.AddTransition("IDLE to BLOCK_ATTACK", "BLOCK_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(3.f);
	})
		.AddTransition("IDLE to WALL_JUMP_ATTACK", "WALL_JUMP_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(4.f);
	})
		.AddTransition("IDLE to ROLL_ATTACK", "ROLL_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(6.f);
	})				
		.AddTransition("IDLE to ATTACK", "ATTACK")
		.Predicator([this]()
	{
		return false;
	})
		.AddTransition("IDLE to CHASE", "CHASE")
		.Predicator([this]()
	{
		return !DistanceTrigger(10.f);
	})
		

		.AddState("CHASE")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(RUN);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 1.f);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("CHASE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f);
	})

		.AddState("BLOCK_ATTACK")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(BLOCK_ATTACK);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("BLOCK_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCK_ATTACK);
	})

		.AddState("ROLL_ATTACK")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(ROLLATTACK);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("ROLL_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ROLLATTACK);
	})


		.AddState("WALL_JUMP_ATTACK")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(WALLJUMP_ATTACK);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("ROLL_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(WALLJUMP_ATTACK);
	})
		
		.AddState("JUMP_BACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(JUMPBACK);
		m_pModelCom->Set_AnimIndex(JUMPBACK);
	})
		.OnExit([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE);
	})		
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("To PARRIED", "PARRIED")
		.Predicator([this]()
	{
		return IsParried();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("JUMP_BACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(JUMPBACK);
	})
	
		.AddState("BIND")
		.OnStart([this]()
	{
		Start_Bind(BIND);
	})
		.OnExit([this]()
	{	
		End_Bind();
	})
		.AddTransition("BIND to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BIND);
	})
		
		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(PARRIED);
		m_pModelCom->Set_AnimIndex(PARRIED);
	})
		.OnExit([this]()
	{
		m_bWeaklyHit = m_bStronglyHit = false;
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("PARRIED to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(PARRIED);
	})

		.AddState("DYING")
		.OnStart([this]()
	{
		Set_Dying(DEATH);
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

HRESULT CVillageGuard::SetUp_Components()
{
	__super::SetUp_Components();
		
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_VillageGuard", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/VillageWarrior_Uv_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/VillageWarrior_Uv_02_OPACITY.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/cv_body_uv_AO.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/cv_body_uv_Emissive.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/TaroBoss_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/TaroBoss_02_EMISSIVE.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/cv_staff_mask_uv_AO.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/cv_staff_mask_uv_EMISSIVE2.png")), E_FAIL);

	m_pModelCom->Set_RootBone("CorruptVillager");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_VillageGuard.json");


	return S_OK;
}

HRESULT CVillageGuard::SetUp_ShaderResources()
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

HRESULT CVillageGuard::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDying, sizeof(_bool)), E_FAIL);
	m_bDying && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

void CVillageGuard::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	
	_matrix	SocketMatrix = m_pWeaponBone->Get_OffsetMatrix() * m_pWeaponBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();	
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	SocketMatrix = m_vPivotMatrix * SocketMatrix;

	_float4x4 mat;
	XMStoreFloat4x4(&mat, SocketMatrix);
	m_pTransformCom->Update_Collider(m_szCloneObjectTag, mat);
}

CVillageGuard* CVillageGuard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVillageGuard*	pInstance = new CVillageGuard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CVillageGuard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVillageGuard::Clone(void* pArg)
{
	CVillageGuard*	pInstance = new CVillageGuard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CVillageGuard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillageGuard::Free()
{
	CMonster::Free();
}

void CVillageGuard::ImGui_PhysXValueProperty()
{
	ImGui::Text("WEAPON");
			
	float fWeaponTrans[3] = { m_vPivotTranslation.x, m_vPivotTranslation.y, m_vPivotTranslation.z };
	ImGui::DragFloat3("WeaponPivotTrans", fWeaponTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vPivotTranslation, fWeaponTrans, sizeof(_float3));


	float fWeaponRot[3] = { m_vPivotRotation.x, m_vPivotRotation.y, m_vPivotRotation.z };
	ImGui::DragFloat3("WeaponPivotRot", fWeaponRot, 0.5f, 0.01f, 365.0f);
	memcpy(&m_vPivotRotation, fWeaponRot, sizeof(_float3));
}
