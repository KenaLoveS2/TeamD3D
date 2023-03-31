#include "stdafx.h"
#include "..\public\BossHunter.h"
#include "GameInstance.h"
#include "Effect_Base_S2.h"

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
	m_pModelCom->Set_AnimIndex(IDLE);
	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pBodyBone = m_pModelCom->Get_BonePtr("char_spine_mid_jnt");

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
	m_vStringDiffuseColor = { 1.0f, 0.05f, 0.46f, 1.f };


	return S_OK;
}

HRESULT CBossHunter::Late_Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Late_Initialize(pArg), E_FAIL);
	// 몸통
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

	// 몸통
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

	m_pTransformCom->Set_WorldMatrix_float4x4(m_Desc.WorldMatrix);

	return S_OK;
}

void CBossHunter::Tick(_float fTimeDelta)
{
	ImGui::Begin("HunterTest");
	if (ImGui::Button("ReCompile"))
	{
		m_pShaderCom->ReCompile();
		m_pRendererCom->ReCompile();
	}
	ImGui::End();

	m_pModelCom->Play_Animation(fTimeDelta);
	Update_Collider(fTimeDelta);

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

	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);
	AdditiveAnim(fTimeDelta);

	for (auto& pArrow : m_pArrows)
		pArrow->Tick(fTimeDelta);
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
}

HRESULT CBossHunter::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (i == 0) // ArrowString
		{
			// ArrowString (Have to another Texture) Pause render until then
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 13), E_FAIL);
		}
		else if (i == 3) //Hair
		{
			// Hair (Have to another Texture) Pause render until then
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			//FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", DEFAULT), E_FAIL);
		}
		else if (i == 4) // Knife UV01
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, 2, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", BOSS_AO_R_M_E), E_FAIL);
		}
		else
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

	Test1(true, 0.f);
	Test2(true, 0.f);
	Test3(true, 0.f);
	Test4(true, 0.f);

}

HRESULT CBossHunter::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("SLEEP")
		.AddState("SLEEP")
		.OnExit([this]()
			{

			})
		.AddTransition("SLEEP to READY_SPAWN", "READY_SPAWN")
				.Predicator([this]()
					{
						// 대기 종결 조건 수정 필요
						m_fSpawnRange = 5.f;
						return DistanceTrigger(m_fSpawnRange);
					})


				.AddState("READY_SPAWN")
						.OnStart([this]()
							{
								// 등장 연출 필요
								m_pModelCom->ResetAnimIdx_PlayTime(IDLE);
								m_pModelCom->Set_AnimIndex(IDLE);

								CUI_ClientManager::UI_PRESENT eBossHP = CUI_ClientManager::TOP_BOSS;
								_float fValue = 30.f; /* == BossHunter Name */
								m_BossHunterDelegator.broadcast(eBossHP, fValue);
							})
						.OnExit([this]()
							{
								m_pTransformCom->LookAt(m_vKenaPos);
								m_bSpawn = true;
							})
								.AddTransition("READY_SPAWN to IDLE", "IDLE")
								.Predicator([this]()
									{
										return AnimFinishChecker(IDLE);
									})

								.AddState("IDLE")
										.OnStart([this]()
											{
												m_pTransformCom->LookAt_NoUpDown(m_vKenaPos);
												m_pTransformCom->Speed_Boost(true, 0.5f);

												m_pModelCom->ResetAnimIdx_PlayTime(IDLE);
												m_pModelCom->Set_AnimIndex(IDLE);

												m_fIdleTimeCheck = m_bDodge ? m_fIdleTimeCheck : 0.f;
												m_bDodge = false;
											})
										.Tick([this](_float fTimeDelta)
											{
												m_fIdleTimeCheck += fTimeDelta;

												if (m_pTransformCom->Get_PositionY() <= m_fFlyHeightY)
												{
													m_pTransformCom->Go_AxisY(fTimeDelta);
													if (m_pTransformCom->Get_PositionY() >= m_fFlyHeightY)
														m_pTransformCom->Set_PositionY(m_fFlyHeightY);
												}

												m_bDodge = m_pKena->Get_State(CKena::STATE_BOW) && (rand() % 3 == 0);
											})
												.OnExit([this]()
													{
														Reset_HitFlag();
														m_pTransformCom->Speed_Boost(true, 1.f);
													})
												.AddTransition("To DYING", "DYING")
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
																																		.AddTransition("To DYING", "DYING")
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
																																										.AddTransition("To DYING", "DYING")
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
																																																		.AddTransition("To DYING", "DYING")
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
																																																										.AddTransition("To DYING", "DYING")
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
																																																																.AddTransition("To DYING", "DYING")
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
																																																																						.AddTransition("To DYING", "DYING")
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
																																																																														.AddTransition("To DYING", "DYING")
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
																																																																																				.AddTransition("To DYING", "DYING")
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
																																																																																												.AddTransition("To DYING", "DYING")
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
																																																																																																				.AddTransition("To DYING", "DYING")
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
																																																																																																										ResetAndSet_Animation(SINGLE_SHOT); // 일반 화살 한발을 날린다 애니메이션 끝날 때쯤 발사 함수 호출 필요
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
																																																																																																										.AddTransition("To DYING", "DYING")
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
																																																																																																																		ResetAndSet_Animation(RAPID_SHOOT); // 일반 화살을 여러발을 날린다 애니메이션 중간중간 발사 함수 호출 필요
																																																																																																																		m_pTransformCom->LookAt(m_vKenaPos);
																																																																																																																		Set_NextAttack();
																																																																																																																	})
																																																																																																																.AddTransition("To DYING", "DYING")
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
																																																																																																																								.AddTransition("To DYING", "DYING")
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
																																																																																																																															})
																																																																																																																														.AddTransition("To DYING", "DYING")
																																																																																																																																.Predicator([this]()
																																																																																																																																	{
																																																																																																																																		return m_pMonsterStatusCom->IsDead();
																																																																																																																																	})
																																																																																																																																.AddTransition("SHOCK_ARROW_LOOP To SHOCK_ARROW_EXIT", "SHOCK_ARROW_EXIT")
																																																																																																																																		.Predicator([this]()
																																																																																																																																			{
																																																																																																																																				return m_pArrows[m_iArrowIndex]->IsEnd();
																																																																																																																																			})


																																																																																																																																		.AddState("SHOCK_ARROW_EXIT")
																																																																																																																																				.OnStart([this]()
																																																																																																																																					{
																																																																																																																																						ResetAndSet_Animation(SHOCK_ARROW_EXIT);
																																																																																																																																					})
																																																																																																																																				.AddTransition("To DYING", "DYING")
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
																																																																																																																																												.AddTransition("To DYING", "DYING")
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
																																																																																																																																																		.AddTransition("To DYING", "DYING")
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
																																																																																																																																																										.AddTransition("To DYING", "DYING")
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
																																																																																																																																																																.AddTransition("To DYING", "DYING")
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
																																																																																																																																																																								.AddTransition("To DYING", "DYING")
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
																																																																																																																																																																												.AddTransition("To DYING", "DYING")
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
																																																																																																																																																																																				m_pTransformCom->Speed_Boost(true, 1.3f);
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
																																																																																																																																																																																				.AddTransition("To DYING", "DYING")
																																																																																																																																																																																						.Predicator([this]()
																																																																																																																																																																																							{
																																																																																																																																																																																								return m_pMonsterStatusCom->IsDead();
																																																																																																																																																																																							})
																																																																																																																																																																																						.AddTransition("FLY to IDLE", "IDLE")
																																																																																																																																																																																								.Predicator([this]()
																																																																																																																																																																																									{
																																																																																																																																																																																										return m_bFlyEnd;
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
																																																																																																																																																																																											})
																																																																																																																																																																																										.AddTransition("DYING to DEATH_SCENE", "DEATH_SCENE")
																																																																																																																																																																																												.Predicator([this]()
																																																																																																																																																																																													{
																																																																																																																																																																																														return m_pModelCom->Get_AnimationFinish();
																																																																																																																																																																																													})

																																																																																																																																																																																												.AddState("DEATH_SCENE")
																																																																																																																																																																																														.OnStart([this]()
																																																																																																																																																																																															{
																																																																																																																																																																																																// 죽은 애니메이션 후 죽음 연출 State
																																																																																																																																																																																															})
																																																																																																																																																																																														.AddTransition("DEATH_SCENE to DEATH", "DEATH")
																																																																																																																																																																																																.Predicator([this]()
																																																																																																																																																																																																	{
																																																																																																																																																																																																		return true;
																																																																																																																																																																																																	})

																																																																																																																																																																																																.AddState("DEATH")
																																																																																																																																																																																																		.OnStart([this]()
																																																																																																																																																																																																			{
																																																																																																																																																																																																				m_pTransformCom->Clear_Actor();
																																																																																																																																																																																																				Clear_Death();
																																																																																																																																																																																																			})

																																																																																																																																																																																																		.Build();
																																																																																																																																																																																																			return S_OK;
}

HRESULT CBossHunter::SetUp_Components()
{
	__super::SetUp_Components();

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Boss_Hunter", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_64.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_135.png")), E_FAIL);

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

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &g_bFalse, sizeof(_bool)), E_FAIL);

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

void CBossHunter::Set_AttackType()
{
}

void CBossHunter::Reset_Attack()
{
}

void CBossHunter::Tick_Attack(_float fTimeDelta)
{
}

void CBossHunter::Set_AFType()
{
}

void CBossHunter::Reset_AF()
{
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
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.003f, 5);
			}
			else
			{
				m_bWeaklyHit = false;
				m_bStronglyHit = true;

				//dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->TimeSleep(0.3f);
				m_pKena->Add_HitStopTime(0.25f);
				m_fHitStopTime += 0.25f;
				dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(0.005f, 5);

				vector<_float4>* vecWeaponPos = m_pKena->Get_WeaponPositions();
				if (vecWeaponPos->size() == 2)
				{
					_vector	vDir = vecWeaponPos->back() - vecWeaponPos->front();
					vDir = XMVectorSetZ(vDir, 0.f);
					dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr())->Camera_Shake(vDir, XMConvertToRadians(30.f));
				}
			}
		}

		if (iColliderIndex == (_int)COL_PLAYER_ARROW)
		{
			m_pMonsterStatusCom->UnderAttack(m_pKena->Get_KenaStatusPtr());

			//m_bStronglyHit = m_pKena->Get_State(CKena::STATE_INJECTBOW);
			//m_bWeaklyHit = !m_bStronglyHit;

			m_bStronglyHit = true;
			m_bWeaklyHit = false;

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

		Safe_AddRef(ArrowDesc.pSocket);
		Safe_AddRef(m_pTransformCom);
	}
}

/* 장전 */
void CBossHunter::Ready_Arrow(CHunterArrow::FIRE_TYPE eFireType)
{
	m_pArrows[m_iArrowIndex]->Execute_Ready(eFireType);
}

/* 실제 쏠때 */
void CBossHunter::Fire_Arrow(_bool bArrowIndexUpdate)
{
	m_pArrows[m_iArrowIndex]->Execute_Fire();
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

	Ready_Arrow(CHunterArrow::SINGLE);
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

	Fire_Arrow(true);
}

void CBossHunter::FireArrow_Charge(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::FireArrow_Charge);
		return;
	}

	Fire_Arrow(true);
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

	Fire_Arrow(true);
}

void CBossHunter::FireArrow_Shock(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::FireArrow_Shock);
		return;
	}

	Fire_Arrow(false);
	m_bReadyStrongArrow = false;
}

void CBossHunter::Reset_HitFlag()
{
	m_bStronglyHit = false;
	m_bWeaklyHit = false;
}

void CBossHunter::Test1(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Test1);
		return;
	}

	_float4 vPos = m_pArrows[m_iArrowIndex]->Get_ArrowHeadPos();
	CGameObject* pArrow = m_pArrows[m_iArrowIndex];

	m_vecEffects[EFFECT_CHARGE_PARTICLE_GATHER]->Activate(vPos);


	m_vecEffects[EFFECT_CHARGE_TEXTURE_CIRCLE]->Activate(nullptr);

	m_vecEffects[EFFECT_CHARGE_TEXTURE_CIRCLE]->Activate(vPos, { -3.f, -3.f });
	m_vecEffects[EFFECT_CHARGE_TEXTURE_SHINE]->Activate(vPos);
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CENTER]->Activate(vPos);
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE1]->Activate(vPos);
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE2]->Activate(vPos);
}

void CBossHunter::Test2(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Test2);
		return;
	}

	m_vecEffects[EFFECT_CHARGE_PARTICLE_GATHER]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CIRCLE]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_SHINE]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_CENTER]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE1]->DeActivate();
	m_vecEffects[EFFECT_CHARGE_TEXTURE_LINE2]->DeActivate();

}

void CBossHunter::Test3(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Test3);
		return;
	}
}

void CBossHunter::Test4(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CBossHunter::Test4);
		return;
	}
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
	}


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
	static vector<string> tags;
	if (ImGui::Button("Refresh"))
	{
		if (!tags.empty())
			tags.clear();

		for (auto pEffect : m_vecEffects)
		{
			wstring wstr = pEffect->Get_ObjectCloneName();
			string str;
			str.assign(wstr.begin(), wstr.end());
			tags.push_back(str);
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
