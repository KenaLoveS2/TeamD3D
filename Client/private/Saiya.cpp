#include "stdafx.h"
#include "..\public\Saiya.h"
#include "GameInstance.h"
#include "CameraForNpc.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Kena.h"
#include "UI_ClientManager.h"
#include "UI_FocusNPC.h"
#include "CinematicCamera.h"
#include "Rot.h"
#include "BossShaman.h"
#include "Camera_Photo.h"

/* For. Delegator Default Value (meaningless) */
_float		fDefaultVal = 345.f; /* Default Chat Value */
_bool		bDefaultVal = false;
wstring		wstrDefault = L"";

#define SND_LAUGHTER_1 L"VOX_Jizo_Kids_Laughter_1.ogg"
#define SND_LAUGHTER_2 L"VOX_Jizo_Kids_Laughter_2.ogg"
#define SND_LAUGHTER_3 L"VOX_Jizo_Kids_Laughter_3.ogg"
#define SND_LAUGHTER_4 L"VOX_Jizo_Kids_Laughter_4.ogg"
#define SND_LAUGHTER_5 L"VOX_Jizo_Kids_Laughter_5.ogg"
#define SND_LAUGHTER_6 L"VOX_Jizo_Kids_Laughter_6.ogg"
#define SND_LAUGHTER_7 L"VOX_Jizo_Kids_Laughter_7.ogg"

CSaiya::CSaiya(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CNpc(pDevice, pContext)
	, m_iChatIndex(0)
	, m_iLineIndex(0)
	, m_pFocus(nullptr)
	, m_bCall(false)
{
}

CSaiya::CSaiya(const CNpc& rhs)
	: CNpc(rhs)
	, m_iChatIndex(0)
	, m_iLineIndex(0)
	, m_pFocus(nullptr)
	, m_bCall(false)
{
}

HRESULT CSaiya::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CSaiya::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.5f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);
	m_pModelCom->Set_AllAnimCommonType();

	if (g_LEVEL == LEVEL_TESTPLAY)
	{
		CGameObject* p_game_object = nullptr;
		CGameInstance::GetInstance()->Clone_AnimObject(g_LEVEL, L"Layer_Rot", L"Prototype_GameObject_Rot", L"Saiya_Rot", nullptr, &p_game_object);
		m_pRot = dynamic_cast<CRot*>(p_game_object);
	}

	return S_OK;
}

HRESULT CSaiya::Late_Initialize(void* pArg)
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

	if (g_LEVEL == LEVEL_TESTPLAY)
	{
		m_pCinecam[NPC_CINE0] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CINE0")));
		m_pCinecam[NPC_CINE1] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CINE1")));
		m_pCinecam[NPC_CINE2] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CINE2")));
		m_pMainCam = dynamic_cast<CCameraForNpc*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CAM")));

		Load_KeyFrame();
		if (!m_keyframes.empty())
		{
			const _float4 vPos = _float4(m_keyframes[m_iKeyFrame].vPos.x, m_keyframes[m_iKeyFrame].vPos.y, m_keyframes[m_iKeyFrame].vPos.z, 1.f);
			const _float4 vLook = _float4(m_keyframes[m_iKeyFrame].vLook.x, m_keyframes[m_iKeyFrame].vLook.y, m_keyframes[m_iKeyFrame].vLook.z, 0.f);
			m_pTransformCom->Set_Position(vPos);
			m_pTransformCom->Set_Look(vLook);
		}
	}
	else if(g_LEVEL == LEVEL_FINAL)
	{
		m_pMainCam = dynamic_cast<CCameraForNpc*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CAM")));
		m_iChatIndex = 3;

		m_pShaman = dynamic_cast<CBossShaman*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Monster", L"BossShaman_0"));

		m_vFinalPosition = _float4(-34.3f, 20.4f, 1230.8f, 1.f);
		m_pTransformCom->Set_Position(m_vFinalPosition);
		m_pTransformCom->Rotation(_float4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	}
	
	Setting_Sound();

	m_pCamera_Photo = (CCamera_Photo*)CGameInstance::GetInstance()->Find_Camera(CAMERA_PHOTO_TAG);
	if (m_pCamera_Photo) m_pCamera_Photo->Set_NpcSaiyaPtr(this);



	return S_OK;
}

void CSaiya::Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Tick(fTimeDelta);
	m_iNumKeyFrame = (_uint)m_keyframes.size();
	Update_Collider(fTimeDelta);

	if (g_LEVEL == LEVEL_FINAL)
	{
		if (m_pShaman->Get_BossClear() == true && false == m_bCall)
		{
			//CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
			//_bool bStart = true;
			//_float fIdx2 = 2;
			//m_SaiyaDelegator.broadcast(tag, bStart, fIdx2, wstrDefault);
			//CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);

			m_bCall = true;
		}
	}
	/*char szName[MAX_PATH] = "";
	CUtile::WideCharToChar(m_szCloneObjectTag, szName);
	ImGui::Begin(szName);
	Imgui_RenderProperty();
	ImGui_AnimationProperty();
	m_pTransformCom->Imgui_RenderProperty();
	ImGui::End();*/

	if (m_pFSM && m_iKeyFrame < m_iNumKeyFrame - 2) m_pFSM->Tick(fTimeDelta);
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pModelCom->Get_AnimIndex() == SAIYA_CHASINGLOOP)
		m_pFocus->Set_Active(false);

	if (g_LEVEL == LEVEL_TESTPLAY)
	{
		_bool isPlay = false;
		for (int i = 0; i < NPC_CINE_END; i++)
		{
			if (m_pCinecam[i]->IsPlaying())
			{
				m_pFocus->Set_Active(false);
				isPlay = true;
				break;
			}
		}

		if (isPlay == false)
			m_pFocus->Set_Active(true);
	}



	if(!m_bPhoto)
		AdditiveAnim(fTimeDelta);

	if (m_bStraight)
		m_pTransformCom->Go_Straight(fTimeDelta);	
}

void CSaiya::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CSaiya::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 7)
			continue;

		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"),E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		if (i == 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SAIYA_EYE), E_FAIL);
		}
		else if(i == 1)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT), E_FAIL);
		}
		else
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AORM),E_FAIL);
		}
	}
	return S_OK;
}

HRESULT CSaiya::RenderShadow()
{
	FAILED_CHECK_RETURN(__super::RenderShadow(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShadowShaderResources(), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CSaiya::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	_float fDistanceBetweenPlayer = m_pTransformCom->Calc_Distance_XYZ(m_pKena->Get_TransformCom());
	ImGui::Text("Distance to Player :	%f", fDistanceBetweenPlayer);

	if(ImGui::Button("NpcCam"))
	{
		CGameInstance::GetInstance()->Work_Camera(m_pMainCam->Get_ObjectCloneName());
		const _float3 vOffset = _float3(0.f, 0.5f, 1.f);
		const _float3 vLookOffset = _float3(0.f, 0.4f, 0.f);
		m_pMainCam->Set_Target(this, CCameraForNpc::OFFSET_FRONT, vOffset, vLookOffset);
	}
	ImGui::SameLine();
	if (ImGui::Button("PlayerCam"))
	{
		CGameInstance::GetInstance()->Work_Camera(TEXT("PLAYER_CAM"));
	}

	ImGui::Checkbox("GoStraight", &m_bStraight);

	if (ImGui::Button("Add KeyFrame"))
	{
		SAIYAKEYFRAME keyframe;
		ZeroMemory(&keyframe, sizeof(SAIYAKEYFRAME));
		keyframe.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		keyframe.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_keyframes.push_back(keyframe);
	}

	char** ppKeyFrameNum = new char* [m_iNumKeyFrame];

	for (_uint i = 0; i < m_iNumKeyFrame; ++i)
	{
		ppKeyFrameNum[i] = new char[16];
		sprintf_s(ppKeyFrameNum[i], sizeof(char) * 16, "%u", i);
	}

	static _int iSelectKeyFrame = -1;

	if (ImGui::Button("Reset"))
		iSelectKeyFrame = -1;

	ImGui::ListBox("KeyFrameList", &iSelectKeyFrame, ppKeyFrameNum, (int)m_iNumKeyFrame);

	if (iSelectKeyFrame != -1	&& iSelectKeyFrame < (int)m_iNumKeyFrame)
	{
		float fPos[3] = { m_keyframes[iSelectKeyFrame].vPos.x , m_keyframes[iSelectKeyFrame].vPos.y,m_keyframes[iSelectKeyFrame].vPos.z };
		ImGui::DragFloat3("KFPos", fPos, 0.01f, -1000.f, 1000.f);
		m_keyframes[iSelectKeyFrame].vPos = _float3(fPos[0], fPos[1], fPos[2]);
		float fLookAT[3] = { m_keyframes[iSelectKeyFrame].vLook.x , m_keyframes[iSelectKeyFrame].vLook.y,m_keyframes[iSelectKeyFrame].vLook.z };
		ImGui::DragFloat3("KFLookAT", fLookAT, 0.01f, -1000.f, 1000.f);
		m_keyframes[iSelectKeyFrame].vLook = _float3(fLookAT[0], fLookAT[1], fLookAT[2]);

		m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[iSelectKeyFrame].vPos));
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[iSelectKeyFrame].vLook));

		if (ImGui::Button("Erase"))
		{
			m_keyframes.erase(m_keyframes.begin() + iSelectKeyFrame);
			iSelectKeyFrame = -1;
		}
	}

	if (ImGui::Button("Clear"))
		m_keyframes.clear();

	for (_uint i = 0; i < m_iNumKeyFrame; ++i)
		Safe_Delete_Array(ppKeyFrameNum[i]);
	Safe_Delete_Array(ppKeyFrameNum);

	ImGui::DragFloat("DefaultVal", &fDefaultVal, 10.f, -1000.f, 1000.f);

	if (ImGui::Button("Save_KeyFrame"))
		Save_KeyFrame();

	if (ImGui::Button("Load_KeyFrame"))
		Load_KeyFrame();
}

void CSaiya::ImGui_AnimationProperty()
{
	if (ImGui::CollapsingHeader("Saiya_Ani"))
	{
		ImGui::BeginTabBar("Saiya Animation & State");

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

void CSaiya::ImGui_ShaderValueProperty()
{
	CNpc::ImGui_ShaderValueProperty();
	if (ImGui::Button("Recompile"))
	{
		m_pRendererCom->ReCompile();
		m_pShaderCom->ReCompile();
	}
	m_pModelCom->Imgui_RenderProperty();
}

void CSaiya::ImGui_PhysXValueProperty()
{
	CNpc::ImGui_PhysXValueProperty();
}

_int CSaiya::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if(pTarget != m_pKena && iColliderIndex == (_int)TRIGGER_PULSE && DistanceTrigger(10.f))
	{
		m_bPulse = true;
		return 0;
	}
	else
	{
		m_bPulse = false;
		return 0;
	}

	return 0;
}

void CSaiya::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

HRESULT CSaiya::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("INIT_CAM")

		.AddState("INIT_CAM")
		.Tick([this](_float fTimeDelta)
	{
		Rot_WispSetPosition();
		Play_LaughSound();
		m_pModelCom->Set_AnimIndex(SAIYA_CHASINGLOOP);
	})
		.OnExit([this]()
	{
		CGameInstance::GetInstance()->Work_Camera(m_pCinecam[NPC_CINE0]->Get_ObjectCloneName());
		m_pCinecam[NPC_CINE0]->Play();
		m_bCinecam[NPC_CINE0] = true;
		Set_PlayerLock(true);
		Play_LaughSound();
	})
		.AddTransition("INIT_CAM to ACTION_0" , "ACTION_0")
		.Predicator([this]()
	{
		return 	DistanceTrigger(10.f) && !m_bCinecam[NPC_CINE0];
	})

		.AddState("ACTION_0")
		.OnStart([this]()
	{
		Play_LaughSound();
	})
		.Tick([this](_float fTimeDelta)
	{
		Play_LaughSound();
		m_pModelCom->Set_AnimIndex(SAIYA_CHASINGLOOP);
		Rot_WispSetPosition();
	})
		.OnExit([this]()
	{
		Set_PlayerLock(false);
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		CGameInstance::GetInstance()->Work_Camera(m_pMainCam->Get_ObjectCloneName());
		const _float3 vOffset = _float3(0.f, 0.5f, 1.f);
		const _float3 vLookOffset = _float3(0.f, 0.4f, 0.f);
		m_pMainCam->Set_Target(this, CCameraForNpc::OFFSET_FRONT, vOffset, vLookOffset);
		m_pCinecam[NPC_CINE0]->CinemaUIOff();
		if (m_pRot)
		{
			CPhysX_Manager::GetInstance()->Create_Trigger(Create_PxTriggerData(TEXT("Saiya_Rot"), this, TRIGGER_ROT, CUtile::Float_4to3(vPos), 1.f));
			m_pRot->Set_WakeUpPos(vPos);
			m_pRot->Get_TransformCom()->Set_Look(m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) * -1.f);
		}
		Play_LaughSound();
	})
		.AddTransition("ACTION_0 to ACTION_1" , "ACTION_1")
		.Predicator([this]()
	{
		return m_pCinecam[0]->CameraFinishedChecker();
	})

		.AddState("ACTION_1")
		.OnStart([this]()
	{
		Play_LaughSound();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[1].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.OnExit([this]()
	{
		CGameInstance::GetInstance()->Work_Camera(TEXT("PLAYER_CAM"));
		/* Quest 0 Start */
		//CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;
		//CUI_ClientManager::UI_PRESENT eQuestLine = CUI_ClientManager::QUEST_LINE;
		//_bool bStart = true;
		//m_SaiyaDelegator.broadcast(eQuest, bStart, fDefaultVal, wstrDefault);
		//_float fIndex0=0, fIndex1=1, fIndex2=2, fIndex3=3;
		//m_SaiyaDelegator.broadcast(eQuestLine, bDefaultVal, fIndex0, wstrDefault); // 수상한 아이들
		//m_SaiyaDelegator.broadcast(eQuestLine, bDefaultVal, fIndex1, wstrDefault); // 아이들을 따라가라
		//m_SaiyaDelegator.broadcast(eQuestLine, bDefaultVal, fIndex2, wstrDefault); // 펄스사용해라
		//m_SaiyaDelegator.broadcast(eQuestLine, bDefaultVal, fIndex3, wstrDefault); // 스킬 열어라

	})
		.AddTransition("ACTION_1 to ACTION_2", "ACTION_2")
		.Predicator([this]()
	{
		return !DistanceTrigger(15.f);
	})


		.AddState("ACTION_2")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_TELEPORT);
		m_pModelCom->Set_AnimIndex(SAIYA_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		Play_LaughSound();
		// 사라질때 이펙트 	
	})
		.OnExit([this]()
	{
		if (!m_keyframes.empty())
		{
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[2].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[2].vLook));
		}
		CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;
		_bool bStart = true;
		_float fIndex = 1.f;
		m_SaiyaDelegator.broadcast(eQuest, bStart, fIndex, wstrDefault);
	})
		.AddTransition("ACTION_2 to ACTION_3" , "ACTION_3")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})


		.AddState("ACTION_3")
		.Tick([this](_float fTimeDelta)
	{
		Play_LaughSound();
		m_pModelCom->Set_AnimIndex(SAIYA_CHASINGLOOP);
	})
		.AddTransition("ACTION_3 to ACTION_4", "ACTION_4")
		.Predicator([this]()
	{
		return DistanceTrigger(15.f) && AnimFinishChecker(SAIYA_CHASINGLOOP) && !m_bCinecam[NPC_CINE1];
	})


		.AddState("ACTION_4")
		.OnStart([this]()
	{
		CGameInstance::GetInstance()->Work_Camera(L"NPC_CINE1");
		m_pCinecam[NPC_CINE1]->Play();
		m_bCinecam[NPC_CINE1] = true;
		Play_LaughSound();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[3].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
		Play_LaughSound();
	})
		.AddTransition("ACTION_4 to ACTION_5", "ACTION_5")
		.Predicator([this]()
	{
		return !DistanceTrigger(30.f);
	})

		.AddState("ACTION_5")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_TELEPORT);
		m_pModelCom->Set_AnimIndex(SAIYA_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		Play_LaughSound();
		// 사라질때 이펙트 	
	})
		.OnExit([this]()
	{
		if (!m_keyframes.empty())
		{
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[4].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[4].vLook));
		}
	})
		.AddTransition("ACTION_5 to ACTION_6", "ACTION_6")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})


		.AddState("ACTION_6")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
	})
		.AddTransition("ACTION_6 to ACTION_7", "ACTION_7")
		.Predicator([this]()
	{
		return dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("MAP_CINE0")))->CameraFinishedChecker(0.7f); // 첫 펄스로 시네캠 생성될때
	})

		.AddState("ACTION_7")
		.OnStart([this]()
	{
	})
		.Tick([this](_float fTimeDelta)
	{
			m_pModelCom->Set_AnimIndex(SAIYA_RUN);
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[5].vLook));
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
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_TELEPORT);
		m_pModelCom->Set_AnimIndex(SAIYA_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.OnExit([this]()
	{
		if (!m_keyframes.empty())
		{
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[6].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[6].vLook));
		}
	})
		.AddTransition("ACTION_8 to IDLE", "IDLE")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})

		.AddState("IDLE")
		.OnStart([this]() {
		/* Quest 0 - 1,2 Clear */
		//CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
		//_bool bStart = true;
		//_float fIdx1 = 1, fIdx2 = 2;
		//m_SaiyaDelegator.broadcast(tag, bStart, fIdx1, wstrDefault);
		//m_SaiyaDelegator.broadcast(tag, bStart, fIdx2, wstrDefault);
		//CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);
			
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
	})
		.OnExit([this]()
	{
		Play_LaughSound();
	})
		.AddTransition("IDLE to CHEER", "CHEER")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f);
	})

		/* Cheer */
		.AddState("CHEER")
		.OnStart([this]()
	{
			CGameInstance::GetInstance()->Work_Camera(m_pMainCam->Get_ObjectCloneName());
			const _float3 vOffset = _float3(0.f, 0.7f, 0.f);
			const _float3 vLookOffset = _float3(0.f, 0.3f, 0.f);
			m_pMainCam->Set_Target(this, CCameraForNpc::OFFSET_FRONT_LERP, vOffset, vLookOffset);
			m_bMeetPlayer = true;
			m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_CHEER);
			m_pModelCom->Set_AnimIndex(SAIYA_CHEER);
			CUI_ClientManager::GetInstance()->Switch_FrontUI(false);
			Set_PlayerLock(true);
			//m_pFocus->Set_Active(true);
	})

		.OnExit([this]()
	{
		CGameInstance::GetInstance()->Work_LightCamera(TEXT("LIGHT_CAM_1"));
		m_pRendererCom->ShootStaticShadow();
	})
		.AddTransition("CHEER to ACTION_9", "ACTION_9")
		.Predicator([this]()
	{
		return  AnimFinishChecker(SAIYA_CHEER);
	})

		.AddState("ACTION_9")
		.OnStart([this]()
	{
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = true;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][0]);
		m_iLineIndex++;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);

		if (CGameInstance::GetInstance()->Key_Down(DIK_E))
		{
			Play_LaughSound();
			if (m_iLineIndex == (int)m_vecChat[m_iChatIndex].size())
			{
				m_iLineIndex++;
				return;
			}
			CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
			_bool bVal = true;
			m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][m_iLineIndex]);
			m_iLineIndex++;
		}
	})
		.OnExit([this]()
	{
		Set_PlayerLock(false);
		/* Chat End */
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = false;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[0][0]);

	//	m_pFocus->Set_Active(false);

			m_iLineIndex = 0;
		m_iChatIndex++;

		CGameInstance::GetInstance()->Work_Camera(TEXT("PLAYER_CAM"));

	})
		.AddTransition("ACTION_9 to ACTION_10", "ACTION_10")
		.Predicator([this]()
	{
		return IsChatEnd();
	})

		.AddState("ACTION_10")
		.OnStart([this]()
	{
		Play_LaughSound();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[7].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_10 to ACTION_11", "ACTION_11")
		.Predicator([this]()
	{
		return !DistanceTrigger(10.f);
	})

		.AddState("ACTION_11")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_TELEPORT);
		m_pModelCom->Set_AnimIndex(SAIYA_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.OnExit([this]()
	{
		if (!m_keyframes.empty())
		{
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[8].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[8].vLook));
		}
	})
		.AddTransition("ACTION_11 to ACTION_12", "ACTION_12")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})

		.AddState("ACTION_12")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);

	}) // 플레이어 또 가까이오면 시네마캠 NPC_CINE2 동작 

		.OnExit([this]()
	{
		CGameInstance::GetInstance()->Work_Camera(m_pCinecam[NPC_CINE2]->Get_ObjectCloneName());
		m_pCinecam[NPC_CINE2]->Play();
		m_bCinecam[NPC_CINE2] = true;
	})
		.AddTransition("ACTION_12 to ACTION_13", "ACTION_13")
		.Predicator([this]()
	{
		return 	DistanceTrigger(25.f) && !m_bCinecam[NPC_CINE2];
	})

		.AddState("ACTION_13")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE); // 어느정도 가까이 오면 아이들 상태에서 Curious한 애니메이션 실행 펄스로 했을때 옆에 NPC 닿으면 Pulse 그 애니메이션하고 놀라워하기 
	})
		.AddTransition("ACTION_13 to ACTION_14", "ACTION_14")
		.Predicator([this]()
	{
		return 	m_pCinecam[NPC_CINE2]->CameraFinishedChecker(0.45f);
	})

		.AddState("ACTION_14")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_CURIOUS);
		m_pModelCom->Set_AnimIndex(SAIYA_CURIOUS);
	})
		.AddTransition("ACTION_14 to ACTION_15", "ACTION_15")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_CURIOUS);
	})

		.AddState("ACTION_15")
		.OnStart([this]()
	{
		m_bPulse = false;
	})
		.Tick([this](_float fTimeDelta)
	{
		Play_LaughSound();
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
	}) // 펄스 닿으면 애니메이션 진행
		.AddTransition("ACTION_15 to ACTION_16", "ACTION_16")
		.Predicator([this]()
	{
		return m_bPulse && m_bCinecam[NPC_CINE2];
	})


		.AddState("ACTION_16")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_PULSE);
		m_pModelCom->Set_AnimIndex(SAIYA_PULSE);
		CGameInstance::GetInstance()->Work_Camera(m_pMainCam->Get_ObjectCloneName());
		const _float3 vOffset = _float3(2.5f, 2.f, 1.f);
		const _float3 vLookOffset = _float3(0.f, 0.3f, 0.f);
		m_pMainCam->Set_Target(this, CCameraForNpc::OFFSET_FRONT, vOffset, vLookOffset);

	//	m_pFocus->Set_Active(true);
	})
		.AddTransition("ACTION_16 to ACTION_17", "ACTION_17")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_PULSE);
	})


		.AddState("ACTION_17")
		.OnStart([this]()
	{
		Set_PlayerLock(true);
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = true;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][0]);
		m_iLineIndex++;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);

		if (CGameInstance::GetInstance()->Key_Down(DIK_E))
		{
			Play_LaughSound();
			if (m_iLineIndex == (int)m_vecChat[m_iChatIndex].size())
			{
				m_iLineIndex++;
				return;
			}
			CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
			_bool bVal = true;
			m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][m_iLineIndex]);
			m_iLineIndex++;
		}
	}) //대화
		.OnExit([this]()
	{
			Set_PlayerLock(false);
			/* Chat End */
			CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
			_bool bVal = false;
			m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[0][0]);
	//		m_pFocus->Set_Active(false);

			m_iLineIndex = 0;
			m_iChatIndex++;
			CGameInstance::GetInstance()->Work_Camera(TEXT("PLAYER_CAM"));

			/* Quest 1 - 1 Clear */
			//CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
			//_bool bStart = true;
			//_float fIdx1 = 1;
			//m_SaiyaDelegator.broadcast(tag, bStart, fIdx1, wstrDefault);
			//CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);

	})
		.AddTransition("ACTION_17 to ACTION_18", "ACTION_18")
		.Predicator([this]()
	{
		return IsChatEnd();
	})

		.AddState("ACTION_18")
		.OnStart([this]()
	{
		Play_LaughSound();
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[9].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_18 to ACTION_19", "ACTION_19")
		.Predicator([this]()
	{
		return !DistanceTrigger(10.f);
	})

		.AddState("ACTION_19")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_TELEPORT);
		m_pModelCom->Set_AnimIndex(SAIYA_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.OnExit([this]()
	{
		if (!m_keyframes.empty())
		{
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[10].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[10].vLook));
		}
	})
		.AddTransition("ACTION_19 to ACTION_20", "ACTION_20")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})

		.AddState("ACTION_20")
		.OnStart([this]()
	{
		m_bPulse = false;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
	})
		.AddTransition("ACTION_20 to ACTION_21", "ACTION_21")
		.Predicator([this]()
	{
		return m_bPulse;// 시네마캠 ㄱㄱㄱ
	})

		.AddState("ACTION_21")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_PULSE);
		m_pModelCom->Set_AnimIndex(SAIYA_PULSE);
		Play_LaughSound();
	})
		.AddTransition("ACTION_21 to ACTION_22", "ACTION_22")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_PULSE);
	})

		.AddState("ACTION_22")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[11].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_22 to ACTION_23", "ACTION_23")
		.Predicator([this]()
	{
		return !DistanceTrigger(10.f);
	})

		.AddState("ACTION_23")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_TELEPORT);
		m_pModelCom->Set_AnimIndex(SAIYA_TELEPORT);
	})
		.Tick([this](_float fTimeDelta)
	{
		// 사라질때 이펙트 	
	})
		.OnExit([this]()
	{
		if (!m_keyframes.empty())
		{
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[12].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[12].vLook));
		}
	})
		.AddTransition("ACTION_23 to ACTION_24", "ACTION_24")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})

		.AddState("ACTION_24")
		.OnStart([this]()
	{
		m_bPulse = false;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
	})
		.AddTransition("ACTION_24 to ACTION_25", "ACTION_25")
		.Predicator([this]()
	{
		return m_bPulse;
	})

		.AddState("ACTION_25")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(SAIYA_PULSE);
		m_pModelCom->Set_AnimIndex(SAIYA_PULSE);
	//	m_pFocus->Set_Active(true);
	})
		.AddTransition("ACTION_25 to ACTION_26", "ACTION_26")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_PULSE);
	})

		.AddState("ACTION_26")
		.OnStart([this]()
	{
		Set_PlayerLock(true);
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = true;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][0]);
		m_iLineIndex++;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
		if (CGameInstance::GetInstance()->Key_Down(DIK_E))
		{
			Play_LaughSound();
			if (m_iLineIndex == (int)m_vecChat[m_iChatIndex].size())
			{
				m_iLineIndex++;
				return;
			}
			CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
			_bool bVal = true;
			m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][m_iLineIndex]);
			m_iLineIndex++;
		}
	}) //대화
		.OnExit([this]()
	{
		Set_PlayerLock(false);
		/* Chat End */
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = false;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[0][0]);
	//	m_pFocus->Set_Active(false);
		m_iLineIndex = 0;
		m_iChatIndex++;
		CGameInstance::GetInstance()->Work_Camera(TEXT("PLAYER_CAM"));

	})
		.AddTransition("ACTION_26 to ACTION_27", "ACTION_27")
		.Predicator([this]()
	{
		return IsChatEnd();
	})

		.AddState("ACTION_27")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
	})

		.Build();
	return S_OK;
}

HRESULT CSaiya::SetUp_StateFinal()
{
		m_pFSM = CFSMComponentBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
		// 위치 정해줘야함
	})
		.AddTransition("IDLE to READY_PHOTO", "READY_PHOTO")
		.Predicator([this]()
	{
		return m_pCamera_Photo && m_pCamera_Photo->Is_Work();
	})
		.AddTransition("IDLE to CHAT", "CHAT")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f) && !m_bChatEnd;
	})

		.AddState("CHAT")
		.OnStart([this]()
	{
			/* Quest 2 - 3 */
		//CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::QUEST_CLEAR;
		//_bool bStart = true;
		//_float fIdx3 = 3;
		//m_SaiyaDelegator.broadcast(tag, bStart, fIdx3, wstrDefault);
		//CGameInstance::GetInstance()->Play_Sound(L"clear.ogg", 1.f, false, SOUND_UI);


		//		m_pFocus->Set_Active(true);
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = true;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][0]);
		m_iLineIndex++;
		CGameInstance::GetInstance()->Work_Camera(m_pMainCam->Get_ObjectCloneName());
		const _float3 vOffset = _float3(0.f, 0.7f, 0.f);
		const _float3 vLookOffset = _float3(0.f, 0.3f, 0.f);
		m_pMainCam->Set_Target(this, CCameraForNpc::OFFSET_FRONT_LERP, vOffset, vLookOffset);
		Set_PlayerLock(true);
    })
	.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_IDLE);
		if (CGameInstance::GetInstance()->Key_Down(DIK_E))
		{
			Play_LaughSound();
			if (m_iLineIndex == (int)m_vecChat[m_iChatIndex].size())
			{
				m_iLineIndex++;
				return;
			}
			CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
			_bool bVal = true;
			m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[m_iChatIndex][m_iLineIndex]);
			m_iLineIndex++;
		}
	})
		.OnExit([this]()
	{
		/* Chat End */
		//		m_pFocus->Set_Active(false);
		Set_PlayerLock(false);
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = false;
		m_SaiyaDelegator.broadcast(eChat, bVal, fDefaultVal, m_vecChat[0][0]);
		m_iLineIndex = 0;
		m_bUIOff = true;
		_float4 vPos = m_pMainCam->Get_Position();
		_float4 vSaiyaPos = m_pTransformCom->Get_Position();
		m_pCamera_Photo->Execute_Move(vPos, vSaiyaPos);
		CGameInstance::GetInstance()->Work_Camera(CAMERA_PHOTO_TAG);
	})
		.AddTransition("CHAT to IDLE", "IDLE")
		.Predicator([this]()
	{
		return IsChatEnd();
	})

		.AddState("READY_PHOTO")
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_pCamera_Photo->Get_Position());
		m_pTransformCom->Set_Position(m_vFinalPosition);
	})
		.AddTransition("READY_PHOTO to PHOTO", "PHOTO")
		.Predicator([this]()
	{
		return m_bPhoto;
	})

		.AddState("PHOTO")
		.OnStart([this]()
	{
		_uint iPhotoAnimIndex = rand() % (SAIYA_PHOTOPOSE_SITTING - SAIYA_PHOTOPOSE_CHEER) + SAIYA_PHOTOPOSE_CHEER;
		m_pModelCom->ResetAnimIdx_PlayTime(iPhotoAnimIndex);
		m_pModelCom->Set_AnimIndex(iPhotoAnimIndex);
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->LookAt_NoUpDown(m_pCamera_Photo->Get_Position());
		m_pTransformCom->Set_Position(m_vFinalPosition);
		if (m_pModelCom->Get_AnimationFinish())
			m_bPhotoAnimEnd = true;
	})
		.AddTransition("PHOTO to IDLE", "IDLE")
		.Predicator([this]()
	{
		return !m_bPhoto;
	})

		.Build();

	return S_OK;
}

HRESULT CSaiya::SetUp_Components()
{
	__super::SetUp_Components();
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Saiya", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_NORMALS, TEXT("../Bin/Resources/Anim/NPC/Eyes_NORMAL.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Saiya/jizokids_mask_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Saiya/jizogirl_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(4, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Saiya/jizogirl_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(5, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Saiya/jizoboy_cloth_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(6, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/NPC/Saiya/jizokids_slingshot_AO_R_M.png")), E_FAIL);
	m_pModelCom->Set_RootBone("jizo_girl_RIG");
	return S_OK;
}

HRESULT CSaiya::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CSaiya::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CSaiya::SetUp_UI()
{
	/* Chat Load */
	Json jLoad;

	string filePath = "../Bin/Data/Chat/Saiya.json";
	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	_int iNumChat[10];
	_uint i = 0;
	for (auto iNum : jLoad["NumChat"])
	{
		iNumChat[i] = iNum;

		string strSub = "Chat" + to_string(i);
		for (size_t j = 1; j <= iNumChat[i]; ++j)
		{
			for (auto jSub : jLoad[strSub])
			{
				string str = "line" + to_string(j);
				string line;
				jSub[str].get_to<string>(line);

				wstring wstr = CUtile::utf8_to_wstring(line);
				m_vecChat[i].push_back(wstr);
			}
		}

		++i;
	}

	/* Link Focus UI */
	CUI_Billboard::BBDESC	tDesc;
	tDesc.fileName = L"UI_FocusNPC";
	tDesc.pOwner = this;
	tDesc.vCorrect.y = m_pTransformCom->Get_vPxPivotScale().y;

	if (FAILED(CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL,
		L"Layer_UI", TEXT("Prototype_GameObject_UI_FocusNPC"),
		L"Clone_FocusUI", &tDesc, (CGameObject**)&m_pFocus)))
	{
		MSG_BOX("Failed To Make Focus UI");
		return E_FAIL;
	}

	//m_pFocus->Set_Active(true);

	return S_OK;
}

void CSaiya::AdditiveAnim(_float fTimeDelta)
{
	m_pModelCom->Set_AdditiveAnimIndexForMonster(SAIYA_MOUTHFLAP);
	m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Saiya.ao");
}

void CSaiya::Set_PlayerLock(_bool bLock)
{
	if (m_pKena)
		m_pKena->Set_StateLock(bLock);
}

_bool CSaiya::IsChatEnd()
{
	if ((size_t)m_iLineIndex > m_vecChat[m_iChatIndex].size())
		return true;
	else
		return false;
}

HRESULT CSaiya::Save_KeyFrame()
{
	string      strSaveDirectory;
	if(g_LEVEL == LEVEL_FINAL)
	{
	     strSaveDirectory = "../Bin/Data/NPC/SaiyaKeyFrameFinal.json";
	}
	else
	{
	      strSaveDirectory = "../Bin/Data/NPC/SaiyaKeyFrame.json";
	}

	ofstream      file(strSaveDirectory.c_str());
	Json   jSaiyaKeyFrameList;

	jSaiyaKeyFrameList["0_KeyFrameSize"] = m_iNumKeyFrame;

	_float      fElement = 0.f;

	for (_uint i = 0; i < m_iNumKeyFrame; ++i)
	{
		Json jChild;
		_float3 vPos = m_keyframes[i].vPos;
		_float3 vLookAt = m_keyframes[i].vLook;
		for (int i = 0; i < 3; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&vPos + i, sizeof(float));
			jChild["0_Pos"].push_back(fElement);
		}

		for (int i = 0; i < 3; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&vLookAt + i, sizeof(float));
			jChild["1_LookAt"].push_back(fElement);
		}

		jSaiyaKeyFrameList["1_Data"].push_back(jChild);
	}

	file << jSaiyaKeyFrameList;
	file.close();

	return S_OK;
}

HRESULT CSaiya::Load_KeyFrame()
{
	string      strLoadDirectory;

	if (g_LEVEL == LEVEL_FINAL)
	{
		strLoadDirectory = "../Bin/Data/NPC/SaiyaKeyFrameFinal.json";
	}
	else
	{
		strLoadDirectory = "../Bin/Data/NPC/SaiyaKeyFrame.json";
	}

	ifstream      file(strLoadDirectory.c_str());
	Json   jLoadCineCamKeyFrameList;
	file >> jLoadCineCamKeyFrameList;
	file.close();

	m_keyframes.clear();

	jLoadCineCamKeyFrameList["0_KeyFrameSize"].get_to<_uint>(m_iNumKeyFrame);

	_float3 vPos;
	_float3 vLookAt;

	for (auto jLoadChild : jLoadCineCamKeyFrameList["1_Data"])
	{
		SAIYAKEYFRAME Desc;
		ZeroMemory(&Desc, sizeof(SAIYAKEYFRAME));
		int k = 0;
		for (float fElement : jLoadChild["0_Pos"])
			memcpy(((float*)&vPos) + (k++), &fElement, sizeof(float));
		Desc.vPos = vPos;
		int j = 0;
		for (float fElement : jLoadChild["1_LookAt"])
			memcpy(((float*)&vLookAt) + (j++), &fElement, sizeof(float));
		Desc.vLook = vLookAt;

		m_keyframes.push_back(Desc);
	}

	return S_OK;
}

void CSaiya::Rot_WispSetPosition()
{
	if (m_pRot)
	{
		CBone* pBone = m_pModelCom->Get_BonePtr("char_spine_low_jnt");
		_matrix			SocketMatrix =
			pBone->Get_OffsetMatrix() *
			pBone->Get_CombindMatrix() *
			m_pModelCom->Get_PivotMatrix() *
			m_pTransformCom->Get_WorldMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
		_float4x4 pivotMatrix;
		XMStoreFloat4x4(&pivotMatrix, SocketMatrix);
		_float4 vRotLook = _float4(pivotMatrix._31, 0.f, pivotMatrix._33, 0.f);
		_float4 vRotPos = _float4(pivotMatrix._41, pivotMatrix._42, pivotMatrix._43, 1.f) + vRotLook * -1.5f;
		m_pRot->Set_Position(vRotPos);
		m_pRot->Set_WispPos(vRotPos);
	}
}

void CSaiya::Setting_Sound()
{
	CSound::SOUND_DESC desc;
	ZeroMemory(&desc, sizeof(CSound::SOUND_DESC));
	desc.bIs3D = true;
	desc.fRange = 20.f;
	desc.pStartTransform = m_pTransformCom;
	desc.pTargetTransform = m_pKena->Get_TransformCom();
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_1, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_2, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_3, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_4, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_5, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_6, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(SND_LAUGHTER_7, desc);
}

void CSaiya::Play_LaughSound(float fVolume)
{
	if (m_bSoundCheck[6] == true)
	{
		for (_int i = 0; i < 7; ++i)
			m_bSoundCheck[i] = false;
	}

	if(!m_bSoundCheck[0])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_1, fVolume, false, SOUND_NPC);
		m_bSoundCheck[0] = true;
	}
	else	if (!m_bSoundCheck[1])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_2, fVolume, false, SOUND_NPC);
		m_bSoundCheck[1] = true;
	}
	else	if (!m_bSoundCheck[2])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_3, fVolume, false, SOUND_NPC);
		m_bSoundCheck[2] = true;
	}
	else	if (!m_bSoundCheck[3])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_4, fVolume, false, SOUND_NPC);
		m_bSoundCheck[3] = true;
	}
	else	if (!m_bSoundCheck[4])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_5, fVolume, false, SOUND_NPC);
		m_bSoundCheck[4] = true;
	}
	else	if (!m_bSoundCheck[5])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_6, fVolume, false, SOUND_NPC);
		m_bSoundCheck[5] = true;
	}
	else	if (!m_bSoundCheck[6])
	{
		CGameInstance::GetInstance()->Play_ManualSound(SND_LAUGHTER_7, fVolume, false, SOUND_NPC);
		m_bSoundCheck[6] = true;
	}
}

void CSaiya::Execute_Photo()
{
	m_bPhoto = true;
}

_bool CSaiya::Is_PhotoAnimEnd()
{
	if (m_bPhotoAnimEnd == false)
		return false;

	return true;
}

CSaiya* CSaiya::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSaiya*	pInstance = new CSaiya(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSaiya");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSaiya::Clone(void* pArg)
{
	CSaiya*	pInstance = new CSaiya(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSaiya");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSaiya::Free()
{
	__super::Free();

	for(_uint i=0;i<10;++i)
		m_vecChat[i].clear();
}
