#include "stdafx.h"
#include "..\public\LiftRot.h"
#include "GameInstance.h"
#include "Kena.h"
#include "Rope_RotRock.h"
#include "E_TeleportRot.h"
#include "LiftRot_Master.h"
#include "Rot.h"

CLiftRot::CLiftRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	: CRot_Base(pDevice, p_context)
{
}

CLiftRot::CLiftRot(const CLiftRot& rhs)
	: CRot_Base(rhs)
{
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
	
	FAILED_CHECK_RETURN(__super::Initialize(nullptr), E_FAIL);
	Set_Type(m_Desc.eType);
			
	return S_OK;
}

HRESULT CLiftRot::Late_Initialize(void * pArg)
{	
	FAILED_CHECK_RETURN(__super::Late_Initialize(nullptr), E_FAIL);
	
	m_pTransformCom->Set_Position(m_Desc.vInitPos);
		
	return S_OK;
}

void CLiftRot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pWorkFSM)
		m_pWorkFSM->Tick(fTimeDelta);
		
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
	m_pTeleportRot->Tick(fTimeDelta);
}

void CLiftRot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	m_pTeleportRot->Late_Tick(fTimeDelta);
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

HRESULT CLiftRot::SetUp_State()
{	
	FAILED_CHECK_RETURN(SetUp_LiftFSM(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_CuteFSM(), E_FAIL);

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
	Safe_Release(m_pLiftFSM);
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
		m_bLiftDownEnd = false;
		m_pTeleportRot->Set_Position(m_pTransformCom->Get_Position());
		m_pTeleportRot->Set_Active(true);
		m_pModelCom->ResetAnimIdx_PlayTime(TELEPORT6);
		m_pModelCom->Set_AnimIndex(TELEPORT6);
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
		m_pModelCom->Set_AnimIndex(WALK);
	})
		.Tick([this](_float fTimeDelta)
	{	
		if (m_bLiftReady == false)
		{
			m_pTransformCom->Chase(m_vLiftPos, fTimeDelta);
			m_bLiftReady = m_pTransformCom->IsClosed_XZ(m_vLiftPos, 0.2f);
		}
		else
		{
			m_pModelCom->Set_AnimIndex(CARRYLOOP);			
			m_pTransformCom->Set_Position(m_vLiftPos);
		}
			
	})
		.AddTransition("LIFT_POS_MOVE to LIFT_READY ", "LIFT")
		.Predicator([this]()
	{	
		return m_bLiftStart;
	})		

		.AddState("LIFT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(CARRYFORWARD);
		m_pModelCom->Set_AnimIndex(CARRYFORWARD);
		m_fTimeCheck = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fTimeCheck += fTimeDelta;
		m_pTransformCom->Set_Position(m_vLiftPos);
	})
		.AddTransition("LIFT to LIFT_MOVE", "LIFT_MOVE")
		.Predicator([this]()
	{	
		// m_bLiftEnd = m_pModelCom->Get_AnimationFinish();
		m_bLiftEnd = m_fTimeCheck >= 1.f;
		return m_bLiftMoveStart;
	})		

		.AddState("LIFT_MOVE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(CARRYFORWARD);
		m_pModelCom->Set_AnimIndex(CARRYFORWARD);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.AddTransition("LIFT_MOVE to GO_SLEEP", "GO_SLEEP")
		.Predicator([this]()
	{		
		return m_bLiftMoveEnd;
	})

	/*	.AddState("LIFT_DOWN")
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
		m_bLiftDownEnd = (m_iAnimationIndex == CRot::LIFT2) && m_pModelCom->Get_AnimationFinish();
		return m_bLiftDownEnd;
	})*/

		.AddState("GO_SLEEP")
		.OnStart([this]()
	{
		
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.OnExit([this]()
	{
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), IDLE);
		
		m_bWakeUp = false;
		m_bCreateStart = false;
		m_bLiftReady = false;
		m_bLiftStart = false;
		m_bLiftEnd = false;
		m_bLiftMoveStart = false;
		m_bLiftMoveEnd = false;
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
		m_pTransformCom->LookAt_NoUpDown(CRot::Get_RotUseKenaPos());
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), TELEPORT7);
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_4], 1.f);
		m_bCuteEnd = false;		
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(CRot::Get_RotUseKenaPos());
		if (m_pTeleportRot->Get_Active() == false)
		{
			m_pTeleportRot->Set_Active(true);
			m_pTeleportRot->Set_Position(m_pTransformCom->Get_Position()); // m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_4], 1.f);
		}
		
		if (m_pModelCom->Get_AnimationFinish())
		{
			m_pModelCom->ResetAnimIdx_PlayTime(TELEPORT2);
			m_pModelCom->Set_AnimIndex(TELEPORT2);
		}			
	})
		.AddTransition("CUTE to GO_SLEEP", "GO_SLEEP")
		.Predicator([this]()
	{	
		return m_bCuteEnd;
	})

		.AddState("GO_SLEEP")
		.OnStart([this]()
	{	
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), TELEPORT7);
		m_pTransformCom->Set_Position(m_Desc.vInitPos);
	})		
		.OnExit([this]()
	{			
		m_bCuteStart = false;
		m_bCuteEnd = false;		
	})
		.AddTransition("GO_SLEEP to SLEEP", "SLEEP")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})

		.Build();

	return S_OK;
}

void CLiftRot::Execute_WakeUp(_float4& vCreatePos, _float4& vLiftPos)
{
	m_pTransformCom->Set_Position(vCreatePos);
	m_vLiftPos = vLiftPos;
	m_bWakeUp = true;
}

void CLiftRot::Execute_LiftStart(_float4 vLookPos)
{
	m_bLiftStart = true;
	m_pTransformCom->LookAt_NoUpDown(vLookPos);	
}

void CLiftRot::Execute_LiftMoveStart()
{
	m_bLiftMoveStart = true;
}

void CLiftRot::Execute_LiftMoveEnd()
{
	m_bLiftMoveEnd = true;
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

void CLiftRot::Set_NewPosition(_float4 vNewPos, _float4 vLookPos)
{	
	m_pTransformCom->Set_Position(vNewPos);
	m_pTransformCom->LookAt_NoUpDown(vLookPos);
}

