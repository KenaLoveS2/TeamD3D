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
	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.pGroupName = L"";
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -10.f;		
		m_Desc.WorldMatrix._42 = 0.f;
		m_Desc.WorldMatrix._43 = -10.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pWeaponBone = m_pModelCom->Get_BonePtr("Moth_RIG");
	m_pModelCom->Set_AnimIndex(TAUNT2);

	return S_OK;
}

HRESULT CMoth::Late_Initialize(void * pArg)
{	
	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = CUtile::Create_CombinedString(m_szCloneObjectTag, TEXT("Moth_Collider_0"));
		PxSphereDesc.vPos = {0.f, 0.f, 0.f};
		PxSphereDesc.fRadius = 0.5f;		
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.fMass = 10.f;
		PxSphereDesc.fLinearDamping = 1.f;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_MONSTER));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	}

	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = CUtile::Create_CombinedString(m_szCloneObjectTag, TEXT("Moth_Collider_1"));
		PxSphereDesc.vPos = { 0.f, 0.f, 0.f };
		PxSphereDesc.fRadius = 0.3f;
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.fMass = 10.f;
		PxSphereDesc.fLinearDamping = 10.f;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, g_IdentityFloat4x4);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));

	return S_OK;
}

void CMoth::Tick(_float fTimeDelta)
{
	/*m_bReadySpawn = true;
	Update_Collider(fTimeDelta);
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	return;*/

	if (m_bDeath) return;
	__super::Tick(fTimeDelta);
	
	Update_Collider(fTimeDelta);
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	
	m_pModelCom->Play_Animation(fTimeDelta);

}

void CMoth::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;
	CMonster::Late_Tick(fTimeDelta);
	
	if (m_pRendererCom && m_bReadySpawn)
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
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ROUGHNESS, "g_RoughnessTexture");
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
	// CMonster::Push_EventFunctions();
	Play_AttackSound(true, 0.f);
	Play_WingFlap1Sound(true, 0.f);
	Play_WingFlap2Sound(true, 0.f);
}

HRESULT CMoth::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Set_PositionY(m_vKenaPos.y + 1.4f);
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
		Start_Spawn();
	})
		.Tick([this](_float fTimeDelta)
	{
		Tick_Spawn(fTimeDelta);
	})
		.OnExit([this]()
	{
		End_Spawn();
	})
		.AddTransition("READY_SPAWN to SPAWN", "SPAWN")
		.Predicator([this]()
	{
		return m_bWispEnd && m_fDissolveTime <= 0.f;
	})


		.AddState("SPAWN")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE], 0.5f);
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
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_FLY], 0.5f);
		m_fIdletoAttackTime = 0.f;
		m_pTransformCom->LookAt(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(COMBATIDLE);
		m_pModelCom->Set_AnimIndex(COMBATIDLE);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdletoAttackTime += fTimeDelta;
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
		.AddTransition("IDLE To BACK", "BACK")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f) && DistanceTrigger(2.2f);
	})		
		.AddTransition("IDLE To MELEE_ATTACK", "MELEE_ATTACK")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f) && DistanceTrigger(4.f);
	})
		.AddTransition("IDLE To CHASE", "CHASE")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f) && !DistanceTrigger(3.9f);
	})
		
		.AddState("BACK")
		.OnStart([this]()
	{
		m_pTransformCom->Speed_Boost(true, 0.5f);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(WALK);
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Go_Backward(fTimeDelta);
	})
		.OnExit([this]()
	{
		m_pTransformCom->Speed_Boost(true, 1.f);	
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
		.AddTransition("BACK to DOWN", "DOWN")
		.Predicator([this]()
	{
		return !DistanceTrigger(3.1f);
	})

		.AddState("CHASE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(WALK);
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta);
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
		.AddTransition("CHASE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return DistanceTrigger(2.8f);
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
		.AddTransition("MELEE_ATTACK to MELEE_RETURN", "MELEE_RETURN")
		.Predicator([this]()
	{	
		return AnimFinishChecker(MELEEATTACK);
	})

		.AddState("MELEE_RETURN")
		.OnStart([this]()
	{	
		m_pModelCom->ResetAnimIdx_PlayTime(MELEEATTACK_RETURN);
		m_pModelCom->Set_AnimIndex(MELEEATTACK_RETURN);
	})
		.OnExit([this]()
	{
		m_bRealAttack = false;		
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
		.AddTransition("MELEE_RETURN to DOWN", "DOWN")
		.Predicator([this]()
	{
		return AnimFinishChecker(MELEEATTACK_RETURN);
	})


		.AddState("DOWN")
		.OnStart([this]()
	{
		m_pTransformCom->Speed_Boost(true, 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(WALK);
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Go_AxisNegY(fTimeDelta);
	})
		.OnExit([this]()
	{
		m_pTransformCom->Speed_Boost(true, 1.f);		
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
		.AddTransition("DOWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pTransformCom->Get_PositionY() < m_vKenaPos.y + 1.4f;
	})

		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(PARRY);
		m_pModelCom->Set_AnimIndex(PARRY);
	})
		.OnExit([this]()
	{	
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("PARRIED To DOWN", "DOWN")
		.Predicator([this]()
	{
		return AnimFinishChecker(PARRY);
	})

		.AddState("DYING")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DIE], 0.5f);
		Set_Dying(DEATHSIM);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fDissolveTime += fTimeDelta * 0.2f;
		m_fDissolveTime = m_fDissolveTime >= 1.f ? 1.f : m_fDissolveTime;
	})
		.AddTransition("DYING to DEATH", "DEATH")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEATHSIM) && m_fDissolveTime >= 1.f;
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

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Moth", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	m_pModelCom->Set_RootBone("Moth_RIG");

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

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
	m_bDissolve&& Bind_Dissolove(m_pShaderCom);

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
	m_pTransformCom->Tick(fTimeDelta);

	_matrix SocketMatrix = m_pWeaponBone->Get_OffsetMatrix() * m_pWeaponBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	
	_float4x4 mat;
	XMStoreFloat4x4(&mat, SocketMatrix);
	m_pTransformCom->Update_AllCollider(mat);
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

void CMoth::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_Moth_Tense.ogg"),
		TEXT("Mon_Moth_Fly.ogg"),
		TEXT("Mon_Moth_Attack.ogg"),
		TEXT("Mon_Moth_Die.ogg"),
		TEXT("Mon_Moth_WingFlap1.ogg"),
		TEXT("Mon_Moth_WingFlap2.ogg"),
	};

	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}

void CMoth::Play_AttackSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CMoth::Play_AttackSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
}

void CMoth::Play_WingFlap1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CMoth::Play_WingFlap1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WINGFLAP1], 0.4f);
}

void CMoth::Play_WingFlap2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CMoth::Play_WingFlap2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WINGFLAP2], 0.2f);
}