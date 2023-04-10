#include "stdafx.h"
#include "..\public\MannequinRot.h"
#include "GameInstance.h"
#include "CameraForRot.h"
#include "E_TeleportRot.h"

CMannequinRot::CMannequinRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CRot_Base(pDevice, p_context)
{
}

CMannequinRot::CMannequinRot(const CMannequinRot& rhs)
	: CRot_Base(rhs)
{
}

HRESULT CMannequinRot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CMannequinRot::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(nullptr), E_FAIL);
		
	Create_Hats();
		
	m_pMyCam = static_cast<CCameraForRot*>(m_pGameInstance->Find_Camera(L"ROT_CAM"));

	return S_OK;
}

HRESULT CMannequinRot::Late_Initialize(void * pArg)
{		
	// FAILED_CHECK_RETURN(__super::Late_Initialize(nullptr), E_FAIL);
	FAILED_CHECK_RETURN(Setup_KenaParamters(), E_FAIL);

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
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), TELEPORT6);
	})	
		.AddTransition("TELEPORT to START_SHOW", "IDLE")
		.Predicator([this]()
	{
		return m_pModelCom->Get_AnimationFinish();
	})


		.AddState("IDLE")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(IDLE);
		m_pModelCom->Set_AnimIndex(IDLE);
		m_fIdleTimeCheck = 0.f;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_fIdleTimeCheck += fTimeDelta;

	})
		.OnExit([this]()
	{
		_uint iIndex = m_bEndShow ? IDLE : m_iPerchaseAnimIndex;
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
		m_iPerchaseAnimIndex = 
			m_iPerchaseAnimIndex == (_uint)PURCHASEHAT1 ? PURCHASEHAT3 : 
			m_iPerchaseAnimIndex == (_uint)PURCHASEHAT3 ? PURCHASEHAT4 : PURCHASEHAT1;			
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
		TurnOn_TeleportEffect(m_pTransformCom->Get_Position(), HIDE5);		
	})
		.Tick([this](_float fTimeDelta)
	{	
		m_fIdleTimeCheck += fTimeDelta;
	})
		.OnExit([this]()
	{	
		m_bShowFlag = m_bRender = m_bEndShow = false;		
		m_pModelCom->Set_AnimIndex(IDLE);
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
