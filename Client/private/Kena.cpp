#include "stdafx.h"
#include "..\public\Kena.h"
#include "GameInstance.h"
#include "AnimationState.h"
#include "Kena_State.h"
#include "Kena_Parts.h"
#include "Camera_Player.h"
#include "Effect_Base.h"

#include "GroundMark.h"
#include "Terrain.h"
#include "Rope_RotRock.h"
#include "Rot.h"

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

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
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
	FAILED_CHECK_RETURN(Ready_Effects(), E_FAIL);

	Push_EventFunctions();

	m_fSSSAmount = 0.09f;
	m_vSSSColor = _float4(0.2f, 0.18f, 0.16f, 1.f);
	m_vMulAmbientColor = _float4(2.45f, 2.f, 2.f, 1.f);
	m_vEyeAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);

	m_fGravity = 9.81f;
	/* InitJumpSpeed = 2.f * Gravity * JumpHeight */
	m_fInitJumpSpeed = sqrtf(2.f * m_fGravity * 1.5f);
	
	m_iObjectProperty = OP_PLAYER;

	return S_OK;
}

HRESULT CKena::Late_Initialize(void * pArg)
{
	_float3 vPos = _float3(0.f, 3.f, 0.f);
	_float3 vPivotScale = _float3(0.2f, 0.5f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.7f, 0.f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
	PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
	PxCapsuleDesc.pActortag = m_szCloneObjectTag;
	PxCapsuleDesc.vPos = vPos;
	PxCapsuleDesc.fRadius = vPivotScale.x;
	PxCapsuleDesc.fHalfHeight = vPivotScale.y;
	PxCapsuleDesc.vVelocity = _float3(0.f,0.f,0.f);
	PxCapsuleDesc.fDensity = 1.f;
	PxCapsuleDesc.fAngularDamping = 0.5f;
	PxCapsuleDesc.fMass = 59.f;
	PxCapsuleDesc.fLinearDamping = 1.f;
	PxCapsuleDesc.bCCD = true;
	PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxCapsuleDesc.fDynamicFriction = 0.5f;
	PxCapsuleDesc.fStaticFriction = 0.5f;
	PxCapsuleDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, true, COL_PLAYER));

	// 여기 뒤에 세팅한 vPivotPos를 넣어주면된다.
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, vPivotPos);
	m_pRendererCom->Set_PhysXRender(true);
	m_pTransformCom->Set_PxPivotScale(vPivotScale);
	m_pTransformCom->Set_PxPivot(vPivotPos);

	CGameInstance* pGameInst = CGameInstance::GetInstance();
	m_pTerrain = (CTerrain*)pGameInst->Get_GameObjectPtr(g_LEVEL, L"Layer_BackGround", L"Terrain");

	return S_OK;
}

void CKena::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	// if (CGameInstance::GetInstance()->IsWorkCamera(TEXT("DEBUG_CAM_1"))) return;	
#endif
	
	__super::Tick(fTimeDelta);

	// Test_Raycast();

	if (m_pAnimation->Get_Preview() == false)
	{
		m_pKenaState->Tick(fTimeDelta);
		m_pStateMachine->Tick(fTimeDelta);
	}
	m_pTransformCom->Tick(fTimeDelta);

	m_bCommonHit = false;
	m_bHeavyHit = false;

	if (m_pModelCom->Get_Preview() == false)
		m_pAnimation->Play_Animation(fTimeDelta);
	else
		m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pPart : m_vecPart)
		pPart->Tick(fTimeDelta);

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Tick(fTimeDelta);
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
	CUI_ClientManager::UI_PRESENT eHP = CUI_ClientManager::HUD_HP;
	CUI_ClientManager::UI_PRESENT ePip = CUI_ClientManager::HUD_PIP;
	CUI_ClientManager::UI_PRESENT eType3 = CUI_ClientManager::HUD_SHIELD;
	CUI_ClientManager::UI_PRESENT eType4 = CUI_ClientManager::HUD_ROT;
	CUI_ClientManager::UI_PRESENT eBomb = CUI_ClientManager::AMMO_BOMB;
	CUI_ClientManager::UI_PRESENT eArrowGuage = CUI_ClientManager::AMMO_ARROW;
	CUI_ClientManager::UI_PRESENT eAim = CUI_ClientManager::AIM_;
	CUI_ClientManager::UI_PRESENT eQuest = CUI_ClientManager::QUEST_;
	CUI_ClientManager::UI_PRESENT eQuestLine = CUI_ClientManager::QUEST_LINE;
	CUI_ClientManager::UI_PRESENT eInv = CUI_ClientManager::INV_;
	CUI_ClientManager::UI_PRESENT eKarma = CUI_ClientManager::INV_KARMA;
	CUI_ClientManager::UI_PRESENT eNumRots = CUI_ClientManager::INV_NUMROTS;
	CUI_ClientManager::UI_PRESENT eCrystal = CUI_ClientManager::INV_CRYSTAL;

	//CUI_ClientManager::UI_PRESENT eUpgrade = CUI_ClientManager::INV_UPGRADE;

	CUI_ClientManager::UI_FUNCTION funcDefault = CUI_ClientManager::FUNC_DEFAULT;
	CUI_ClientManager::UI_FUNCTION funcLevelup = CUI_ClientManager::FUNC_LEVELUP;
	CUI_ClientManager::UI_FUNCTION funcSwitch = CUI_ClientManager::FUNC_SWITCH;
	CUI_ClientManager::UI_FUNCTION funcCheck = CUI_ClientManager::FUNC_CHECK;

	if (CGameInstance::GetInstance()->Key_Down(DIK_M))
	{
		_float fTag = 0.f;
		_float fCurrency[3] = { 200.f, 13.f, 230.f };
		m_PlayerDelegator.broadcast(eInv, funcDefault, fTag);
		m_PlayerDelegator.broadcast(eKarma, funcDefault, fCurrency[0]);
		m_PlayerDelegator.broadcast(eNumRots, funcDefault, fCurrency[1]);
		m_PlayerDelegator.broadcast(eCrystal, funcDefault, fCurrency[2]);

		//	m_PlayerDelegator.broadcast(eUpgrade, funcDefault, fTag);
	}

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

		m_PlayerDelegator.broadcast(eQuest, funcSwitch, fZero);

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

		/* Quest Open Test */
		static _float fQuestIndex = 0.f;
		m_PlayerDelegator.broadcast(eQuestLine, funcSwitch, fQuestIndex);
		fQuestIndex += 1.f;

	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_I))
	{
		fNum -= 0.1f;
		m_PlayerDelegator.broadcast(eHP, funcDefault, fNum);
		m_PlayerDelegator.broadcast(ePip, funcDefault, fNum);
		m_PlayerDelegator.broadcast(eType3, funcDefault, fNum);

		/* Quest Check Test */
		static _float fQuestClear = 0.f;
		m_PlayerDelegator.broadcast(eQuestLine, funcCheck, fQuestClear);
		fQuestClear += 1.f;


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

	for (auto& pEffect : m_mapEffect)
		pEffect.second->Late_Tick(fTimeDelta);
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
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_HAIR_DEPTH, "g_DetailNormal");
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
	m_pTransformCom->Imgui_RenderProperty_ForJH();

	ImGui::BeginTabBar("Kena Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("State"))
	{
		ImGui::BulletText("Kena State");
		if (ImGui::Button("Rebuild StateMachine"))
		{
			m_pStateMachine->Rebuild();
			Safe_Release(m_pKenaState);
			m_pKenaState = CKena_State::Create(this, m_pStateMachine, m_pModelCom, m_pAnimation, m_pTransformCom, m_pCamera);
		}
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
		 system("../../Copy.bat");
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

void CKena::ImGui_PhysXValueProperty()
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

	// 이게 사실상 px 매니저 imgui_render에 있긴함
	//PxRigidActor* pRigidActor =	CPhysX_Manager::GetInstance()->Find_DynamicActor(m_szCloneObjectTag);
// 	_float fMass = ((PxRigidDynamic*)pRigidActor)->getMass();
// 	ImGui::DragFloat("Mass", &fMass, 0.01f, -100.f, 500.f);
// 	_float fLinearDamping = ((PxRigidDynamic*)pRigidActor)->getLinearDamping();
// 	ImGui::DragFloat("LinearDamping", &fLinearDamping, 0.01f, -100.f, 500.f);
// 	_float fAngularDamping = ((PxRigidDynamic*)pRigidActor)->getAngularDamping();
// 	ImGui::DragFloat("AngularDamping", &fAngularDamping, 0.01f, -100.f, 500.f);
// 	_float3 vVelocity = CUtile::ConvertPosition_PxToD3D(((PxRigidDynamic*)pRigidActor)->getLinearVelocity());
// 	float fVelocity[3] = { vVelocity.x, vVelocity.y, vVelocity.z };
// 	ImGui::DragFloat3("PxVelocity", fVelocity, 0.01f, 0.1f, 100.0f);
// 	vVelocity.x = fVelocity[0]; vVelocity.y = fVelocity[1]; vVelocity.z = fVelocity[2];
	//CPhysX_Manager::GetInstance()->Set_Velocity(pRigidActor, _float3(0.f, m_fCurJumpSpeed, 0.f));
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
	TurnOnAttack(true, 0.f);
	TurnOffAttack(true, 0.f);
	TurnOnCharge(true, 0.f);
	TurnOffCharge(true, 0.f);
}

void CKena::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
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

HRESULT CKena::Ready_Effects()
{
	CEffect_Base*	pEffectBase = nullptr;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* Pulse */
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulse", L"KenaPulse"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);

	pEffectBase->Set_Parent(this);
	m_mapEffect.emplace("KenaPulse", pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CKena::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

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
	// Detail_Normal
	m_pModelCom->SetUp_Material(1, WJTextureType_HAIR_DEPTH, TEXT("../Bin/Resources/Anim/Kena/PostProcess/T_FabricDetailNormal.png"));

	// AO_R_M
	m_pModelCom->SetUp_Material(5, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_AO_R_M.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(5, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_SSS_MASK.png"));

	// AO_R_M
	m_pModelCom->SetUp_Material(6, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_AO_R_M.png"));
	// SSS_MASK
	m_pModelCom->SetUp_Material(6, WJTextureType_SSS_MASK, TEXT("../Bin/Resources/Anim/Kena/PostProcess/kena_head_SSS_MASK.png"));

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
	m_pModelCom->Set_RootBone("kena_RIG");
	m_pAnimation = CAnimationState::Create(this, m_pModelCom, "kena_RIG", "../Bin/Data/Animation/Kena.json");
	return S_OK;

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
	pAnimState->m_strStateName = "RUN_STOP";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::RUN_STOP);
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
	pAnimState->m_strStateName = "ATTACK_2";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_2);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_2_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_2_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_2_INTO_WALK";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_2_INTO_WALK);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_2_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_2_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_3";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_3);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_3_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_3_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_3_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_3_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_4";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_4);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_4_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_4_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ATTACK_4_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ATTACK_4_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_CHARGE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_CHARGE_ADD);

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_CHARGE_FULL";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_CHARGE_FULL_ADD);

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_CHARGE_LOOP";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_CHARGE_LOOP_ADD);

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_CHARGE_LOOP_RUN_FORWARD_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_FORWARD);
	pAnimState->m_pBlendAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_RUN_RIGHT);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_CHARGE_LOOP_ADD);
	pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "kena_hip_jnt", CBone::LOCKTO_CHILD });
	pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "kena_spine_low_jnt", CBone::LOCKTO_ALONE});
	pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "staff_root_jnt", CBone::LOCKTO_PARENT});

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_RELEASE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_REFPOSE);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_RELEASE_ADD);
	pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "kena_lf_ankle_jnt", CBone::LOCKTO_ALONE });
	pAdditiveAnim->m_listLockedJoint.push_back(CAnimationState::JOINTSET{ "kena_rt_ankle_jnt", CBone::LOCKTO_ALONE });

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_RECHARGE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_RECHARGE_ADD);

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_REFPOSE);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIM_REFPOSE);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_RETURN_ADD);

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

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
	pAnimState->m_strStateName = "BACKFLIP";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BACKFLIP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL_LEFT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL_RIGHT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL_INTO_IDLE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL_INTO_IDLE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL_INTO_WALK";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL_INTO_WALK);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "ROLL_INTO_FALL";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::ROLL_INTO_FALL);
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
	pAnimState->m_strStateName = "INTO_SPRINT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::INTO_SPRINT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SPRINT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SPRINT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SPRINT_STOP";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SPRINT_STOP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SPRINT_LEAN_LEFT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SPRINT_LEAN_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SPRINT_LEAN_RIGHT";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SPRINT_LEAN_RIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SPRINT_TURN_180";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SPRINT_TURN_180);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SPRINT_ATTACK";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SPRINT_ATTACK);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "IDLE_INTO_LOCK_ON";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::IDLE_INTO_LOCK_ON);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LOCK_ON_IDLE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::LOCK_ON_IDLE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LOCK_ON_TO_IDLE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::LOCK_ON_TO_IDLE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "COMBAT_IDLE_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::COMBAT_IDLE_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "COMBAT_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::COMBAT_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "COMBAT_RUN_ADDITIVE_POSE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::COMBAT_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAdditiveAnim = new CAdditiveAnimation;
	pAdditiveAnim->m_eControlRatio = CAdditiveAnimation::RATIOTYPE_MAX;
	pAdditiveAnim->m_fAdditiveRatio = 1.f;
	pAdditiveAnim->m_fMaxAdditiveRatio = 1.f;
	pAdditiveAnim->m_pRefAnim = m_pModelCom->Find_Animation((_uint)CKena_State::COMBAT_RUN);
	pAdditiveAnim->m_pAdditiveAnim = m_pModelCom->Find_Animation((_uint)CKena_State::COMBAT_RUN_ADDITIVE_POSE_ADD);

	pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);

	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_1_CHARGE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_1_CHARGE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_1_RELEASE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_1_RELEASE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_1_RELEASE_PERFECT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_1_RELEASE_PERFECT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_1_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_1_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_1_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_1_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_2_CHARGE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_2_CHARGE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_2_RELEASE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_2_RELEASE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_2_RELEASE_PERFECT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_2_RELEASE_PERFECT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_2_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_2_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_2_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_2_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_3_CHARGE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_3_CHARGE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_3_RELEASE";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_3_RELEASE);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_3_RELEASE_PERFECT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_3_RELEASE_PERFECT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_3_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.1f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_3_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_COMBO";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_COMBO);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_COMBO_RETURN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_COMBO_RETURN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "HEAVY_ATTACK_COMBO_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::HEAVY_ATTACK_COMBO_INTO_RUN);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "JUMP_SQUAT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::JUMP_SQUAT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "JUMP";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::JUMP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "RUNNING_JUMP_SQUAT";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::RUNNING_JUMP_SQUAT_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "RUNNING_JUMP";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::RUNNING_JUMP_LEFT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "PULSE_JUMP";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::PULSE_JUMP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LAND";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::LAND);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LAND_HEAVY";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::LAND_HEAVY);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LAND_WALKING";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::LAND_WALKING);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LAND_RUNNING";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::DELETE_KENA_LAND_RUNNING);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "BOW_LAND";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::BOW_LAND);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "LEDGE_LAND";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::LEDGE_LAND);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "SLIDE_LAND";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::SLIDE_LAND);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIR_ATTACK_1";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIR_ATTACK_LIGHT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIR_ATTACK_2";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIR_ATTACK_LIGHT_ALT);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIR_ATTACK_SLAM_INTO";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIR_ATTACK_SLAM_INTO);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIR_ATTACK_SLAM_LOOP";
	pAnimState->m_bLoop = true;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIR_ATTACK_SLAM_LOOP);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "AIR_ATTACK_SLAM_FINISH";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::AIR_ATTACK_SLAM_FINISH);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "FALL";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::FALL);
	m_pAnimation->Add_State(pAnimState);

	pAnimState = new CAnimState;
	pAnimState->m_strStateName = "FALL_INTO_RUN";
	pAnimState->m_bLoop = false;
	pAnimState->m_fLerpDuration = 0.2f;
	pAnimState->m_pMainAnim = m_pModelCom->Find_Animation((_uint)CKena_State::FALL_INTO_RUN);
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

void CKena::TurnOnAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnAttack);
		return;
	}

	m_bAttack = true;
}

void CKena::TurnOffAttack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffAttack);
		return;
	}

	m_bAttack = false;
}

void CKena::TurnOnCharge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOnCharge);
		return;
	}

	m_bChargeLight = true;
}

void CKena::TurnOffCharge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CKena::TurnOffCharge);
		return;
	}

	m_bChargeLight = false;
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

	for (auto& Pair : m_mapEffect)
		Safe_Release(Pair.second);
	m_mapEffect.clear();

	Safe_Release(m_pAnimation);
	Safe_Release(m_pStateMachine);
	Safe_Release(m_pKenaState);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

_int CKena::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	/* Terrain */
	if (m_bJump)
	{
		if (pTarget == nullptr || iColliderIndex == COLLISON_DUMMY || iColliderIndex == COL_GROUND || iColliderIndex == COL_ENVIROMENT)
		{
			m_bOnGround = true;
			m_bJump = false;
			m_bPulseJump = false;
			m_fCurJumpSpeed = 0.f;
		}
	}
	else
	{
		if (pTarget == nullptr || iColliderIndex == COLLISON_DUMMY) return 0;

		if (iColliderIndex == COL_MONSTER_WEAPON)
		{
			m_bCommonHit = true;
			//m_bHeavyHit = true;
		}
	}
	
	return 0;
}
void CKena::Test_Raycast()
{
	if (m_pTerrain == nullptr)
		return;

	if (GetKeyState('T') & 0x8000)
	{
		if (m_pRopeRotRock)
		{
			m_pRopeRotRock->Set_ChoiceFlag(true);
		}
	}

	if (GetKeyState(VK_LSHIFT) & 0x0800)
	{
		CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
		CGameInstance* pGameInst = CGameInstance::GetInstance();

		_vector vCamPos = pGameInst->Get_CamPosition();
		_vector vCamLook = pGameInst->Get_CamLook_Float4();
		_float3 vOut;

		if (pPhysX->Raycast_Collision(vCamPos, vCamLook, 10.f, &vOut))
		{
			m_pTerrain->Set_BrushPosition(vOut);
			
			if (GetKeyState('R') & 0x8000)
			{	
				if (m_pRopeRotRock && m_pRopeRotRock->Get_MoveFlag() == false)
				{
					m_pRopeRotRock->Set_MoveFlag(true);
					m_pRopeRotRock->Set_MoveTargetPosition(vOut);
				}			
			}			
		}
	}
	else
	{
		m_pTerrain->Set_BrushPosition(_float3(-1000.f, 0.f , 0.f));
	}
}
