#include "stdafx.h"
#include "..\public\BossWarrior.h"
#include "GameInstance.h"
#include "Bone.h"
#include "BossWarrior_Hat.h"

CBossWarrior::CBossWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CBossWarrior::CBossWarrior(const CBossWarrior& rhs)
	:CMonster(rhs)
{
}

HRESULT CBossWarrior::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBossWarrior::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
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
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -10.f;
		m_Desc.WorldMatrix._43 = -10.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CBossWarrior::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);
	// 몸통
	{
		_float3 vPos = _float3(20.f + (float)(rand() % 10), 3.f, 0.f);
		//_float3 vPivotScale = _float3(0.25f, 0.25f, 1.f);
		_float3 vPivotScale = _float3(0.5f, 0.5f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.5f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = m_szCloneObjectTag;
		PxCapsuleDesc.vPos = _float3( 0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
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

		// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
		m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
		m_pTransformCom->Set_PxPivotScale(vPivotScale);
		m_pTransformCom->Set_PxPivot(vPivotPos);
	}

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	return S_OK;
}

void CBossWarrior::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);
	m_pHat->Tick(fTimeDelta);
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CBossWarrior::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	m_pHat->Late_Tick(fTimeDelta);
	if (m_pRendererCom /*&& m_bSpawn*/)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CBossWarrior::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if(i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M_E);
		}
		else if(i == 1)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			//m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_OpacityTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AO_R_M);
		}
	}
	return S_OK;
}

HRESULT CBossWarrior::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CBossWarrior::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	m_pHat->Imgui_RenderProperty();
	float fEmissiveColor[3] = { m_fEmissiveColor.x, m_fEmissiveColor.y, m_fEmissiveColor.z };
	ImGui::DragFloat3("EmissiveColor", fEmissiveColor, 0.01f, 0.1f, 100.0f);
	m_fEmissiveColor.x = fEmissiveColor[0]; m_fEmissiveColor.y = fEmissiveColor[1]; m_fEmissiveColor.z = fEmissiveColor[2];
	ImGui::DragFloat("HDRIntensity", &m_fHDRIntensity, 0.01f, 0.f, 100.f);
}

void CBossWarrior::ImGui_AnimationProperty()
{
	if (ImGui::CollapsingHeader("BossWarrior"))
	{
		ImGui::BeginTabBar("BossWarrior Animation & State");

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

void CBossWarrior::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CBossWarrior::ImGui_PhysXValueProperty()
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
}

HRESULT CBossWarrior::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CBossWarrior::Push_EventFunctions()
{
	CMonster::Push_EventFunctions();
}

HRESULT CBossWarrior::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")

		.Build();

	return S_OK;
}

HRESULT CBossWarrior::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Boss_Warrior", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_01_EMISSIVE.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/VillageWarrior_Uv_02_OPACITY.png")), E_FAIL);

	m_pModelCom->Set_RootBone("VL_Warrior");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	//m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_Sticks01.json");

	CBossWarrior_Hat::MONSTERWEAPONDESC		WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CBossWarrior_Hat::MONSTERWEAPONDESC));

	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("HatJoint");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
		m_pHat = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BossWarrior_Hat"), L"BossWarrior_Hat", &WeaponDesc);
	assert(m_pHat && "BranchTosser Weapon is nullptr");
	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CBossWarrior::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_EmissiveColor", &m_fEmissiveColor, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fHDRIntensity", &m_fHDRIntensity, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDying, sizeof(_bool)), E_FAIL);
	m_bDying && Bind_Dissolove(m_pShaderCom);

	return S_OK;
}

HRESULT CBossWarrior::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

void CBossWarrior::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
	//{
	//	CBone* pBone = m_pModelCom->Get_BonePtr("HatJoint");
	//	_matrix			SocketMatrix = pBone->Get_OffsetMatrix() * pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	//	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	//	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	//	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	//	SocketMatrix = XMMatrixRotationX(m_vecPivotRot[COLL_HAT].x)
	//		* XMMatrixRotationY(m_vecPivotRot[COLL_HAT].y)
	//		* XMMatrixRotationZ(m_vecPivotRot[COLL_HAT].z)
	//		*XMMatrixTranslation(m_vecPivot[COLL_HAT].x, m_vecPivot[COLL_HAT].y, m_vecPivot[COLL_HAT].z)
	//		* SocketMatrix;

	//	_float4x4 mat;
	//	XMStoreFloat4x4(&mat, SocketMatrix);
	//	m_pTransformCom->Update_Collider(m_vecColliderName[COLL_WEAPON].c_str(), mat);
	//}
}

void CBossWarrior::AdditiveAnim(_float fTimeDelta)
{
	CMonster::AdditiveAnim(fTimeDelta);
}

void CBossWarrior::Set_AttackType()
{
}

void CBossWarrior::Reset_Attack()
{
}

void CBossWarrior::Tick_Attack(_float fTimeDelta)
{
}

void CBossWarrior::Set_AFType()
{
}

void CBossWarrior::Reset_AF()
{
}

CBossWarrior* CBossWarrior::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossWarrior*	pInstance = new CBossWarrior(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBossWarrior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossWarrior::Clone(void* pArg)
{
	CBossWarrior*	pInstance = new CBossWarrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBossWarrior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossWarrior::Free()
{
	CMonster::Free();
}
