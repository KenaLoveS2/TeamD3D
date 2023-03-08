#include "stdafx.h"
#include "RotEater.h"
#include "GameInstance.h"
#include "Bone.h"

CRotEater::CRotEater(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CRotEater::CRotEater(const CRotEater & rhs)
	: CMonster(rhs)
{
}

HRESULT CRotEater::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRotEater::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	// SetUp_Component(); Monster가 불러줌
	//	Push_EventFunctions();
	m_pTransformCom->Set_Translation(_float4(10.f, 0.f, 0.f, 1.f), _float4());

	m_pModelCom->Set_AllAnimCommonType();

	return S_OK;
}

HRESULT CRotEater::Late_Initialize(void * pArg)
{
	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		_float3 vPivotScale = _float3(1.f, 0.001f, 1.f);
		_float3 vPivotPos = _float3(0.f, 1.f, 0.f);

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
		PxCapsuleDesc.fMass = 30.f;
		PxCapsuleDesc.fLinearDamping = 10.f;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this));

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
		_float3 vWeaponPivot = _float3(0.35f, -1.f, 0.f);
		m_vecPivot.push_back(vWeaponPivot);
		_float3 vWeaponScalePivot = _float3(0.2f, 0.1f, 0.1f);

		m_vecPivotScale.push_back(vWeaponScalePivot);

		m_vecColliderName[COLL_WEAPON] = m_szCloneObjectTag;
		m_vecColliderName[COLL_WEAPON] += L"Weapon";

		CBone* pBone = m_pModelCom->Get_BonePtr("char_lf_toe_jnt");
		_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix = XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z)
			* SocketMatrix;

		_float4x4 pivotMatrix;
		XMStoreFloat4x4(&pivotMatrix, SocketMatrix);

		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = m_vecColliderName[COLL_WEAPON].c_str();
		PxSphereDesc.vPos = _float3(0.f, 5.f, 0.f);
		PxSphereDesc.fRadius = m_vecPivotScale[COLL_WEAPON].x;
		PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxSphereDesc.fDensity = 1.f;
		PxSphereDesc.fAngularDamping = 0.5f;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;
		PxSphereDesc.fDynamicFriction = 0.5f;
		PxSphereDesc.fStaticFriction = 0.5f;
		PxSphereDesc.fRestitution = 0.1f;

		CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false));

		m_pTransformCom->Add_Collider(m_vecColliderName[COLL_WEAPON].c_str(), pivotMatrix);
		m_pRendererCom->Set_PhysXRender(true);
	}

	m_pTransformCom->Set_Position(_float4(27.f, 0.3f, 6.f, 1.f));

	return S_OK;
}

void CRotEater::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	if (m_pFSM)
		m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);

	AdditiveAnim(fTimeDelta);
}

void CRotEater::Late_Tick(_float fTimeDelta)
{
	CMonster::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CRotEater::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		if(i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",AO_R_M_E);
		}

		if(i == 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
		}
	}
	return S_OK;
}

HRESULT CRotEater::RenderShadow()
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

void CRotEater::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	if (ImGui::Button("SPAWN"))
		m_bSpawn = true;
}

void CRotEater::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("RotEater Animation & State");

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

void CRotEater::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();

	// shader Value 조절
}

void CRotEater::ImGui_PhysXValueProperty()
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

HRESULT CRotEater::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CRotEater::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CRotEater::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("NONE")
		.AddState("NONE")
		.AddTransition("NONE to AWAKE", "AWAKE")
		.Predicator([this]()
	{
		return true;
	})

		.AddState("AWAKE")
		.OnStart([this]()
	{
		m_iAwakeType = rand() % 2;
	})
		.Tick([this](_float fTimeDelta)
	{
		if(!m_bSpawn)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(AWAKE);
			m_pModelCom->ResetAnimIdx_PlayTime(AWAKEALT);
		}
		else
		{
			if (m_iAwakeType == AWAKE_0)
				m_pModelCom->Set_AnimIndex(AWAKE);
			else if (m_iAwakeType == AWAKE_1)
				m_pModelCom->Set_AnimIndex(AWAKEALT);
		}
	})
		.AddTransition("AWAKE to IDLE" , "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(AWAKE) || AnimFinishChecker(AWAKEALT);
	})

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_fIdletoAttackTime = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdletoAttackTime += fTimeDelta;
		m_pModelCom->Set_AnimIndex(IDLE);
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
		.AddTransition("IDLE to RUN", "RUN")		
		.Predicator([this]()
	{
		return TimeTrigger(m_fIdletoAttackTime, 3.f);
	})

		.AddState("RUN")
		.OnStart([this]()
	{
		Set_AttackType();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(RUN);
		Tick_Attack(fTimeDelta);
	})
		.OnExit([this]()
	{
		Reset_Attack();
	})
		.AddTransition("RUN to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("RUN to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("RUN to SWIPEATTACK	", "SWIPEATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bSwipeAttack;
	})
		.AddTransition("RUN to JUMPATTACK", "JUMPATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bJumpAttack;
	})
		.AddTransition("RUN to LONGRANGEATTACK", "LONGRANGEATTACK")
		.Predicator([this]()
	{
		return m_bRealAttack && m_bLongRangeAttack;
	})

		.AddState("SWIPEATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SWIPE);
		m_pModelCom->Set_AnimIndex(SWIPE);
	})
		.OnExit([this]()
	{
		m_iAfterSwipeAttack++;

		if (m_iAfterSwipeAttack > 1)
			m_iAfterSwipeAttack = 0;
	})
		.AddTransition("SWIPEATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("SWIPEATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("SWIPEATTACK to JUMPBACK", "JUMPBACK")
		.Predicator([this]()
	{
		return AnimFinishChecker(SWIPE) && m_iAfterSwipeAttack == 0;
	})
		.AddTransition("SWIPEATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(SWIPE) && m_iAfterSwipeAttack == 1;
	})

		.AddState("JUMPBACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(JUMPBACK);
		m_pModelCom->Set_AnimIndex(JUMPBACK);
	})
		.AddTransition("JUMPBACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("JUMPBACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("JUMPBACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(JUMPBACK);
	})

		.AddState("JUMPATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(JUMPATTACK);
		m_pModelCom->Set_AnimIndex(JUMPATTACK);
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
		.AddTransition("JUMPATTACK to SWIPEATTACK", "SWIPEATTACK")
		.Predicator([this]()
	{
		return AnimFinishChecker(JUMPATTACK);
	})

		.AddState("LONGRANGEATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK_LONGRANGE);
		m_pModelCom->Set_AnimIndex(ATTACK_LONGRANGE);
	})
		.AddTransition("LONGRANGEATTACK to BIND", "BIND")
		.Predicator([this]()
	{
		return m_bBind;
	})
		.AddTransition("LONGRANGEATTACK to TAKEDAMAGE", "TAKEDAMAGE")
		.Predicator([this]()
	{
		return m_bStronglyHit;
	})
		.AddTransition("LONGRANGEATTACK to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ATTACK_LONGRANGE);
	})

		.AddState("BIND")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BIND);
		m_pModelCom->Set_AnimIndex(BIND);
		m_bStronglyHit = false;
		// 묶인 상태에서 맞았을때는 ADDITIVE 실행 
	})
		.OnExit([this]()
	{
		// 맞는 애니메이션일때도 맞는가?
		m_bBind = false;
		Reset_Attack();
	})
		.AddTransition("BIND to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(BIND);
	})

		.AddState("TAKEDAMAGE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(STTAGER_FRONT);
		m_pModelCom->ResetAnimIdx_PlayTime(STTAGER_BACK);
		m_pModelCom->ResetAnimIdx_PlayTime(STTAGER_LEFT);
		m_pModelCom->ResetAnimIdx_PlayTime(STTAGER_RIGHT);

		if (m_PlayerLookAt_Dir == FRONT)
			m_pModelCom->Set_AnimIndex(STTAGER_FRONT);
		else if (m_PlayerLookAt_Dir == BACK)
			m_pModelCom->Set_AnimIndex(STTAGER_BACK);
		else if (m_PlayerLookAt_Dir == LEFT)
			m_pModelCom->Set_AnimIndex(STTAGER_LEFT);
		else if (m_PlayerLookAt_Dir == RIGHT)
			m_pModelCom->Set_AnimIndex(STTAGER_RIGHT);
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
		.AddTransition("TAKEDAMAGE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(STTAGER_FRONT) ||
			AnimFinishChecker(STTAGER_LEFT) ||
			AnimFinishChecker(STTAGER_BACK) ||
			AnimFinishChecker(STTAGER_RIGHT);
	})

		.AddState("DEATH")
		.OnStart([this]()
	{
		if (m_PlayerLookAt_Dir == FRONT)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(DEATH3);
			m_pModelCom->Set_AnimIndex(DEATH3);
		}
		else	if (m_PlayerLookAt_Dir == BACK)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(DEATH);
			m_pModelCom->Set_AnimIndex(DEATH);
		}
	})

		.Build();

	return S_OK;
}

HRESULT CRotEater::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimMonsterModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_RotEater", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/RotEater/rot_eater_body_uv_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/RotEater/rot_eater_props_uv_AO_R_M.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/RotEater/rot_eater_body_uv_EMISSIVE.png")), E_FAIL);

	m_pModelCom->Set_RootBone("Rot_Eater_RIG");
	
	return S_OK;
}

HRESULT CRotEater::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CRotEater::SetUp_ShadowShaderResources()
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

void CRotEater::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	CBone* pBone = m_pModelCom->Get_BonePtr("char_lf_toe_jnt");
	_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = XMMatrixTranslation(m_vecPivot[COLL_WEAPON].x, m_vecPivot[COLL_WEAPON].y, m_vecPivot[COLL_WEAPON].z)
		* SocketMatrix;
	_float4x4 mat;
	XMStoreFloat4x4(&mat, SocketMatrix);
	m_pTransformCom->Update_Collider(m_vecColliderName[COLL_WEAPON].c_str(), mat);
}

void CRotEater::AdditiveAnim(_float fTimeDelta)
{
	_float fRatio = Calc_PlayerLookAtDirection();

	if(m_pFSM->IsCompareState("IDLE") || m_pFSM->IsCompareState("LONGRANGEATTACK"))
	{
		if (fRatio >= 0.f)
		{
			fRatio *= 1.5f;
			m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOK_LEFT);
			m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_RotEater.ao");
		}
		else
		{
			fRatio *= -1.5f;
			m_pModelCom->Set_AdditiveAnimIndexForMonster(LOOK_RIGHT);
			m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, fRatio, "SK_RotEater.ao");
		}
	}

	if (m_bWeaklyHit)
	{
		m_pModelCom->Set_AdditiveAnimIndexForMonster(TAKEDAMAGE);
		m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_RotEater.ao");

		if (AnimFinishChecker(TAKEDAMAGE))
			m_bWeaklyHit = false;
	}
}

void CRotEater::Set_AttackType()
{
	m_bRealAttack = false;
	m_bJumpAttack = false;
	m_bLongRangeAttack = false;
	m_bSwipeAttack = false;

	if (!DistanceTrigger(5.f))
	{
		m_bRangedType = !m_bRangedType;
		//  원거리 공격 가능 5이상일때
		if(m_bRangedType)
		m_iAttackType = AT_ATTACKLONGRANGE;
		else
			m_iAttackType = AT_JUMPATTACK;
	}
	else
		m_iAttackType = AT_SWIPE;

	switch (m_iAttackType)
	{
	case AT_ATTACKLONGRANGE:
		m_bLongRangeAttack = true;
		break;
	case AT_JUMPATTACK:
		m_bJumpAttack = true;
		break;
	case AT_SWIPE:
		m_bSwipeAttack = true;
		break;
	default:
		break;
	}
}

void CRotEater::Reset_Attack()
{
	m_bRealAttack = false;
	m_bJumpAttack = false;
	m_bSwipeAttack = false;
	m_bLongRangeAttack = false;
	m_iAttackType = ATTACKTYPE_END;
}

void CRotEater::Tick_Attack(_float fTimeDelta)
{
	switch (m_iAttackType)
	{
	case AT_SWIPE:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 2.f);
		if (DistanceTrigger(2.f))
			m_bRealAttack = true;
		break;
	case AT_JUMPATTACK:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 6.f);
		if (DistanceTrigger(6.f))
			m_bRealAttack = true;
		break;
	case AT_ATTACKLONGRANGE:
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 6.f);
		if (DistanceTrigger(6.f))
			m_bRealAttack = true;
		break;
	default:
		break;
	}
}

CRotEater* CRotEater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotEater*	pInstance = new CRotEater(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRotEater");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRotEater::Clone(void* pArg)
{
	CRotEater*	pInstance = new CRotEater(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRotEater");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotEater::Free()
{
	CMonster::Free();
}
