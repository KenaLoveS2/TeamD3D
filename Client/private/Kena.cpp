#include "stdafx.h"
#include "..\public\Kena.h"
#include "GameInstance.h"
#include "AnimationState.h"
#include "Kena_State.h"
#include "Kena_Parts.h"
#include "Camera_Player.h"

CKena::CKena(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CKena::CKena(const CKena & rhs)
	: CGameObject(rhs)
{
}

_double CKena::Get_AnimationPlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

HRESULT CKena::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CKena::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Find_Camera(L"PLAYER_CAM"));
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);
	m_pCamera->Set_Player(this, m_pTransformCom);
	CGameInstance::GetInstance()->Work_Camera(L"PLAYER_CAM");

	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_pKenaState = CKena_State::Create(this, m_pStateMachine, m_pModelCom, m_pAnimation, m_pTransformCom, m_pCamera);

	FAILED_CHECK_RETURN(Ready_Parts(), E_FAIL);

	//m_pModelCom->Set_AnimIndex(CKena_State::IDLE);

	Push_EventFunctions();

	m_fSSSAmount = 0.09f;
	m_vSSSColor = _float4(0.2f, 0.18f, 0.16f, 1.f);
	m_vMulAmbientColor = _float4(2.45f, 2.f, 2.f, 1.f);
	m_vEyeAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);

	/*
	CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
	PxBoxDesc.eType = BOX_DYNAMIC;
	PxBoxDesc.pActortag = TEXT("TEST");
	PxBoxDesc.vPos = _float3(0.f, 5.f, 0.f);
	PxBoxDesc.vSize = { 0.2f, 0.2f, 0.2f };
	PxBoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxBoxDesc.fDensity = 10.f;
	PxBoxDesc.fAngularDamping = 0.5f;

	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(this));
	m_pTransformCom->Connect_PxActor(TEXT("TEST"));
	*/
	
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = TEXT("TEST_SPERE");
	PxSphereDesc.vPos = _float3(0.f, 10.f, 0.f);
	PxSphereDesc.fRadius = 1.f;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 10.f;
	PxSphereDesc.fAngularDamping = 0.5f;
			
	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this));	
	m_pTransformCom->Connect_PxActor(TEXT("TEST_SPERE"));

	/*
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_STATIC;
	PxCapsuleDesc.pActortag = TEXT("TEST_CAPSULE");
	PxCapsuleDesc.vPos = _float3(1.f, 5.f, 1.f);
	PxCapsuleDesc.fRadius = 1.f;
	PxCapsuleDesc.fHalfHeight = 0.2f;
	PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxCapsuleDesc.fDensity = 10.f;
	PxCapsuleDesc.fAngularDamping = 0.5f;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this));
	m_pTransformCom->Connect_PxActor(TEXT("TEST_CAPSULE"));
	*/

	// CPhysX_Manager::GetInstance()->Set_GravityFlag(TEXT("TEST_SPERE"), true);
	m_pRendererCom->Set_PhysXRender(true);
	
	return S_OK;
}

void CKena::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
		
	m_pKenaState->Tick(fTimeDelta);
	m_pStateMachine->Tick(fTimeDelta);
	
	for (auto& pPart : m_vecPart)
		pPart->Tick(fTimeDelta);

	if (m_pModelCom->Get_Preview() == false)
		m_pAnimation->Play_Animation(fTimeDelta);
	else
		m_pModelCom->Play_Animation(fTimeDelta);

	//m_pTransformCom->Set_Translation(XMVectorSet(0.f, 0.f, 0.f, 1.f), _float4(1.f, 0.f, 0.f, 0.f));
}

void CKena::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pKenaState->Late_Tick(fTimeDelta);

	if (CGameInstance::GetInstance()->Key_Down(DIK_UP))
		m_iAnimationIndex++;
	if (CGameInstance::GetInstance()->Key_Down(DIK_DOWN))
		m_iAnimationIndex--;
	CUtile::Saturate<_int>(m_iAnimationIndex, 0, 499);	
	
	/************** Delegator Test *************/
	CUI_ClientManager::UI_PRESENT eHP		= CUI_ClientManager::HUD_HP;
	CUI_ClientManager::UI_PRESENT ePip			= CUI_ClientManager::HUD_PIP;
	CUI_ClientManager::UI_PRESENT eType3		= CUI_ClientManager::HUD_SHIELD;
	CUI_ClientManager::UI_PRESENT eType4		= CUI_ClientManager::HUD_ROT;
	CUI_ClientManager::UI_PRESENT eBomb			= CUI_ClientManager::AMMO_BOMB;
	CUI_ClientManager::UI_PRESENT eArrowGuage	= CUI_ClientManager::AMMO_ARROW;
	CUI_ClientManager::UI_PRESENT eAim			= CUI_ClientManager::AIM_;

	CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	CUI_ClientManager::UI_FUNCTION funcLevelup = CUI_ClientManager::FUNC_LEVELUP;
	static _float fNum = 3.f;
	_float fZero = 0.f;
	if (CGameInstance::GetInstance()->Key_Down(DIK_U))
	{
		static _float fLevel = 0.f;
		fLevel += 1.f;
		m_PlayerDelegator.broadcast(eArrowGuage, funcLevelup, fLevel);
		m_PlayerDelegator.broadcast(eBomb, funcLevelup, fLevel);
		m_PlayerDelegator.broadcast(ePip, funcLevelup, fLevel);
		m_PlayerDelegator.broadcast(eHP, funcLevelup, fLevel);
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_P))
	{
		/* Pip Guage pop test */
		fNum -= 1.f;
		m_PlayerDelegator.broadcast(ePip, funcDefault, fZero);


		/* Rot icon chagne test */
		static _float fIcon = 0;
		fIcon = _float(_uint(fIcon + 1) % 4);
		m_PlayerDelegator.broadcast(eType4, funcDefault, fIcon);

		/* Bomb Guage test */
		static _float fBomb = 0.f;
		m_PlayerDelegator.broadcast(eBomb, funcDefault, fBomb);

		/* Arrow Guage test */
		static _float fArrow = 1.f; 
		m_PlayerDelegator.broadcast(eArrowGuage, funcDefault, fArrow);

		/* Aim Test */
		static _float fAim = 1.f;
		m_PlayerDelegator.broadcast(eAim, funcDefault, fAim);

	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_I))
	{
		fNum -= 0.1f;
		m_PlayerDelegator.broadcast(eHP, funcDefault, fNum);
		m_PlayerDelegator.broadcast(ePip, funcDefault, fNum);
		m_PlayerDelegator.broadcast(eType3, funcDefault, fNum);
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_O))
	{
		fNum += 0.1f;
		m_PlayerDelegator.broadcast(eHP, funcDefault, fNum);
		m_PlayerDelegator.broadcast(ePip, funcDefault, fNum);
		m_PlayerDelegator.broadcast(eType3, funcDefault, fNum);
	}

	/************** ~Delegator Test *************/

	if (m_pRendererCom != nullptr)
	{
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_F7))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	for (auto& pPart : m_vecPart)
		pPart->Late_Tick(fTimeDelta);
}

HRESULT CKena::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		if (i == 1)
		{
			// Arm & Leg
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveMaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SSS_MASK, "g_SSSMaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		}
		else if (i == 4)
		{
			// Eye Render
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		}
		else if (i ==5 || i == 6)
		{
			// HEAD
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SSS_MASK, "g_SSSMaskTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 4);
		}
		else if (i == 0)
		{
			// Render Off
			continue;
		}
		else
		{
			// Eye Lash
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 7);
		}
	}

	return S_OK;
}

HRESULT CKena::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");

	return S_OK;
}

void CKena::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CKena::ImGui_AnimationProperty()
{
	ImGui::BeginTabBar("Kena Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("State"))
	{
		ImGui::BulletText("Kena State");
		m_pStateMachine->Imgui_RenderProperty();
		ImGui::Separator();
		ImGui::BulletText("Animation State");
		m_pAnimation->ImGui_RenderProperty();
		ImGui::Separator();

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CKena::ImGui_ShaderValueProperty()
{
	if(ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}

	__super::ImGui_ShaderValueProperty();
	{
		static _float2 AmountMinMax{ -10.f, 10.f };
		ImGui::InputFloat2("SSSAmoutMinMax", (float*)&AmountMinMax);
		ImGui::DragFloat("SSSAmount", &m_fSSSAmount, 0.001f, AmountMinMax.x, AmountMinMax.y);

		_float fColor[3] = { m_vSSSColor.x, m_vSSSColor.y, m_vSSSColor.z };
		static _float2 sssMinMax{ -1.f, 1.f };
		ImGui::InputFloat2("SSSMinMax", (float*)&sssMinMax);
		ImGui::DragFloat3("SSSColor", fColor, 0.001f, sssMinMax.x, sssMinMax.y);
		m_vSSSColor.x = fColor[0];
		m_vSSSColor.y = fColor[1];
		m_vSSSColor.z = fColor[2];
	}

	{
		_float fColor[3] = { m_vMulAmbientColor.x, m_vMulAmbientColor.y, m_vMulAmbientColor.z };
		static _float2 maMinMax{ 0.f, 255.f };
		ImGui::InputFloat2("MAMinMax", (float*)&maMinMax);
		ImGui::DragFloat3("MAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
		m_vMulAmbientColor.x = fColor[0];
		m_vMulAmbientColor.y = fColor[1];
		m_vMulAmbientColor.z = fColor[2];
	}

	{
		_float fColor[3] = { m_vEyeAmbientColor.x, m_vEyeAmbientColor.y, m_vEyeAmbientColor.z };
		static _float2 maMinMax{ 0.f, 255.f };
		ImGui::InputFloat2("EyeAAMinMax", (float*)&maMinMax);
		ImGui::DragFloat3("EyeAAmount", fColor, 0.01f, maMinMax.x, maMinMax.y);
		m_vEyeAmbientColor.x = fColor[0];
		m_vEyeAmbientColor.y = fColor[1];
		m_vEyeAmbientColor.z = fColor[2];
	}

	{
		static _float2 LashWidthMinMax{ 0.f, 100.f };
		ImGui::InputFloat2("LashWidthMinMax", (float*)&LashWidthMinMax);
		ImGui::DragFloat("LashWidthAmount", &m_fLashWidth, 0.1f, LashWidthMinMax.x, LashWidthMinMax.y);

		static _float2 LashDensityMinMax{ 0.f, 100.f };
		ImGui::InputFloat2("LashDensityAmoutMinMax", (float*)&LashDensityMinMax);
		ImGui::DragFloat("LashDensityAmount", &m_fLashDensity, 0.1f, LashDensityMinMax.x, LashDensityMinMax.y);

		static _float2 LashIntensity{ 0.f, 100.f };
		ImGui::InputFloat2("LashIntensityMinMax", (float*)&LashIntensity);
		ImGui::DragFloat("LashIntensityAmount", &m_fLashIntensity, 0.1f, LashIntensity.x, LashIntensity.y);
	}

	for (auto& pPart : m_vecPart)
	{
		ImGui::NewLine();
		pPart->ImGui_ShaderValueProperty();
	}
}

void CKena::Update_Child()
{
	//for (auto& pPart : m_vecPart)
	//	pPart->Model_Synchronization(m_pModelCom->Get_PausePlay());
}

HRESULT CKena::Call_EventFunction(const string & strFuncName)
{
	return S_OK;
}

void CKena::Push_EventFunctions()
{
	Test(true, 0.f);
}

HRESULT CKena::Ready_Parts()
{
	CKena_Parts*	pPart = nullptr;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CKena_Parts::KENAPARTS_DESC	PartDesc;

	/* Staff */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_STAFF;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_Staff", L"Kena_Staff", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(LEVEL_GAMEPLAY, pPart);
	m_pAnimation->Add_AnimSharingPart(dynamic_cast<CModel*>(pPart->Find_Component(L"Com_Model")), false);

	/* MainOutfit */
	ZeroMemory(&PartDesc, sizeof(CKena_Parts::KENAPARTS_DESC));

	PartDesc.pPlayer = this;
	PartDesc.eType = CKena_Parts::KENAPARTS_OUTFIT;

	pPart = dynamic_cast<CKena_Parts*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Kena_MainOutfit", L"Kena_MainOutfit", &PartDesc));
	NULL_CHECK_RETURN(pPart, E_FAIL);

	m_vecPart.push_back(pPart);
	pGameInstance->Add_AnimObject(LEVEL_GAMEPLAY, pPart);
	m_pAnimation->Add_AnimSharingPart(dynamic_cast<CModel*>(pPart->Find_Component(L"Com_Model")), true);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CKena::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Kena", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	//For.Cloth			
	// AO_R_M
	m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_AO_R_M.png"));
	// EMISSIVE
	m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_EMISSIVE.png"));
	// EMISSIVE_MASK
	m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVEMASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_EMISSIVE_MASK.png"));
	// MASK
	m_pModelCom->SetUp_Material(1, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_MASK.png"));
	// SPRINT_EMISSIVE
	m_pModelCom->SetUp_Material(1, WJTextureType_SPRINT_EMISSIVE, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_sprint_EMISSIVE.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(1, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_cloth_SSS_MASK.png"));

	// AO_R_M
	m_pModelCom->SetUp_Material(5, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_AO_R_M.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(5, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_SSS_MASK.png"));

	// AO_R_M
	m_pModelCom->SetUp_Material(6, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_AO_R_M.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(6, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_SSS_MASK.png"));

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Collider_SPHERE", L"Com_RangeCol", (CComponent**)&m_pRangeCol, &ColliderDesc, this), E_FAIL);

	CNavigation::NAVIDESC		NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Navigation", L"Com_Navigation", (CComponent**)&m_pNavigationCom, &NaviDesc, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachine, nullptr, this), E_FAIL);

	return S_OK;
}

HRESULT CKena::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	m_pShaderCom->Set_RawValue("g_fSSSAmount", &m_fSSSAmount, sizeof(float));
	m_pShaderCom->Set_RawValue("g_vSSSColor", &m_vSSSColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vAmbientColor", &m_vMulAmbientColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vAmbientEyeColor", &m_vEyeAmbientColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_fLashDensity", &m_fLashDensity, sizeof(float));
	m_pShaderCom->Set_RawValue("g_fLashWidth", &m_fLashWidth, sizeof(float));
	m_pShaderCom->Set_RawValue("g_fLashIntensity", &m_fLashIntensity, sizeof(float));

	return S_OK;
}

HRESULT CKena::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CKena::SetUp_State()
{
	m_pAnimation = CAnimationState::Create(this, m_pModelCom);

	CAnimState*			pAnimState = nullptr;
	CAdditiveAnimation*	pAdditiveAnim = nullptr;

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "IDLE";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::IDLE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "RUN";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_INTO";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_INTO);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_LOOP";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_LOOP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_FORWARD";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_FORWARD);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_FORWARD_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_FORWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_FORWARD_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_FORWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_BACKWARD";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_BACKWARD);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_BACKWARD_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_BACKWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_BACKWARD_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_BACKWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIM_RUN_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_1";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_1);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_1_FROM_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_1_FROM_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_1_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_1_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_1_INTO_WALK";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_1_INTO_WALK);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_1_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_1_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "INTO_PULSE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::INTO_PULSE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "INTO_PULSE_FROM_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::INTO_PULSE_FROM_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_LOOP";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_LOOP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_INTO_COMBAT_END";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_INTO_COMBAT_END);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_INTO_IDLE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_INTO_IDLE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_PARRY";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_PARRY);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_FORWARD";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_FORWARD);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_FRONT_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_FORWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_FRONT_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_FORWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_BACKWARD";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_BACKWARD);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_BACK_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_BACK_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_BACK_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_BACK_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_WALK_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_WALK_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_SQUAT_SPRINT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_SQUAT_SPRINT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_AIM_RIGHT_ADD";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_AIR_LOOP);
	//pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_RIGHT);

 	pAdditiveAnim = new CAdditiveAnimation;
 	pAdditiveAnim->m_bOneKeyFrame = true;
 	pAdditiveAnim->m_bControlRatio = false;
 	pAdditiveAnim->m_fAdditiveRatio = 1.f;
 	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_AIR_REFPOSE);
 	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_UP_LEFT_ADD);
 	//pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "kena_hip_jnt", CBone::LOCKTO_CHILD });
	pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "kena_spine_end_jnt", CBone::LOCKTO_PARENT });
 	
 	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	return S_OK;
}

void CKena::Test(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::Test);
		return;
	}
}

CKena * CKena::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKena*	pInstance = new CKena(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CKena::Clone(void * pArg)
{
	CKena*	pInstance = new CKena(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKena::Free()
{
	__super::Free();

	for (auto& pPart : m_vecPart)
		Safe_Release(pPart);
	m_vecPart.clear();

	Safe_Release(m_pAnimation);
	Safe_Release(m_pStateMachine);
	Safe_Release(m_pKenaState);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
