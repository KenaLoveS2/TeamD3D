#include "stdafx.h"
#include "..\public\BossHunter.h"

#include "CameraForNpc.h"
#include "CinematicCamera.h"
#include "GameInstance.h"
#include "Effect_Base_S2.h"
#include "SpiritArrow.h"
#include "E_RectTrail.h"
#include "ControlRoom.h"
#include "E_HunterTrail.h"
#include "E_Swipes_Charged.h"
#include "Rot.h"
#include "Light.h"

CBossHunter::CBossHunter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
	, m_fStringDissolve(0.f)
	, m_fStringDissolveSpeed(0.f)
	, m_fStringHDRIntensity(0.f)
	, m_vStringDiffuseColor(1.f, 1.f, 1.f, 1.f)
	, m_pSelectedEffect(nullptr)
{
	for (_uint i = 0; i < 2; ++i)
		m_fUVSpeeds[i] = 0.f;
}

CBossHunter::CBossHunter(const CBossHunter& rhs)
	: CMonster(rhs)
	, m_fStringDissolve(0.f)
	, m_fStringDissolveSpeed(0.f)
	, m_fStringHDRIntensity(0.f)
	, m_vStringDiffuseColor(1.f, 1.f, 1.f, 1.f)
	, m_pSelectedEffect(nullptr)
{
	for (_uint i = 0; i < 2; ++i)
		m_fUVSpeeds[i] = 0.f;
}

HRESULT CBossHunter::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CBossHunter::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 6.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	// FAILED_CHECK_RETURN(__super::Ready_EnemyWisp(CUtile::Create_DummyString()), E_FAIL);
	// FAILED_CHECK_RETURN(SetUp_UI(), E_FAIL);

	ZeroMemory(&m_Desc, sizeof(CMonster::DESC));

	if (pArg != nullptr)
	{
		memcpy(&m_Desc, pArg, sizeof(CMonster::DESC));
	}
	else
	{
		m_Desc.iRoomIndex = 0;
		m_Desc.WorldMatrix = _smatrix();
		m_Desc.WorldMatrix._41 = -12.f;
		m_Desc.WorldMatrix._42 = m_fFlyHeightY;
		m_Desc.WorldMatrix._43 = 12.f;
	}

	m_pModelCom->Set_AllAnimCommonType();
	m_pModelCom->Init_AnimIndex(IDLE);
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pBodyBone = m_pModelCom->Get_BonePtr("char_spine_mid_jnt");
	m_pWeaponTrailBone = m_pModelCom->Get_BonePtr("Knife_EndJnt");

	Create_Arrow();
	/********************************************/
	/*			For. Shader & Effect			*/
	/********************************************/
	if (FAILED(Create_Effects()))
	{
		MSG_BOX("Failed To Create Effects : CBossHunter");
		return E_FAIL;
	}
	/* For. String */
	m_fStringDissolveSpeed = 10.f;
	m_fStringHDRIntensity = 5.0f;
	m_vStringDiffuseColor = _float4(1.0f, 0.05f, 0.46f, 1.f) ;

	FAILED_CHECK_RETURN(Create_Trail(), E_FAIL);

	if (g_LEVEL == LEVEL_TESTPLAY)
	{
		CGameObject* p_game_object = nullptr;
		CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL, L"Layer_Rot", L"Prototype_GameObject_Rot", L"Hunter_Rot", nullptr, &p_game_object);
		m_pRot = dynamic_cast<CRot*>(p_game_object);
	}

	return S_OK;
}

HRESULT CBossHunter::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);
	{
		_float3 vPivotScale = _float3(0.5f, 0.5f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.5f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = TEXT_COL_HUNTER_BODY;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fLinearDamping = MONSTER_LINEAR_DAMING;
		PxCapsuleDesc.fAngularDamping = MONSTER_ANGULAR_DAMING;
		PxCapsuleDesc.fMass = MONSTER_MASS;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_BODY;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER));
		m_pTransformCom->Add_Collider(TEXT_COL_HUNTER_BODY, g_IdentityFloat4x4);
	}

	{
		_float3 vPivotScale = _float3(0.4f, 0.4f, 1.f);
		_float3 vPivotPos = _float3(0.f, 0.5f, 0.f);

		// Capsule X == radius , Y == halfHeight
		CPhysX_Manager::PX_CAPSULE_DESC PxCapsuleDesc;
		PxCapsuleDesc.eType = CAPSULE_DYNAMIC;
		PxCapsuleDesc.pActortag = TEXT_COL_HUNTER_WEAPON;
		PxCapsuleDesc.vPos = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fRadius = vPivotScale.x;
		PxCapsuleDesc.fHalfHeight = vPivotScale.y;
		PxCapsuleDesc.vVelocity = _float3(0.f, 0.f, 0.f);
		PxCapsuleDesc.fDensity = 1.f;
		PxCapsuleDesc.fLinearDamping = MONSTER_LINEAR_DAMING;
		PxCapsuleDesc.fAngularDamping = MONSTER_ANGULAR_DAMING;
		PxCapsuleDesc.fMass = MONSTER_MASS;
		PxCapsuleDesc.fDynamicFriction = 0.5f;
		PxCapsuleDesc.fStaticFriction = 0.5f;
		PxCapsuleDesc.fRestitution = 0.1f;
		PxCapsuleDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		CPhysX_Manager::GetInstance()->Create_Capsule(PxCapsuleDesc, Create_PxUserData(this, false, COL_MONSTER_WEAPON));
		m_pTransformCom->Add_Collider(TEXT_COL_HUNTER_WEAPON, g_IdentityFloat4x4);
	}

	m_pCineCam[0] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("BOSSHUNTER_START")));
	m_pCineCam[1] = dynamic_cast<CCinematicCamera*>(CGameInstance::GetInstance()->Find_Camera(TEXT("BOSSHUNTER_END")));

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);
	m_pHunterTrail->Late_Initialize(nullptr);

	
	m_vecEffects[EFFECT_BOWTRAIL1]->Activate(this, "Bow_TopJnt9");
	m_vecEffects[EFFECT_BOWTRAIL2]->Activate(this, "Bow_BotJoint9");

	return S_OK;
}

void CBossHunter::Tick(_float fTimeDelta)
{	

	m_bDissolve = false;
	m_pModelCom->Play_Animation(fTimeDelta);
	Update_Collider(fTimeDelta);
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	//if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	if (m_pHunterTrail) m_pHunterTrail->Tick(fTimeDelta);
	if (m_pHunterTrail->Get_Active() == true) Update_Trail(nullptr);

	 /* For. String */
	m_fUVSpeeds[0] += 0.245f * fTimeDelta;
	m_fUVSpeeds[0] = fmodf(m_fUVSpeeds[0], 1);
	m_fStringDissolve += m_fStringDissolveSpeed * fTimeDelta;
	if (m_fStringDissolve > 0.5)
	{
		m_fStringDissolve = 0.5f;
		m_fStringDissolveSpeed *= -1;
		m_fStringDissolveSpeed = -0.9f;
	}
	else if (m_fStringDissolve < 0.f)
	{
		m_fStringDissolve = 0.f;
		m_fStringDissolveSpeed *= -1;
		m_fStringDissolveSpeed = 0.9f;
	}
	/* ~ For. String */
	for (auto& pArrow : m_pArrows)
		pArrow->Tick(fTimeDelta);
	for (auto& pEffect : m_vecEffects)
		pEffect->Tick(fTimeDelta);
	return;

	if (m_bDeath) return;

	__super::Tick(fTimeDelta);

	Update_Collider(fTimeDelta);
	if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	if (m_pHunterTrail) m_pHunterTrail->Tick(fTimeDelta);
	if (m_pHunterTrail->Get_Active() == true) Update_Trail(nullptr);

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);

	/* For. String */
	m_fUVSpeeds[0] += 0.245f * fTimeDelta;
	m_fUVSpeeds[0] = fmodf(m_fUVSpeeds[0], 1);

	m_fStringDissolve += m_fStringDissolveSpeed * fTimeDelta;
	if (m_fStringDissolve > 0.5)
	{
		m_fStringDissolve = 0.5f;
		m_fStringDissolveSpeed *= -1;
		m_fStringDissolveSpeed = -0.9f;

	}
	else if (m_fStringDissolve < 0.f)
	{
		m_fStringDissolve = 0.f;
		m_fStringDissolveSpeed *= -1;
		m_fStringDissolveSpeed = 0.9f;

	}
	/* ~ For. String */
	for (auto& pArrow : m_pArrows)
		pArrow->Tick(fTimeDelta);

	for (auto& pEffect : m_vecEffects)
		pEffect->Tick(fTimeDelta);

	if (ImGui::Button("DamageHunter"))
		m_pMonsterStatusCom->UnderAttack(50);
		
}

void CBossHunter::Late_Tick(_float fTimeDelta)
{
	if (m_bDeath) return;

	CMonster::Late_Tick(fTimeDelta);
	if (m_pRendererCom /*&& m_bSpawn*/)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	for (auto& pArrow : m_pArrows)
		pArrow->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_vecEffects)
		pEffect->Late_Tick(fTimeDelta);

	if (m_pHunterTrail) m_pHunterTrail->Late_Tick(fTimeDelta);
}

HRESULT CBossHunter::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (i == 0 && !m_bDissolve) // ArrowString
		{
			// ArrowString
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", HUNTER_ARROW), E_FAIL);
		}
		else if (i == 3) //Hair
		{
			// Hair 
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVEMASK, "g_EmissiveMaskTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", HUNTER_HAIR), E_FAIL);
		}
		else if (i == 4) // Knife UV01
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 2, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
		}
		else if (i == 1) // face and body 
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", HUNTER_HAIR), E_FAIL);
		}
		else if(i != 0)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
		}
	}
	return S_OK;
}

HRESULT CBossHunter::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", SHADOW);

	return S_OK;
}

void CBossHunter::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();

	float fTrans[3] = { m_vBodyPivotTrans.x, m_vBodyPivotTrans.y, m_vBodyPivotTrans.z };
	ImGui::DragFloat3("Weapon Trans", fTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vBodyPivotTrans, fTrans, sizeof(_float3));

	float fRot[3] = { m_vBodyPivotRot.x, m_vBodyPivotRot.y, m_vBodyPivotRot.z };
	ImGui::DragFloat3("Weapon Rot", fRot, 0.01f, -100.f, 100.0f);
	memcpy(&m_vBodyPivotRot, fRot, sizeof(_float3));

	ImGui::Separator();
	/* HDR Intensity (Diffuse) */
	static _float fAlpha = 1.f;
	fAlpha = m_fStringHDRIntensity;
	if (ImGui::DragFloat("HDR Intensity", &fAlpha, 0.1f, 0.f, 50.f))
		m_fStringHDRIntensity = fAlpha;

	/* Diffuse Color */
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_vStringDiffuseColor;

	if (ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL))
		m_vStringDiffuseColor = vSelectColor;
	if (ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags))
		m_vStringDiffuseColor = vSelectColor;
}

void CBossHunter::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty();
	//m_pTransformCom->Imgui_RenderProperty_ForJH();
	//m_pArrows[m_iArrowIndex]->Get_TransformCom()->Imgui_RenderProperty_ForJH();

	if (ImGui::CollapsingHeader("Effect"))
		ImGui_EffectProperty();

	if (ImGui::CollapsingHeader("BossHunter"))
	{
		ImGui::BeginTabBar("BossHunter Animation & State");

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

void CBossHunter::ImGui_ShaderValueProperty()
{
	CMonster::ImGui_ShaderValueProperty();
}

void CBossHunter::ImGui_PhysXValueProperty()
{
	CMonster::ImGui_PhysXValueProperty();

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

HRESULT CBossHunter::Call_EventFunction(const string& strFuncName)
{
	return CMonster::Call_EventFunction(strFuncName);
}

void CBossHunter::Push_EventFunctions()
{
	LookAt_Knife(true, 0.f);
	LookAt_Arrow(true, 0.f);

	ReadyArrow_Single(true, 0.f);
	ReadyArrow_Charge(true, 0.f);
	ReadyArrow_Rapid(true, 0.f);
	ReadyArrow_Shock(true, 0.f);

	FireArrow_Single(true, 0.f);
	FireArrow_Charge(true, 0.f);
	FireArrow_Rapid(true, 0.f);
	FireArrow_Shock(true, 0.f);

	ShockEffect_On(true, 0.f);
	ShockEffect_Off(true, 0.f);
	DustEffect_On(true, 0.f);
	StunEffect_On(true, 0.f);
	StunEffect_Off(true, 0.f);
	BowTrailEffect_On(true, 0.f);
	BowTrailEffect_Off(true, 0.f);

	TurnOnTrail(true, 0.f);
	TUrnOffTrail(true, 0.f);

	// Sound CallBack
	Play_Attack1Sound(true, 0.f);
	Play_Attack2Sound(true, 0.f);
	Play_Attack3Sound(true, 0.f);
	Play_Attack4Sound(true, 0.f);
	Play_Attack5Sound(true, 0.f);
	Play_Attack6Sound(true, 0.f);
	Play_Attack7Sound(true, 0.f);
	Play_Attack8Sound(true, 0.f);
	Play_Attack9Sound(true, 0.f);
	Play_Attack10Sound(true, 0.f);
	Play_Attack11Sound(true, 0.f);
	Play_Attack12Sound(true, 0.f);
	Play_Attack13Sound(true, 0.f);

	Play_Hurt1Sound(true, 0.f);
	Play_Hurt2Sound(true, 0.f);
	Play_Hurt3Sound(true, 0.f);
	Play_Hurt4Sound(true, 0.f);
	Play_Hurt5Sound(true, 0.f);
	Play_Hurt6Sound(true, 0.f);
	Play_Hurt7Sound(true, 0.f);
	Play_Hurt8Sound(true, 0.f);
	Play_Hurt9Sound(true, 0.f);
	Play_Hurt10Sound(true, 0.f);
	Play_Hurt11Sound(true, 0.f);

	Play_Whoosh1Sound(true, 0.f);
	Play_Whoosh2Sound(true, 0.f);
	Play_Whoosh3Sound(true, 0.f);
	Play_Whoosh4Sound(true, 0.f);
	Play_Whoosh5Sound(true, 0.f);
	Play_Whoosh6Sound(true, 0.f);
	Play_Whoosh7Sound(true, 0.f);
	Play_Whoosh8Sound(true, 0.f);

	Play_Elemental1Sound(true, 0.f);
	Play_Elemental2Sound(true, 0.f);
	Play_Elemental3Sound(true, 0.f);
	Play_Elemental4Sound(true, 0.f);
	Play_Elemental5Sound(true, 0.f);
	Play_Elemental6Sound(true, 0.f);
	Play_Elemental7Sound(true, 0.f);
	Play_Elemental8Sound(true, 0.f);
	Play_Elemental9Sound(true, 0.f);
	Play_Elemental10Sound(true, 0.f);
	Play_Elemental11Sound(true, 0.f);
	Play_Elemental12Sound(true, 0.f);
	Play_Elemental13Sound(true, 0.f);
	Play_Elemental14Sound(true, 0.f);
	Play_Elemental15Sound(true, 0.f);

	Play_Impact1Sound(true, 0.f);
	Play_Impact2Sound(true, 0.f);
	Play_Impact3Sound(true, 0.f);
	Play_Impact4Sound(true, 0.f);
	Play_Impact5Sound(true, 0.f);

	Play_Knife1Sound(true, 0.f);
	Play_Knife2Sound(true, 0.f);
	Play_Knife3Sound(true, 0.f);	

	Play_ArrowChargeSound(true, 0.f);

	Stop_ShockArrowUpY(true, 0.f);
}

_float4 CBossHunter::Get_ComputeBonePosition(const char* pBoneName)
{
	_float4 vPos;
	CBone* pBone = m_pModelCom->Get_BonePtr(pBoneName);
	if (pBone != nullptr)
	{
		_matrix SocketMatrix = pBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();
		vPos = matWorldSocket.r[3];
	}
	else
		vPos = m_pTransformCom->Get_Position();

	return vPos;
}

HRESULT CBossHunter::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
	.InitState("SLEEP")
	.AddState("SLEEP")
	.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fFlyHeightY);
		m_bDissolve = true;
		m_fDissolveTime = 1.f;
	})
	.OnExit([this]()
	{	
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_bReadySpawn = true;
	})
	.AddTransition("SLEEP to CINEMA", "CINEMA")
	.Predicator([this]()
	{
		m_fSpawnRange = 20.f;
		return DistanceTrigger(m_fSpawnRange);
	})

	.AddState("CINEMA")
	.OnStart([this]()
	{
		BossFight_Start();
	})
	.AddTransition("CINEMA to READY_SPAWN", "READY_SPAWN")
	.Predicator([this]()
	{
		return m_pCineCam[0]->CameraFinishedChecker(0.3f);
	})

	.AddState("READY_SPAWN")
	.OnStart([this]()
	{
		m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fFlyHeightY);
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_pModelCom->ResetAnimIdx_PlayTime(RAMPAGE);
		m_pModelCom->Set_AnimIndex(RAMPAGE);

		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = 30.f; /* == BossHunter Name */
		m_BossHunterDelegator.broadcast(eBossHP, fValue);
	})
	.Tick([this](_float fTimeDelta)
	{
		m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fFlyHeightY);

		m_fDissolveTime -= fTimeDelta;
		if (m_fDissolveTime < -0.5f)
			m_bDissolve = false;

		m_fFogRange -= fTimeDelta * 50.f;
		if (m_fFogRange < 60.f)
			m_fFogRange = 60.f;
		const _float4 vColor = _float4(130.f / 255.f, 144.f / 255.f, 196.f / 255.f, 1.f);
		m_pRendererCom->Set_FogValue(vColor, m_fFogRange);
		
		m_fLightRange += fTimeDelta * 50.f;
		if (m_fLightRange > 90.f)
			m_fLightRange = 90.f;
		CGameInstance::GetInstance()->Get_Light(1)->Set_Range(m_fLightRange);

		if(AnimIntervalChecker(RAMPAGE,0.9f,1.f))
			m_pModelCom->FixedAnimIdx_PlayTime(RAMPAGE, 0.95f);
	})
	.OnExit([this]()
	{
		m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
		m_bSpawn = true;
		CGameInstance::GetInstance()->Work_Camera(L"PLAYER_CAM");
		m_pCineCam[0]->CinemaUIOff();
		m_pKena->Set_StateLock(false);
	})
	.AddTransition("READY_SPAWN to IDLE", "IDLE")
	.Predicator([this]()
	{
		return m_pCineCam[0]->CameraFinishedChecker() && m_bDissolve == false;
	})

	.AddState("IDLE")
	.OnStart([this]()
	{
	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	m_pTransformCom->Speed_Boost(true, 0.7f);

	m_pModelCom->ResetAnimIdx_PlayTime(IDLE);
	m_pModelCom->Set_AnimIndex(IDLE);

	m_fIdleTimeCheck = m_bDodge ? m_fIdleTimeCheck : 0.f;
	m_bDodge = false;
	})
	.Tick([this](_float fTimeDelta)
	{
	m_fIdleTimeCheck += fTimeDelta;

	if (m_pTransformCom->Get_PositionY() <= m_vKenaPos.y + m_fFlyHeightY)
	{
		m_pTransformCom->Go_AxisY(fTimeDelta);
		if (m_pTransformCom->Get_PositionY() >= m_vKenaPos.y + m_fFlyHeightY)
			m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fFlyHeightY);
	}

	m_bDodge = m_pKena->Get_State(CKena::STATE_BOW) && (rand() % 5 == 0);
	})
	.OnExit([this]()
	{
	Reset_HitFlag();
	m_pTransformCom->Speed_Boost(true, 1.f);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("IDLE To DODGE", "DODGE")
	.Predicator([this]()
	{
	return !TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_bDodge;
	})
	.AddTransition("IDLE To START_ATTACK_KNIFE", "START_ATTACK_KNIFE")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_KNIFE;
	})
	.AddTransition("IDLE To START_ATTACK_KNIFE_SLAM", "START_ATTACK_KNIFE_SLAM")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_KNIFE_SLAM;
	})
	.AddTransition("IDLE To START_SWOOP_ATTACK", "START_SWOOP_ATTACK")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_SWOOP_ATTACK;
	})


	.AddTransition("IDLE To SINGLE_SHOT", "SINGLE_SHOT")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_SINGLE_SHOT;
	})
	.AddTransition("IDLE To RAPID_SHOOT", "RAPID_SHOOT")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_RAPID_SHOOT;
	})
	.AddTransition("IDLE To START_SHOCK_ARROW", "START_SHOCK_ARROW")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_SHOCK_ARROW;
	})
	.AddTransition("IDLE To START_CHARGE_ARROW", "START_CHARGE_ARROW")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_CHARGE_ARROW;
	})
	.AddTransition("IDLE To START_CHARGE_ARROW", "FLY")
	.Predicator([this]()
	{
	return TimeTrigger(m_fIdleTimeCheck, m_fIdleTime) && m_eAttackType == AT_FLY;
	})

	.AddState("DODGE")
	.OnStart([this]()
	{
	m_iDodgeAnimIndex++;
	m_iDodgeAnimIndex = m_iDodgeAnimIndex > DODGE_FAR_RIGHT ? DODGE_DOWN : m_iDodgeAnimIndex;
	ResetAndSet_Animation(m_iDodgeAnimIndex);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("DODGE To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(m_iDodgeAnimIndex);
	})


	.AddState("START_ATTACK_KNIFE")
	.OnStart([this]()
	{
	Set_NextAttack();
	m_pTransformCom->Speed_Boost(true, 0.5f);
	})
	.Tick([this](_float fTimeDelta)
	{
	m_pTransformCom->Go_AxisNegY(fTimeDelta);
	})
	.OnExit([this]()
	{
	m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fKenaPosOffsetY);
	m_pTransformCom->Speed_Boost(true, 1.f);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("START_ATTACK_KNIFE To ATTACK_KNIFE", "ATTACK_KNIFE")
	.Predicator([this]()
	{
	return m_pTransformCom->Get_PositionY() <= m_vKenaPos.y + m_fKenaPosOffsetY;
	})


	.AddState("ATTACK_KNIFE")
	.OnStart([this]()
	{
	m_bRealAttack = true;
	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	ResetAndSet_Animation(KNIFE_ATTACK);
	})
	.OnExit([this]()
	{
	m_bRealAttack = false;
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("ATTACK_KNIFE To KNIFE_PARRY", "KNIFE_PARRY")
	.Predicator([this]()
	{
	return m_bStronglyHit;
	})
	.AddTransition("ATTACK_KNIFE_SLAM To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(KNIFE_ATTACK);
	})


	.AddState("START_ATTACK_KNIFE_SLAM")
	.OnStart([this]()
	{
	ResetAndSet_Animation(KNIFE_SLAM_TELEPORT);
	Set_NextAttack();
	})
	.OnExit([this]()
	{

	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("START_ATTACK_SWOOP To ATTACK_KNIFE_SLAM", "ATTACK_KNIFE_SLAM")
	.Predicator([this]()
	{
	return AnimFinishChecker(KNIFE_SLAM_TELEPORT);
	})


	.AddState("ATTACK_KNIFE_SLAM")
	.OnStart([this]()
	{
	_float fX = CUtile::Get_RandomFloat(-2.f, 2.f);
	_float fZ = CUtile::Get_RandomFloat(-2.f, 2.f);
	fX = fX < 0.f ? fX + 2.f : fX - 2.f;
	fZ = fZ < 0.f ? fZ + 2.f : fZ - 2.f;

	_float4 vTeleportPos = m_vKenaPos + _float4(fX, m_fKenaPosOffsetY, fZ, 0.f);
	m_pTransformCom->Set_Position(vTeleportPos);
	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	ResetAndSet_Animation(KNIFE_SLAM_ATTACK);
	m_bRealAttack = true;
	})
	.OnExit([this]()
	{
	m_bRealAttack = false;
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("ATTACK_KNIFE_SLAM To KNIFE_PARRY", "KNIFE_PARRY")
	.Predicator([this]()
	{
	return m_bStronglyHit;
	})
	.AddTransition("ATTACK_KNIFE_SLAM To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(KNIFE_SLAM_ATTACK);
	})


	.AddState("KNIFE_PARRY")
	.OnStart([this]()
	{
	m_bStronglyHit = false;
	ResetAndSet_Animation(KNIFE_PARRY);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("KNIFE_PARRY To KNIFE_PARRY_STUN_LOOP", "KNIFE_PARRY_STUN_LOOP")
	.Predicator([this]()
	{
	return AnimFinishChecker(KNIFE_PARRY);
	})


	.AddState("KNIFE_PARRY_STUN_LOOP")
	.OnStart([this]()
	{
	m_bStronglyHit = false;
	// ResetAndSet_Animation(KNIFE_PARRY_STUN_LOOP);
	m_fStunTimeCheck = 0.f;
	})
	.Tick([this](_float fTimeDelta)
	{
	m_fStunTimeCheck += fTimeDelta;
	})
	.OnExit([this]()
	{
	m_fStunTimeCheck = 0.f;
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("KNIFE_PARRY_STUN_LOOP To KNIFE_PARRY_EXIT", "KNIFE_PARRY_EXIT")
	.Predicator([this]()
	{
	return TimeTrigger(m_fStunTimeCheck, m_fStunTime);
	})

	.AddState("KNIFE_PARRY_EXIT")
	.OnStart([this]()
	{
	m_bStronglyHit = false;
	ResetAndSet_Animation(KNIFE_PARRY_EXIT);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("KNIFE_PARRY_EXIT To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(KNIFE_PARRY_EXIT);
	})


	.AddState("START_SWOOP_ATTACK")
	.OnStart([this]()
	{
	m_pTransformCom->LookAt(m_vKenaPos);
	m_pTransformCom->Speed_Boost(true, 0.5f);
	Set_NextAttack();
	})
	.Tick([this](_float fTimeDelta)
	{
	m_pTransformCom->Go_AxisNegY(fTimeDelta);
	})
	.OnExit([this]()
	{
	m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fKenaPosOffsetY);
	m_pTransformCom->Speed_Boost(true, 1.f);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("START_SWOOP_ATTACK To SWOOP_ATTACK", "SWOOP_ATTACK")
	.Predicator([this]()
	{
	return m_pTransformCom->Get_PositionY() <= m_vKenaPos.y + m_fKenaPosOffsetY;
	})

	.AddState("SWOOP_ATTACK")
	.OnStart([this]()
	{
	m_bRealAttack = true;
	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
	ResetAndSet_Animation(SWOOP_ATTACK);
	})
	.OnExit([this]()
	{
	m_bRealAttack = false;
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("SWOOP_ATTACK To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(SWOOP_ATTACK);
	})


	.AddState("SINGLE_SHOT")
	.OnStart([this]()
	{
	ResetAndSet_Animation(SINGLE_SHOT); 
	m_pTransformCom->LookAt(m_vKenaPos);
	Set_NextAttack();
	})
	.Tick([this](_float fTimeDelta)
	{
	m_pTransformCom->LookAt(m_vKenaPos);
	})
	.OnExit([this]()
	{

	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("KNIFE_PARRY_EXIT To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(SINGLE_SHOT);
	})


	.AddState("RAPID_SHOOT")
	.OnStart([this]()
	{
	ResetAndSet_Animation(RAPID_SHOOT); 
	m_pTransformCom->LookAt(m_vKenaPos);
	Set_NextAttack();
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("KNIFE_PARRY_EXIT To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(RAPID_SHOOT);
	})



	.AddState("START_SHOCK_ARROW")
	.OnStart([this]()
	{
	ResetAndSet_Animation(SHOCK_ARROW_ATTACK);
	Set_NextAttack();
	m_bShockArrowUpY = true;
	})
		.Tick([this](_float fTimeDelta)
	{
		m_bShockArrowUpY ? m_pTransformCom->Go_AxisY(fTimeDelta) : 0;
	})
	.OnExit([this]()
	{
	if (m_bReadyStrongArrow)
	{
	m_pArrows[m_iArrowIndex]->Execute_Finish();
	m_bReadyStrongArrow = false;
	Update_ArrowIndex();
	}
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("START_SHOCK_ARROW To START_STUN", "START_STUN")
	.Predicator([this]()
	{
	return m_bStronglyHit && m_bReadyStrongArrow;
	})
	.AddTransition("START_SHOCK_ARROW To SHOCK_ARROW_LOOP", "SHOCK_ARROW_LOOP")
	.Predicator([this]()
	{
	return AnimFinishChecker(SHOCK_ARROW_ATTACK);
	})


	.AddState("SHOCK_ARROW_LOOP")
	.OnStart([this]()
	{
		ResetAndSet_Animation(SHOCK_ARROW_LOOP);
		m_bShockArrowDownY = false;
	})
		.Tick([this](_float fTimeDelta)
	{
		if (m_pTransformCom->Get_PositionY() > m_vKenaPos.y + m_fFlyHeightY)
		{
			m_pTransformCom->Go_AxisNegY(fTimeDelta);
			if (m_pTransformCom->Get_PositionY() < m_vKenaPos.y + m_fFlyHeightY)
			{
				m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fFlyHeightY);
				m_bShockArrowDownY = true;
			}				
		}
	})

	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
		return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("SHOCK_ARROW_LOOP To SHOCK_ARROW_EXIT", "SHOCK_ARROW_EXIT")
	.Predicator([this]()
	{
		return m_pArrows[m_iArrowIndex]->IsEnd() && m_bShockArrowDownY;
	})


	.AddState("SHOCK_ARROW_EXIT")
	.OnStart([this]()
	{
	ResetAndSet_Animation(SHOCK_ARROW_EXIT);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("SHOCK_ARROW_EXIT To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(SHOCK_ARROW_EXIT);
	})



	.AddState("START_CHARGE_ARROW")
	.OnStart([this]()
	{
	Reset_HitFlag();
	ResetAndSet_Animation(CHARGE_ARROW_FIRST_SHOT);
	m_pTransformCom->LookAt(m_vKenaPos);
	Set_NextAttack();
	})
	.OnExit([this]()
	{
	if (m_bReadyStrongArrow)
	{
	m_pArrows[m_iArrowIndex]->Execute_Finish();
	m_bReadyStrongArrow = false;
	Update_ArrowIndex();
	}
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("START_CHARGE_ARROW To START_STUN", "START_STUN")
	.Predicator([this]()
	{
	return m_bStronglyHit && m_bReadyStrongArrow;
	})
	.AddTransition("START_CHARGE_ARROW To CHARGE_ARROW_EXIT", "CHARGE_ARROW_EXIT")
	.Predicator([this]()
	{
	return AnimFinishChecker(CHARGE_ARROW_FIRST_SHOT);
	})


	.AddState("CHARGE_ARROW_EXIT")
	.OnStart([this]()
	{
	ResetAndSet_Animation(CHARGE_ARROW_EXIT);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("CHARGE_ARROW_EXIT To IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(CHARGE_ARROW_EXIT);
	})


	.AddState("START_STUN")
	.OnStart([this]()
	{
	ShockEffect_Off(false, 0.f);
	Reset_HitFlag();
	ResetAndSet_Animation(STUN_INTO_FALL);
	m_pTransformCom->Speed_Boost(true, 0.5f);
	})
	.Tick([this](_float fTimeDelta)
	{
	m_pTransformCom->Go_AxisNegY(fTimeDelta);
	})
	.OnExit([this]()
	{
	m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fKenaPosOffsetY);
	m_pTransformCom->Speed_Boost(true, 1.f);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("START_STUN to STUN_INTO", "STUN_INTO")
	.Predicator([this]()
	{
	return m_pTransformCom->Get_PositionY() <= m_vKenaPos.y + m_fKenaPosOffsetY;
	})


	.AddState("STUN_INTO")
	.OnStart([this]()
	{
	ResetAndSet_Animation(STUN_INTO);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("STUN_INTO to STUN_LOOP", "STUN_LOOP")
	.Predicator([this]()
	{
	return AnimFinishChecker(STUN_INTO);
	})

	.AddState("STUN_LOOP")
	.OnStart([this]()
	{
	ResetAndSet_Animation(STUN_LOOP);
	m_fStunTimeCheck = 0.f;
	})
	.Tick([this](_float fTimeDelta)
	{
	m_fStunTimeCheck += fTimeDelta;
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("STUN_LOOP to STUN_EXIT", "STUN_EXIT")
	.Predicator([this]()
	{
	return TimeTrigger(m_fStunTimeCheck, m_fStunTime);
	})


	.AddState("STUN_EXIT")
	.OnStart([this]()
	{
	ResetAndSet_Animation(STUN_EXIT);
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("STUN_EXIT to IDLE", "IDLE")
	.Predicator([this]()
	{
	return AnimFinishChecker(STUN_EXIT);
	})


	.AddState("FLY")
	.OnStart([this]()
	{
	m_pTransformCom->Speed_Boost(true, 1.5f);
	ResetAndSet_Animation(FLY_FORWRAD_FAST);

	_float4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fX = vCurPos.x - m_vKenaPos.x, fZ = vCurPos.z - m_vKenaPos.z;

	// x' = (x-a) * cosR - (y-b)sinR + a
	// y' = (x-a) * sinR + (y-b)cosR + b

	_float fDegree = (rand() % 2) == 0 ? -45.f : 45.f;
	for (_uint i = 0; i < FLY_POS_COUNT; i++)
	{
	_uint index = i + 1;
	m_vFlyTargetPos[i].x = fX * cosf(XMConvertToRadians(fDegree * index)) - fZ * sinf(XMConvertToRadians(fDegree * index)) + m_vKenaPos.x;
	m_vFlyTargetPos[i].y = vCurPos.y + (i != (FLY_POS_COUNT - 1)) * CUtile::Get_RandomFloat(-1.5f, 3.f);
	m_vFlyTargetPos[i].z = fX * sinf(XMConvertToRadians(fDegree * index)) + fZ * cosf(XMConvertToRadians(fDegree * index)) + m_vKenaPos.z;
	m_vFlyTargetPos[i].w = 1.f;
	}
	Set_NextAttack();

	})
	.Tick([this](_float fTimeDelta)
	{
	m_pTransformCom->Chase(m_vFlyTargetPos[m_vFlyTargetIndex], fTimeDelta);
	if (m_pTransformCom->IsClosed_XZ(m_vFlyTargetPos[m_vFlyTargetIndex], 1.f))
	{
	m_vFlyTargetIndex++;
	m_bFlyEnd = (m_vFlyTargetIndex == FLY_POS_COUNT);
	m_vFlyTargetIndex %= FLY_POS_COUNT;
	}
	})
	.OnExit([this]()
	{
	m_pTransformCom->Speed_Boost(true, 1.f);
	m_vFlyTargetIndex = 0;
	m_bFlyEnd = false;
	})
	.AddTransition("To DYING", "DYING_DOWN")
	.Predicator([this]()
	{
	return m_pMonsterStatusCom->IsDead();
	})
	.AddTransition("FLY to IDLE", "IDLE")
	.Predicator([this]()
	{
	return m_bFlyEnd;
	})


	.AddState("DYING_DOWN")
	.OnStart([this]()
	{
	m_pModelCom->Set_AnimIndex(IDLE);
	})
	.Tick([this](_float fTimeDelta)
	{
		if (m_pTransformCom->Get_PositionY() >= m_vKenaPos.y + m_fKenaPosOffsetY)
		{
			m_pTransformCom->Go_AxisNegY(fTimeDelta);
			if (m_pTransformCom->Get_PositionY() < m_vKenaPos.y + m_fKenaPosOffsetY)
				m_pTransformCom->Set_PositionY(m_vKenaPos.y + m_fKenaPosOffsetY);
		}
	})
	.AddTransition("DYING_DOWN to DYING", "DYING")
	.Predicator([this]()
	{
	return m_pTransformCom->Get_PositionY() <= m_vKenaPos.y + m_fKenaPosOffsetY;
	})

	.AddState("DYING")
	.OnStart([this]()
	{
		CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
		_float fValue = -1.f;
		m_BossHunterDelegator.broadcast(eBossHP, fValue);
		
		m_pModelCom->ResetAnimIdx_PlayTime(DEATH);
		m_pModelCom->Set_AnimIndex(DEATH);

		m_pKena->Dead_FocusRotIcon(this);

		m_bDying = true;
		BossFight_End();
	})
	.AddTransition("DYING to DEATH_SCENE", "DEATH_SCENE")
	.Predicator([this]()
	{
	return m_pModelCom->Get_AnimationFinish();
	})

	.AddState("DEATH_SCENE")
	.OnStart([this]()
	{
		m_bDissolve = true;
		m_fEndTime = 0.f;
	})
	.Tick([this](_float fTimeDelta)
	{
		m_fEndTime += fTimeDelta;
		m_fDissolveTime = m_fEndTime * 0.1f;
		if (m_fDissolveTime >= 1.f)
			m_bDissolve = false;

		m_fFogRange += fTimeDelta * 50.f;
		if (m_fFogRange >= 100.f)
			m_pRendererCom->Set_Fog(false);
		const _float4 vColor = _float4(130.f / 255.f, 144.f / 255.f, 196.f / 255.f, 1.f);
		m_pRendererCom->Set_FogValue(vColor, m_fFogRange);

		m_fLightRange -= fTimeDelta * 50.f;
		if (m_fLightRange < 0.f)
			CGameInstance::GetInstance()->Get_Light(1)->Set_Enable(false);

		CGameInstance::GetInstance()->Get_Light(1)->Set_Range(m_fLightRange);
	})
	.OnExit([this]()
	{
		CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));
		pCtrlRoom->DeadZoneObject_Change(true);
		pCtrlRoom->Boss_HunterDeadGimmick();

		// 시네캠 하나 만들자
		CGameInstance::GetInstance()->Work_Camera(m_pCineCam[1]->Get_ObjectCloneName());
		m_pCineCam[1]->Play();
	})
	.AddTransition("DEATH_SCENE to DEATH", "DEATH")
	.Predicator([this]()
	{
			return m_fEndTime >= 10.f;
	})

	.AddState("DEATH")
	.OnStart([this]()
	{
		g_bDayOrNight = true;
		m_pTransformCom->Clear_Actor();
		Clear_Death();
	})

	.Build();
	return S_OK;
}

HRESULT CBossHunter::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Boss_Hunter", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	// String
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_64.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_135.png")), E_FAIL);

	// Hair
	//FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_100.png")), E_FAIL);
	//FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/Ramp6.png")), E_FAIL);
	//FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(3, WJTextureType_EMISSIVEMASK, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_54.png")), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/VillageHunter_Uv01_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/VillageHunter_Uv01_EMISSIVE.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/VillageHunter_Uv02_AO_R_M.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(2, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/VillageHunter_Uv02_EMISSIVE.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(4, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/VillageHunter_Knife_EMISSIVE.png")), E_FAIL);
	m_pModelCom->Set_RootBone("VillageElder02");

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_MonsterStatus", L"Com_Status", (CComponent**)&m_pMonsterStatusCom, nullptr, this), E_FAIL);
	m_pMonsterStatusCom->Load("../Bin/Data/Status/Mon_BossHunter.json");

	return S_OK;
}

HRESULT CBossHunter::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_EmissiveColor", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4)), E_FAIL);
	float fHDRIntensity = 0.f;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fHDRIntensity", &fHDRIntensity, sizeof(_float)), E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;

	/* For. String */
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fStringHDR", &m_fStringHDRIntensity, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fUVSpeedX", &m_fUVSpeeds[0], sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fStringDissolve", &m_fStringDissolve, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vColor", &m_vStringDiffuseColor, sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CBossHunter::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

	return S_OK;
}

void CBossHunter::Update_Collider(_float fTimeDelta)
{
	{
		_matrix SocketMatrix = m_pBodyBone->Get_OffsetMatrix() * m_pBodyBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		XMStoreFloat4x4(&m_BodyPivotMatrix,
			XMMatrixRotationX(m_vBodyPivotRot.x) * XMMatrixRotationY(m_vBodyPivotRot.y) * XMMatrixRotationZ(m_vBodyPivotRot.z)
			* XMMatrixTranslation(m_vBodyPivotTrans.x, m_vBodyPivotTrans.y, m_vBodyPivotTrans.z));

		SocketMatrix = XMLoadFloat4x4(&m_BodyPivotMatrix) * SocketMatrix;

		_float4x4 mat;
		XMStoreFloat4x4(&mat, SocketMatrix);
		m_pTransformCom->Update_AllCollider(mat);
	}

	m_pTransformCom->Tick(fTimeDelta);
}

void CBossHunter::AdditiveAnim(_float fTimeDelta)
{
	CMonster::AdditiveAnim(fTimeDelta);
}

void CBossHunter::BossFight_Start()
{
	g_bDayOrNight = false;
	CGameInstance::GetInstance()->Work_Camera(m_pCineCam[0]->Get_ObjectCloneName());
	m_pCineCam[0]->Play();
	m_bDissolve = true;
	m_fDissolveTime = 1.f;
	m_pRendererCom->Set_Fog(true);
	const _float4 vColor = _float4(130.f / 255.f, 144.f / 255.f, 196.f / 255.f, 1.f);
	m_pRendererCom->Set_FogValue(vColor, 100.f);
	CGameInstance::GetInstance()->Get_Light(1)->Set_Enable(true);
}

void CBossHunter::BossFight_End()
{
	CGameInstance::GetInstance()->Work_Camera(TEXT("NPC_CAM"));
	const _float3 vOffset = _float3(0.f, 1.5f, 0.f);
	const _float3 vLookOffset = _float3(0.f, 0.3f, 0.f);
	dynamic_cast<CCameraForNpc*>(CGameInstance::GetInstance()->Find_Camera(TEXT("NPC_CAM")))->Set_Target(this, CCameraForNpc::OFFSET_FRONT_LERP, vOffset, vLookOffset,0.9f, 2.f);

	_float4 vRotPos = _float4(192.3f, 28.f, 490.f,1.f);
	if (m_pRot)
	{
		CPhysX_Manager::GetInstance()->Create_Trigger(Create_PxTriggerData(TEXT("Hunter_Rot"), this, TRIGGER_ROT, CUtile::Float_4to3(vRotPos), 1.f));
		m_pRot->Set_Position(vRotPos);
		m_pRot->Set_WispPos(vRotPos);
		m_pRot->Set_WakeUpPos(vRotPos);
		m_pRot->Get_TransformCom()->Set_Look(m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_LOOK) * -1.f);
	}
}

CBossHunter* CBossHunter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossHunter* pInstance = new CBossHunter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBossHunter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossHunter::Clone(void* pArg)
{
	CBossHunter* pInstance = new CBossHunter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBossHunter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHunter::Free()
{
	CMonster::Free();

	Safe_Release(m_pHunterTrail);

	for (auto& pEffect : m_vecEffects)
		Safe_Release(pEffect);

	for (auto& pArrow : m_pArrows)
		Safe_Release(pArrow);
}


_int CBossHunter::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget && m_bSpawn)
	{
		if ((iColliderIndex == (_int)COL_PLAYER_WEAPON && m_pKena->Get_State(CKena::STATE_ATTACK)))
		{
			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());

			CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
			_float fGauge = m_pMonsterStatusCom->Get_PercentHP();
			m_BossHunterDelegator.broadcast(eBossHP, fGauge);

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);

			if (m_pKena->Get_State(CKena::STATE_HEAVYATTACK) == false)
			{
				m_bWeaklyHit = true;
				m_bStronglyHit = false;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.15f);
				m_pKena->Add_HitStopTime(0.15f);
				m_fHitStopTime += 0.15f;
				CCamera_Player* pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
				if (pCamera != nullptr)
					pCamera->Camera_Shake(0.003f, 5);
			}
			else
			{
				m_bWeaklyHit = false;
				m_bStronglyHit = true;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.3f);
				m_pKena->Add_HitStopTime(0.25f);
				m_fHitStopTime += 0.25f;
				CCamera_Player* pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
				if (pCamera != nullptr)
					pCamera->Camera_Shake(0.005f, 5);

				vector<_float4>* vecWeaponPos = m_pKena->Get_WeaponPositions();
				if (vecWeaponPos->size() == 2)
				{
					_vector	vDir = vecWeaponPos->back() - vecWeaponPos->front();
					vDir = XMVectorSetZ(vDir, 0.f);
					pCamera->Camera_Shake(vDir, XMConvertToRadians(30.f));
				}
			}
		}

		if (iColliderIndex == (_int)COL_PLAYER_ARROW)
		{
			CSpiritArrow* pArrow = dynamic_cast<CSpiritArrow*>(pTarget);

			m_pKena->Get_KenaStatusPtr()->Plus_CurPIPGuage(KENA_PLUS_PIP_GUAGE_VALUE);
			//m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());
			m_pMonsterStatusCom->UnderAttack(pArrow->Get_Damage());

			CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
			_float fGauge = m_pMonsterStatusCom->Get_PercentHP();
			m_BossHunterDelegator.broadcast(eBossHP, fGauge);

			//m_bStronglyHit = m_pKena->Get_State(CKena::STATE_INJECTBOW);
			//m_bWeaklyHit = !m_bStronglyHit;
			
			if (pArrow->Get_CurrentState() == CSpiritArrow::ARROW_INJECT_FIRE)
			{
				m_bStronglyHit = true;
				m_bWeaklyHit = false;
			}
			else
			{
				m_bWeaklyHit = true;
				m_bStronglyHit = false;
			}

			m_pKenaHit->Set_Active(true);
			m_pKenaHit->Set_Position(vCollisionPos);
		}
	}

	return 0;
}

void CBossHunter::ResetAndSet_Animation(_uint iAnimIndex)
{
	m_pModelCom->ResetAnimIdx_PlayTime(iAnimIndex);
	m_pModelCom->Set_AnimIndex(iAnimIndex);
}

void CBossHunter::Set_NextAttack()
{
	_uint iTemp = m_eAttackType;
	iTemp++;
	iTemp %= ATTACKTYPE_END;
	m_eAttackType = (ATTACKTYPE)iTemp;
}

void CBossHunter::Create_Arrow()
{
	CMonsterWeapon::MONSTERWEAPONDESC ArrowDesc;
	ZeroMemory(&ArrowDesc, sizeof(CMonsterWeapon::MONSTERWEAPONDESC));
	XMStoreFloat4x4(&ArrowDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ArrowDesc.pSocket = m_pModelCom->Get_BonePtr("char_lf_hand_jnt"); // Bow_RootJnt // Bow_BotJoint1 // Bow_TopStringJnt
	ArrowDesc.pTargetTransform = m_pTransformCom;
	ArrowDesc.pOwnerMonster = this;

	for (_uint i = 0; i < ARROW_COUNT; i++)
	{
		m_pArrows[i] = (CHunterArrow*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_HunterArrow"), CUtile::Create_DummyString(TEXT("HunterArrow"), i), &ArrowDesc);
		assert(m_pArrows[i] && "Failed!! -> CBossHunter::Create_Arrow()");
		m_pArrows[i]->Late_Initialize(nullptr);
		m_pArrows[i]->Set_BowBonePtr(m_pModelCom->Get_BonePtr("Bow_RootJnt"));

		//Safe_AddRef(ArrowDesc.pSocket);
		//Safe_AddRef(m_pTransformCom);
	}
}

void CBossHunter::Ready_Arrow(CHunterArrow::FIRE_TYPE eFireType)
{
	m_pArrows[m_iArrowIndex]->Execute_Ready(eFireType);
}

void CBossHunter::Fire_Arrow(_bool bArrowIndexUpdate, _bool bTargetLook, _float4 vTargetPos)
{
	m_pArrows[m_iArrowIndex]->Execute_Fire(bTargetLook, vTargetPos);
	bArrowIndexUpdate ? Update_ArrowIndex() : 0;
}

void CBossHunter::Update_ArrowIndex()
{
	m_iArrowIndex++;
	m_iArrowIndex %= ARROW_COUNT;
}

void CBossHunter::LookAt_Knife(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::LookAt_Knife);
		return;
	}

	m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
}

void CBossHunter::LookAt_Arrow(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::LookAt_Knife);
		return;
	}

	m_pTransformCom->LookAt(m_vKenaPos);
}

void CBossHunter::ReadyArrow_Single(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::ReadyArrow_Single);
		return;
	}

	_uint iLastArrowIndex = m_iArrowIndex + SINGLE_SHOT_FRIEND_COUNT * 2 + 1;
	_uint iIndex;
	for (_uint i = m_iArrowIndex; i < iLastArrowIndex; i++)
	{
		iIndex = i % ARROW_COUNT;
		m_pArrows[iIndex]->Execute_Ready(CHunterArrow::SINGLE);
	}
}

void CBossHunter::ReadyArrow_Charge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::ReadyArrow_Charge);
		return;
	}

	Ready_Arrow(CHunterArrow::CHARGE);
	m_bReadyStrongArrow = true;
}

void CBossHunter::ReadyArrow_Rapid(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::ReadyArrow_Rapid);
		return;
	}

	Ready_Arrow(CHunterArrow::RAPID);
}

void CBossHunter::ReadyArrow_Shock(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::ReadyArrow_Shock);
		return;
	}

	Ready_Arrow(CHunterArrow::SHOCK);
	m_bReadyStrongArrow = true;
}

void CBossHunter::FireArrow_Single(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::FireArrow_Single);
		return;
	}
	
	_vector vKenaPos = m_vKenaPos + m_vTargetOffset;
	_uint iBackupIndex = m_iArrowIndex;
	Fire_Arrow(true, true, vKenaPos);

	_vector vRight = XMVector3Normalize(m_pArrows[iBackupIndex]->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT));
	_vector vDirTable[2] = { vRight, -vRight };
	
	_float fDist = 2.f;
	for (_uint i = 0; i < 2; i++)
	{
		for (_uint j = 0; j < SINGLE_SHOT_FRIEND_COUNT; j++)
		{
			m_vSingleShotTargetPosTable[i * SINGLE_SHOT_FRIEND_COUNT + j] = vKenaPos + vDirTable[i] * (j + 1) * fDist;
		}
	}
	
	for (_uint i = 0; i < SINGLE_SHOT_FRIEND_COUNT * 2; i++)
		Fire_Arrow(true, true, m_vSingleShotTargetPosTable[i]);	
}

void CBossHunter::FireArrow_Charge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::FireArrow_Charge);
		return;
	}
		
	Fire_Arrow(true, true, m_vKenaPos + m_vTargetOffset);

	m_bReadyStrongArrow = false;
}

void CBossHunter::FireArrow_Rapid(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::FireArrow_Rapid);
		return;
	}
		
	Fire_Arrow(true, true, m_vKenaPos + m_vTargetOffset);
}

void CBossHunter::FireArrow_Shock(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::FireArrow_Shock);
		return;
	}
		
	Fire_Arrow(true, false, _float4());
	m_bReadyStrongArrow = false;
}

void CBossHunter::Reset_HitFlag()
{
	m_bStronglyHit = false;
	m_bWeaklyHit = false;
}

void CBossHunter::ShockEffect_On(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::ShockEffect_On);
		return;
	}

	_float4 vPos = m_pArrows[m_iArrowIndex]->Get_ArrowHeadPos();
	//vPos.x += 0.30f;

	m_vecEffects[EFFECT_CHARGE_PARTICLE_GATHER]->Activate(vPos);
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CIRCLE]->Activate_Scaling(vPos, { -3.f, -3.f });
	m_vecEffects[EFFECT_CHARGE_TEXTURE_SHINE]->Activate(vPos);
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CENTER]->Activate_Spread(vPos, { 0.7f, 0.7f });
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE1]->Activate_Spread(vPos, { 1.f, 1.f });
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE2]->Activate_Spread(vPos, { 1.f, 1.f });
}

void CBossHunter::ShockEffect_Off(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::ShockEffect_Off);
		return;
	}

	m_vecEffects[EFFECT_CHARGE_PARTICLE_GATHER]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CIRCLE]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_SHINE]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CENTER]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE1]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE2]->DeActivate();

}

void CBossHunter::DustEffect_On(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::DustEffect_On);
		return;
	}

	_float4 vPos;

	CBone* pStaffBonePtr = m_pModelCom->Get_BonePtr("char_lf_ball_jnt");
	if (pStaffBonePtr != nullptr)
	{
		_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();
		vPos = matWorldSocket.r[3];
	}
	else
		vPos = m_pTransformCom->Get_Position();

	_float4		vCenterPos = m_pTransformCom->Get_Position();
	vPos.x = vCenterPos.x;
	vPos.z = vCenterPos.z;

	m_vecEffects[EFFECT_DUST_PARTICLE_BIG]->Activate(vPos);
	m_vecEffects[EFFECT_DUST_PARTICLE_SMALL]->Activate(vPos);
	m_vecEffects[EFFECT_DUST_MESH_DUSTPLANE]->Activate(vPos);
}

void CBossHunter::StunEffect_Off(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::StunEffect_Off);
		return;
	}

	m_vecEffects[EFFECT_STUN_MESH_GUAGE]->DeActivate_Slowly();
	m_vecEffects[EFFECT_STUN_MESH_BASE]->DeActivate_Slowly();
	m_vecEffects[EFFECT_STUN_MESH_BASE2]->DeActivate_Slowly();

}

void CBossHunter::BowTrailEffect_On(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::BowTrailEffect_On);
		return;
	}

	m_vecEffects[EFFECT_BOWTRAIL1]->Activate(this, "Bow_TopJnt9");
	m_vecEffects[EFFECT_BOWTRAIL2]->Activate(this, "Bow_BotJoint9");
}

void CBossHunter::BowTrailEffect_Off(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::BowTrailEffect_Off);
		return;
	}

	m_vecEffects[EFFECT_BOWTRAIL1]->DeActivate();
	m_vecEffects[EFFECT_BOWTRAIL2]->DeActivate();
}

void CBossHunter::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::TurnOnTrail);
		return;
	}
	m_pHunterTrail->Set_Active(true);

	// KnifeAttack : 0.6 ~ 0.739
	// KnifeParry_Exit : 0.462 ~ 0.632
	// KnifeSlam_Attack : 0.423 ~ 0.79
}

void CBossHunter::TUrnOffTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::TUrnOffTrail);
		return;
	}
	m_pHunterTrail->Set_Active(false);
	m_pHunterTrail->ResetInfo();
}

void CBossHunter::StunEffect_On(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::StunEffect_On);
		return;
	}
	_float4 vPos;

	CBone* pStaffBonePtr = m_pModelCom->Get_BonePtr("char_rt_upArmWeightSplit_1_jnt");
	if (pStaffBonePtr != nullptr)
	{
		_matrix SocketMatrix = pStaffBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();
		vPos = matWorldSocket.r[3];
	}
	else
		vPos = m_pTransformCom->Get_Position();

	_float4		vCenterPos = m_pTransformCom->Get_Position();
	vPos.x = vCenterPos.x;
	vPos.z = vCenterPos.z;

	m_vecEffects[EFFECT_STUN_MESH_GUAGE]->Activate_Slowly(vPos);
	m_vecEffects[EFFECT_STUN_MESH_BASE]->Activate_Slowly(vPos);
	m_vecEffects[EFFECT_STUN_MESH_BASE2]->Activate_Slowly(vPos);
}

HRESULT CBossHunter::Create_Effects()
{
	string strFilePath = "../Bin/Data/Effect_UI/EffectList_Hunter.json";

	Json jLoad;

	ifstream file(strFilePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	_int iNumEffects;
	jLoad["01. NumEffects"].get_to<_int>(iNumEffects);

	for (auto jSub : jLoad["02. FileName"])
	{
		string strEffect;
		jSub.get_to<string>(strEffect);
		wstring wstr;
		wstr.assign(strEffect.begin(), strEffect.end());
		_tchar* cloneTag = CUtile::Create_StringAuto(wstr.c_str());

		string type;
		for (auto c : strEffect)
		{
			if (c == '_')
				break;
			type += c;
		}

		CEffect_Base_S2* pEffect = nullptr;

		if (type == "Particle")
			pEffect = static_cast<CEffect_Base_S2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Particle_Base", cloneTag, cloneTag));
		else if (type == "Mesh")
			pEffect = static_cast<CEffect_Base_S2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Mesh_Base", cloneTag, cloneTag));
		else if (type == "Texture")
			pEffect = static_cast<CEffect_Base_S2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Texture_Base", cloneTag, cloneTag));

		if (pEffect != nullptr)
		{
			//pEffect->Set_Target(this);
			m_vecEffects.push_back(pEffect);
		}
		else
			return E_FAIL;

	}


	return S_OK;
}

HRESULT CBossHunter::Create_Trail()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Trail */
	m_pHunterTrail = dynamic_cast<CE_HunterTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_HunterTrail", L"H_Trail"));
	NULL_CHECK_RETURN(m_pHunterTrail, E_FAIL);
	m_pHunterTrail->Set_Parent(this);
	/* Trail */

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

bool	EffectList_Getter(void* data, int index, const char** output)
{
	vector<string>* pVec = (vector<string>*)data;
	*output = (*pVec)[index].c_str();
	return true;
}

void CBossHunter::ImGui_EffectProperty()
{
	if (ImGui::Button("ReCompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
	ImGui::SameLine();

	static vector<string> tags;
	if (ImGui::Button("Refresh"))
	{
		//if (!tags.empty())
		//	tags.clear();
		m_pSelectedEffect = nullptr;

		for (auto& pEffect : m_vecEffects)
			Safe_Release(pEffect);
		m_vecEffects.clear();
		tags.clear();
		Create_Effects();

		for (auto pEffect : m_vecEffects)
		{
			wstring wstr = pEffect->Get_ObjectCloneName();
			string str = CUtile::wstring_to_utf8(wstr);
			//str.assign(wstr.begin(), wstr.end());
			tags.push_back(str);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Arrow Effects"))
	{
		for(auto& arrow : m_pArrows)
		{
			arrow->Reset_Effects();
		}
	}


	static _int iSelectedEffect;
	_int iSelectedBefore = iSelectedEffect;
	if (ImGui::ListBox("Effect List", &iSelectedEffect, EffectList_Getter, &tags, (_int)tags.size(), 5))
	{
		m_pSelectedEffect = m_vecEffects[iSelectedEffect];
		m_pSelectedEffect->Set_ActiveFlip();
		m_pSelectedEffect->Set_EffectTag(tags[iSelectedEffect]);
	}

	if (nullptr == m_pSelectedEffect)
		return;

	if (ImGui::Button("All off"))
	{
		for (auto eff : m_vecEffects)
			eff->Set_Active(false);
	} ImGui::SameLine();
	if (ImGui::Button("All On"))
	{
		for (auto eff : m_vecEffects)
			eff->Set_Active(true);
	};

	m_pSelectedEffect->Imgui_RenderProperty();

	// Set_Target(m_pArrows[m_iArrowIndex]);
}

void CBossHunter::Update_Trail(const char* pBoneTag)
{
	_matrix SocketMatrix = m_pWeaponTrailBone->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	m_pHunterTrail->Get_TransformCom()->Set_WorldMatrix(SocketMatrix);

	if (m_pHunterTrail->Get_Active() == true)
		m_pHunterTrail->Trail_InputPos(SocketMatrix.r[3]);
}

//void CBossHunter::Test(_bool bIsInit, _float fTimeDelta)
//{
//	if (bIsInit == true)
//	{
//		const _tchar* pFuncName = __FUNCTIONW__;
//		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Test);
//		return;
//	}
//
//	MSG_BOX("Test");
//
//}

void CBossHunter::Create_CopySoundKey()
{
	_tchar szOriginKeyTable[COPY_SOUND_KEY_END][64] = {
		TEXT("Mon_BossHunter_Attack1.ogg"), TEXT("Mon_BossHunter_Attack2.ogg"), TEXT("Mon_BossHunter_Attack3.ogg"), TEXT("Mon_BossHunter_Attack4.ogg"),
		TEXT("Mon_BossHunter_Attack5.ogg"), TEXT("Mon_BossHunter_Attack6.ogg"), TEXT("Mon_BossHunter_Attack7.ogg"), TEXT("Mon_BossHunter_Attack8.ogg"),
		TEXT("Mon_BossHunter_Attack9.ogg"), TEXT("Mon_BossHunter_Attack10.ogg"), TEXT("Mon_BossHunter_Attack11.ogg"), TEXT("Mon_BossHunter_Attack12.ogg"),
		TEXT("Mon_BossHunter_Attack13.ogg"),

		TEXT("Mon_BossHunter_Hurt1.ogg"), TEXT("Mon_BossHunter_Hurt2.ogg"), TEXT("Mon_BossHunter_Hurt3.ogg"), TEXT("Mon_BossHunter_Hurt4.ogg"),
		TEXT("Mon_BossHunter_Hurt5.ogg"), TEXT("Mon_BossHunter_Hurt6.ogg"), TEXT("Mon_BossHunter_Hurt7.ogg"), TEXT("Mon_BossHunter_Hurt8.ogg"),
		TEXT("Mon_BossHunter_Hurt9.ogg"), TEXT("Mon_BossHunter_Hurt10.ogg"), TEXT("Mon_BossHunter_Hurt11.ogg"),

		TEXT("Mon_BossHunter_Whoosh1.ogg"), TEXT("Mon_BossHunter_Whoosh2.ogg"), TEXT("Mon_BossHunter_Whoosh3.ogg"), TEXT("Mon_BossHunter_Whoosh4.ogg"),
		TEXT("Mon_BossHunter_Whoosh5.ogg"), TEXT("Mon_BossHunter_Whoosh6.ogg"), TEXT("Mon_BossHunter_Whoosh7.ogg"), TEXT("Mon_BossHunter_Whoosh8.ogg"),

		TEXT("Mon_BossHunter_Elemental1.ogg"), TEXT("Mon_BossHunter_Elemental2.ogg"), TEXT("Mon_BossHunter_Elemental3.ogg"), TEXT("Mon_BossHunter_Elemental4.ogg"),
		TEXT("Mon_BossHunter_Elemental5.ogg"), TEXT("Mon_BossHunter_Elemental6.ogg"), TEXT("Mon_BossHunter_Elemental7.ogg"), TEXT("Mon_BossHunter_Elemental8.ogg"),
		TEXT("Mon_BossHunter_Elemental9.ogg"), TEXT("Mon_BossHunter_Elemental10.ogg"), TEXT("Mon_BossHunter_Elemental11.ogg"), TEXT("Mon_BossHunter_Elemental12.ogg"),
		TEXT("Mon_BossHunter_Elemental13.ogg"), TEXT("Mon_BossHunter_Elemental14.ogg"), TEXT("Mon_BossHunter_Elemental15.ogg"),

		TEXT("Mon_Attack_Impact1.ogg"), TEXT("Mon_Attack_Impact2.ogg"), TEXT("Mon_Attack_Impact3.ogg"), TEXT("Mon_Attack_Impact4.ogg"), TEXT("Mon_Attack_Impact5.ogg"),
		TEXT("Mon_Knife1.ogg"), TEXT("Mon_Knife2.ogg"), TEXT("Mon_Knife3.ogg"),

		TEXT("Mon_BossHunter_ArrowCharge.ogg"),
	};

	_tchar szTemp[MAX_PATH] = { 0, };

	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		SaveBufferCopySound(szOriginKeyTable[i], szTemp, &m_pCopySoundKey[i]);
	}
}

void CBossHunter::Play_Attack1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK1], 0.5f);
}

void CBossHunter::Play_Attack2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK2], 0.5f);
}

void CBossHunter::Play_Attack3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK3], 0.5f);
}

void CBossHunter::Play_Attack4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK4], 0.5f);
}

void CBossHunter::Play_Attack5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK5], 0.5f);
}

void CBossHunter::Play_Attack6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK6], 0.5f);
}

void CBossHunter::Play_Attack7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK7], 0.5f);
}

void CBossHunter::Play_Attack8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK8], 0.5f);
}

void CBossHunter::Play_Attack9Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack9Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK9], 0.5f);
}

void CBossHunter::Play_Attack10Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack10Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK10], 0.5f);
}

void CBossHunter::Play_Attack11Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack11Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK11], 0.5f);
}

void CBossHunter::Play_Attack12Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack12Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK12], 0.5f);
}

void CBossHunter::Play_Attack13Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Attack13Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ATTACK13], 0.5f);
}

void CBossHunter::Play_Hurt1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT1], 0.5f);
}

void CBossHunter::Play_Hurt2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT2], 0.5f);
}

void CBossHunter::Play_Hurt3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT3], 0.5f);
}

void CBossHunter::Play_Hurt4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT4], 0.5f);
}

void CBossHunter::Play_Hurt5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT5], 0.5f);
}

void CBossHunter::Play_Hurt6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT6], 0.5f);
}

void CBossHunter::Play_Hurt7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT7], 0.5f);
}

void CBossHunter::Play_Hurt8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT8], 0.5f);
}

void CBossHunter::Play_Hurt9Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt9Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT9], 0.5f);
}

void CBossHunter::Play_Hurt10Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt10Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT10], 0.5f);
}

void CBossHunter::Play_Hurt11Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Hurt11Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_HURT11], 0.5f);
}


void CBossHunter::Play_Whoosh1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH1], 0.5f);
}

void CBossHunter::Play_Whoosh2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH2], 0.5f);
}

void CBossHunter::Play_Whoosh3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH3], 0.5f);
}

void CBossHunter::Play_Whoosh4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH4], 0.5f);
}

void CBossHunter::Play_Whoosh5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH5], 0.5f);
}

void CBossHunter::Play_Whoosh6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH6], 0.5f);
}

void CBossHunter::Play_Whoosh7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH7], 0.5f);
}

void CBossHunter::Play_Whoosh8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Whoosh8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH8], 0.5f);
}

void CBossHunter::Play_Elemental1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL1], 0.5f);
}

void CBossHunter::Play_Elemental2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL2], 0.5f);
}

void CBossHunter::Play_Elemental3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL3], 0.5f);
}

void CBossHunter::Play_Elemental4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL4], 0.5f);
}

void CBossHunter::Play_Elemental5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL5], 0.5f);
}

void CBossHunter::Play_Elemental6Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental6Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL6], 0.5f);
}

void CBossHunter::Play_Elemental7Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental7Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL7], 0.5f);
}

void CBossHunter::Play_Elemental8Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental8Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL8], 0.5f);
}

void CBossHunter::Play_Elemental9Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental9Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL9], 0.5f);
}

void CBossHunter::Play_Elemental10Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental10Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL10], 0.5f);
}

void CBossHunter::Play_Elemental11Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental11Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL11], 0.5f);
}

void CBossHunter::Play_Elemental12Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental12Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL12], 0.5f);
}

void CBossHunter::Play_Elemental13Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental13Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL13], 0.5f);
}


void CBossHunter::Play_Elemental14Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental14Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL14], 0.5f);
}

void CBossHunter::Play_Elemental15Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Elemental15Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ELEMENTAL15], 0.5f);
}

void CBossHunter::Play_Impact1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Impact1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT1], 0.5f);
}

void CBossHunter::Play_Impact2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Impact2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT2], 0.5f);
}

void CBossHunter::Play_Impact3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Impact3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT3], 0.5f);
}

void CBossHunter::Play_Impact4Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Impact4Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT4], 0.5f);
}

void CBossHunter::Play_Impact5Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Impact5Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_IMPACT5], 0.5f);
}

void CBossHunter::Play_Knife1Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Knife1Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_KNIFE1], 0.5f);
}

void CBossHunter::Play_Knife2Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Knife2Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_KNIFE2], 0.5f);
}

void CBossHunter::Play_Knife3Sound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_Knife3Sound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_KNIFE3], 0.5f);
}

void CBossHunter::Play_ArrowChargeSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Play_ArrowChargeSound);
		return;
	}

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_ARROW_CHARGE], 0.5f);
}

void CBossHunter::Stop_ShockArrowUpY(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Stop_ShockArrowUpY);
		return;
	}

	m_bShockArrowUpY = false;
}
