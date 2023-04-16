#include "stdafx.h"
#include "..\public\Sticks01.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Mage.h"
#include "Kena.h"

CSticks01::CSticks01(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CSticks01::CSticks01(const CSticks01 & rhs)
	: CMonster(rhs)
{
}

HRESULT CSticks01::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CSticks01::Initialize(void* pArg)
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
	{
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));		
	}	
	else
	{
		m_Desc.pGroupName = L"N/A";
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -15.f;
		m_Desc.WorldMatrix._43 = -15.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pModelCom->Set_AnimIndex(COMBATIDLE);
	m_pWeaponBone = m_pModelCom->Get_BonePtr("staff_skin8_jnt");
	
	/* Create MovementTrail */
	SetUp_MovementTrail();
		
	m_vFocusIconPosOffset = { 0.f, 1.6f, 0.f, 0.f };

	return S_OK;
}

HRESULT CSticks01::Late_Initialize(void * pArg)
{	
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(0.25f, 0.4f, 1.f);
		//_float3 vPivotScale = _float3(0.5f, 0.5f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.65f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = {0.f, 0.f, 0.f};
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
		_float3 vWeaponPivot = _float3(-0.55f,0.f, -1.15f);
		m_vecPivot.push_back(vWeaponPivot);
		_float3 vWeaponScalePivot = _float3(0.25f, 0.1f, 0.1f);

		m_vecPivotScale.push_back(vWeaponScalePivot);

		m_vecColliderName[COLL_WEAPON] = m_szCloneObjectTag;
		m_vecColliderName[COLL_WEAPON] += L"Weapon";

		CBone* pBone = m_pModelCom->Get_BonePtr("staff_skin8_jnt");
		_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix =  XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z)
			* SocketMatrix;

		_float4x4 pivotMatrix;
		XMStoreFloat4x4(&pivotMatrix, SocketMatrix);

		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = m_vecColliderName[COLL_WEAPON].c_str();
		PxSphereDesc.vPos = _float3(0.f,5.f,0.f);
		PxSphereDesc.fRadius = m_vecPivotScale[COLL_WEAPON].x;
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));

		m_pTransformCom->Add_Collider(m_vecColliderName[COLL_WEAPON].c_str(), pivotMatrix);
		m_pRendererCom->Set_PhysXRender(true);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));

	return S_OK;
}

void CSticks01::Tick(_float fTimeDelta)
{	
	if (m_bDeath) return;
		
	/* Update MovementTrail */
	Update_MovementTrail("char_weightSpine_c_jnt");

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	// m_bReadySpawn = true;
	if(m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	if (m_fHitStopTime <= 0.f)
		m_pModelCom->Play_Animation(fTimeDelta);
	else
	{
		m_fHitStopTime -= fTimeDelta;
		CUtile::Saturate<_float>(m_fHitStopTime, 0.f, 3.f);
	}

	AdditiveAnim(fTimeDelta);
}

void CSticks01::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bReadySpawn)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CSticks01::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_GlowTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_G);
	}
	return S_OK;
}

HRESULT CSticks01::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CSticks01::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	static _bool bSpawn = false;

	if (ImGui::Button("SPAWN"))
		bSpawn = true;

	/*
	if(bSpawn && m_pEnemyWisp->IsActiveState())
	{
		m_bSpawn = true;
	}
	*/
}

void CSticks01::ImGui_AnimationProperty()
{
	if(ImGui::CollapsingHeader("Sticks01"))
	{
		ImGui::BeginTabBar("Sticks01 Animation & State");

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

void CSticks01::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CSticks01::ImGui_PhysXValueProperty()		
{
	CMonster::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();
	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_float3 vPxPivot = 	m_pTransformCom->Get_vPxPivot();
	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	m_pTransformCom->Set_PxPivot(vPxPivot);

	_uint nActorListCount = static_cast<_uint>(m_pTransformCom->Get_ActorList()->size());

	ImGui::BulletText("ColliderLists");			// 무기
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
		// WEAPON
		_float3 vWeaponScalePivot = m_vecPivotScale[COLL_WEAPON];
		float fWeaponScale[3] = { vWeaponScalePivot.x, vWeaponScalePivot.y, vWeaponScalePivot.z };
		ImGui::DragFloat3("WeaponPivotScale", fWeaponScale, 0.01f, 0.01f, 100.0f);
		m_vecPivotScale[COLL_WEAPON].x = fWeaponScale[0];
		m_vecPivotScale[COLL_WEAPON].y = fWeaponScale[1];
		m_vecPivotScale[COLL_WEAPON].z = fWeaponScale[2];

		_float3 vWeaponPivot = m_vecPivot[0];
		float fWeaponPos[3] = { vWeaponPivot.x, vWeaponPivot.y, vWeaponPivot.z };
		ImGui::DragFloat3("WeaponPivotPos", fWeaponPos, 0.01f, -100.f, 100.0f);
		m_vecPivot[COLL_WEAPON].x = fWeaponPos[0];
		m_vecPivot[COLL_WEAPON].y = fWeaponPos[1];
		m_vecPivot[COLL_WEAPON].z = fWeaponPos[2];

		CPhysX_Manager::GetInstance()->Set_ActorScaling(m_vecColliderName[COLL_WEAPON].c_str(), m_vecPivotScale[COLL_WEAPON]);
	}
}

HRESULT CSticks01::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CSticks01::Push_EventFunctions()
{
	// CMonster::Push_EventFunctions();

	Play_AxeSound(true, 0.f);
	Play_ImpactSound(true, 0.f);
	Play_WalkSound(true, 0.f);
}

HRESULT CSticks01::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("RESURRECT")
		.AddState("RESURRECT")
		.Tick([this](_float fTimeDelta)
	{			
		if (!m_bSpawn)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(RESURRECT);
			m_pModelCom->Set_AnimIndex(RESURRECT);
		}	
	})
		.OnExit([this]()
	{		
		m_pEnemyWisp->IsActiveState();		
	})
		.AddTransition("RESURRECT to READY_SPAWN", "READY_SPAWN")
		.Predicator([this]()
	{
		return DistanceTrigger(m_fSpawnRange) || m_bSpawnByMaster || m_bGroupAwaken;
		// return AnimFinishChecker(RESURRECT) && m_bSpawn;
	})


		.AddState("READY_SPAWN")		
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE3], 0.8f);
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
		.AddTransition("READY_SPAWN to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return m_bWispEnd && m_fDissolveTime <= 0.f;
	})


		.AddState("COMBATIDLE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_CALM], 0.8f);
		m_fIdletoAttackTime = 0.f;
		Set_AFType();
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_fIdletoAttackTime += fTimeDelta;
		m_pModelCom->Set_AnimIndex(COMBATIDLE);
	})
		.OnExit([this]()
	{
		Reset_AF();
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
		.AddTransition("COMBATIDLE to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("COMBATIDLE to CHARGE", "CHEER")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f) &&  m_bCheer;
	})
		.AddTransition("COMBATIDLE to STRAFELEFT", "STRAFELEFT")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f) && m_bStrafeLeft;
	})
		.AddTransition("COMBATIDLE to STRAFERIGHT", "STRAFERIGHT")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f) && m_bStrafeRight;
	})

		.AddState("CHEER")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE2], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(CHEER);
		m_pModelCom->Set_AnimIndex(CHEER);
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
		.AddTransition("CHEER to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("CHEER to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(CHEER);
	})

		.AddState("STRAFELEFT")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE2], 0.7f);
		m_fIdletoAttackTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(STRAFELEFT);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Go_Left(fTimeDelta);
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
		.AddTransition("STRAFELEFT to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("STRAFELEFT to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime,3.f);
	})
	
		.AddState("STRAFERIGHT")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE2], 0.7f);
		m_fIdletoAttackTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(STRAFERIGHT);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Go_Right(fTimeDelta);
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
		.AddTransition("STRAFERIGHT to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("STRAFERIGHT to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 3.f);
	})

		.AddState("INTOCHARGE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.7f);
		m_bIsFightReady = true;
		m_pModelCom->ResetAnimIdx_PlayTime(INTOCHARGE);
		m_pModelCom->Set_AnimIndex(INTOCHARGE);
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
		.AddTransition("INTOCHARGE to CHARGE", "CHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(INTOCHARGE);
	})
		
		.AddState("CHARGE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE1], 0.7f);
		Set_AttackType();
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_pModelCom->Set_AnimIndex(CHARGE);
		Tick_Attack(fTimeDelta);
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
		.AddTransition("CHARGE to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("CHARGE to CHARGEATTACK", "CHARGEATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bChargeAttack;
	})
		.AddTransition("CHARGE to JUMPATTACK", "JUMPATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bJumpAttack;
	})
		.AddTransition("CHARGE to ATTACK1", "ATTACK1")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bAttack1;
	})
		.AddTransition("CHARGE to ATTACK2", "ATTACK2")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bAttack2;
	})
		.AddTransition("CHARGE to COMBOATTACK", "COMBOATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bComboAttack;
	})

		.AddState("CHARGEATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
		
		m_pModelCom->ResetAnimIdx_PlayTime(CHARGEATTACK);
		m_pModelCom->Set_AnimIndex(CHARGEATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("CHARGEATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("CHARGEATTACK to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(CHARGEATTACK);
	})

		.AddState("JUMPATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
		
		m_pModelCom->ResetAnimIdx_PlayTime(JUMPATTACK);
		m_pModelCom->Set_AnimIndex(JUMPATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("JUMPATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("JUMPATTACK to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(JUMPATTACK);
	})

		.AddState("ATTACK1")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
			
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK);
		m_pModelCom->Set_AnimIndex(ATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("ATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("ATTACK to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ATTACK);
	})

		.AddState("ATTACK2")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
		
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK2);
		m_pModelCom->Set_AnimIndex(ATTACK2);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("ATTACK2 to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("ATTACK2 to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ATTACK2);
	})

		.AddState("COMBOATTACK")
		.OnStart([this]()
	{	
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.7f);
		
		m_pModelCom->ResetAnimIdx_PlayTime(COMBOATTACK);
		m_pModelCom->Set_AnimIndex(COMBOATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("COMBOATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("COMBOATTACK to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(COMBOATTACK);
	})

		// 어느 타이밍에 패링이 되는지?
		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT], 1.f);
		m_pModelCom->ResetAnimIdx_PlayTime(PARRIED);
		m_pModelCom->Set_AnimIndex(PARRIED);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("PARRIED to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(PARRIED);
	})
		

		// 항상 폭탄이 날라오면 이렇게 뛰는가?
		.AddState("RECEIVEBOMB")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(RECEIVEBOMB);
		m_pModelCom->Set_AnimIndex(RECEIVEBOMB);
	})
		.AddTransition("RECEIVEBOMB to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(RECEIVEBOMB);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("TAKEDAMAGE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT], 1.f);

		if (m_PlayerLookAt_Dir == FRONT)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGEBIG);
			m_pModelCom->Set_AnimIndex(TAKEDAMAGEBIG);
		}
		else if (m_PlayerLookAt_Dir == BACK)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGEB);
			m_pModelCom->Set_AnimIndex(TAKEDAMAGEB);
		}
		else if (m_PlayerLookAt_Dir == LEFT)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGEL);
			m_pModelCom->Set_AnimIndex(TAKEDAMAGEL);
		}
		else if (m_PlayerLookAt_Dir == RIGHT)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(TAKEDAMAGER);
			m_pModelCom->Set_AnimIndex(TAKEDAMAGER);
		}
	})
		.OnExit([this]()
	{
		// 맞는 애니메이션일때도 맞는가?
		m_bStronglyHit = false;
		Reset_Attack();
	})		
		.AddTransition("TAKEDAMAGE to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TAKEDAMAGEL) || 
			AnimFinishChecker(TAKEDAMAGEB) || 
			AnimFinishChecker(TAKEDAMAGER) || 
			AnimFinishChecker(TAKEDAMAGEBIG);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})


		.AddState("DYING")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DIE], 1.f);
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
		
		if (m_pMaster && m_bSpawnByMaster)
		{
			m_bSpawnByMaster = false;
			m_pMaster->Clear_ByMinion(this);
			m_pMaster = nullptr;
		}
	})		
		.Build();

	return S_OK;
}

HRESULT CSticks01::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Sticks01", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/mask_01_AO_R_M.png")),E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/stick_01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/axe_01_AO_R_M.png")), E_FAIL);

	// 추후 GLOW를 위함.
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/mask_01_Glow.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/stick_corrupted_glow_1k.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/axe_corrupted_glow_1k.png")), E_FAIL);

	m_pModelCom->Set_RootBone("Sticks_01_RIG");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_Sticks01.json");

	return S_OK;
}

HRESULT CSticks01::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
	m_bDissolve && Bind_Dissolove(m_pShaderCom);
	
	return S_OK;
}

HRESULT CSticks01::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

void CSticks01::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
		
	_matrix SocketMatrix = m_pWeaponBone->Get_OffsetMatrix() * m_pWeaponBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z) * SocketMatrix;
	
	_float4x4 mat;
	XMStoreFloat4x4(&mat, SocketMatrix);
	m_pTransformCom->Update_Collider(m_vecColliderName[COLL_WEAPON].c_str(), mat);
}

void CSticks01::AdditiveAnim(_float fTimeDelta)
{
	_float fRatio = Calc_PlayerLookAtDirection();

	if(m_bWeaklyHit)
	{
		if (m_PlayerLookAt_Dir == BACK)
		{
			m_pModelCom->Set_AdditiveAnimIndexForMonster(TWITCH_B);
			m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Sticks01.ao");
		}
		else if(m_PlayerLookAt_Dir == FRONT)
		{
			m_pModelCom->Set_AdditiveAnimIndexForMonster(TWITCH_F);
			m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Sticks01.ao");
		}
		else if (m_PlayerLookAt_Dir == LEFT)
		{
			m_pModelCom->Set_AdditiveAnimIndexForMonster(TWITCH_L);
			m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Sticks01.ao");
		}
		else if (m_PlayerLookAt_Dir == RIGHT)
		{
			m_pModelCom->Set_AdditiveAnimIndexForMonster(TWITCH_R);
			m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Sticks01.ao");
		}

		m_bWeaklyHit = false;
	}
}

void CSticks01::Set_AttackType()
{
	m_iAttackType = rand() % ATTACKTYPE_END;

	m_bChargeAttack = false;
	m_bJumpAttack = false;
	m_bAttack1 = false;
	m_bAttack2 = false;
	m_bComboAttack = false;
	m_bRealAttack = false;
	
	// ATTACKTYPE 이 정해지면 거기까지 달려가기
	switch (m_iAttackType)
	{
	case AT_CHARGEATTACK:
		m_bChargeAttack = true;
		break;
	case AT_JUMPATTACK:
		m_bJumpAttack = true;
		break;
	case AT_ATTACK1:
		m_bAttack1 = true;
		break;
	case AT_ATTACK2:
		m_bAttack2 = true;
		break;
	case AT_COMBOATTACK:
		m_bComboAttack = true;
		break;	
	default:
		break;
	}
}

void CSticks01::Reset_Attack()
{
	m_bChargeAttack = false;
	m_bJumpAttack = false;
	m_bAttack1 = false;
	m_bAttack2 = false;
	m_bComboAttack = false;	
	m_iAttackType = ATTACKTYPE_END;
}

void CSticks01::Tick_Attack(_float fTimeDelta)
{
	switch (m_iAttackType)
	{
	case AT_CHARGEATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 5.f);
		if (DistanceTrigger(5.f))
			m_bRealAttack = true;
		break;
	case AT_JUMPATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 4.f);
		if (DistanceTrigger(4.f))
			m_bRealAttack = true;
		break;
	case AT_ATTACK1:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 3.f);
		if (DistanceTrigger(3.f))
			m_bRealAttack = true;
		break;
	case AT_ATTACK2:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 3.f);
		if (DistanceTrigger(3.f))
			m_bRealAttack = true;
		break;
	case AT_COMBOATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 3.f);
		if (DistanceTrigger(3.f))
			m_bRealAttack = true;
		break;	
	default:
		break;
	}
}

void CSticks01::Set_AFType()
{
	m_iAfterAttackType = rand() % 3;

	m_bCheer = false;
	m_bStrafeLeft = false;
	m_bStrafeRight = false;

	switch (m_iAfterAttackType)
	{
	case AF_CHEER:
		m_bCheer = true;
		break;
	case AF_STRAFELEFT:
		m_bStrafeLeft = true;
		break;
	case AF_STRAFERIGHT:
		m_bStrafeRight = true;
	default:
		break;
	}
}

void CSticks01::Reset_AF()
{
	m_bCheer = false;
	m_bStrafeLeft = false;
	m_bStrafeRight = false;
}

CSticks01* CSticks01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSticks01*	pInstance = new CSticks01(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSticks01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSticks01::Clone(void* pArg)
{
	CSticks01*	pInstance = new CSticks01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSticks01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSticks01::Free()
{
	CMonster::Free();
}

void CSticks01::Spawn_ByMaster(CMonster* pMaster, _float4 vPos)
{
	m_bDeath = false;

	m_Desc.WorldMatrix._41 = vPos.x;
	m_Desc.WorldMatrix._42 = vPos.y;
	m_Desc.WorldMatrix._43 = vPos.z;

	m_pTransformCom->Set_Position(vPos);
	m_pEnemyWisp->Set_Position(vPos);

	m_bSpawnByMaster = true;
	Reset_Attack();

	m_vKenaPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

	m_pMonsterStatusCom->Revive();
	m_pMaster = pMaster;
};

void CSticks01::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_Sticks_Tense.ogg"),
		TEXT("Mon_Sticks_Tense2.ogg"),
		TEXT("Mon_Sticks_Tense3.ogg"),
		TEXT("Mon_Sticks_Calm.ogg"),
		TEXT("Mon_Sticks_Attack.ogg"),
		TEXT("Mon_Sticks_Hurt.ogg"),
		TEXT("Mon_Sticks_Die.ogg"),
		TEXT("Mon_Sticks_Axe_Whoosh.ogg"),
		TEXT("Mon_Attack_Impact1.ogg"),
		TEXT("Mon_Walk_S.ogg"),
	};
	
	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}

void CSticks01::Play_AxeSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CSticks01::Play_AxeSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_AXE_WOOSH], 1.f);
}

void CSticks01::Play_ImpactSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CSticks01::Play_ImpactSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT], 0.2f);
}

void CSticks01::Play_WalkSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CSticks01::Play_WalkSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WALK], 1.f);
}