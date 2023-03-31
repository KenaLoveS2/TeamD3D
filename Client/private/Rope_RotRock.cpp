#include "stdafx.h"
#include "..\public\Rope_RotRock.h"
#include "Kena.h"
#include "LiftRot_Master.h"

CRope_RotRock::CRope_RotRock(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CEnviroment_Interaction(pDevice, p_context)
{
}

CRope_RotRock::CRope_RotRock(const CRope_RotRock& rhs)
	: CEnviroment_Interaction(rhs)
{
}

HRESULT CRope_RotRock::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CRope_RotRock::Initialize(void* pArg)
{	
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
		
	Push_EventFunctions();
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_pModelCom->Set_AllAnimCommonType();

	m_pTransformCom->Set_Speed(1.f);
	m_pTransformCom->Set_RotatePerSecond(0.5f);

	return S_OK;
}

HRESULT CRope_RotRock::Late_Initialize(void* pArg)
{	
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_DYNAMIC;
	BoxDesc.vPos = _float3(0.f, 0.f, 0.f);
	BoxDesc.vSize = _float3(0.9f, 1.25f, 0.9f);
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);		
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = true;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, true, COL_ENVIROMENT));
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag);
	m_pTransformCom->Set_PxPivot(m_vInitPivot);
	m_pTransformCom->Set_Position(_float4(5.f, 0.f, 5.f, 1.f));
	
	m_vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	
	m_pKena = (CKena*)m_pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	if (m_pKena == nullptr) return E_FAIL;

	m_pLiftRotMaster = (CLiftRot_Master*)m_pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Rot"), TEXT("LiftRot_Master"));
	if (m_pLiftRotMaster == nullptr) return E_FAIL;
	m_pLiftRotMaster->Set_OwnerRopeRotRokPtr(this);

	CLiftRot::DESC CutRotDesc;
	CutRotDesc.eType = CLiftRot::CUTE;
	CutRotDesc.vInitPos = _float4(-70.f, 0.f, -70.f, 1.f);
	
	// m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(55.741f, 15.093f, 1066.333f, 1.f));

	m_pCuteLiftRot = (CLiftRot*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LiftRot"), CUtile::Create_DummyString(), &CutRotDesc);
	if (m_pCuteLiftRot == nullptr) return E_FAIL;
	
	return S_OK;
}

void CRope_RotRock::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(m_pFSM) 
		m_pFSM->Tick(fTimeDelta);

	m_pCuteLiftRot->Tick(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
}

void CRope_RotRock::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pCuteLiftRot->Late_Tick(fTimeDelta);
}

void CRope_RotRock::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

HRESULT CRope_RotRock::SetUp_Components()
{	
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom))) return E_FAIL;

	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Rope_Rock"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
		
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	m_iShaderOption = 4;

	return S_OK;
}

HRESULT CRope_RotRock::Call_EventFunction(const string& strFuncName)
{
	return CGameObject::Call_EventFunction(strFuncName);
}

void CRope_RotRock::Push_EventFunctions()
{
	// Test(true, 0.f);
}

CRope_RotRock* CRope_RotRock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRope_RotRock*	pInstance = new CRope_RotRock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRope_RotRock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRope_RotRock::Clone(void* pArg)
{
	CRope_RotRock*	pInstance = new CRope_RotRock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRope_RotRock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRope_RotRock::Free()
{
	__super::Free();

	Safe_Release(m_pCuteLiftRot);
}

HRESULT CRope_RotRock::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")		
		.Tick([this](_float fTimeDelta)
	{
		_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		if (m_pTransformCom->IsClosed_XZ(vPlayerPos, 5.f))
			m_pKena->Set_RopeRotRockPtr(this);
	})
	.OnExit([this]()
	{
	})
		.AddTransition("IDLE to CHOICE", "CHOICE")
		.Predicator([this]()
	{
		return m_bChoiceFlag;
	})		
	
		
		.AddState("CHOICE")
		.OnStart([this]()
	{
		_float4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPosition.y += 2.5f;		
		vPosition.w = 1.f;
		m_pCuteLiftRot->Execute_StartCute(vPosition);
	})
		.Tick([this](_float fTimeDelta)
	{
		_float3 vPlayerPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		if (m_pTransformCom->IsClosed_XZ(vPlayerPos, 5.f))
			m_pKena->Set_RopeRotRockPtr(this);
	})
		.OnExit([this]()
	{
		m_bMoveFlag = true;
		m_pCuteLiftRot->Execute_StartCute(m_vMoveTargetPosition);
		m_pLiftRotMaster->Execute_WakeUp(m_pTransformCom->Get_WorldMatrix(), m_vRotCreatePosOffset, m_vRotLiftPosOffset);
	})
		.AddTransition("CHOICE to UP_WAIT", "UP_WAIT")
		.Predicator([this]()
	{
		return m_bMoveFlag;
	})
		
		
		.AddState("UP_WAIT")
		.OnStart([this]()
	{	
	})
		.Tick([this](_float fTimeDelta)
	{	
	})
		.OnExit([this]()
	{
	})
		.AddTransition("UP_WAIT to UP", "UP")
		.Predicator([this]()
	{
		return m_pLiftRotMaster->Is_LiftReady();
	})
		
		
		.AddState("UP")
		.OnStart([this]()
	{
		m_pLiftRotMaster->Execute_LiftStart();
		m_vPxPivotDist = m_pTransformCom->Get_vPxPivot();
		m_vNewPivot = m_vInitPivot;
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_vNewPivot.y -= fOffsetY * fTimeDelta;
		if (m_vNewPivot.y <= m_vUpPivot.y)
			m_vNewPivot.y = m_vUpPivot.y;

		m_pTransformCom->Set_PxPivot(m_vNewPivot);
	})
		.OnExit([this]()
	{
		
	})
		.AddTransition("UP to TURN", "TURN")
		.Predicator([this]()
	{
		return m_pLiftRotMaster->Is_LiftEnd();
	})
		
		.AddState("TURN")
		.OnStart([this]()
	{
		m_pLiftRotMaster->Execute_LiftMoveStart();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pLiftRotMaster->Execute_Move(m_pTransformCom->Get_WorldMatrix(), m_vRotLiftPosOffset);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	})
		.AddTransition("TURN to MOVE", "MOVE")
		.Predicator([this]()
	{
		return m_pTransformCom->IsLook(m_vMoveTargetPosition, 5.f);
	})


		.AddState("MOVE")
		.OnStart([this]()
	{
		
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_vMoveTargetPosition, fTimeDelta);
		m_pLiftRotMaster->Execute_Move(m_pTransformCom->Get_WorldMatrix(), m_vRotLiftPosOffset);

		if (m_pTransformCom->IsClosed_XZ(m_vMoveTargetPosition, 5.f))
			m_pCuteLiftRot->Execute_EndCute();
	})	
		.OnExit([this]()
	{
		m_pLiftRotMaster->Execute_LiftMoveEnd();		
	})
		.AddTransition("MOVE to DOWN ", "DOWN")
		.Predicator([this]()
	{	
		_bool bClosed = m_pTransformCom->IsClosed_XZ(m_vMoveTargetPosition, 0.1f);
		return bClosed;
	})
		

		.AddState("DOWN")
		.OnStart([this]()
	{
		m_vNewPivot = m_vUpPivot;
		m_bLiftDownEndFlag = false;		
	})
		.Tick([this](_float fTimeDelta)
	{
		
		m_vNewPivot.y += fOffsetY * fTimeDelta;
		if (m_vNewPivot.y >= m_vInitPivot.y)
		{
			m_vNewPivot.y = m_vInitPivot.y;
			m_bLiftDownEndFlag = true;
		}	

		m_pTransformCom->Set_PxPivot(m_vNewPivot);
	})
	.OnExit([this]()
	{	
		m_bChoiceFlag = false;
		m_bMoveFlag = false;		
		m_pKena->End_LiftRotRock();
	})
		.AddTransition("DOWN to IDLE ", "IDLE")
		.Predicator([this]()
	{		
		return m_bLiftDownEndFlag;
	})		
		.Build();


	return S_OK;
}
