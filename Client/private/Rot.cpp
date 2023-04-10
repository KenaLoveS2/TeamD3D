#include "stdafx.h"
#include "..\public\Rot.h"
#include "FSMComponent.h"
#include "Rope_RotRock.h"
#include "CameraForRot.h"
#include "RotWisp.h"
#include "E_TeleportRot.h"

#include "Kena.h"
#include "Kena_Status.h"
#include "RotHat.h"

_uint CRot::m_iEveryRotCount = 0;
_uint CRot::m_iKenaFindRotCount = 0;
vector<CRot*> CRot::m_vecKenaConnectRot;
_float4 CRot::m_vKenaPos = {0.f, 0.f, 0.f, 1.f};

CRot::CRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	: CRot_Base(pDevice, p_context)	
{
}

CRot::CRot(const CRot& rhs)
	: CRot_Base(rhs)	
{
}

HRESULT CRot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRot::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(nullptr), E_FAIL);
		
	ZeroMemory(&m_Desc, sizeof(CRot::DESC));
	if (pArg != nullptr)
		memcpy(&m_Desc, pArg, sizeof(CRot::DESC));
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = (_float)m_iEveryRotCount;
		m_Desc.WorldMatrix._43 = -5.f;
	}

	m_pRotHat = (CRotHat*)CGameInstance::GetInstance()->Clone_GameObject(TEXT("Prototype_GameObject_RotHat"), CUtile::Create_DummyString(TEXT("Hat"), m_iEveryRotCount));
	assert(m_pRotHat && " CRot::Initialize()");

	m_iEveryRotCount++;
	m_iObjectProperty = OP_ROT;

	return S_OK;
}

HRESULT CRot::Late_Initialize(void * pArg)
{	
	__super::Late_Initialize(nullptr);

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_vWakeUpPosition = _float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f);
	
	m_pMyCam = static_cast<CCameraForRot*>(m_pGameInstance->Find_Camera(L"ROT_CAM"));	

	wstring wstrTag = wstring(m_szCloneObjectTag) + TEXT("Wisp");
	m_pRotWisp = static_cast<CRotWisp*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RotWisp"), CUtile::Create_StringAuto(wstrTag.c_str())));
	m_pRotWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42 + 0.3f, m_Desc.WorldMatrix._43, 1.f));

	if(!wcscmp(m_szCloneObjectTag, TEXT("Saiya_Rot")) || !wcscmp(m_szCloneObjectTag, TEXT("Hunter_Rot")))
		m_bWakeUp = true;
	else if(!m_bManualWakeUp)
		CPhysX_Manager::GetInstance()->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_ROT, CUtile::Float_4to3(m_vWakeUpPosition), 1.f));

	if (m_iThisRotIndex == FIRST_ROT && m_bManualWakeUp == false)
		m_vecKenaConnectRot.reserve(m_iEveryRotCount);

	return S_OK;
}

void CRot::Tick(_float fTimeDelta)
{
	 m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	 m_pModelCom->Play_Animation(fTimeDelta);
	 m_pRotHat->Tick(fTimeDelta);
	 // m_pTransformCom->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42 + 0.3f, m_Desc.WorldMatrix._43, 1.f));
	 m_pTransformCom->Tick(fTimeDelta);
	 return;

	__super::Tick(fTimeDelta);

	if (m_pRotWisp->Get_Collect())
	{
		if (m_pFSM) m_pFSM->Tick(fTimeDelta);

		m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
		m_pModelCom->Play_Animation(fTimeDelta);
	}
	else if (m_bWakeUp)
	{
		m_pRotWisp->Tick(fTimeDelta);
		m_pKena->Set_RotWispInteractable(true);
	}

	if (m_pTeleportRot)
		m_pTeleportRot->Tick(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
	m_pRotHat->Tick(fTimeDelta);
}

void CRot::Late_Tick(_float fTimeDelta)
{
	 __super::Late_Tick(fTimeDelta);
	 m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	 m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	 m_pRotHat->Late_Tick(fTimeDelta);
	 return;

	if(m_pRotWisp->Get_Collect())
	{
		__super::Late_Tick(fTimeDelta);
		if (m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
	}
	else if (m_bWakeUp)
	{
		m_pRotWisp->Late_Tick(fTimeDelta);
	}

	if (m_pTeleportRot)
		m_pTeleportRot->Late_Tick(fTimeDelta);

	m_pRotHat->Late_Tick(fTimeDelta);
}

void CRot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CRot::ImGui_AnimationProperty()
{
	//m_pRotWisp->ImGui_AnimationProperty();

 	ImGui::BeginTabBar("Rot Animation & State");
 
 	if (ImGui::BeginTabItem("Animation"))
 	{
 		m_pModelCom->Imgui_RenderProperty();
 		ImGui::EndTabItem();
 	}
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
}

void CRot::AlreadyRot()
{
	m_iThisRotIndex = m_iKenaFindRotCount++;
	m_vecKenaConnectRot.push_back(this);
	
	m_bManualWakeUp = true;
	m_pRotWisp->Set_Collect(true);
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

	Safe_Release(m_pRotWisp);
	Safe_Release(m_pRotHat);	
}

HRESULT CRot::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.Tick([this](_float fTimeDelta)
	{
	
	})
		.OnExit([this]()
	{
		if(m_bManualWakeUp == false)
		{
			m_iThisRotIndex = m_iKenaFindRotCount++;
			//m_pkenaState->Set_RotCount(m_iKenaFindRotCount);
			m_pkenaState->Add_RotCount();

			m_vecKenaConnectRot.push_back(this);

			if (m_iThisRotIndex == FIRST_ROT)
			{
				m_pKena->Set_FirstRotPtr(this);
				m_vKenaPos = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION);
			}

			m_pTransformCom->Set_Position(m_vWakeUpPosition);
		}		

		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	})
	.AddTransition("SLEEP to IDLE", "IDLE")
	.Predicator([this]()
	{
		return m_bManualWakeUp;
	})
		.AddTransition("SLEEP to WAKE_UP", "WAKE_UP")
		.Predicator([this]()
	{
		return m_bWakeUp;
	})
		
		
		.AddState("WAKE_UP")
		.OnStart([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pKena->Set_RotWispInteractable(false);		
		m_pMyCam->Set_Target(this);
		m_pGameInstance->Work_Camera(L"ROT_CAM");
		
		TurnOn_TeleportEffect(m_vWakeUpPosition, TELEPORT3);
	})	
		.AddTransition("WAKE_UP to COLLECT ", "COLLECT")
		.Predicator([this]()
	{	
		return m_pModelCom->Get_AnimationFinish();
	})
		
		.AddState("COLLECT")
		.OnStart([this]()
	{
		// PHOTOPOSE1, PHOTOPOSE2, PHOTOPOSE3, PHOTOPOSE4, PHOTOPOSE5, PHOTOPOSE6, PHOTOPOSE7, PHOTOPOSE8,
		// COLLECT, COLLECT2, COLLECT3, COLLECT4, COLLECT5, COLLECT6, COLLECT7, COLLECT8,

		// m_iCuteAnimIndex = rand() % (PHOTOPOSE8 - PHOTOPOSE1) + PHOTOPOSE1;
		m_iCuteAnimIndex = rand() % (COLLECT8 - COLLECT) + COLLECT;
		
		m_pModelCom->ResetAnimIdx_PlayTime(m_iCuteAnimIndex);
		m_pModelCom->Set_AnimIndex(m_iCuteAnimIndex);
		_tchar* pSound = m_pCopySoundKey[(rand() % 4) * 2];
		CGameInstance::GetInstance()->Play_Sound(pSound, 1.f);
	})
		.Tick([this](_float fTimeDelta)
	{

	})
		.OnExit([this]()
	{
		if (m_pKena->Get_State(CKena::STATE_LEVELUP) == true)
			m_pKena->Set_State(CKena::STATE_LEVELUP_READY, true);

		m_pMyCam->Set_Target(nullptr);
		m_pGameInstance->Work_Camera(L"PLAYER_CAM");		
	})
		.AddTransition("COLLECT to IDLE ", "IDLE")
		.Predicator([this]()
	{
		_bool bCuteAnimFinish = (m_iCuteAnimIndex == m_pModelCom->Get_AnimIndex()) && m_pModelCom->Get_AnimationFinish();
		return bCuteAnimFinish;
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
		m_pTeleportRot->Set_Active(false);
		return !m_pTransformCom->IsClosed_XZ(m_vKenaPos, m_fKenaToRotDistance);
	})
		
		.AddState("FOLLOW_KENA")
		.OnStart([this]()
	{
		m_pModelCom->Set_AnimIndex(RUN);		
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_pTransformCom->Chase(m_vKenaPos, fTimeDelta, 1.f);
	})
		.OnExit([this]()
	{

	})
		.AddTransition("FOLLOW_KENA to IDLE", "IDLE")
		.Predicator([this]()
	{	
		return m_pTransformCom->IsClosed_XZ(m_vKenaPos, m_fKenaToRotDistance);
	})		
		.AddTransition("FOLLOW_KENA to TELEPORT_KENA ", "TELEPORT_KENA")
		.Predicator([this]()
	{
		return !m_pTransformCom->IsClosed_XZ(m_vKenaPos, m_fTeleportDistance);
	})

		.AddState("TELEPORT_KENA")
		.OnStart([this]()
	{
		_float4 vKenaLook = m_pKenaTransform->Get_State(CTransform::STATE_LOOK);
		_float4 vTeleportPos = m_vKenaPos - vKenaLook * 1.f + _float4(CUtile::Get_RandomFloat(-0.5f, 1.f), 0.f, CUtile::Get_RandomFloat(-0.5f, 1.f), 0.f);

		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Set_Position(vTeleportPos);

		TurnOn_TeleportEffect(vTeleportPos, TELEPORT7);		
	})
		.AddTransition("FOLLOW_KENA to IDLE ", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.Build();


	return S_OK;
}

void CRot::Set_WispPos(_float4 vPos)
{
	if(m_pRotWisp)
		m_pRotWisp->Set_Position(vPos);
}

void CRot::Set_WakeUpPos(_float4 vPos)
{
	m_vWakeUpPosition = vPos;
}

_int CRot::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{	
	if (pTarget && iColliderIndex == (_int)COL_PLAYER)
	{
		m_bWakeUp = true;
	}

	return 0;
}

_int CRot::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget && iTriggerIndex == (_uint)ON_TRIGGER_PARAM_ACTOR && iColliderIndex == (_int)TRIGGER_PULSE)
	{
		m_bWakeUp = true;
	}

	return 0;
}

void CRot::Create_Hat(_uint iHatIndex)
{
	CRotHat::HAT_DESC HatDesc;
	HatDesc.eHatType = (CRotHat::HAT_TYPE)iHatIndex;
	HatDesc.pSocket = m_pModelCom->Get_BonePtr("hat_socket_jnt");
	HatDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	HatDesc.pTargetTransform = m_pTransformCom;
	HatDesc.pOwnerRot = this;

	m_pRotHat->Create_HatModel(HatDesc);
}

void CRot::Buy_Hat(_uint iHatIndex)
{
	if (IsBuyPossible() == false) return;

	m_vecKenaConnectRot[m_iBuyHatIndex++]->Create_Hat(iHatIndex);
}

_bool CRot::IsBuyPossible()
{
	return m_iThisRotIndex == FIRST_ROT && m_iBuyHatIndex < m_vecKenaConnectRot.size();
}

_int CRot::Get_KenaConnectRotCount()
{ 
	if (m_iThisRotIndex != FIRST_ROT) return -1;

	return (_int)m_vecKenaConnectRot.size();
}

_int CRot::Get_HaveHatRotCount()
{
	if (m_iThisRotIndex != FIRST_ROT) return -1;

	return m_iBuyHatIndex;
}

_int CRot::Get_NoHatRotCount()
{
	if (m_iThisRotIndex != FIRST_ROT) return -1;

	return (_int)m_vecKenaConnectRot.size() - m_iBuyHatIndex;
}

void CRot::Clear()
{
	m_iEveryRotCount = 0;
	m_iKenaFindRotCount = 0;
	m_vecKenaConnectRot.clear();
}
