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
#include "E_P_Rot.h"
#include "Monster_Manager.h"
#include "Camera_Photo.h"

_uint CRot::m_iEveryRotCount = 0;
_uint CRot::m_iKenaFindRotCount = 0;
vector<CRot*> CRot::m_vecKenaConnectRot;
_float4 CRot::m_vKenaPos = {0.f, 0.f, 0.f, 1.f};

CRot::CRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	: CRot_Base(pDevice, p_context)	
	, m_pMonster_Manager(CMonster_Manager::GetInstance())
{
}

CRot::CRot(const CRot& rhs)
	: CRot_Base(rhs)	
	, m_pMonster_Manager(CMonster_Manager::GetInstance())
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

	wstring wstrTag = wstring(m_szCloneObjectTag) + TEXT("Wisp");
	m_pRotWisp = static_cast<CRotWisp*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RotWisp"), CUtile::Create_StringAuto(wstrTag.c_str())));
	m_pRotWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42 + 0.3f, m_Desc.WorldMatrix._43, 1.f));

	m_pRotHat = (CRotHat*)CGameInstance::GetInstance()->Clone_GameObject(TEXT("Prototype_GameObject_RotHat"), CUtile::Create_DummyString(TEXT("Hat"), m_iEveryRotCount));
	assert(m_pRotHat && " CRot::Initialize()");

	m_pRotAcc = dynamic_cast<CE_P_Rot*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Rot_P", CUtile::Create_DummyString(TEXT("RotAcc"), m_iEveryRotCount)));
	NULL_CHECK_RETURN(m_pRotAcc, E_FAIL);
	m_pRotAcc->Set_Parent(this);

	m_iEveryRotCount++;
	m_iObjectProperty = OP_ROT;

	Push_EventFunctions();

	return S_OK;
}

HRESULT CRot::Late_Initialize(void * pArg)
{	
	__super::Late_Initialize(nullptr);

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_vWakeUpPosition = _float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f);
	
	m_pMyCam = static_cast<CCameraForRot*>(m_pGameInstance->Find_Camera(L"ROT_CAM"));	
	m_pCamera_Photo = (CCamera_Photo*)m_pGameInstance->Find_Camera(CAMERA_PHOTO_TAG);

	if(!wcscmp(m_szCloneObjectTag, TEXT("Saiya_Rot")) || !wcscmp(m_szCloneObjectTag, TEXT("Hunter_Rot")))
		m_bWakeUp = true;
	else if(!m_bManualWakeUp)
		CPhysX_Manager::GetInstance()->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_ROT, CUtile::Float_4to3(m_vWakeUpPosition), 1.f));

	if (m_iThisRotIndex == FIRST_ROT && m_bManualWakeUp == false)
		m_vecKenaConnectRot.reserve(m_iEveryRotCount);

	//m_pRotAcc
	m_pRotAcc->Late_Initialize(nullptr);
	//m_pRotAcc

	return S_OK;
}

void CRot::Tick(_float fTimeDelta)
{
	 //m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	 //m_pModelCom->Play_Animation(fTimeDelta);
	 //m_pRotHat->Tick(fTimeDelta);
	 //// m_pTransformCom->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42 + 0.3f, m_Desc.WorldMatrix._43, 1.f));
	 //m_pTransformCom->Tick(fTimeDelta);
	 //return;

	__super::Tick(fTimeDelta);
	
	if (m_pRotWisp->Get_Collect())
	{
		if (m_pFSM) m_pFSM->Tick(fTimeDelta);
		
		m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
		m_pModelCom->Play_Animation(fTimeDelta * m_bPlayAnimation);
	}
	else if (m_bWakeUp && m_pTransformCom->Calc_Distance_XYZ(m_pKena->Get_TransformCom()) < 20.f)
	{
		m_pRotWisp->Tick(fTimeDelta);
		m_pKena->Set_RotWispInteractable(true);
	}

	if (m_pTeleportRot)
		m_pTeleportRot->Tick(fTimeDelta);

	if (m_pRotAcc) m_pRotAcc->Tick(fTimeDelta);
	m_pTransformCom->Tick(fTimeDelta);
	m_pRotHat->Tick(fTimeDelta);
}

void CRot::Late_Tick(_float fTimeDelta)
{
	 //__super::Late_Tick(fTimeDelta);
	 //m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	 //m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	 //m_pRotHat->Late_Tick(fTimeDelta);
	 //return;

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

	if (m_pRotAcc) m_pRotAcc->Late_Tick(fTimeDelta);
	m_pRotHat->Late_Tick(fTimeDelta);
}

void CRot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::Button("Camera Photo"))
	{
		if (m_pCamera_Photo)
		{
			if (m_bTemp == false)
			{
				_float4 vPos = m_pTransformCom->Get_Position();
				m_pCamera_Photo->Execute_Move(vPos + _float4(2.f, 1.f, 2.f, 0.f), vPos);
				// m_pCamera_Photo->Execute_Move(m_vKenaPos + _float4(2.f, 1.f, 2.f, 0.f), m_vKenaPos + _float4(0.f, 0.5f, 0.f, 0.f));
				m_pGameInstance->Work_Camera(CAMERA_PHOTO_TAG);
				m_bTemp = true;
			}
			else
			{
				m_pGameInstance->Work_Camera(TEXT("PLAYER_CAM"));
				m_bTemp = false;
			}
		}			
	}
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
	Safe_Release(m_pRotAcc);
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
				if (m_pCamera_Photo)
					m_pCamera_Photo->Set_FirstRotPtr(this);
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
		m_iCuteAnimIndex = (rand() % (COLLECT8 - COLLECT)) + COLLECT;
		
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

		m_bIsKenaFriend = true;
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
		if (m_pRotAcc->Get_Active() == false)
			m_pRotAcc->Set_Active(true);

		m_pModelCom->Set_AnimIndex(IDLE);
	})
		.AddTransition("IDLE to PHOTO", "READY_PHOTO")
		.Predicator([this]()
	{			
		return m_pCamera_Photo && m_pCamera_Photo->Is_Work();
	})
		.AddTransition("IDLE to HIDE", "HIDE")
		.Predicator([this]()
	{
		// return false; // 임시
		return m_pMonster_Manager->Is_Battle();
	})
		.AddTransition("IDLE to FOLLOW_KENA ", "FOLLOW_KENA")
		.Predicator([this]()
	{
		// return false; // 임시
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
		_float4 vTeleportPos = m_vKenaPos + _float4(CUtile::Get_RandomFloat(-0.8f, 1.5f), 0.f, CUtile::Get_RandomFloat(-0.8f, 1.5f), 0.f);

		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pTransformCom->Set_Position(vTeleportPos);

		TurnOn_TeleportEffect(vTeleportPos, TELEPORT7);		
	})
		.AddTransition("FOLLOW_KENA to IDLE ", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.AddState("HIDE")
		.OnStart([this]()
	{	
		m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HIDE1 + rand() % 3], 0.5f);
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), HIDE + rand() % 2);
	})
		.OnExit([this]()
	{
		m_pTransformCom->Set_Position(g_vInvisiblePosition);
	})
		.AddTransition("RUN_AWAY to HIDE_WAIT ", "HIDE_WAIT")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.AddState("HIDE_WAIT")		
		.AddTransition("HIDE_WAIT to TELEPORT_KENA ", "TELEPORT_KENA")
		.Predicator([this]()
	{
		return m_pMonster_Manager->Is_Battle() == false;
	})


		.AddState("READY_PHOTO")		
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_pCamera_Photo->Get_Position());
	})
		.AddTransition("SELFIE to IDLE", "PHOTO")
		.Predicator([this]()
	{
		return m_bPhoto;
	})		

		.AddState("PHOTO")
		.OnStart([this]()
	{
		// PHOTOPOSE1, PHOTOPOSE2, PHOTOPOSE3, PHOTOPOSE4, PHOTOPOSE5, PHOTOPOSE6, PHOTOPOSE7, PHOTOPOSE8,
		_uint iPhotoAnimIndex = PHOTOPOSE1 + (m_iThisRotIndex % (PHOTOPOSE8 - PHOTOPOSE1));

		m_pModelCom->ResetAnimIdx_PlayTime(iPhotoAnimIndex);
		m_pModelCom->Set_AnimIndex(iPhotoAnimIndex);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_pCamera_Photo->Get_Position());

		m_bPhotoAnimEnd = !m_bPlayAnimation;
	})
		.AddTransition("SELFIE to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_bPhoto == false;
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
	if (pTarget && iColliderIndex == (_int)COL_PLAYER && m_bOrdinaryRotFlag)
	{
		m_bWakeUp = true;
	}

	return 0;
}

_int CRot::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget && iTriggerIndex == (_uint)ON_TRIGGER_PARAM_ACTOR && iColliderIndex == (_int)TRIGGER_PULSE && m_bOrdinaryRotFlag)
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

void CRot::Set_NewWorldMatrix(_float4x4 NewWroldMatrix)
{
	m_Desc.WorldMatrix = NewWroldMatrix;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_vWakeUpPosition = _float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42, m_Desc.WorldMatrix._43, 1.f);
	m_pRotWisp->Set_Position(_float4(m_Desc.WorldMatrix._41, m_Desc.WorldMatrix._42 + 0.3f, m_Desc.WorldMatrix._43, 1.f));
}

void CRot::Execute_WakeUp()
{
	m_bWakeUp = true;
	m_pRotWisp->Set_Pulse(true);
}

void CRot::Execute_Photo()
{
	if (m_iThisRotIndex != FIRST_ROT) return;

	for (auto& pRot : m_vecKenaConnectRot)
	{
		pRot->m_bPhoto = true;
	}	
}

_bool CRot::Is_PhotoAnimEnd()
{
	if (m_iThisRotIndex != FIRST_ROT) return false;

	for (auto& pRot : m_vecKenaConnectRot)
	{
		if (pRot->m_bPhotoAnimEnd == false)
			return false;
	}

	return true;
}

void CRot::Execute_PhotoTeleport()
{
	static _bool bTeleportFlag = false;

	if (m_iThisRotIndex != FIRST_ROT || bTeleportFlag) return;
	
	_float4 vKenaRight = m_pKenaTransform->Get_State(CTransform::STATE_RIGHT);

	_uint i = 0;
	for (auto& pRot : m_vecKenaConnectRot)
	{
		if(i % 2 == 0)
			pRot->Set_Position(m_vKenaPos + vKenaRight * (pRot->m_iThisRotIndex + 0.5f));
		else
			pRot->Set_Position(m_vKenaPos + vKenaRight * (pRot->m_iThisRotIndex + 0.5f) * -1.f);

		i++;
	}

	bTeleportFlag = true;
}

void CRot::Push_EventFunctions()
{
	Stop_Animation(true, 0.f);
}

void CRot::Stop_Animation(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CRot::Stop_Animation);
		return;
	}

	m_bPlayAnimation = false;
}
