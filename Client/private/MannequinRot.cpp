#include "stdafx.h"
#include "..\public\MannequinRot.h"
#include "GameInstance.h"
#include "CameraForRot.h"
#include "E_TeleportRot.h"

CMannequinRot::CMannequinRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
{
}

CMannequinRot::CMannequinRot(const CMannequinRot& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMannequinRot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CMannequinRot::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GaemObjectDesc.TransformDesc.fSpeedPerSec = CUtile::Get_RandomFloat(1.3f,2.f);
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Effects(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pModelCom->Set_AnimIndex(CRot::IDLE);
	m_pModelCom->Set_AllAnimCommonType();
	Push_EventFunctions();

	Create_Hats();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pMyCam = static_cast<CCameraForRot*>(pGameInstance->Find_Camera(L"ROT_CAM"));


	return S_OK;
}

HRESULT CMannequinRot::Late_Initialize(void * pArg)
{		
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	
	return S_OK;
}

void CMannequinRot::Tick(_float fTimeDelta)
{
	if (m_bShowFlag == false) return;

	__super::Tick(fTimeDelta);

	m_pFSM->Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTeleportRot->Tick(fTimeDelta);
	for(auto& pHat : m_pRotHats)
		pHat->Tick(fTimeDelta);
}

void CMannequinRot::Late_Tick(_float fTimeDelta)
{	
	if (m_bShowFlag == false) return;

	__super::Late_Tick(fTimeDelta);
	// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	m_pRendererCom && m_bRender && m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pTeleportRot->Late_Tick(fTimeDelta);
	for (auto& pHat : m_pRotHats)
		pHat->Late_Tick(fTimeDelta);
}

HRESULT CMannequinRot::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	 iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 27), E_FAIL);
		}
		else if (i == 1)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 32), E_FAIL);
		}
		else if (i == 2)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_AlphaTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 31), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CMannequinRot::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 11);

	return S_OK;
}

void CMannequinRot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CMannequinRot::ImGui_AnimationProperty()
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

void CMannequinRot::ImGui_ShaderValueProperty()
{
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
}

HRESULT CMannequinRot::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Rot", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	m_pModelCom->Set_RootBone("Rot_RIG");
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Rot/rh_body_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Rot/rot_fur_ALPHA.png")), E_FAIL);

	return S_OK;
}

HRESULT CMannequinRot::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

HRESULT CMannequinRot::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

CMannequinRot* CMannequinRot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMannequinRot*	pInstance = new CMannequinRot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMannequinRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMannequinRot::Clone(void* pArg)
{
	CMannequinRot*	pInstance = new CMannequinRot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMannequinRot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMannequinRot::Free()
{
	__super::Free();
		
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pFSM);
		
	Safe_Release(m_pTeleportRot);
	for(auto &pHat : m_pRotHats)
		Safe_Release(pHat);
}

HRESULT CMannequinRot::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")		
		.AddTransition("SLEEP to TELEPORT", "TELEPORT")
		.Predicator([this]()
	{
		return m_pMyCam->IsClosed();
	})
		
		.AddState("TELEPORT")
		.OnStart([this]()
	{	
		m_bRender = true;
		m_pModelCom->ResetAnimIdx_PlayTime(CRot::TELEPORT6);
		m_pModelCom->Set_AnimIndex(CRot::TELEPORT6);
		m_pTeleportRot->Set_Active(true);
		m_pTeleportRot->Set_Position(m_pTransformCom->Get_Position());
	})	
		.AddTransition("TELEPORT to START_SHOW", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(CRot::IDLE);
		m_pModelCom->Set_AnimIndex(CRot::IDLE);
		m_fIdleTimeCheck = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdleTimeCheck += fTimeDelta;

	})
		.OnExit([this]()
	{
		_uint iIndex = m_bEndShow ? CRot::IDLE : m_iPerchaseAnimIndex;
	})
		.AddTransition("IDLE to END_SHOW", "END_SHOW")
		.Predicator([this]()
	{
		return m_bEndShow;
	})
		.AddTransition("IDLE to SHOW", "PREVIEW_HAT")
		.Predicator([this]()
	{	
		return m_fIdleTimeCheck >= 2.f;
	})

		.AddState("PREVIEW_HAT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(m_iPerchaseAnimIndex);
		m_pModelCom->Set_AnimIndex(m_iPerchaseAnimIndex);		
		m_iPerchaseAnimIndex = m_iPerchaseAnimIndex == (_uint)CRot::PURCHASEHAT3 ? CRot::PURCHASEHAT4 : CRot::PURCHASEHAT3;
	})		
		.AddTransition("PREVIEW_HAT to END_SHOW", "END_SHOW")
		.Predicator([this]()
	{
		return m_bEndShow;
	})
		.AddTransition("PREVIEW_HAT to IDLE", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.AddState("END_SHOW")
		.OnStart([this]()
	{
		m_fIdleTimeCheck = 0.f;
		m_pModelCom->Set_AnimIndex(CRot::HIDE5);
		m_pTeleportRot->Set_Active(true);
		m_pTeleportRot->Set_Position(m_pTransformCom->Get_Position());
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_fIdleTimeCheck += fTimeDelta;
	})
		.OnExit([this]()
	{	
		m_bShowFlag = m_bRender = m_bEndShow = false;		
		m_pModelCom->Set_AnimIndex(CRot::IDLE);
		m_pMyCam->Clear();
		CGameInstance::GetInstance()->Work_Camera(L"PLAYER_CAM");		
	})
		.AddTransition("END_SHOW to SLEEP", "SLEEP")
		.Predicator([this]()
	{
		return m_fIdleTimeCheck >= 2.5f; // m_pModelCom->Get_AnimationFinish();
	})


		.Build();

	return S_OK;
}

HRESULT CMannequinRot::SetUp_Effects()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTeleportRot = dynamic_cast<CE_TeleportRot*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_TeleportRot", TEXT("MannequinRot_Teleport")));
	NULL_CHECK_RETURN(m_pTeleportRot, E_FAIL);
//	m_pTeleportRot->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMannequinRot::Create_Hats()
{
	CRotHat::HAT_DESC HatDesc;	
	HatDesc.pSocket = m_pModelCom->Get_BonePtr("hat_socket_jnt");
	HatDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	HatDesc.pTargetTransform = m_pTransformCom;
	HatDesc.pOwnerRot = nullptr;

	for (_uint i = 0; i < (_uint)CRotHat::HAT_TYPE_END; i++)
	{
		m_pRotHats[i] = (CRotHat*)CGameInstance::GetInstance()->Clone_GameObject(TEXT("Prototype_GameObject_RotHat"), CUtile::Create_DummyString(TEXT("MannequinHat"), i));
		assert(m_pRotHats[i] && "CMannequinRot::Create_Hats()");

		HatDesc.eHatType = (CRotHat::HAT_TYPE)i;
		m_pRotHats[i]->Create_HatModel(HatDesc, false);
	}
}

void CMannequinRot::Change_Hat(_int iHatIndex)
{
	if (iHatIndex < -1 || iHatIndex >= (_int)CRotHat::HAT_TYPE_END) 
		return;

	if (m_iCurrentHatIndex != -1)
		m_pRotHats[m_iCurrentHatIndex]->Set_ShowFlag(false);
	
	if (iHatIndex != -1)
		m_pRotHats[iHatIndex]->Set_ShowFlag(true);

	m_iCurrentHatIndex = iHatIndex;
}

void CMannequinRot::Start_FashiomShow()
{	
	m_bShowFlag = true;

	m_pMyCam->Set_Target(this);
	CGameInstance::GetInstance()->Work_Camera(L"ROT_CAM");
}

void CMannequinRot::End_FashiomShow()
{
	m_bEndShow = true;	

	for (auto& pHat : m_pRotHats)
		pHat->Set_ShowFlag(false);

	m_iCurrentHatIndex = -1;
}