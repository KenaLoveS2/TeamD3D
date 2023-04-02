﻿#include "stdafx.h"
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

/* For. Delegator Default Value (meaningless) */
_float		fDefaultVal = 345.f; /* Default Chat Value */
_bool		bDefaultVal = false;
wstring		wstrDefault = L"";

CSaiya::CSaiya(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CNpc(pDevice, pContext)
	, m_iChatIndex(0)
	, m_iLineIndex(0)
	, m_pFocus(nullptr)
{
}

CSaiya::CSaiya(const CNpc& rhs)
	: CNpc(rhs)
	, m_iChatIndex(0)
	, m_iLineIndex(0)
	, m_pFocus(nullptr)
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

	Load_KeyFrame();
	if(!m_keyframes.empty())
	{
		const _float4 vPos = _float4( m_keyframes[m_iKeyFrame].vPos.x, m_keyframes[m_iKeyFrame].vPos.y, m_keyframes[m_iKeyFrame].vPos.z,1.f);
		const _float4 vLook = _float4(m_keyframes[m_iKeyFrame].vLook.x, m_keyframes[m_iKeyFrame].vLook.y, m_keyframes[m_iKeyFrame].vLook.z,0.f);
		m_pTransformCom->Set_Position(vPos);
		m_pTransformCom->Set_Look(vLook);
	}

	m_pCinecam[0] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CINE0")));
	m_pCinecam[1] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CINE1")));

	return S_OK;
}

void CSaiya::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_iNumKeyFrame = (_uint)m_keyframes.size();
	Update_Collider(fTimeDelta);
	if (m_pFSM && m_iKeyFrame < m_iNumKeyFrame - 2) m_pFSM->Tick(fTimeDelta);
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);

	if (m_bStraight)
		m_pTransformCom->Go_Straight(fTimeDelta);
}

void CSaiya::Late_Tick(_float fTimeDelta)
{
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

		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		if (i == 0)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SAIYA_EYE);
		else if(i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT);
		else
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", AORM);
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
	//CNpc::Imgui_RenderProperty();

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
	if (ImGui::CollapsingHeader("Saiya"))
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

void CSaiya::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

HRESULT CSaiya::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("INIT_CAM")

		.AddState("INIT_CAM")
		.OnExit([this]()
	{
		CGameInstance::GetInstance()->Work_Camera(L"NPC_CINE0");
		dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Play();
		m_bCinecam[0] = true;
	})
		.AddTransition("INIT_CAM to ACTION_0" , "ACTION_0")
		.Predicator([this]()
	{
		return 	DistanceTrigger(10.f) && !m_bCinecam[0];
	})

		.AddState("ACTION_0")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_CHASINGLOOP);
	})
		.AddTransition("ACTION_0 to ACTION_1" , "ACTION_1")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_CHASINGLOOP) && m_pCinecam[0]->CameraFinishedChecker();
	})


		.AddState("ACTION_1")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[1].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
	})
		.AddTransition("ACTION_1 to ACTION_2", "ACTION_2")
		.Predicator([this]()
	{
		return !DistanceTrigger(10.f);
	})


		.AddState("ACTION_2")
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
			m_pTransformCom->Set_Position(CUtile::Float3toFloat4Position(m_keyframes[2].vPos));
			m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[2].vLook));
		}
	})
		.AddTransition("ACTION_2 to ACTION_3" , "ACTION_3")
		.Predicator([this]()
	{
		return AnimFinishChecker(SAIYA_TELEPORT);
	})


		.AddState("ACTION_3")
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_CHASINGLOOP);
	})
		.AddTransition("ACTION_3 to ACTION_4", "ACTION_4")
		.Predicator([this]()
	{
		return DistanceTrigger(5.f) && AnimFinishChecker(SAIYA_CHASINGLOOP) && !m_bCinecam[1];
	})


		.AddState("ACTION_4")
		.OnStart([this]()
	{
		CGameInstance::GetInstance()->Work_Camera(L"NPC_CINE1");
		dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Play();
		m_bCinecam[1] = true;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(SAIYA_RUN);
		m_pTransformCom->Set_Look(CUtile::Float3toFloat4Look(m_keyframes[3].vLook));
		m_pTransformCom->Go_Straight(fTimeDelta);
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


		.Build();
	return S_OK;
}

HRESULT CSaiya::SetUp_Components()
{
	__super::SetUp_Components();
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Saiya", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
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

	return S_OK;
}

void CSaiya::AdditiveAnim(_float fTimeDelta)
{
	m_pModelCom->Set_AdditiveAnimIndexForMonster(SAIYA_MOUTHFLAP);
	m_pModelCom->Play_AdditiveAnimForMonster(fTimeDelta, 1.f, "SK_Saiya.ao");
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
	string      strSaveDirectory = "../Bin/Data/NPC/SaiyaKeyFrame.json";

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
	string      strLoadDirectory = "../Bin/Data/NPC/SaiyaKeyFrame.json";
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
