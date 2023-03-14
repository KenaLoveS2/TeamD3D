#include "stdafx.h"
#include "..\public\Rot.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "Rope_RotRock.h"

#include "Kena.h"
#include "Kena_Status.h"

_uint CRot::m_iEveryRotCount = 0;
_uint CRot::m_iKenaFindRotCount = 0;
vector<CRot*> CRot::m_vecKenaConnectRot;

CRot::CRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CRot::CRot(const CRot& rhs)
	:CGameObject(rhs)
{
}

const _double& CRot::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

HRESULT CRot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRot::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 1.5f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_pTransformCom->Set_Translation(_float4(5.f, 0.f, 5.f, 1.f), _float4());

	m_pModelCom->Set_AnimIndex(IDLE);
	m_pModelCom->Set_AllAnimCommonType();

	Push_EventFunctions();

	m_iEveryRotCount++;
	m_iObjectProperty = OP_ROT;

	return S_OK;
}

HRESULT CRot::Late_Initialize(void * pArg)
{	
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	assert(m_pKena != nullptr && "CRot::Late_Initialize");

	m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Get_TransformCom());
	assert(m_pKenaTransform != nullptr && "CRot::Late_Initialize");

	m_pkenaState = dynamic_cast<CKena_Status*>(m_pKena->Get_Status());
	assert(m_pkenaState != nullptr && "CRot::Late_Initialize");
	
	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = m_szCloneObjectTag;
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

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_ROT));

	// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.15f, 0.f));
	m_pTransformCom->Set_Position(_float3(-50.f, 0.f, -50.f));

	m_vWakeUpPosition = _float4(3.f, 0.f, 3.f, 1.f);

	m_pTriggerDAta = Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_ROT, CUtile::Float_4to3(m_vWakeUpPosition), 1.f);

	CPhysX_Manager::GetInstance()->Create_Trigger(m_pTriggerDAta);

	if (m_iThisRotIndex == FIRST_ROT)
		m_vecKenaConnectRot.reserve(m_iEveryRotCount);

	return S_OK;
}

_float Temp =  1.f;

void CRot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (GetKeyState('F') & 0x8000)
	{
		// m_bWakeUp = true;
		Temp += 1.f * fTimeDelta;
		CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, Temp);
	}

	if (m_pFSM)
		m_pFSM->Tick(fTimeDelta);
		
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
}

void CRot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CRot::Render()
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
		else	if (i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
		else		if (i == 2)
		{
			// 머리카락 모르겠음.
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_AlphaTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices",2);
		}
	}

	return S_OK;
}

HRESULT CRot::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);

	return S_OK;
}

void CRot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CRot::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Rot Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	//if (ImGui::BeginTabItem("State"))
	//{
	//	m_pStateMachine->Imgui_RenderProperty();
	//	ImGui::EndTabItem();
	//}

	ImGui::EndTabBar();
}

void CRot::ImGui_ShaderValueProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
}

void CRot::ImGui_PhysXValueProperty()
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

	// 이게 사실상 px 매니저 imgui_render에 있긴함
	/*PxRigidActor* pRigidActor =	CPhysX_Manager::GetInstance()->Find_DynamicActor(m_szCloneObjectTag);
	_float fMass = ((PxRigidDynamic*)pRigidActor)->getMass();
	ImGui::DragFloat("Mass", &fMass, 0.01f, -100.f, 500.f);
	_float fLinearDamping = ((PxRigidDynamic*)pRigidActor)->getLinearDamping();
	ImGui::DragFloat("LinearDamping", &fLinearDamping, 0.01f, -100.f, 500.f);
	_float fAngularDamping = ((PxRigidDynamic*)pRigidActor)->getAngularDamping();
	ImGui::DragFloat("AngularDamping", &fAngularDamping, 0.01f, -100.f, 500.f);
	_float3 vVelocity = CUtile::ConvertPosition_PxToD3D(((PxRigidDynamic*)pRigidActor)->getLinearVelocity());
	float fVelocity[3] = { vVelocity.x, vVelocity.y, vVelocity.z };
	ImGui::DragFloat3("PxVelocity", fVelocity, 0.01f, 0.1f, 100.0f);
	vVelocity.x = fVelocity[0]; vVelocity.y = fVelocity[1]; vVelocity.z = fVelocity[2];
	CPhysX_Manager::GetInstance()->Set_DynamicParameter(pRigidActor, fMass, fLinearDamping, vVelocity);*/
}

HRESULT CRot::Call_EventFunction(const string& strFuncName)
{
	return CGameObject::Call_EventFunction(strFuncName);
}

void CRot::Push_EventFunctions()
{
	Test(true, 0.f);
}

HRESULT CRot::SetUp_Components()
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

HRESULT CRot::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CRot::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

void CRot::Test(_bool bIsInit, _float fTimeDelta)
{
}

CRot* CRot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRot*	pInstance = new CRot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRot::Clone(void* pArg)
{
	CRot*	pInstance = new CRot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRot::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

HRESULT CRot::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.Tick([this](_float fTimeDelta)
	{
	
	})
		.AddTransition("SLEEP to WAKE_UP", "WAKE_UP")
		.Predicator([this]()
	{
		return m_bWakeUp;
	})
		.OnExit([this]()
	{
		m_iThisRotIndex = m_iKenaFindRotCount++;
		m_pkenaState->Set_RotCount(m_iKenaFindRotCount);

		m_vecKenaConnectRot.push_back(this);
		
		if (m_iThisRotIndex == 0)
			m_pKena->Set_FirstRotPtr(this);

		m_pTransformCom->Set_Position(m_vWakeUpPosition);
	})


		.AddState("WAKE_UP")
		.OnStart([this]()
	{	
		m_pModelCom->Set_AnimIndex(TELEPORT3);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.AddTransition("WAKE_UP to COLLECT ", "COLLECT")
		.Predicator([this]()
	{	
		return m_pModelCom->Get_AnimationFinish();
	})
		.OnExit([this]()
	{

	})
		.AddState("COLLECT")
		.OnStart([this]()
	{
		// PHOTOPOSE1, PHOTOPOSE2, PHOTOPOSE3, PHOTOPOSE4, PHOTOPOSE5, PHOTOPOSE6, PHOTOPOSE7, PHOTOPOSE8,
		// m_iCuteAnimIndex = rand() % (PHOTOPOSE8 - PHOTOPOSE1) + PHOTOPOSE1;

		// COLLECT, COLLECT2, COLLECT3, COLLECT4, COLLECT5, COLLECT6, COLLECT7, COLLECT8,
		m_iCuteAnimIndex = rand() % (COLLECT8 - COLLECT) + COLLECT;
		m_pModelCom->Set_AnimIndex(m_iCuteAnimIndex);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.AddTransition("COLLECT to IDLE ", "IDLE")
		.Predicator([this]()
	{
		_bool bCuteAnimFinish = (m_iCuteAnimIndex == m_pModelCom->Get_AnimIndex()) && m_pModelCom->Get_AnimationFinish();
		return bCuteAnimFinish;
	})
		.OnExit([this]()
	{

	})

		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.Tick([this](_float fTimeDelta)
	{
		
	})
		.AddTransition("IDLE to FOLLOW_KENA ", "FOLLOW_KENA")
		.Predicator([this]()
	{
		_float4 vPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);

		return !m_pTransformCom->IsClosed_XZ(vPos, m_fKenaToRotDistance);
	})

		.AddState("FOLLOW_KENA")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(WALK);		
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION), fTimeDelta, 1.f);
	})
		.AddTransition("FOLLOW_KENA to IDLE", "IDLE")
		.Predicator([this]()
	{	
		_float4 vPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
		return m_pTransformCom->IsClosed_XZ(vPos, m_fKenaToRotDistance);
	})		
		.OnExit([this]()
	{

	})

		.Build();


	return S_OK;
}

_int CRot::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{	
	return 0;
}

_int CRot::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget && iTriggerIndex == ON_TRIGGER_PARAM_TRIGGER && iColliderIndex == COL_PLAYER)
	{
		m_bWakeUp = true;
	}

	return 0;
}