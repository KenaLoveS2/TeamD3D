#include "stdafx.h"
#include "..\public\Sticks01.h"
#include "GameInstance.h"
#include "Bone.h"

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

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	// SetUp_Component(); Monster가 불러줌
	//	Push_EventFunctions();

	m_pModelCom->Set_AllAnimCommonType();
	
	return S_OK;
}

HRESULT CSticks01::Late_Initialize(void * pArg)
{	
	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(0.25f, 0.25f, 1.f);
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

	m_pTransformCom->Set_Translation(_float4(20.f + (float)(rand() % 10), 0.f, 0.f, 1.f), _float4());

	return S_OK;
}

void CSticks01::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	// if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	if (DistanceTrigger(10.f))
		m_bSpawn = true;

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CSticks01::Late_Tick(_float fTimeDelta)
{
	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CSticks01::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
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
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CSticks01::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	m_pFSM->Imgui_RenderProperty();

	if (ImGui::Button("WeaklyDamage"))
		m_bWeaklyHit = true;

	if (ImGui::Button("StronglyDamage"))
		m_bStronglyHit = true;

	if (ImGui::Button("BIND"))
		m_bBind = true;
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
	CMonster::Push_EventFunctions();
}

HRESULT CSticks01::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("RESURRECT")
		.AddState("RESURRECT")
		.Tick([this](_float fTimeDelta)
	{
		if(!m_bSpawn)
			m_pModelCom->ResetAnimIdx_PlayTime(RESURRECT);

		m_pModelCom->Set_AnimIndex(RESURRECT);
	})
		.AddTransition("RESURRECT to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(RESURRECT);
	})

		.AddState("COMBATIDLE")
		.OnStart([this]()
	{
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
		.AddTransition("COMBATIDLE to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_pModelCom->ResetAnimIdx_PlayTime(CHEER);
		m_pModelCom->Set_AnimIndex(CHEER);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.AddTransition("CHEER to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_fIdletoAttackTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(STRAFELEFT);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Go_Left(fTimeDelta);
		m_fIdletoAttackTime += fTimeDelta;
	})
		.AddTransition("STRAFELEFT to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_fIdletoAttackTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(STRAFERIGHT);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Go_Right(fTimeDelta);
		m_fIdletoAttackTime += fTimeDelta;
	})
		.AddTransition("STRAFERIGHT to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_pModelCom->ResetAnimIdx_PlayTime(INTOCHARGE);
		m_pModelCom->Set_AnimIndex(INTOCHARGE);
	})
		.AddTransition("INTOCHARGE to CHARGE", "CHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(INTOCHARGE);
	})

		.AddState("CHARGE")
		.OnStart([this]()
	{
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
		.AddTransition("CHARGE to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		.AddTransition("CHARGE to ROCKTHROW", "ROCKTHROW")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bThrowRock;
	})

		.AddState("CHARGEATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(CHARGEATTACK);
		m_pModelCom->Set_AnimIndex(CHARGEATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
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
		.AddTransition("CHARGEATTACK to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(CHARGEATTACK);
	})

		.AddState("JUMPATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(JUMPATTACK);
		m_pModelCom->Set_AnimIndex(JUMPATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.AddTransition("JUMPATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK);
		m_pModelCom->Set_AnimIndex(ATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.AddTransition("ATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK2);
		m_pModelCom->Set_AnimIndex(ATTACK2);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.AddTransition("ATTACK2 to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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
		m_pModelCom->ResetAnimIdx_PlayTime(COMBOATTACK);
		m_pModelCom->Set_AnimIndex(COMBOATTACK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.AddTransition("COMBOATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
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

		.AddState("ROCKTHROW")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ROCKTHROW);
		m_pModelCom->Set_AnimIndex(ROCKTHROW);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
		.AddTransition("ROCKTHROW to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("ROCKTHROW to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("ROCKTHROW to COMBATIDLE", "COMBATIDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ROCKTHROW);
	})

		.AddState("BIND")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BIND);
		m_pModelCom->Set_AnimIndex(BIND);
		m_bStronglyHit = false;
		// 묶인 상태에서 맞았을때는 ADDITIVE 실행 
	})
		.Tick([this](_float fTimeDelta)
	{
		// Additive Animation		
	})
		.OnExit([this]()
	{
		// 맞는 애니메이션일때도 맞는가?
		m_bBind = false;
		Reset_Attack();
	})

		.AddTransition("BIND to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BIND);
	})

	// 어느 타이밍에 패링이 되는지?
		.AddState("PARRIED")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(PARRIED);
		m_pModelCom->Set_AnimIndex(PARRIED);
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

		.AddState("TAKEDAMAGE")
		.OnStart([this]()
	{
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
		.AddTransition("TAKEDAMAGE to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("TAKEDAMAGE to INTOCHARGE", "INTOCHARGE")
		.Predicator([this]()
	{
		return AnimFinishChecker(TAKEDAMAGEL) || 
			AnimFinishChecker(TAKEDAMAGEB) || 
			AnimFinishChecker(TAKEDAMAGER) || 
			AnimFinishChecker(TAKEDAMAGEBIG);
	})

		.AddState("DEATH")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(DEATH);
	})

				.Build();

	return S_OK;
}

HRESULT CSticks01::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Sticks01", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/mask_01_AO_R_M.png")),E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/stick_01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/axe_01_AO_R_M.png")), E_FAIL);

	// 추후 GLOW를 위함.
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/mask_01_Glow.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/stick_corrupted_glow_1k.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/axe_corrupted_glow_1k.png")), E_FAIL);

	m_pModelCom->Set_RootBone("Sticks_01_RIG");

	return S_OK;
}

HRESULT CSticks01::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CSticks01::SetUp_ShadowShaderResources()
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

void CSticks01::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	CBone* pBone = m_pModelCom->Get_BonePtr("staff_skin8_jnt");
	_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix =	 XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z)
							* SocketMatrix;
	_float4x4 mat;
	XMStoreFloat4x4(&mat, SocketMatrix);
	m_pTransformCom->Update_Collider(m_vecColliderName[COLL_WEAPON].c_str(), mat);

	// 맞았을때 약공격을 맞았으면 TWITCH~ Additive로

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
	}
}

void CSticks01::Set_AttackType()
{
	m_iAttackType = rand() % 6;

	m_bChargeAttack = false;
	m_bJumpAttack = false;
	m_bAttack1 = false;
	m_bAttack2 = false;
	m_bComboAttack = false;
	m_bRealAttack = false;
	m_bThrowRock = false;

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
	case AT_ROCKTHROW:
		m_bThrowRock = true;
	default:
		break;
	}
}

void CSticks01::Reset_Attack()
{
	m_bRealAttack = false;
	m_bChargeAttack = false;
	m_bJumpAttack = false;
	m_bAttack1 = false;
	m_bAttack2 = false;
	m_bComboAttack = false;
	m_bThrowRock = false;
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
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 2.f);
		if (DistanceTrigger(2.f))
			m_bRealAttack = true;
		break;
	case AT_ATTACK2:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 2.f);
		if (DistanceTrigger(2.f))
			m_bRealAttack = true;
		break;
	case AT_COMBOATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 2.f);
		if (DistanceTrigger(2.f))
			m_bRealAttack = true;
		break;
	case AT_ROCKTHROW:
		m_bRealAttack = true;
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
