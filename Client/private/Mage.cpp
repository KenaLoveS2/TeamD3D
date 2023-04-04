#include "stdafx.h"
#include "..\public\Mage.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Utile.h"
#include "Sticks01.h"
#include "FireBullet.h"

CMage::CMage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CMage::CMage(const CMage & rhs)
	: CMonster(rhs)
{
}

HRESULT CMage::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CMage::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(1.8f), E_FAIL); // Mage HpBar Position

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -12.f;
		m_Desc.WorldMatrix._43 = -12.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_pMageHaneBonePtr = m_pModelCom->Get_BonePtr("char_lf_index_a_jnt");

	Create_Sticks();

	m_bRotable = true;
	return S_OK;
}

HRESULT CMage::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(0.45f, 0.6f, 1.f);
		_float3 vPivotPos = _float3(0.f, 1.1f, 0.f);

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
		m_pTransformCom->Set_PxPivotScale(vPivotScale);
		m_pTransformCom->Set_PxPivot(vPivotPos);
	}

	// 무기
	{
		wstring WeaponPivot;
		m_vecColliderName.push_back(WeaponPivot);
		_float3 vWeaponPivot = _float3(-1.15f, 0.f, -1.15f);
		m_vecPivot.push_back(vWeaponPivot);
		_float3 vWeaponScalePivot = _float3(0.2f, 0.8f, 0.1f);
		m_vecPivotScale.push_back(vWeaponScalePivot);
		_float3 vWeaponRotPivot = _float3(1.65f, 0.f, 0.f);
		m_vecPivotRot.push_back(vWeaponRotPivot);

		m_vecColliderName[COLL_WEAPON] = m_szCloneObjectTag;
		m_vecColliderName[COLL_WEAPON] += L"Weapon";

		CBone* pBone = m_pModelCom->Get_BonePtr("Staff_Jnt_9");
		_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix =
			XMMatrixRotationX(m_vecPivotRot[COLL_WEAPON].x)
			* XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z)
			* SocketMatrix;

		_float4x4 pivotMatrix;
		XMStoreFloat4x4(&pivotMatrix, SocketMatrix);

		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_vecColliderName[COLL_WEAPON].c_str();
		PxCapsuleDesc.vPos = _float3(0.f, 5.f, 0.f);
		PxCapsuleDesc.fRadius = m_vecPivotScale[COLL_WEAPON].x;
		PxCapsuleDesc.fHalfHeight = m_vecPivotScale[COLL_WEAPON].y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 1.f;
		PxCapsuleDesc.fLinearDamping = 1.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false,COL_MONSTER_WEAPON));

		m_pTransformCom->Add_Collider(m_vecColliderName[COLL_WEAPON].c_str(), pivotMatrix);
		m_pRendererCom->Set_PhysXRender(true);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));
	

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pFireBullet = (CFireBullet*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FireBullet"), CUtile::Create_DummyString());
	assert(m_pFireBullet != nullptr && "CMage::Initialize()");
	m_pFireBullet->Set_OwnerMonsterPtr(this);
	m_pFireBullet->Late_Initialize(nullptr);

	return S_OK;
}

void CMage::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
	if(m_bSpawn) AdditiveAnim(fTimeDelta);

	m_pFireBullet->Tick(fTimeDelta);
}

void CMage::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;
	
	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bReadySpawn)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	m_pFireBullet->Late_Tick(fTimeDelta);
}

HRESULT CMage::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		if (i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			//m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_MaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
		}

		if(i == 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
		}
	}
	return S_OK;
}

HRESULT CMage::RenderShadow()
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

void CMage::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	if (ImGui::Button("SPAWN"))
		m_bSpawn = true;
}

void CMage::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Mage Animation & State");

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

void CMage::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();

	// shader Value 조절
}

void CMage::ImGui_PhysXValueProperty()
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

HRESULT CMage::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CMage::Push_EventFunctions()
{
	 CMonster::Push_EventFunctions();

	TurnOnFireBullet(true, 0.0f);
}

HRESULT CMage::SetUp_State()
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
		m_pModelCom->ResetAnimIdx_PlayTime(ENTER);

		m_pModelCom->Set_AnimIndex(ENTER);
	})		
		.AddTransition("SPAWN to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ENTER);
	})

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_fIdletoAttackTime = 0.f;
		m_iIdletoDash++;
		Set_AttackType();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdletoAttackTime += fTimeDelta;
		m_pModelCom->Set_AnimIndex(IDLE);
		Tick_Attack(fTimeDelta);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.OnExit([this]()
	{
		Reset_Attack();
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
		.AddTransition("IDLE to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("IDLE to ENTER", "ENTER")
		.Predicator([this]()
	{
		return	m_iIdletoDash >= 3;
	})
		.AddTransition("IDLE to DASH" , "DASH")
		.Predicator([this]()
	{
		return DistanceTrigger(1.5f);
	})
		.AddTransition("IDLE to SUMMON", "SUMMON")
		.Predicator([this]()
	{
		return	TimeTrigger(m_fIdletoAttackTime, 3.f) &&  m_bRealAttack && m_bSummonAttack;
	})
		.AddTransition("IDLE to RANGEDATTACK", "RANGEDATTACK")
		.Predicator([this]()
	{
		return	TimeTrigger(m_fIdletoAttackTime, 3.f) && m_bRealAttack && m_bRangedAttack && m_pFireBullet->IsActiveState() == false;
	})
		.AddTransition("IDLE to CLOSEATTACK", "CLOSEATTACK")
		.Predicator([this]()
	{
		return	TimeTrigger(m_fIdletoAttackTime, 3.f) && m_bRealAttack && m_bCloseAttack;
	})
		.AddTransition("IDLE to CLOSEATTACK", "CLOSEATTACK")
		.Predicator([this]()
	{
		return	TimeTrigger(m_fIdletoAttackTime, 3.f) && m_bRealAttack && m_bCloseAttack;
	})
		
		.AddState("SUMMON")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SUMMON);
		m_pModelCom->Set_AnimIndex(SUMMON); // 3마리 생성
		Summon();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("SUMMON to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("SUMMON to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(SUMMON);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("RANGEDATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(RANGEDATTACK);
		m_pModelCom->Set_AnimIndex(RANGEDATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("RANGEDATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("RANGEDATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
			return AnimFinishChecker(RANGEDATTACK);
	})
	

		.AddState("CLOSEATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(CLOSEATTACK);
		m_pModelCom->Set_AnimIndex(CLOSEATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("CLOSEATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("CLOSEATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(CLOSEATTACK);
	})
	

		.AddState("DASH")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(DASH_B);
		m_pModelCom->ResetAnimIdx_PlayTime(DASH_F);
		m_pModelCom->ResetAnimIdx_PlayTime(DASH_L);
		m_pModelCom->ResetAnimIdx_PlayTime(DASH_R);

		if (m_PlayerLookAt_Dir == RIGHT)
			m_pModelCom->Set_AnimIndex(DASH_L);
		if (m_PlayerLookAt_Dir == LEFT)
			m_pModelCom->Set_AnimIndex(DASH_R);
		if (m_PlayerLookAt_Dir == FRONT)
			m_pModelCom->Set_AnimIndex(DASH_B);
		if (m_PlayerLookAt_Dir == BACK)
			m_pModelCom->Set_AnimIndex(DASH_F);
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
		.AddTransition("DASH to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("DASH to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(DASH_L) || 
			AnimFinishChecker(DASH_R) || 
			AnimFinishChecker(DASH_B) ||
			AnimFinishChecker(DASH_F);
	})
	
		
		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(PARRY);
		m_pModelCom->Set_AnimIndex(PARRY);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})		
		.AddTransition("PARRIED to ENTER", "ENTER")
		.Predicator([this]()
	{
		return AnimFinishChecker(PARRY);
	})
		
		// 어느 타이밍에 패링이 되는지?
		.AddState("ENTER")
		.OnStart([this]()
	{
		m_iIdletoDash = 0;
		m_pModelCom->ResetAnimIdx_PlayTime(EXIT);
		m_pModelCom->Set_AnimIndex(EXIT);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("ENTER to EXIT", "EXIT")
		.Predicator([this]()
	{
		return AnimFinishChecker(EXIT);
	})
		
		// 어느 타이밍에 패링이 되는지?
		.AddState("EXIT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ENTER);
		m_pModelCom->Set_AnimIndex(ENTER);
		_float4 vPos = CUtile::Get_RandomVector(m_vKenaPos - _float3(2.f, 2.f, 2.f), m_vKenaPos - _float3(2.f, 2.f, 2.f));
		_float4 vPrePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPos.y = vPrePos.y;
		vPos.w = 1.f;
		m_pTransformCom->Set_Position(vPos);
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("EXIT to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("EXIT to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ENTER);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("TAKEDAMAGE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_B);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_L);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_R);

		if (m_PlayerLookAt_Dir == FRONT)
			m_pModelCom->Set_AnimIndex(STAGGER);
		else if (m_PlayerLookAt_Dir == BACK)
			m_pModelCom->Set_AnimIndex(STAGGER_B);
		else if (m_PlayerLookAt_Dir == LEFT)
			m_pModelCom->Set_AnimIndex(STAGGER_L);
		else if (m_PlayerLookAt_Dir == RIGHT)
			m_pModelCom->Set_AnimIndex(STAGGER_R);
	})
		.OnExit([this]()
	{
		// 맞는 애니메이션일때도 맞는가?
		m_bStronglyHit = false;
		Reset_Attack();
	})
		.AddTransition("To BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("TAKEDAMAGE to DASH", "DASH")
		.Predicator([this]()
	{
		return AnimFinishChecker(STAGGER) || AnimFinishChecker(STAGGER_L) ||
				AnimFinishChecker(STAGGER_B) || AnimFinishChecker(STAGGER_R);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("BIND")
		.OnStart([this]()
	{
		Start_Bind(COMMAND);
	})
		.OnExit([this]()
	{	
		Reset_Attack();
		End_Bind();
	})
		.AddTransition("BIND to ENTER", "ENTER")
		.Predicator([this]()
	{
		return AnimFinishChecker(COMMAND);
	})

		.AddState("DYING")
		.OnStart([this]()
	{
		Set_Dying(DEATH);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fDissolveTime += fTimeDelta * 0.2f;
		m_fDissolveTime = m_fDissolveTime >= 1.f ? 1.f : m_fDissolveTime;
	})
		.AddTransition("DYING to DEATH", "DEATH")
		.Predicator([this]()
	{
		return AnimFinishChecker(DEATH) && m_fDissolveTime >= 1.f;
	})


		.AddState("DEATH")
		.OnStart([this]()
	{
		Clear_Death();
	})		
		.Build();

	return S_OK;
}

HRESULT CMage::SetUp_Components()
{
	__super::SetUp_Components();
		
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Mage", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Mage/WoodMage_UV_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Mage/WoodMage_UV_02_ALPHA.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Mage/WoodMage_UV_01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Mage/WoodMage_UV_01_CRYSTALMASK.png")), E_FAIL);
	
	m_pModelCom->Set_RootBone("WoodMage");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_Mage.json");


	return S_OK;
}

HRESULT CMage::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
	m_bDissolve&& Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CMage::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

void CMage::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	{
		CBone* pBone = m_pModelCom->Get_BonePtr("Staff_Jnt_9");
		_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix = XMMatrixRotationX(m_vecPivotRot[COLL_WEAPON].x)
			* XMMatrixRotationY(m_vecPivotRot[COLL_WEAPON].y)
			* XMMatrixRotationZ(m_vecPivotRot[COLL_WEAPON].z)
			*XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z)
			* SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(m_vecColliderName[COLL_WEAPON].c_str(), mat);
	}
}

void CMage::AdditiveAnim(_float fTimeDelta)
{
	_float fRatio = Calc_PlayerLookAtDirection();
	if (fRatio >= 0.f)
	{
		fRatio *= 1.5f;
		m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOKLEFT);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_Mage.ao");
	}
	else
	{
		fRatio *= -1.5f;
		m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOKRIGHT);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_Mage.ao");
	}

	if (m_bWeaklyHit)
	{
		m_pModelCom->Set_AdditiveAnimIndexForMonster(TAKEDAMAGE);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Mage.ao");

		if (AnimFinishChecker(TAKEDAMAGE))
			m_bWeaklyHit = false;
	}
}

void CMage::Set_AttackType()
{
	//	TODO : stick summon
	if(m_SticksList.empty() || !m_bFirstAttack)
	{
		m_iAttackType = AT_SUMMON;
		m_bFirstAttack = true;
	}
	else	if (DistanceTrigger(3.f))
		m_iAttackType = AT_CLOSEATTACK;
	else
		m_iAttackType = AT_RANGEDATTACK;

	switch (m_iAttackType)
	{
	case AT_CLOSEATTACK:
		m_bCloseAttack = true;
		break;
	case AT_RANGEDATTACK:
		m_bRangedAttack = true;
		break;
	case AT_SUMMON:
		m_bSummonAttack = true;
		break;
	default:
		break;
	}
}

void CMage::Reset_Attack()
{
	 m_iAttackType = ATTACKTYPE_END;
	 m_bRealAttack = false;
	 m_bCloseAttack = false;
	 m_bRangedAttack = false;
	 m_bSummonAttack = false;
}

void CMage::Tick_Attack(_float fTimeDelta)
{
	// 얘는 따라가는거 없음
	switch (m_iAttackType)
	{
	case AT_CLOSEATTACK:
		m_bRealAttack = true;
		break;
	case AT_RANGEDATTACK:
		m_bRealAttack = true;
		break;
	case AT_SUMMON:
		m_bRealAttack = true;
		break;
	default:
		break;
	}
}

void CMage::Create_Sticks()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < MAGE_STICK_COUNT; i++)
	{
		pGameInstance->Clone_AnimObject(
			g_LEVEL, 
			TEXT("Layer_Monster"), 
			TEXT("Prototype_GameObject_Sticks01"), 
			CUtile::Create_DummyString(), 
			nullptr, 
			(CGameObject**)&m_pSticks[i]);

		m_pSticks[i]->Set_DeathFlag(true);
		// m_pSticks[i]->Late_Initialize(nullptr);
	}
	RELEASE_INSTANCE(CGameInstance)
}

void CMage::Summon()
{
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vOffset[MAGE_STICK_COUNT] = {
		_float4(0.f, 0.f, 3.f, 0.f),
		_float4(3.f, 0.f, 0.f, 0.f),
		_float4(-3.f, 0.f, 0.f, 0.f),
	};

	for (_uint i = 0; i < MAGE_STICK_COUNT; i++)
	{
		m_pSticks[i]->Spawn_ByMaster(this, vPos + vOffset[i]);
		m_SticksList.push_back(m_pSticks[i]);
	}
}

void CMage::TurnOnFireBullet(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CMage::TurnOnFireBullet);
		return;
	}
	
	_matrix SocketMatrix = m_pMageHaneBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();		
	m_pFireBullet->Execute_Create(SocketMatrix.r[3]);
}

CMage* CMage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMage*	pInstance = new CMage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMage::Clone(void* pArg)
{
	CMage*	pInstance = new CMage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMage::Free()
{
	CMonster::Free();

	Safe_Release(m_pFireBullet);
}

void CMage::Clear_ByMinion(CMonster* pMinion)
{
	for (auto iter = m_SticksList.begin(); iter != m_SticksList.end();)
	{
		if (*iter == pMinion)
			iter = m_SticksList.erase(iter);
		else
			iter++;
	}
}
