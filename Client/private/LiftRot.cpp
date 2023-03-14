#include "stdafx.h"
#include "..\public\LiftRot.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "Kena.h"
#include "Rope_RotRock.h"

CLiftRot::CLiftRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CLiftRot::CLiftRot(const CLiftRot& rhs)
	:CGameObject(rhs)
{
}

const _double& CLiftRot::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

HRESULT CLiftRot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CLiftRot::Initialize(void* pArg)
{
	if (pArg) memcpy(&m_Desc, pArg, sizeof(DESC));
	else {
		m_Desc.eType = LIFT;
		m_Desc.vInitPos = _float4(-50.f, 0.f, -50.f, 1.f);
	}

	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 1.5f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_LiftFSM(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_CuteFSM(), E_FAIL);
	Set_Type(m_Desc.eType);

	m_pModelCom->Set_AnimIndex(CRot::IDLE);
	m_pModelCom->Set_AllAnimCommonType();

	Push_EventFunctions();
	
	return S_OK;
}

HRESULT CLiftRot::Late_Initialize(void * pArg)
{	
	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = m_szCloneObjectTag;
	PxCapsuleDesc.isGravity = true;
	PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fRadius = 0.1f;
	PxCapsuleDesc.fHalfHeight = 0.04f;
	PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fDensity = 1.f;
	PxCapsuleDesc.fAngularDamping = 0.5f;
	PxCapsuleDesc.fMass = 10.f;
	PxCapsuleDesc.fLinearDamping = 10.f;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;
	PxCapsuleDesc.eFilterType = FILTER_DEFULAT;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_DEFAULT));

	// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.15f, 0.f));
	m_pTransformCom->Set_Position(m_Desc.vInitPos);

	return S_OK;
}

void CLiftRot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pLiftFSM)
		m_pLiftFSM->Tick(fTimeDelta);
		
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
}

void CLiftRot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CLiftRot::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	 iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");

		if(i == 0)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",1);
		}

		if (i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

		if (i == 2)
		{
			// 머리카락 모르겠음.
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_AlphaTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",2);
		}
	}

	return S_OK;
}

HRESULT CLiftRot::RenderShadow()
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

void CLiftRot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CLiftRot::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Rot Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CLiftRot::ImGui_ShaderValueProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
}

void CLiftRot::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();

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

HRESULT CLiftRot::Call_EventFunction(const string& strFuncName)
{
	return CGameObject::Call_EventFunction(strFuncName);
}

HRESULT CLiftRot::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimRotModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Rot", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	m_pModelCom->Set_RootBone("Rot_RIG");

	//  0 : Body
	//	 1 : Eye
	//	 2 : Hair
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Rot/rh_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Rot/rot_fur_ALPHA.png")), E_FAIL);

	return S_OK;
}

HRESULT CLiftRot::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CLiftRot::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLiftRot* CLiftRot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLiftRot*	pInstance = new CLiftRot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLiftRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLiftRot::Clone(void* pArg)
{
	CLiftRot*	pInstance = new CLiftRot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLiftRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiftRot::Free()
{
	__super::Free();

	Safe_Release(m_pCuteFSM);
	Safe_Release(m_pWorkFSM);
	Safe_Release(m_pLiftFSM);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

HRESULT CLiftRot::SetUp_LiftFSM()
{
	m_pLiftFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.Tick([this](_float fTimeDelta)
	{
	
	})
		.OnExit([this]()
	{
		m_bCreateStart = true;
	})
		.AddTransition("SLEEP to CREATE", "CREATE")
		.Predicator([this]()
	{
		return m_bWakeUp;
	})
		
		.AddState("CREATE")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::TELEPORT6);
	})
		.Tick([this](_float fTimeDelta)
	{
	})
		.AddTransition("CREATE to LIFT_POS_MOVE", "LIFT_POS_MOVE")
		.Predicator([this]()
	{			
		return m_pModelCom->Get_AnimationFinish();
	})

		.AddState("LIFT_POS_MOVE")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::WALK);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_vLiftPos, fTimeDelta);
	})
		.AddTransition("LIFT_POS_MOVE to LIFT_READY ", "LIFT")
		.Predicator([this]()
	{		
		m_bLiftReady = m_pTransformCom->IsClosed_XZ(m_vLiftPos, 0.2f);
		if(m_bLiftReady)
			m_pModelCom->Set_AnimIndex(CRot::CARRYLOOP);

		return m_bLiftStart;
	})		

		.AddState("LIFT")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::LIFT);
	})
		.Tick([this](_float fTimeDelta)
	{
		
	})
		.AddTransition("LIFT to LIFT_MOVE", "LIFT_MOVE")
		.Predicator([this]()
	{	
		m_bLiftEnd = m_iAnimationIndex == CRot::LIFT && m_pModelCom->Get_AnimationFinish();
		return m_bLiftMoveStart;
	})		

		.AddState("LIFT_MOVE")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::CARRYFORWARD);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.AddTransition("LIFT_MOVE to LIFT_DOWN", "LIFT_DOWN")
		.Predicator([this]()
	{		
		return m_bLiftMoveEnd;
	})

		.AddState("LIFT_DOWN")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::LIFT2);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.AddTransition("LIFT_DOWN to GO_SLEEP", "GO_SLEEP")
		.Predicator([this]()
	{
		return m_bLiftDownEnd;
	})

		.AddState("GO_SLEEP")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::LIFT2);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.OnExit([this]()
	{
		m_bWakeUp = false;
		m_bCreateStart = false;
		m_bLiftReady = false;
		m_bLiftStart = false;
		m_bLiftEnd = false;
		m_bLiftMoveStart = false;
		m_bLiftMoveEnd = false;
		m_bLiftDownEnd = false;
		m_pTransformCom->Set_Position(m_Desc.vInitPos);
	})
		.AddTransition("GO_SLEEP to SLEEP", "SLEEP")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})
		.Build();

	return S_OK;
}

HRESULT CLiftRot::SetUp_CuteFSM()
{
	m_pCuteFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.Tick([this](_float fTimeDelta)
	{

	})
		.AddTransition("SLEEP to CUTE", "CUTE")
		.Predicator([this]()
	{
		return m_bCuteStart;
	})

		.AddState("CUTE")
		.OnStart([this]()
	{
		// 나타나는 이펙트
		m_pModelCom->Set_AnimIndex(CRot::TELEPORT7);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(CRot::TELEPORT2);
	})
		.AddTransition("CUTE to GO_SLEEP", "GO_SLEEP")
		.Predicator([this]()
	{	
		return m_bCuteEnd;
	})

		.AddState("GO_SLEEP")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(CRot::IDLE); // 사라지는 애님
		
	})
		.Tick([this](_float fTimeDelta)
	{
		
	})
		.OnExit([this]()
	{
		// 사라지는 이펙트
		m_bCuteStart = false;
		m_bCuteEnd = false;
		m_pTransformCom->Set_Position(m_Desc.vInitPos);
	})
		.AddTransition("GO_SLEEP to SLEEP", "SLEEP")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})

		.Build();

	return S_OK;
}

_int CLiftRot::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget)
	{
		
	}
	
	return 0;
}

void CLiftRot::Execute_WakeUp(_float4& vCreatePos, _float4& vLiftPos)
{
	m_pTransformCom->Set_Position(vCreatePos);
	m_vLiftPos = vLiftPos;	
	m_bWakeUp = true;
}

void CLiftRot::Execute_LiftStart()
{
	m_bLiftStart = true;
}

void CLiftRot::Execute_LiftMoveStart()
{
	m_bLiftMoveStart = true;
}

void CLiftRot::Execute_LiftMoveEnd()
{
	m_bLiftMoveEnd = true;
}

void CLiftRot::Execute_LiftDownEnd()
{
	m_bLiftDownEnd = true;
}

void CLiftRot::Execute_StartCute(_float4& vCreatePos)
{
	m_pTransformCom->Set_Position(vCreatePos);
	m_bCuteStart = true;
}

void CLiftRot::Execute_EndCute()
{
	m_bCuteEnd = true;	
}

void CLiftRot::Set_NewPosition(_float4 vNewPos)
{
	m_pTransformCom->Set_Position(vNewPos);
}