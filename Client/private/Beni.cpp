#include "stdafx.h"
#include "..\public\Beni.h"
#include "GameInstance.h"
#include "CameraForNpc.h"

CBeni::CBeni(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CNpc(pDevice, pContext)
{
}

CBeni::CBeni(const CNpc& rhs)
	:CNpc(rhs)
{
}

HRESULT CBeni::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBeni::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	m_pModelCom->Set_AllAnimCommonType();

	return S_OK;
}

HRESULT CBeni::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	// 몸통
	{
		_float3 vPos = _float3(0.f, 0.f, 0.f);
		_float3 vPivotScale = _float3(0.2f, 0.1f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.3f, 0.f);

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
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
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

	m_pTransformCom->Set_Position(_float4(69.f,0.f,136.f,1.f));

	return S_OK;
}

void CBeni::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Update_Collider(fTimeDelta);
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);
}

void CBeni::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CBeni::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 2)
			continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if(i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BENI_EYE);
		else if (i == 4)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT);
		else
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AORM);
		}
	}
	return S_OK;
}

HRESULT CBeni::RenderShadow()
{
	FAILED_CHECK_RETURN(__super::RenderShadow(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShadowShaderResources(), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CBeni::Imgui_RenderProperty()
{
	CNpc::Imgui_RenderProperty();
}

void CBeni::ImGui_AnimationProperty()
{
	if (ImGui::CollapsingHeader("Beni"))
	{
		ImGui::BeginTabBar("Beni Animation & State");

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

void CBeni::ImGui_ShaderValueProperty()
{
	CNpc::ImGui_ShaderValueProperty();
	if(ImGui::Button("Recompile"))
	{
		m_pRendererCom->ReCompile();
		m_pShaderCom->ReCompile();
	}
	m_pModelCom->Imgui_RenderProperty();
}

void CBeni::ImGui_PhysXValueProperty()
{
	CNpc::ImGui_PhysXValueProperty();
}

void CBeni::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

HRESULT CBeni::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("IDLE to CHEER" , "CHEER")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f) && !m_bMeetPlayer;
	})

		.AddState("CHEER")
		.OnStart([this]()
	{
		//CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)
		//m_pMyCam->Set_Target(this);
		//p_game_instance->Work_Camera(L"NPC_CAM");
		//RELEASE_INSTANCE(CGameInstance)
		m_bMeetPlayer = true;
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_CHEER);
		m_pModelCom->Set_AnimIndex(BENI_CHEER);
	})

		.OnExit([this]()
	{
		//	// 누나가 끄고싶을때 끄면댐~
		//CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)
		//m_pMyCam->Set_Target(nullptr);
		//p_game_instance->Work_Camera(L"PLAYER_CAM");
		//RELEASE_INSTANCE(CGameInstance)
	})
		.AddTransition("CHEER to IDLE", "IDLE")
		.Predicator([this]()
	{
		return  AnimFinishChecker(BENI_CHEER);
	})

		.Build();

	return S_OK;
}

HRESULT CBeni::SetUp_Components()
{
	__super::SetUp_Components();
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Beni", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Beni/jizoboy_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_NORMALS, TEXT("../Bin/Resources/Anim/NPC/Eyes_NORMAL.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Beni/jizoboy_cloth_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(5, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Beni/jizokids_mask_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(6, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Beni/jizokids_slingshot_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(7, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Beni/jizoboy_body_AO_R_M.png")), E_FAIL);
	m_pModelCom->Set_RootBone("jizo_boy_RIG");
	return S_OK;
}

HRESULT CBeni::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CBeni::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CBeni::SetUp_UI()
{
	return CNpc::SetUp_UI();
}

void CBeni::AdditiveAnim(_float fTimeDelta)
{
	m_pModelCom->Set_AdditiveAnimIndexForMonster(BENI_MOUTHFLAP);
	m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Beni.ao");
}

CBeni* CBeni::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBeni*	pInstance = new CBeni(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBeni");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBeni::Clone(void* pArg)
{
	CBeni*	pInstance = new CBeni(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBeni");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeni::Free()
{
	__super::Free();
}
