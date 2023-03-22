#include "stdafx.h"
#include "..\public\BranchTosser.h"
#include "GameInstance.h"
#include "BranchTosser_Weapon.h"
#include "BranchTosser_Tree.h"

CBranchTosser::CBranchTosser(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CBranchTosser::CBranchTosser(const CBranchTosser & rhs)
	: CMonster(rhs)
{
}

HRESULT CBranchTosser::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBranchTosser::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC GameObjectDesc;
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
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -8.f;
		m_Desc.WorldMatrix._43 = -8.f;
	}

	m_pModelCom->Set_AllAnimCommonType();

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pWeaponBone = m_pModelCom->Get_BonePtr("Branch_Projectile_jnt");

	return S_OK;
}

HRESULT CBranchTosser::Late_Initialize(void * pArg)
{

	// ¸öÅë
	{
		_float3 vPos = _float3(0.f, 5.f, -15.f);
		_float3 vPivotScale = _float3(0.45f, 0.1f, 0.1f);
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
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 20.f;
		PxCapsuleDesc.fLinearDamping = 100.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER));
		_float4x4 Temp;
		XMStoreFloat4x4(&Temp, XMMatrixTranslation(0.f, 0.5f, 0.f));
		m_pTransformCom->Add_Collider(m_szCloneObjectTag, Temp);
	}
	
	_float4 vPos = { m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f };
	m_pTree->Set_Position(vPos);
	m_pTree->Late_Initialize(nullptr);

	m_Desc.WorldMatrix._41 += 0.2f;
	m_Desc.WorldMatrix._42 += 3.2f;
	m_Desc.WorldMatrix._43 += 0.2f;

	m_pModelCom->Set_DurationRate(ATTACK, 0.6f);
	m_pModelCom->Set_DurationRate(DEATH, 3.f);

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));

	return S_OK;
}

void CBranchTosser::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_pTransformCom->LookAt(m_vKenaPos);

	for(_uint i=0; i<BRANCH_TOSSER_WEAPON_COUNT; i++)
		m_pWeapon[i]->Tick(fTimeDelta);

	m_pTree->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CBranchTosser::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);

	for (_uint i = 0; i<BRANCH_TOSSER_WEAPON_COUNT; i++)
		m_pWeapon[i]->Late_Tick(fTimeDelta);

	m_pTree->Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bSpawn)
	{	
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CBranchTosser::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
	}
	return S_OK;
}

HRESULT CBranchTosser::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CBranchTosser::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	// m_pWeapon->Imgui_RenderProperty();
}

void CBranchTosser::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("BranchTosser Animation & State");

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

void CBranchTosser::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();

	// shader Value Á¶Àý
}

void CBranchTosser::ImGui_PhysXValueProperty()
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

	_uint nActorListCount = static_cast<_uint>(m_pTransformCom->Get_ActorList()->size());

	ImGui::BulletText("ColliderLists");
	{
		static _int iSelect = -1;
		char** ppObjectTag = new char*[nActorListCount];
		_uint iTagLength = 0;
		_uint i = 0;
		for (auto& Pair : *m_pTransformCom->Get_ActorList())
			ppObjectTag[i++] = CUtile::WideCharToChar(Pair.pActorTag);
		ImGui::ListBox("Collider List", &iSelect, ppObjectTag, nActorListCount);
		if (iSelect != -1)
		{
			ImGui::BulletText("Current Collider Object : ");
			ImGui::SameLine();
			ImGui::Text(ppObjectTag[iSelect]);
		}
		for (_uint i = 0; i < nActorListCount; ++i)
			Safe_Delete_Array(ppObjectTag[i]);
		Safe_Delete_Array(ppObjectTag);
	}

	{
		ImGui::Text("WEAPON");
		// WEAPON
		_float3 vWeaponScalePivot = m_vecPivotScale[COLL_WEAPON];
		float fWeaponScale[3] = { vWeaponScalePivot.x, vWeaponScalePivot.y, vWeaponScalePivot.z };
		ImGui::DragFloat3("WeaponPivotScale", fWeaponScale, 0.01f, 0.01f, 100.0f);
		m_vecPivotScale[COLL_WEAPON].x = fWeaponScale[0];
		m_vecPivotScale[COLL_WEAPON].y = fWeaponScale[1];
		m_vecPivotScale[COLL_WEAPON].z = fWeaponScale[2];

		CPhysX_Manager::GetInstance()->Set_ActorScaling(m_vecColliderName[COLL_WEAPON].c_str(), m_vecPivotScale[COLL_WEAPON]);

		_float3 vWeaponRotPivot = m_vecPivotRot[COLL_WEAPON];
		float fWeaponRotPos[3] = { vWeaponRotPivot.x, vWeaponRotPivot.y, vWeaponRotPivot.z };
		ImGui::DragFloat3("WeaponPivotRot", fWeaponRotPos, 0.01f, -100.f, 100.0f);
		m_vecPivotRot[COLL_WEAPON].x = fWeaponRotPos[0];
		m_vecPivotRot[COLL_WEAPON].y = fWeaponRotPos[1];
		m_vecPivotRot[COLL_WEAPON].z = fWeaponRotPos[2];

		_float3 vWeaponPivot = m_vecPivot[COLL_WEAPON];
		float fWeaponPos[3] = { vWeaponPivot.x, vWeaponPivot.y, vWeaponPivot.z };
		ImGui::DragFloat3("WeaponPivotPos", fWeaponPos, 0.01f, -100.f, 100.0f);
		m_vecPivot[COLL_WEAPON].x = fWeaponPos[0];
		m_vecPivot[COLL_WEAPON].y = fWeaponPos[1];
		m_vecPivot[COLL_WEAPON].z = fWeaponPos[2];
	}
}

HRESULT CBranchTosser::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CBranchTosser::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CBranchTosser::SetUp_State()
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
		m_fSpawnRange = 7.f;
		return DistanceTrigger(m_fSpawnRange);
	})

		.AddState("READY_SPAWN")
		.OnExit([this]()
	{	
		m_bSpawn = true;
		m_pUIHPBar->Set_Active(true);
	})
		.AddTransition("READY_SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pEnemyWisp->IsActiveState();
	})

		.AddState("IDLE")
		.OnStart([this]()
	{		
		m_fIdleToAttack = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdleToAttack += fTimeDelta;
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
		.AddTransition("IDLE to ATTACK", "TAUNT")
		.Predicator([this]()
	{
		return m_iAttackCount >= BRANCH_TOSSER_WEAPON_COUNT;
	})
		.AddTransition("IDLE to ATTACK", "ATTACK")
		.Predicator([this]()
	{	
		return m_fIdleToAttack >= 1.5f && m_iAttackCount < BRANCH_TOSSER_WEAPON_COUNT;
	})
		

		.AddState("ATTACK")
		.OnStart([this]()
	{	
		m_pWeapon[m_iAttackCount]->Set_GoStraight(false);
		m_bRealAttack = true;
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK);
		m_pModelCom->Set_AnimIndex(ATTACK);
	})		
		.OnExit([this]()
	{
		m_pWeapon[m_iAttackCount++]->Set_GoStraight(true);
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
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("ATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ATTACK);
	})
		
		.AddState("TAUNT")
		.OnStart([this]()
	{
		m_iAttackCount = 0;		
		m_pModelCom->ResetAnimIdx_PlayTime(TAUNT4);
		m_pModelCom->Set_AnimIndex(TAUNT4);
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
		.AddTransition("TAUNT to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TAUNT4);
	})

		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGE);
		m_pModelCom->Set_AnimIndex(TAKEDAMAGE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("PARRIED to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TAKEDAMAGE);
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

		
		.AddState("DYING")
		.OnStart([this]()
	{
		Set_Dying(DEATH);		
		
		m_pTree->Set_ShaderPassIndex(5);
		m_pTree->Clear();

		for (_uint i = 0; i < BRANCH_TOSSER_WEAPON_COUNT; i++)
		{
			m_pWeapon[i]->Set_ShaderPassIndex(5);
			m_pWeapon[i]->Clear();
		}			
	})
		.AddTransition("DYING to DEATH", "DEATH")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEATH);
	})
		.AddState("DEATH")
		.OnStart([this]()
	{		
		Clear_Death();
	
	})		
		.Build();

	return S_OK;
}

HRESULT CBranchTosser::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_BranchTosser", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_BranshTosser.json");
	
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_Material(i, WJTextureType_AMBIENT_OCCLUSION, L"../Bin/Resources/Anim/Enemy/BranchTosser/bt_AO_R_M_1k.png");
		m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVE, L"../Bin/Resources/Anim/Enemy/BranchTosser/bt_EMISSIVE_1k.png");
	}

	m_pModelCom->Set_RootBone("BranchTosser");

	CBranchTosser_Weapon::MONSTERWEAPONDESC		WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CBranchTosser_Weapon::MONSTERWEAPONDESC));

	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Branch_Projectile_jnt");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	for (_uint i = 0; i < BRANCH_TOSSER_WEAPON_COUNT; i++)
	{
		m_pWeapon[i] = (CBranchTosser_Weapon*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BranchTosserWeapon"), 
			CUtile::Create_DummyString(L"BranchTosserWeapon", i), &WeaponDesc);

		assert(m_pWeapon[i] && "BranchTosser Weapon is nullptr");
		m_pWeapon[i]->Set_OwnerBranchTosser(this);
		m_pWeapon[i]->Late_Initialize(nullptr);
	}
				
	m_pTree = (CBranchTosser_Tree*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BranchTosserTree"), L"BranchTosserTree");
	assert(m_pTree && "BranchTosser Tree is nullptr");
	m_pTree->Set_OwnerBranchTosser(this);

	return S_OK;
}

HRESULT CBranchTosser::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDying, sizeof(_bool)), E_FAIL);
	m_bDying && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CBranchTosser::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;

	return S_OK;
}

void CBranchTosser::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

void CBranchTosser::AdditiveAnim(_float fTimeDelta)
{
	if(m_bWeaklyHit || m_bStronglyHit)
	{
		m_pModelCom->Set_AdditiveAnimIndexForMonster(TAKEDAMAGE);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_WoodKnight.ao");
	}
}

CBranchTosser* CBranchTosser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBranchTosser*	pInstance = new CBranchTosser(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBranchTosser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBranchTosser::Clone(void* pArg)
{
	CBranchTosser*	pInstance = new CBranchTosser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBranchTosser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBranchTosser::Free()
{
	CMonster::Free();

	for (_uint i = 0; i < BRANCH_TOSSER_WEAPON_COUNT; i++)
		Safe_Release(m_pWeapon[i]);
	
	Safe_Release(m_pTree);
}
