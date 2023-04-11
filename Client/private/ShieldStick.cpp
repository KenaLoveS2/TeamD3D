#include "stdafx.h"
#include "..\public\ShieldStick.h"
#include "GameInstance.h"
#include "Bone.h"
#include "ShieldStick_Weapon.h"

CShieldStick::CShieldStick(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CShieldStick::CShieldStick(const CShieldStick & rhs)
	: CMonster(rhs)
{
}

HRESULT CShieldStick::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CShieldStick::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.pGroupName = L"";
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -14.f;
		m_Desc.WorldMatrix._43 = -14.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CShieldStick::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(0.f, 5.f, -15.f);
		_float3 vPivotScale = _float3(0.25f, 0.25f, 0.1f);
		_float3 vPivotPos = _float3(0.f, 0.5f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = vPos;
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fLinearDamping = MONSTER_LINEAR_DAMING;
		PxCapsuleDesc.fAngularDamping = MONSTER_ANGULAR_DAMING;
		PxCapsuleDesc.fMass = MONSTER_MASS;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

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

void CShieldStick::Tick(_float fTimeDelta)
{	
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	if(m_bReadySpawn)
		m_pWeapon->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	if (m_fHitStopTime <= 0.f)
		m_pModelCom->Play_Animation(fTimeDelta);
	else
	{
		m_fHitStopTime -= fTimeDelta;
		CUtile::Saturate<_float>(m_fHitStopTime, 0.f, 3.f);
	}
}

void CShieldStick::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bReadySpawn)
	{	
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pWeapon->Late_Tick(fTimeDelta);
	}
}

HRESULT CShieldStick::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);


	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if(i==2) continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
	}
	return S_OK;
}

HRESULT CShieldStick::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (i == 2) continue;

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);
	}
		

	return S_OK;
}

void CShieldStick::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
}

void CShieldStick::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("ShieldStick Animation & State");

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

void CShieldStick::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CShieldStick::ImGui_PhysXValueProperty()
{
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

HRESULT CShieldStick::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CShieldStick::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CShieldStick::SetUp_State()
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
		return DistanceTrigger(m_fSpawnRange);
	})
		
		.AddState("READY_SPAWN")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.7f);
		Start_Spawn();
		m_pWeapon->Show_Dissolve();
	})
		.Tick([this](_float fTimeDelta)
	{
		Tick_Spawn(fTimeDelta);
	})
		.OnExit([this]()
	{
		End_Spawn();
	})
		.AddTransition("READY_SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_bWispEnd && m_fDissolveTime <= 0.f;
	})

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_CALM], 0.7f);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.OnExit([this]()
	{

	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("IDLE to TAKEDAMAGE", "TAKEDAMAGE_OR_PARRIED")
		.Predicator([this]()
	{
		return m_bStronglyHit || IsParried();
	})		
		.AddTransition("IDLE to JUMPBACK", "JUMPBACK")
		.Predicator([this]()
	{
		return DistanceTrigger(2.f);
	})
		.AddTransition("IDLE to CHARGE_ATTACK", "CHARGE_ATTACK")
		.Predicator([this]()
	{
		return DistanceTrigger(3.5f);
	})
		.AddTransition("IDLE to CHASE", "CHASE")
		.Predicator([this]()
	{
		return DistanceTrigger(10.f);
	})
	
		
		.AddState("CHASE")
		.OnStart([this]()
	{	
		m_pModelCom->ResetAnimIdx_PlayTime(UNARMED_WALK);
		m_pModelCom->Set_AnimIndex(WALK);
	})	
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 0.5f);
		m_pModelCom->Set_AnimIndex(WALK);

		if(AnimFinishChecker(WALK))
			m_pModelCom->ResetAnimIdx_PlayTime(WALK);
	})		
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("CHASE to TAKEDAMAGE_OR_PARRIED", "TAKEDAMAGE_OR_PARRIED")
		.Predicator([this]()
	{
		return m_bStronglyHit || IsParried();
	})
		.AddTransition("CHASE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return DistanceTrigger(2.5f);
	})
	

		.AddState("CHARGE_ATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(CHARGEATTACK);
		m_pModelCom->Set_AnimIndex(CHARGEATTACK);
		m_bRealAttack = true;
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
		.AddTransition("CHARGE_ATTACK to TAKEDAMAGE_OR_PARRIED", "TAKEDAMAGE_OR_PARRIED")
		.Predicator([this]()
	{
		return m_bStronglyHit || IsParried();
	})
		.AddTransition("CHARGE_ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})
	

		.AddState("JUMPBACK")
		.OnStart([this]()
	{
		// m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(JUMPBACK);
		m_pModelCom->Set_AnimIndex(JUMPBACK);		
	})	
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("JUMPBACK to TAKEDAMAGE_OR_PARRIED", "TAKEDAMAGE_OR_PARRIED")
		.Predicator([this]()
	{
		return m_bStronglyHit || IsParried();
	})
		.AddTransition("JUMPBACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})
		

		.AddState("TAKEDAMAGE_OR_PARRIED")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(INTOCHARGE);
		m_pModelCom->Set_AnimIndex(INTOCHARGE);
	})
		.OnExit([this]()
	{	
		m_bStronglyHit = false;		
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})		
		.AddTransition("TAKEDAMAGE_OR_PARRIED to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})
				
		.AddState("DYING")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DIE], 0.7f);
		Set_Dying(HAEDSHOT);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fDissolveTime += fTimeDelta * 0.2f;
		m_fDissolveTime = m_fDissolveTime >= 1.f ? 1.f : m_fDissolveTime;
	})
		.AddTransition("DYING to DEATH", "DEATH")
		.Predicator([this]()
	{
		return AnimFinishChecker(HAEDSHOT) && m_fDissolveTime >= 1.f;
	})


		.AddState("DEATH")
		.OnStart([this]()
	{
		Clear_Death();
	})
		.Build();

	return S_OK;
}

HRESULT CShieldStick::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_ShieldStick", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_ShieldSticks.json");
		
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, L"../Bin/Resources/Anim/Enemy/ShieldSticks/stick_03_AO_R_M.png");
	m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, L"../Bin/Resources/Anim/Enemy/ShieldSticks/mask_03_AO_R_M.png");

	m_pModelCom->Set_RootBone("Sticks_03_RIG");

	CMonsterWeapon::MONSTERWEAPONDESC		WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CMonsterWeapon::MONSTERWEAPONDESC));

	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Shield_Root_Jnt");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_pWeapon = (CShieldStick_Weapon*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ShieldStickWeapon"), L"ShieldStickWeapon", &WeaponDesc);
	assert(m_pWeapon && "ShieldStick Weapon is nullptr");
	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CShieldStick::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bReadySpawn, sizeof(_bool)), E_FAIL);
	m_bReadySpawn && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CShieldStick::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CShieldStick::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

void CShieldStick::AdditiveAnim(_float fTimeDelta)
{
}

CShieldStick* CShieldStick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShieldStick*	pInstance = new CShieldStick(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShieldStick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShieldStick::Clone(void* pArg)
{
	CShieldStick*	pInstance = new CShieldStick(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CShieldStick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShieldStick::Free()
{
	CMonster::Free();
	Safe_Release(m_pWeapon);
}

void CShieldStick::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_ShieldStick_Calm.ogg"),
		TEXT("Mon_ShieldStick_Tense.ogg"),
		TEXT("Mon_ShieldStick_Attack.ogg"),
		TEXT("Mon_ShieldStick_Hurt.ogg"),
		TEXT("Mon_ShieldStick_Die.ogg"),		
	};

	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}