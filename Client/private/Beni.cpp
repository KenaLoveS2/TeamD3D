#include "stdafx.h"
#include "..\public\Beni.h"
#include "GameInstance.h"
#include "CameraForNpc.h"
#include "Saiya.h"

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
	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
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

	m_pSaiya = dynamic_cast<CSaiya*>( CGameInstance::GetInstance()->Get_GameObjectPtr(LEVEL_TESTPLAY, TEXT("Layer_NPC"), TEXT("Saiya")));

	if(m_pSaiya == nullptr)
	{
		MSG_BOX("!!!!!There is No Saiya!!!!!");
	}

	_float4 vPos =	m_pSaiya->Get_TransformCom()->Get_Position();
	_float4 vLook = m_pSaiya->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->Set_Position(_float3(vPos.x - 0.5f, vPos.y, vPos.z));
	m_pTransformCom->Set_Look(vLook);

	return S_OK;
}

void CBeni::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Update_Collider(fTimeDelta);
	SaiyaFunc(fTimeDelta);
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
		.InitState("PLAY")

		.AddState("PLAY")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_CHASINGLOOP);
	})

		.AddTransition("PLAY to PLAYERBACKRUN", "PLAYERBACKRUN")
		.Predicator([this]()
	{
		return  DistanceTrigger(5.f) && AnimFinishChecker(BENI_CHASINGLOOP);
	})

		.AddState("PLAYERBACKRUN")
		.OnStart([this]()
	{
		SaiyaPos();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("PLAYERBACKRUN to DISAPPEAR0", "DISAPPEAR0")
		.Predicator([this]()
	{
		return m_strState == "DISAPPEAR0";
	})


		.AddState("DISAPPEAR0")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		//디졸브
	})
		.OnExit([this]()
	{
		SaiyaPos();
	})
		.AddTransition("PLAYERBACKRUN to PLAY2", "PLAY2")
		.Predicator([this]()
	{
		return AnimFinishChecker(BENI_TELEPORT);
	})

		.AddState("PLAY2")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_CHASINGLOOP);
	})

		.AddTransition("PLAY2 to PLAYERBACKRUN2", "PLAYERBACKRUN2")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f) && AnimFinishChecker(BENI_CHASINGLOOP);
	})

		.AddState("PLAYERBACKRUN2")
		.OnStart([this]()
	{
		SaiyaPos();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("PLAYERBACKRUN2 to IDLE", "IDLE")
		.Predicator([this]()
	{
		return	m_strState == "IDLE";
	})

		/* Idle */
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
			m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("IDLE to CHEER", "CHEER")
		.Predicator([this]()
	{
			return m_strState == "CHEER";
	})

			/* Cheer */
		.AddState("CHEER")
		.OnStart([this]()
	{
		SaiyaPos();
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_CHEER);
		m_pModelCom->Set_AnimIndex(BENI_CHEER);
		CUI_ClientManager::GetInstance()->Switch_FrontUI(false);
	})

		.AddTransition("CHEER to CHAT", "CHAT")
		.Predicator([this]()
	{
		return  m_strState == "CHAT";
	})

		/* Chat */
		.AddState("CHAT")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_IDLE);
	})
		.AddTransition("CHAT to RUN", "RUN")
		.Predicator([this]()
	{
		return m_strState == "RUN";
	})

		.AddState("RUN")
		.OnStart([this]()
	{
		SaiyaPos();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(BENI_RUN);
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("RUN to DISAPPEAR", "DISAPPEAR")
		.Predicator([this]()
	{
		return m_strState == "DISAPPEAR";
	})

		.AddState("DISAPPEAR")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(BENI_TELEPORT);
		m_pModelCom->Set_AnimIndex(BENI_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
			//디졸브
	})
		.OnExit([this]()
	{
		SaiyaPos();
	})
		.AddTransition("DISAPPEAR to IDLE", "IDLE")
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
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Beni", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

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
		m_strState = m_pSaiya->Get_FSM()->GetCurStateName();
}

void CBeni::SaiyaPos()
{
	_float4 vPos = m_pSaiya->Get_TransformCom()->Get_Position();
	_float4 vLook = m_pSaiya->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->Set_Position(_float3(vPos.x - 0.5f, vPos.y, vPos.z));
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
