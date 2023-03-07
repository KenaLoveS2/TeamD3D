#include "stdafx.h"
#include "..\public\BranchTosser.h"
#include "GameInstance.h"
#include "Bone.h"
#include "BranchTosser_Weapon.h"

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
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
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

HRESULT CBranchTosser::Late_Initialize(void * pArg)
{
	// 몸통
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
		_float3 vWeaponPivot = _float3(0.2f, -0.1f, -1.4f);
		m_vecPivot.push_back(vWeaponPivot);
		_float3 vWeaponScalePivot = _float3(0.07f, 0.5f, 0.1f);
		m_vecPivotScale.push_back(vWeaponScalePivot);
		_float3 vWeaponRotPivot = _float3(0.8f, 0.1f, 0.5f);
		m_vecPivotRot.push_back(vWeaponRotPivot);

		m_vecColliderName[COLL_WEAPON] = m_szCloneObjectTag;
		m_vecColliderName[COLL_WEAPON] += L"Weapon";

		CBone* pBone = m_pModelCom->Get_BonePtr("Branch_Projectile_jnt");
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
		PxCapsuleDesc.vVelocity = _float3(0.f, -1.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fAngularDamping = 0.5f;
		PxCapsuleDesc.fMass = 10.f;
		PxCapsuleDesc.fLinearDamping = 1.f;
		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false));

		m_pTransformCom->Add_Collider(m_vecColliderName[COLL_WEAPON].c_str(), pivotMatrix);
		m_pRendererCom->Set_PhysXRender(true);
	}

	m_pTransformCom->Set_Translation(_float4(0.f, 10.f, -15.f, 1.f), _float4());

	return S_OK;
}

void CBranchTosser::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);

	m_pWeapon->Tick(fTimeDelta);

	if (m_pFSM)
		m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CBranchTosser::Late_Tick(_float fTimeDelta)
{
	CMonster::Late_Tick(fTimeDelta);

	m_pWeapon->Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CBranchTosser::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
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
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CBranchTosser::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	m_pWeapon->Imgui_RenderProperty();
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

	// shader Value 조절
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
		.Tick([this](_float fTimeDelta)
	{
	})
		.AddTransition("NONE to DROP" , "DROP")
		.Predicator([this]()
	{
		return true;
	})

		.AddState("DROP")
		.OnStart([this]()
	{
		m_iDropType = rand() % 3;
		if (m_iDropType == DROP_1)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(L_DROP1);
			m_pModelCom->Set_AnimIndex(L_DROP1);
		}
		else if (m_iDropType == DROP_2)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(L_DROP2);
			m_pModelCom->Set_AnimIndex(L_DROP2);
		}
		else if (m_iDropType == DROP_3)
		{
			m_pModelCom->ResetAnimIdx_PlayTime(L_DROP3);
			m_pModelCom->Set_AnimIndex(L_DROP3);
		}
	})
		.AddTransition("DROP TO IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(L_DROP1) ||
			AnimFinishChecker(L_DROP2) ||
			AnimFinishChecker(L_DROP3);
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
		.AddTransition("IDLE to ATTACK", "ATTACK")
		.Predicator([this]()
	{
		return m_fIdleToAttack >= 3.f;
	})

		.AddState("ATTACK")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ATTACK);
		m_pModelCom->Set_AnimIndex(ATTACK);
	})
		.AddTransition("ATTACK to FLEE", "FLEE")
		.Predicator([this]()
	{
		return AnimFinishChecker(ATTACK);
	})

		.AddState("FLEE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(FLEETONEW);
		m_pModelCom->Set_AnimIndex(FLEETONEW);
	})
		.AddTransition("FLEE to DROP", "DROP")
		.Predicator([this]()
	{
		return AnimFinishChecker(FLEETONEW);
	})

		.Build();
	return S_OK;
}

HRESULT CBranchTosser::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Shader_VtxAnimMonsterModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_BranchTosser", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_pWeapon = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BranchTosserWeapon"), L"BranchTosserWeapon",&WeaponDesc);
	assert(m_pWeapon && "BranchTosser Weapon is nullptr");
	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CBranchTosser::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CBranchTosser::SetUp_ShadowShaderResources()
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

void CBranchTosser::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	{
		CBone* pBone = m_pModelCom->Get_BonePtr("Branch_Projectile_jnt");
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
	Safe_Release(m_pWeapon);
}
