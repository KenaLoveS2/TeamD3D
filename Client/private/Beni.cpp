#include "stdafx.h"
#include "..\public\Beni.h"
#include "GameInstance.h"
#include "CameraForNpc.h"
#include "Saiya.h"
#include "CinematicCamera.h"
#include "Camera_Photo.h"

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
	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.5f;
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

	m_pSaiya = dynamic_cast<CSaiya*>( CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_NPC"), TEXT("Saiya")));

	if(m_pSaiya == nullptr)
	{
		MSG_BOX("!!!!!There is No Saiya!!!!!");
	}

	const _float4 vPos =	m_pSaiya->Get_TransformCom()->Get_Position();
	const _float4 vChangePos =	_float4(vPos.x - 0.5f, vPos.y, vPos.z, 1.f);
	const _float4 vLook = m_pSaiya->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Set_Position(vChangePos);
	m_pTransformCom->Set_Look(vLook);

	m_pCamera_Photo = (CCamera_Photo*)CGameInstance::GetInstance()->Find_Camera(CAMERA_PHOTO_TAG);

	return S_OK;
}

void CBeni::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);
	Update_Collider(fTimeDelta);
	SaiyaFunc(fTimeDelta);
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	if (!m_pSaiya->Get_Photo())
		AdditiveAnim(fTimeDelta);

	_float3 vLook = m_pSaiya->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_float3 vRight = _float3(1.f, 0.f, 0.f);

	if(vLook.Dot(vRight) <=0.f)
		m_vOffsetPos = _float3(0.5, 0.f, 0.f);
	else
		m_vOffsetPos = _float3(-0.5, 0.f, 0.f);

	SaiyaPos(m_vOffsetPos);
}

void CBeni::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

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
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"),E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		if(i == 1)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BENI_EYE), E_FAIL);
		}
		else if (i == 4)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT), E_FAIL);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AORM), E_FAIL);
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
		.InitState("ACTION_0")
		.AddState("ACTION_0")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_CHASINGLOOP);
	})
		.AddTransition("ACTION_0 to ACTION_1", "ACTION_1")
		.Predicator([this]()
	{
		return m_strState == "ACTION_1";
	})

		.AddState("ACTION_1")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_1 to ACTION_2", "ACTION_2")
	.Predicator([this]()
	{
		return m_strState == "ACTION_2";
	})

		.AddState("ACTION_2")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.AddTransition("ACTION_2 to ACTION_3", "ACTION_3")
		.Predicator([this]()
	{
		return m_strState == "ACTION_3";
	})

		.AddState("ACTION_3")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_CHASINGLOOP);
	})
		.AddTransition("ACTION_3 to ACTION_4", "ACTION_4")
		.Predicator([this]()
	{
		return m_strState == "ACTION_4" && AnimFinishChecker(BENI_CHASINGLOOP);;
	})

		.AddState("ACTION_4")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_4 to ACTION_5", "ACTION_5")
		.Predicator([this]()
	{
		return m_strState == "ACTION_5";
	})

		.AddState("ACTION_5")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.AddTransition("ACTION_5 to ACTION_6", "ACTION_6")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_TELEPORT);
	})

		.AddState("ACTION_6")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_6 to ACTION_7", "ACTION_7")
		.Predicator([this]()
	{
		return dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("MAP_CINE0")))->CameraFinishedChecker(0.7f); // 첫 펄스로 시네캠 생성될때
	})

		.AddState("ACTION_7")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_7 to ACTION_8", "ACTION_8")
		.Predicator([this]()
	{
		return true;
	})

		.AddState("ACTION_8")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.AddTransition("ACTION_8 to ACTION_9", "ACTION_9")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_TELEPORT);
	})

		.AddState("ACTION_9")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_9 to ACTION_10", "ACTION_10")
		.Predicator([this]()
	{
		return m_strState == "ACTION_10";
	})

		.AddState("ACTION_10")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_10 to ACTION_11", "ACTION_11")
		.Predicator([this]()
	{
		return m_strState == "ACTION_11";
	})

		.AddState("ACTION_11")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.AddTransition("ACTION_11 to ACTION_12", "ACTION_12")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_TELEPORT);
	})

		.AddState("ACTION_12")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_12 to ACTION_14", "ACTION_14")
		.Predicator([this]()
	{
		return m_strState == "ACTION_14";
	})

		.AddState("ACTION_14")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_CURIOUS);
		m_pModelCom->Set_AnimIndex(BENI_CURIOUS);
	})
		.AddTransition("ACTION_14 to ACTION_15", "ACTION_15")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_CURIOUS);
	})

		.AddState("ACTION_15")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_15 to ACTION_16", "ACTION_16")
		.Predicator([this]()
	{
		return m_strState == "ACTION_16";
	})

		.AddState("ACTION_16")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_PULSE);
		m_pModelCom->Set_AnimIndex(BENI_PULSE);
	})
		.AddTransition("ACTION_16 to ACTION_17", "ACTION_17")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_PULSE);
	})

		.AddState("ACTION_17")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_17 to ACTION_18", "ACTION_18")
		.Predicator([this]()
	{
		return m_strState == "ACTION_18";
	})

		.AddState("ACTION_18")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_18 to ACTION_19", "ACTION_19")
		.Predicator([this]()
	{
		return m_strState == "ACTION_19";
	})

		.AddState("ACTION_19")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.AddTransition("ACTION_19 to ACTION_20", "ACTION_20")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_TELEPORT);
	})

		.AddState("ACTION_20")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_20 to ACTION_21", "ACTION_21")
		.Predicator([this]()
	{
		return m_strState == "ACTION_21";
	})

		.AddState("ACTION_21")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_PULSE);
		m_pModelCom->Set_AnimIndex(BENI_PULSE);
	})
		.AddTransition("ACTION_21 to ACTION_22", "ACTION_22")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_PULSE) && m_strState == "ACTION_22";
	})

		.AddState("ACTION_22")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_22 to ACTION_23", "ACTION_23")
		.Predicator([this]()
	{
		return m_strState == "ACTION_23";
	})

		.AddState("ACTION_23")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.AddTransition("ACTION_23 to ACTION_24", "ACTION_24")
		.Predicator([this]()
	{
		return  AnimFinishChecker(BENI_TELEPORT);
	})

		.AddState("ACTION_24")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("ACTION_24 to ACTION_25", "ACTION_25")
		.Predicator([this]()
	{
		return m_strState == "ACTION_25";
	})
		.AddState("ACTION_25")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_PULSE);
		m_pModelCom->Set_AnimIndex(BENI_PULSE);
	})
		.AddTransition("ACTION_25 to ACTION_26", "ACTION_26")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_PULSE);
	})

		.AddState("ACTION_26")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})

		.Build();

	return S_OK;
}

HRESULT CBeni::SetUp_StateFinal()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
		// 위치 정해줘야함
	})
		.AddTransition("IDLE to READY_PHOTO", "READY_PHOTO")
		.Predicator([this]()
	{
		return m_strState == "READY_PHOTO";
	})

		.AddState("READY_PHOTO")
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_pCamera_Photo->Get_Position());
	})
		.AddTransition("READY_PHOTO to PHOTO", "PHOTO")
		.Predicator([this]()
	{
		return m_strState == "PHOTO";
	})

		.AddState("PHOTO")
		.OnStart([this]()
	{
		_uint iPhotoAnimIndex = rand() % (BENI_PHOTOPOSE_SMILING - BENI_PHOTOPOSE_CHEER) + BENI_PHOTOPOSE_CHEER;
		m_pModelCom->ResetAnimIdx_PlayTime(iPhotoAnimIndex);
		m_pModelCom->Set_AnimIndex(iPhotoAnimIndex);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_pCamera_Photo->Get_Position());

		if (m_pModelCom->Get_AnimationFinish())
			m_bPhotoAnimEnd = true;
	})
		.AddTransition("PHOTO to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_strState == "IDLE";
	})

		.Build();

	return S_OK;
}

HRESULT CBeni::SetUp_Components()
{
	__super::SetUp_Components();
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Beni", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Beni/jizoboy_body_AO_R_M.png")), E_FAIL);
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

void CBeni::SaiyaFunc(_float fTimeDelta)
{
	if (m_pSaiya)
	{
		m_strState = m_pSaiya->Get_FSM()->GetCurStateName();
		if (m_pSaiya->Get_Disappear())
			m_bDeath = true;

		m_bPhotoAnimEnd = m_pSaiya->Get_PhotoAnimeEnd();
	}
}

void CBeni::SaiyaPos(_float3 vOffSetPos)
{
	const _float4 vPos = m_pSaiya->Get_TransformCom()->Get_Position();
	const _float4 vChangePos = _float4(vPos.x + vOffSetPos.x, vPos.y, vPos.z + vOffSetPos.z, 1.f);
	const _float4 vLook = m_pSaiya->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->Set_Position(vChangePos);
	m_pTransformCom->Set_Look(vLook);
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
