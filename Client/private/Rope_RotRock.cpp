#include "stdafx.h"
#include "..\public\Rope_RotRock.h"
#include "Rot_State.h"
#include "Kena.h"

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
	BoxDesc.vSize = _float3(1.f, 1.25f, 1.f);
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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, true));
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag);
	m_pTransformCom->Set_PxPivot(_float3(0.f, 1.2f, 0.f));
	m_pTransformCom->Set_Position(_float4(5.f, 0.f, 5.f, 1.f));
	
	m_vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	
	m_pPlayerKena = (CKena*)m_pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	if (m_pPlayerKena == nullptr) return E_FAIL;

	return S_OK;
}

void CRope_RotRock::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(m_pFSM) 
		m_pFSM->Tick(fTimeDelta);

	m_bMoveFlag;
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_pTransformCom->Set_Position(_float4(2.f, 0.f, 2.f, 1.f));
	}

	m_pTransformCom->Tick(fTimeDelta);
}

void CRope_RotRock::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
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
}

HRESULT CRope_RotRock::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE") // IDLE STATE 추가		
		.OnStart([this]()
	{
		m_vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);		
	})
		.Tick([this](_float fTimeDelta)
	{
		// m_pTransformCom->Set_Position(m_vInitPosition);

		_float3 vPlayerPos = m_pPlayerKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);

		if (m_pTransformCom->IsClosed_XZ(vPlayerPos, 5.f))
			m_pPlayerKena->Set_RopeRotRockPtr(this);
	})
		.AddTransition("IDLE to MOVE", "MOVE")
		.Predicator([this]()
	{
		return m_bMoveFlag;
	})		
		.OnExit([this]()
	{
		m_bMoveFlag = true;
	})
		.AddState("MOVE") // MOVE STATE 추가
		.OnStart([this]()
	{
		m_vMoveTargetPosition;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Chase(m_vMoveTargetPosition, fTimeDelta);
	})		
		.AddTransition("MOVE to IDLE ", "IDLE")
		.Predicator([this]()
	{
		_bool bClosed = m_pTransformCom->IsClosed_XZ(m_vMoveTargetPosition, 0.1f);
		return bClosed;
	})
		.OnExit([this]()
	{
		m_bMoveFlag = false;		
	})
		.Build();


	return S_OK;
}
