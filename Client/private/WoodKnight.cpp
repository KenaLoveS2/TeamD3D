#include "stdafx.h"
#include "..\public\WoodKnight.h"
#include "GameInstance.h"
#include "Bone.h"
#include "RotForMonster.h"

CWoodKnight::CWoodKnight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CWoodKnight::CWoodKnight(const CWoodKnight & rhs)
	: CMonster(rhs)
{
}

HRESULT CWoodKnight::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CWoodKnight::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(2.f), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if(pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	else
	{
		m_Desc.pGroupName = L"";
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -17.f;
		m_Desc.WorldMatrix._43 = -17.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_bRotable = true;
	m_vRotIconPosOffset = { 0.f, 1.5f, 0.f, 0.f };

	return S_OK;
}

HRESULT CWoodKnight::Late_Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(0.6f, 0.7f, 1.f);
		_float3 vPivotPos = _float3(0.f, 1.3f, 0.f);

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

	// 무기
	{
		wstring WeaponPivot;
		m_vecColliderName.push_back(WeaponPivot);
		_float3 vWeaponPivot = _float3(-1.65f, 0.f, -1.75f);
		m_vecPivot.push_back(vWeaponPivot);
		_float3 vWeaponScalePivot = _float3(0.25f, 0.8f, 0.1f);
		m_vecPivotScale.push_back(vWeaponScalePivot);
		_float3 vWeaponRotPivot = _float3(1.6f, 0.f, 0.f);
		m_vecPivotRot.push_back(vWeaponRotPivot);

		m_vecColliderName[COLL_WEAPON] = m_szCloneObjectTag;
		m_vecColliderName[COLL_WEAPON] += L"Weapon";

		CBone* pBone = m_pModelCom->Get_BonePtr("char_sword_jnt2");
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

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));

		m_pTransformCom->Add_Collider(m_vecColliderName[COLL_WEAPON].c_str(), pivotMatrix);
		m_pRendererCom->Set_PhysXRender(true);
	}

	//// 무기
	{
		wstring WeaponPivot;
		m_vecColliderName.push_back(WeaponPivot);
		_float3 vWeaponPivot = _float3(0.85f, 0.03f, -1.f);
		m_vecPivot.push_back(vWeaponPivot);
		_float3 vWeaponScalePivot = _float3(0.15f, 0.2f, 0.1f);
		m_vecPivotScale.push_back(vWeaponScalePivot);
		_float3 vWeaponRotPivot = _float3(-1.1f, 0.f, 0.f);
		m_vecPivotRot.push_back(vWeaponRotPivot);

		m_vecColliderName[COLL_PUNCH] = m_szCloneObjectTag;
		m_vecColliderName[COLL_PUNCH] += L"Punch";

		CBone* pBone = m_pModelCom->Get_BonePtr("char_lf_wristWeight_jnt");
		_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix =
			XMMatrixRotationX(m_vecPivotRot[COLL_PUNCH].x)
			* XMMatrixRotationY(m_vecPivotRot[COLL_PUNCH].y)
			* XMMatrixRotationZ(m_vecPivotRot[COLL_PUNCH].z)
			* XMMatrixTranslation(m_vecPivot[COLL_PUNCH].x, m_vecPivot[COLL_PUNCH].y, m_vecPivot[COLL_PUNCH].z)
			* SocketMatrix;

		_float4x4 pivotMatrix;
		XMStoreFloat4x4(&pivotMatrix, SocketMatrix);

		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_vecColliderName[COLL_PUNCH].c_str();
		PxCapsuleDesc.vPos = _float3(0.f, 5.f, 0.f);
		PxCapsuleDesc.fRadius = m_vecPivotScale[COLL_PUNCH].x;
		PxCapsuleDesc.fHalfHeight = m_vecPivotScale[COLL_PUNCH].y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 1.f;
		PxCapsuleDesc.fLinearDamping = 1.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));

		m_pTransformCom->Add_Collider(m_vecColliderName[COLL_PUNCH].c_str(), pivotMatrix);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pEnemyWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f));

	return S_OK;
}

void CWoodKnight::Tick(_float fTimeDelta)
{		
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	// m_bReadySpawn = true;
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CWoodKnight::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bReadySpawn)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CWoodKnight::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);



	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if( i == 3)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",AO_R_M);
		}
		else
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveMaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",AO_R_M_EEM);
		}
	}
	return S_OK;
}

HRESULT CWoodKnight::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CWoodKnight::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	if (ImGui::Button("SPAWN"))
		m_bSpawn = true;

	if (ImGui::Button("BLOCK"))
		m_bBlock = true;
}

void CWoodKnight::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("WoodKnight Animation & State");

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

void CWoodKnight::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();

	// shader Value 조절
}

void CWoodKnight::ImGui_PhysXValueProperty()
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

	{
		ImGui::Text("PUNCH");

		// PUNCH
		_float3 vScalePivot = m_vecPivotScale[COLL_PUNCH];
		float fScale[3] = { vScalePivot.x, vScalePivot.y, vScalePivot.z };
		ImGui::DragFloat3("PunchPivotScale", fScale, 0.01f, 0.01f, 100.0f);
		m_vecPivotScale[COLL_PUNCH].x = fScale[0];
		m_vecPivotScale[COLL_PUNCH].y = fScale[1];
		m_vecPivotScale[COLL_PUNCH].z = fScale[2];

		CPhysX_Manager::GetInstance()->Set_ActorScaling(m_vecColliderName[COLL_PUNCH].c_str(), m_vecPivotScale[COLL_PUNCH]);

		_float3 vRotPivot = m_vecPivotRot[COLL_PUNCH];
		float fRotPos[3] = { vRotPivot.x, vRotPivot.y, vRotPivot.z };
		ImGui::DragFloat3("PunchPivotRot", fRotPos, 0.01f, -100.f, 100.0f);
		m_vecPivotRot[COLL_PUNCH].x = fRotPos[0];
		m_vecPivotRot[COLL_PUNCH].y = fRotPos[1];
		m_vecPivotRot[COLL_PUNCH].z = fRotPos[2];

		_float3 vPivot = m_vecPivot[COLL_PUNCH];
		float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
		ImGui::DragFloat3("PunchPivotPos", fPos, 0.01f, -100.f, 100.0f);
		m_vecPivot[COLL_PUNCH].x = fPos[0];
		m_vecPivot[COLL_PUNCH].y = fPos[1];
		m_vecPivot[COLL_PUNCH].z = fPos[2];
	}
}

HRESULT CWoodKnight::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CWoodKnight::Push_EventFunctions()
{
	// CMonster::Push_EventFunctions();

	Play_WoooshSound(true, 0.f);
	Play_WalkSound(true, 0.f);
	Play_SlamSound(true, 0.f);
	Play_CatchSound(true, 0.f);	
	Play_HurtSound(true, 0.f);
	LookAt_Kena(true, 0.f);

	Stop_BindRotAttackSound(true, 0.f);
}

HRESULT CWoodKnight::SetUp_State()
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
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(SLEEP);
		m_pModelCom->Set_AnimIndex(SLEEP);
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
		.AddTransition("READY_SPAWN to ALERT", "ALERT")
		.Predicator([this]()	
	{
		return m_bWispEnd && m_fDissolveTime <= 0.f;
	})
		
		.AddState("ALERT")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_TENSE], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(ALERT);
		m_pModelCom->Set_AnimIndex(ALERT);			
	})
		.AddTransition("ALERT to CHARGEATTACK", "CHARGEATTACK")
		.Predicator([this]()
	{
		return AnimFinishChecker(ALERT);
	})

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IDLE], 0.7f);
		
		m_fIdletoAttackTime = m_fIdletoAttackTime * m_bBlock;
		m_bBlock = false;			
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdletoAttackTime += fTimeDelta;
		m_pModelCom->Set_AnimIndex(IDLE);

		if (TimeTrigger(m_fIdletoAttackTime, 1.f) == false && m_pKena->Get_State(CKena::STATE_ATTACK) && DistanceTrigger(2.f))
			m_bBlock = true;
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("IDLE to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("IDLE to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("IDLE to BLOCK_INTO", "BLOCK_INTO")
		.Predicator([this]()
	{
		return m_bBlock;
	})
		.AddTransition("IDLE to WALK", "WALK")
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 1.f);
	})
		

		.AddState("BLOCK_INTO")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCK_INTO);
		m_pModelCom->Set_AnimIndex(BLOCK_INTO);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("BLOCK_INTO to BLOCK_LOOP", "BLOCK_LOOP")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCK_INTO);
	})
		

		.AddState("BLOCK_LOOP")
		.OnStart([this]()
	{
		m_fBlocktoAttackTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fBlocktoAttackTime += fTimeDelta;
		m_pModelCom->Set_AnimIndex(BLOCK_LOOP);
	})
		.AddTransition("BLOCK_LOOP to BLOCK_EXIT", "BLOCK_EXIT")
		.Predicator([this]()
	{
		return TimeTrigger(m_fBlocktoAttackTime, 5.f);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("BLOCK_EXIT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCK_EXIT);
		m_pModelCom->Set_AnimIndex(BLOCK_EXIT);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("BLOCK_LOOP to BLOCK_EXIT", "BLOCK_AFTER")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCK_EXIT);
	})
	

		.AddState("BLOCK_AFTER")
		.OnStart([this]()
	{	
		Set_BlockAfterType();
	})
		.Tick([this](_float fTimeDelta)
	{
		if (m_iBlockType == (_int)BA_FRONT)
			m_bBlockAfterFront = true;
		else if (m_iBlockType == (_int)BA_BACK)
			m_bBlockAfterBack = true;
		else if (m_iBlockType == (_int)BA_NONE)
			m_bBlockNone = true;
	})
		.OnExit([this]()
	{
		Reset_BlockAfterType();
	})
		.AddTransition("BLOCK_AFTER to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_bBlockNone;
	})
		.AddTransition("BLOCK_AFTER to BLOCK_COUNTERATTACK", "BLOCK_COUNTERATTACK")
		.Predicator([this]()
	{
		return m_bBlockAfterFront;
	})
		.AddTransition("BLOCK_AFTER to BLOCKATTACK_180", "BLOCKATTACK_180")
		.Predicator([this]()
	{
		return m_bBlockAfterBack;
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("BLOCK_COUNTERATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCK_COUNTERATTACK);
		m_pModelCom->Set_AnimIndex(BLOCK_COUNTERATTACK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("BLOCK_COUNTERATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCK_COUNTERATTACK);
	})
		

		.AddState("BLOCKATTACK_180")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(BLOCKATTACK_180);
		m_pModelCom->Set_AnimIndex(BLOCKATTACK_180);
	})
		.AddTransition("BLOCKATTACK_180 to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BLOCKATTACK_180);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("WALK")
		.OnStart([this]()
	{
		Set_AttackType();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(WALK);
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta);
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
		.AddTransition("WALK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("WALK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("WALK to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bChargeAttack;
	})
		.AddTransition("WALK to RANGEDATTACK", "RANGEDATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bRangedAttack;
	})
		.AddTransition("WALK to COMBOATTACK_LUNGE", "COMBOATTACK_LUNGE")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bComboAttack_Lunge;
	})
		.AddTransition("WALK to COMBOATTACK_OVERHEAD", "COMBOATTACK_OVERHEAD")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bComboAttack_Overhead;
	})
		.AddTransition("WALK to DOUBLEATTACK", "DOUBLEATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bDoubleAttack;
	})
		.AddTransition("WALK to UPPERCUTATTACK", "UPPERCUTATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bUppercutAttack;
	})


		.AddState("INTOCHARGE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(INTOCHARGE);
		m_pModelCom->Set_AnimIndex(INTOCHARGE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("INTOCHARGE to CHARGEATTACK", "CHARGEATTACK")
		.Predicator([this]()
	{
		return AnimFinishChecker(INTOCHARGE);
	})
		

		.AddState("INTOCHARGE_BACKUP")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(INTOCHARGE_BACKUP);
		m_pModelCom->Set_AnimIndex(INTOCHARGE_BACKUP);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("INTOCHARGE_BACKUP to CHARGEATTACK", "CHARGEATTACK")
		.Predicator([this]()
	{
		return AnimFinishChecker(INTOCHARGE_BACKUP);
	})
		

		.AddState("CHARGEATTACK")
		.OnStart([this]()
	{	
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(CHARGEATTACK);
		m_pModelCom->Set_AnimIndex(CHARGEATTACK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("CHARGEATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("CHARGEATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("CHARGEATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(CHARGEATTACK);
	})
		

		.AddState("RANGEDATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.7f);
		m_pModelCom->ResetAnimIdx_PlayTime(RANGEDATTACK);
		m_pModelCom->Set_AnimIndex(RANGEDATTACK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("RANGEDATTACK to BIND", "BIND")
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


		.AddState("COMBOATTACK_LUNGE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(COMBOATTACK_LUNGE);
		m_pModelCom->Set_AnimIndex(COMBOATTACK_LUNGE);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("COMBOATTACK_LUNGE to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("COMBOATTACK_LUNGE to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("COMBOATTACK_LUNGE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(COMBOATTACK_LUNGE);
	})


		.AddState("COMBOATTACK_OVERHEAD")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(COMBOATTACK_OVERHEAD);
		m_pModelCom->Set_AnimIndex(COMBOATTACK_OVERHEAD);
	})
		.AddTransition("COMBOATTACK_OVERHEAD to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("COMBOATTACK_OVERHEAD to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("COMBOATTACK_OVERHEAD to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(COMBOATTACK_OVERHEAD);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("DOUBLEATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(DOUBLEATTACK);
		m_pModelCom->Set_AnimIndex(DOUBLEATTACK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
		.AddTransition("DOUBLEATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("DOUBLEATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("DOUBLEATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(DOUBLEATTACK);
	})


		.AddState("UPPERCUTATTACK")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK_START], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(UPPERCUTATTACK);
		m_pModelCom->Set_AnimIndex(UPPERCUTATTACK);
	})
		.AddTransition("UPPERCUTATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("UPPERCUTATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("UPPERCUTATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(UPPERCUTATTACK);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("BIND")
		.OnStart([this]()
	{			
		Start_Bind(BIND);
		m_bStronglyHit = false;
	})
		.OnExit([this]()
	{
		// 맞는 애니메이션일때도 맞는가?
		Reset_Attack();
		End_Bind();
	})
		.AddTransition("BIND to INTOCHARGE_BACKUP", "INTOCHARGE_BACKUP")
		.Predicator([this]()
	{
		return AnimFinishChecker(BIND);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		// 어느 타이밍에 패링이 되는지?
		.AddState("HITDEFLECT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(HITDEFLECT);
		m_pModelCom->Set_AnimIndex(HITDEFLECT);
	})
		.AddTransition("HITDEFLECT to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(HITDEFLECT);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})

		.AddState("TAKEDAMAGE")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT], 0.5f);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_ALT);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_B);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_L);
		m_pModelCom->ResetAnimIdx_PlayTime(STAGGER_R);

		if(m_PlayerLookAt_Dir == FRONT)
			m_pModelCom->Set_AnimIndex(STAGGER_ALT);
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
		.AddTransition("TAKEDAMAGE to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("TAKEDAMAGE to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(STAGGER_ALT) ||
			AnimFinishChecker(STAGGER_L) || 
			AnimFinishChecker(STAGGER_B) || 
			AnimFinishChecker(STAGGER_R);
	})
		.AddTransition("To DYING", "DYING")
		.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})


		.AddState("DYING")
		.OnStart([this]()
	{
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_DIE], 0.5f);		
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

HRESULT CWoodKnight::SetUp_Components()
{
	__super::SetUp_Components();
	
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_WoodKnight", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_WoodKnight.json");

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes() - 1;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/WoodKnight_AO_R_M_1k.png")), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/WoodKnight_EMISSIVE_1k.png")), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_EMISSIVEMASK, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/WoodKnight_MASK_EMISSIVE_1k.png")), E_FAIL);
	}

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/Props_AO_R_M_1k.png")), E_FAIL);

	m_pModelCom->Set_RootBone("WoodKnight");
	
	return S_OK;
}

HRESULT CWoodKnight::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL)

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);
	m_bDissolve && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CWoodKnight::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

void CWoodKnight::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	{
		CBone* pBone = m_pModelCom->Get_BonePtr("char_sword_jnt2");
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

	{
		CBone* pBone = m_pModelCom->Get_BonePtr("char_lf_wristWeight_jnt");
		_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix =
			XMMatrixRotationX(m_vecPivotRot[COLL_PUNCH].x)
			* XMMatrixRotationY(m_vecPivotRot[COLL_PUNCH].y)
			* XMMatrixRotationZ(m_vecPivotRot[COLL_PUNCH].z)
			* XMMatrixTranslation(m_vecPivot[COLL_PUNCH].x, m_vecPivot[COLL_PUNCH].y, m_vecPivot[COLL_PUNCH].z)
			* SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_Collider(m_vecColliderName[COLL_PUNCH].c_str(), mat);
	}
}

void CWoodKnight::AdditiveAnim(_float fTimeDelta)
{
	_float fRatio =	Calc_PlayerLookAtDirection();
	if (fRatio >= 0.f)
	{
		fRatio *= 1.5f;
		m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOK_LEFT);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_WoodKnight.ao");
	}
	else
	{
		fRatio *= -1.5f;
		m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOK_RIGHT);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_WoodKnight.ao");
	}

	if(m_bWeaklyHit)
	{
		m_pModelCom->Set_AdditiveAnimIndexForMonster(TAKEDAMAGE);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_WoodKnight.ao");

		if (AnimFinishChecker(TAKEDAMAGE))
			m_bWeaklyHit = false;
	}
}

void CWoodKnight::Set_AttackType()
{
	m_bRealAttack = false;
	m_bRangedAttack = false;
	m_bChargeAttack = false;
	m_bComboAttack_Lunge = false;
	m_bComboAttack_Overhead = false;
	m_bDoubleAttack = false;
	m_bUppercutAttack = false;

	if(IntervalDistanceTrigger(6.f,10.f))
	{
		m_isFarRange = !m_isFarRange;

		if(m_isFarRange)
			m_iAttackType = AT_RANGEDATTACK;
		else
			m_iAttackType = AT_CHARGEATTACK;
	}
	else if (IntervalDistanceTrigger(3.f, 6.f))
	{
		m_isMiddleRange = !m_isMiddleRange;

		if(m_isMiddleRange)
			m_iAttackType = AT_COMBOATTACK_LUNGE;
		else
			m_iAttackType = AT_COMBOATTACK_OVERHEAD;
	}
	else	if(IntervalDistanceTrigger(0.f, 3.f))
	{
		m_isCloseRange = !m_isCloseRange;

		if(m_isCloseRange)
			m_iAttackType = AT_DOUBLEATTACK;
		else
			m_iAttackType = AT_UPPERCUTATTACK;
	}
	else 
		m_iAttackType = rand() % 6;

	// ATTACKTYPE이 플레이어 거리에 따라 달라짐
	switch (m_iAttackType)
	{
	case AT_RANGEDATTACK:
		m_bRangedAttack = true;
		break;
	case AT_CHARGEATTACK:
		m_bChargeAttack = true;
		break;
	case AT_COMBOATTACK_LUNGE:
		m_bComboAttack_Lunge = true;
		break;
	case AT_COMBOATTACK_OVERHEAD:
		m_bComboAttack_Overhead = true;
		break;
	case AT_DOUBLEATTACK:
		m_bDoubleAttack = true;
		break;
	case AT_UPPERCUTATTACK:
		m_bUppercutAttack = true;
	default:
		break;
	}
}

void CWoodKnight::Reset_Attack()
{
	m_bRealAttack = false;
	m_bRangedAttack = false;
	m_bChargeAttack = false;
	m_bComboAttack_Lunge = false;
	m_bComboAttack_Overhead = false;
	m_bDoubleAttack = false;
	m_bUppercutAttack = false;
	m_iAttackType = ATTACKTYPE_END;
}

void CWoodKnight::Tick_Attack(_float fTimeDelta)
{
	switch (m_iAttackType)
	{
	case AT_RANGEDATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 10.f);
		if (DistanceTrigger(10.f))
			m_bRealAttack = true;
		break;
	case AT_CHARGEATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 10.f);
		if (DistanceTrigger(10.f))
			m_bRealAttack = true;
		break;
	case AT_COMBOATTACK_LUNGE:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 6.f);
		if (DistanceTrigger(6.f))
			m_bRealAttack = true;
		break;
	case AT_COMBOATTACK_OVERHEAD:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 6.f);
		if (DistanceTrigger(6.f))
			m_bRealAttack = true;
		break;
	case AT_DOUBLEATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 3.f);
		if (DistanceTrigger(3.f))
			m_bRealAttack = true;
		break;
	case AT_UPPERCUTATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 3.f);
		if (DistanceTrigger(3.f))
			m_bRealAttack = true;
	default:
		break;
	}
}

void CWoodKnight::Set_BlockAfterType()
{
	if(DistanceTrigger(3.f))
	{
		if (m_PlayerLookAt_Dir == BACK)
			m_iBlockType = BA_BACK;
		else
			m_iBlockType = BA_FRONT;
	}
	else
		m_iBlockType = BA_NONE;
}

void CWoodKnight::Reset_BlockAfterType()
{
	m_bBlockAfterFront = false;
	m_bBlockAfterBack = false;
	m_bBlockNone = false;
}

CWoodKnight* CWoodKnight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWoodKnight*	pInstance = new CWoodKnight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWoodKnight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWoodKnight::Clone(void* pArg)
{
	CWoodKnight*	pInstance = new CWoodKnight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWoodKnight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWoodKnight::Free()
{
	CMonster::Free();
}

void CWoodKnight::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_WoodKnight_Tense.ogg"),
		TEXT("Mon_WoodKnight_AttackStart.ogg"),
		TEXT("Mon_WoodKnight_Attack.ogg"),		
		TEXT("Mon_WoodKnight_Idle.ogg"),
		TEXT("Mon_WoodKnight_Woosh.ogg"),
		TEXT("Mon_Walk_L.ogg"),
		TEXT("Mon_WoodKnight_Slam.ogg"),
		TEXT("Mon_WoodKnight_Die.ogg"),
		TEXT("Mon_WoodKnight_Catch.ogg"),
		TEXT("Mon_WoodKnight_Hurt.ogg"),		
	};

	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}

void CWoodKnight::Play_WoooshSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CWoodKnight::Play_WoooshSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK], 0.5f);
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WOOSH], 0.5f);
}

void CWoodKnight::Play_WalkSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CWoodKnight::Play_WalkSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WALK], 0.7f);
}

void CWoodKnight::Play_SlamSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CWoodKnight::Play_SlamSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_SLAM], 0.7f);
}

void CWoodKnight::Play_CatchSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CWoodKnight::Play_CatchSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_CATCH], 0.7f);
}

void CWoodKnight::Play_HurtSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CWoodKnight::Play_HurtSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT], 0.5f);
}

void CWoodKnight::LookAt_Kena(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CWoodKnight::LookAt_Kena);
		return;
	}

	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
}


