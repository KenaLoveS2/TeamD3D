#include "stdafx.h"
#include "..\public\HunterArrow.h"
#include "BossHunter.h"
#include "Effect_Base_S2.h"
#include "E_Swipes_Charged.h"

CHunterArrow::CHunterArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonsterWeapon(pDevice, pContext)
	, m_fTrailTime(0.f)
	, m_fTrailTimeAcc(0.f)
	, m_bTrailOn(false)
	, m_iTrailIndex(0)
	, m_bShockwaveOn(false)
	, m_fRingTime(0.0f)
	, m_fRingTimeAcc(0.0f)
	, m_iRingIndex(0)
	, m_iHitIndex(0)
	, m_pGameInstance(CGameInstance::GetInstance())
{	
}

CHunterArrow::CHunterArrow(const CHunterArrow& rhs)
	: CMonsterWeapon(rhs)
	, m_fTrailTime(0.f)
	, m_fTrailTimeAcc(0.f)
	, m_bTrailOn(false)
	, m_iTrailIndex(0)
	, m_bShockwaveOn(false)
	, m_fRingTime(0.0f)
	, m_fRingTimeAcc(0.0f)
	, m_iRingIndex(0)
	, m_iHitIndex(0)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CHunterArrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHunterArrow::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	CGameObject::GAMEOBJECTDESC GameObjDesc;
	ZeroMemory(&GameObjDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjDesc.TransformDesc.fSpeedPerSec = 20.f;

	if (FAILED(__super::Initialize(&GameObjDesc)))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pTransformCom->Set_Position(m_vInvisiblePos);

	if (FAILED(SetUp_Effects()))
	{
		MSG_BOX("Failed To Ready Effects : CHunterArrow");
		return E_FAIL;
	}

	m_fDissolveTime = 0.f;
	m_fTrailTime = 0.1f;
	m_fTrailTimeAcc = 0.f;
	m_fRingTime = 0.30f;

	_tchar szSoundTable[COPY_SOUND_KEY_END][64] = {	
		TEXT("Mon_BossHunter_ArrowHit.ogg"),
		TEXT("Mon_BossHunter_ArrowHit.ogg"),
		TEXT("Mon_BossHunter_ArrowLaunch.ogg"),
		TEXT("Mon_BossHunter_ArrowWhoosh.ogg"),
		TEXT("Mon_Attack_Impact3.ogg"),		
	};
	_tchar szTemp[MAX_PATH] = { 0, };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	for (_uint i = 0; i < (_uint)COPY_SOUND_KEY_END; i++)
	{
		pGameInstance->Copy_Sound(szSoundTable[i], szTemp);
		m_pCopySoundKey[i] = CUtile::Create_StringAuto(szTemp);
	}

	return S_OK;
}

HRESULT CHunterArrow::Late_Initialize(void* pArg)
{
	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = m_szCloneObjectTag;
		PxSphereDesc.fRadius = 0.1f;
		PxSphereDesc.isGravity = false;
		PxSphereDesc.bCCD = false;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		pPhysX->Create_Sphere(PxSphereDesc, Create_PxUserData(m_WeaponDesc.pOwnerMonster, false, COL_MONSTER_ARROW));

		XMStoreFloat4x4(&m_ColliderPivotMatrix, XMMatrixTranslation(m_vColliderPivotPos.x, m_vColliderPivotPos.y, m_vColliderPivotPos.z));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, m_ColliderPivotMatrix);
	}

	{
		CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
		PxSphereDesc.eType = SPHERE_DYNAMIC;
		PxSphereDesc.pActortag = CUtile::Create_DummyString();
		PxSphereDesc.fRadius = 0.2f;
		PxSphereDesc.isGravity = false;
		PxSphereDesc.bCCD = false;
		PxSphereDesc.eFilterType = PX_FILTER_TYPE::MONSTER_WEAPON;

		pPhysX->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COLLISON_DUMMY));
		m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, m_ColliderPivotMatrix);
	}

	/* SwipeCharged */
	{
		m_pSwipesCharged = dynamic_cast<CE_Swipes_Charged*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Swipes_Charged", L"Hunter_Charged"));
		NULL_CHECK_RETURN(m_pSwipesCharged, E_FAIL);
		m_pSwipesCharged->Set_Parent(this);
		m_pSwipesCharged->Late_Initialize(this);
	}

	m_pHunter = (CBossHunter*)m_pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Monster"), TEXT("BossHunter_0"));



	return S_OK;
}

void CHunterArrow::Tick(_float fTimeDelta)
{
	if (m_eArrowState == STATE_END) return;

	if(m_pHunter == nullptr)
		m_pHunter = (CBossHunter*)m_pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Monster"), TEXT("BossHunter_0"));

	__super::Tick(fTimeDelta);

	// CHunterArrow::Imgui_RenderProperty();

	ArrowProc(fTimeDelta);
	Update_Collider(fTimeDelta);

	if (m_pSwipesCharged)m_pSwipesCharged->Tick(fTimeDelta);
	for (_uint i = 0; i < (_uint)EFFECT_END; ++i)
	{
		for (auto& eff : m_vecEffects[i])
			eff->Tick(fTimeDelta);
	}
}

void CHunterArrow::Late_Tick(_float fTimeDelta)
{
	if (m_eArrowState == STATE_END) return;

	__super::Late_Tick(fTimeDelta);

	if (m_pSwipesCharged)m_pSwipesCharged->Late_Tick(fTimeDelta);
	for (_uint i = 0; i < (_uint)EFFECT_END; ++i)
	{
		for (auto& eff : m_vecEffects[i])
			eff->Late_Tick(fTimeDelta);
	}

	m_pRendererCom&& m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHunterArrow::Render()
{
	if (FAILED(__super::Render())) return E_FAIL;
	if (FAILED(SetUp_ShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_MaskTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 13);
	}

	return S_OK;
}

HRESULT CHunterArrow::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);

	return S_OK;
}

void CHunterArrow::Imgui_RenderProperty()
{
	CMonsterWeapon::Imgui_RenderProperty();

	ImGui::Begin("Monster_Weapon");
	_float3 vPivot = m_vHandPivotPos;
	float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
	ImGui::DragFloat3("Hand PivotPos", fPos, 0.01f, -100.f, 100.0f);
	m_vHandPivotPos.x = fPos[0];
	m_vHandPivotPos.y = fPos[1];
	m_vHandPivotPos.z = fPos[2];

	static char szTable[FIRE_TYPE_END][32] = {
		"Bow PivotPos - CHARGE",
		"Bow PivotPos - RAPID",
		"Bow PivotPos - SHOCK",
		"Bow PivotPos - SINGLE",
	};

	for (_uint i = 0; i < FIRE_TYPE_END; i++)
	{
		_float3 vPivot = m_vBowPivotPos[i];
		float fPos[3] = { vPivot.x, vPivot.y, vPivot.z };
		ImGui::DragFloat3(szTable[i], fPos, 0.01f, -100.f, 100.0f);
		m_vBowPivotPos[i].x = fPos[0];
		m_vBowPivotPos[i].y = fPos[1];
		m_vBowPivotPos[i].z = fPos[2];
	}

	_float3 vColPivot = m_vColliderPivotPos;
	float fColPos[3] = { vColPivot.x, vColPivot.y, vColPivot.z };
	ImGui::DragFloat3("Collider", fColPos, 0.01f, -100.f, 100.0f);
	m_vColliderPivotPos.x = fColPos[0];
	m_vColliderPivotPos.y = fColPos[1];
	m_vColliderPivotPos.z = fColPos[2];

	ImGui::End();
}

void CHunterArrow::ImGui_ShaderValueProperty()
{
	CMonsterWeapon::ImGui_ShaderValueProperty();
}

void CHunterArrow::ImGui_PhysXValueProperty()
{
	CMonsterWeapon::ImGui_PhysXValueProperty();
}

void CHunterArrow::Update_Collider(_float fTimeDelta)
{
	m_pTransformCom->Tick(fTimeDelta);
}

HRESULT CHunterArrow::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Boss_Hunter_Arrow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->SetUp_Material(0, aiTextureType::WJTextureType_DIFFUSE, TEXT("../Bin/Resources/NonAnim/Boss_HunterArrow/E_Effect_20.png"));
	m_pModelCom->SetUp_Material(0, aiTextureType::WJTextureType_NORMALS, TEXT("../Bin/Resources/NonAnim/Boss_HunterArrow/Noise_cloudsmed_Normal.png"));
	m_pModelCom->SetUp_Material(0, aiTextureType::WJTextureType_MASK, TEXT("../Bin/Resources/NonAnim/Boss_HunterArrow/Noise_cloudsmed.png"));

	return S_OK;
}

HRESULT CHunterArrow::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;

	/*_float4x4 SocketWorldMatrix;
	_float4x4 OriginWorld = m_pTransformCom->Get_WorldMatrixFloat4x4();
	XMStoreFloat4x4(&SocketWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_SocketMatrix));
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &SocketWorldMatrix))) return E_FAIL;*/

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;

	_float4 vColor = { 1.0f, 0.05f, 0.46f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4)))) return E_FAIL;

	_float fHDR = 5.f;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_fHDRIntensity", &fHDR, sizeof(_float)))) return E_FAIL;

	_bool bDissolve;
	if (m_fDissolveTime > 0.f)
	{
		bDissolve = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &bDissolve, sizeof(_bool)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
	}
	else
	{
		bDissolve = false;
		if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &bDissolve, sizeof(_bool)))) return E_FAIL;
	}


	return S_OK;
}

HRESULT CHunterArrow::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom) return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;

	return S_OK;
}

CHunterArrow* CHunterArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHunterArrow* pInstance = new CHunterArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHunterArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHunterArrow::Clone(void* pArg)
{
	CHunterArrow* pInstance = new CHunterArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHunterArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHunterArrow::Free()
{
	__super::Free();

	Safe_Release(m_pSwipesCharged);

	for (_uint i = 0; i < (_uint)EFFECT_END; ++i)
	{
		for (auto& eff : m_vecEffects[i])
		{
			Safe_Release(eff);
			m_vecEffects[i].clear();
		}
	}
}

void CHunterArrow::ArrowProc(_float fTimeDelta)
{
	switch (m_eArrowState)
	{
	case REDAY:
	{	
		_matrix HandSocketMatrix = m_WeaponDesc.pSocket->Get_OffsetMatrix() * m_WeaponDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);
		HandSocketMatrix = XMMatrixTranslation(m_vHandPivotPos.x, m_vHandPivotPos.y, m_vHandPivotPos.z) * HandSocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();
		m_pTransformCom->Set_Position(HandSocketMatrix.r[3]);

		_matrix BowSocketMatrix = m_pBowBone->Get_OffsetMatrix() * m_pBowBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);
		BowSocketMatrix = XMMatrixTranslation(m_vBowPivotPos[m_eFireType].x, m_vBowPivotPos[m_eFireType].y, m_vBowPivotPos[m_eFireType].z) * BowSocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();
		m_pTransformCom->LookAt(BowSocketMatrix.r[3]);

		m_bTrailOn = false;
	}
	case FIRE:
	{	
		m_pTransformCom->Go_Straight(fTimeDelta);

		Play_TrailEffect(fTimeDelta);

		Play_RingEffect(fTimeDelta);

		break;
	}
	case FINISH:
	{
		m_bTrailOn = false;
		m_bShockwaveOn = false;

		m_fDissolveTime += 0.5f * fTimeDelta;
		if (m_fDissolveTime > 1.f)
		{
			m_eArrowState = STATE_END;
			m_pTransformCom->Set_Position(m_vInvisiblePos);
			m_fDissolveTime = 0.0f;
		}

		break;
	}
	case STATE_END:
	{
		// 대기 상태다
		break;
	}
	}
}

void CHunterArrow::Execute_Ready(FIRE_TYPE eFireType)
{
	m_eFireType = eFireType;
	m_eArrowState = REDAY;
}

void CHunterArrow::Execute_Fire(_bool bLookTargetFlag, _float4 vTargetPos)
{
	m_eArrowState = FIRE;
	m_bTrailOn = true;

	if (m_eFireType == SHOCK)
		m_bShockwaveOn = true;
	
	bLookTargetFlag ? m_pTransformCom->LookAt(vTargetPos) : 0;

	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_LAUNCH], 0.5f);	
	m_pGameInstance->Play_Sound(m_pCopySoundKey[CSK_WHOOSH], 0.5f);
}

void CHunterArrow::Execute_Finish()
{
	m_eArrowState = FINISH;
	Play_FireFinishSound();
}

void CHunterArrow::Play_TrailEffect(_float fTimedelta)
{
	if (m_bTrailOn)
	{
		m_fTrailTimeAcc += fTimedelta;
		m_fTrailTime = 0.01f;
		if (m_fTrailTimeAcc > m_fTrailTime)
		{
			m_vecEffects[EFFECT_TRAIL][m_iTrailIndex]->Activate(m_pTransformCom->Get_Position());
			m_iTrailIndex++;
			m_iTrailIndex %= MAX_TRAIL_EFFECTS;
			m_fTrailTimeAcc = 0.f;
		}
	}
}

void CHunterArrow::Play_RingEffect(_float fTimeDelta)
{
	if (m_bShockwaveOn)
	{
		m_fRingTimeAcc += fTimeDelta;
		m_fRingTime = 0.1f;
		if (m_fRingTimeAcc > m_fRingTime)
		{
			_float4 vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float4 vPos = m_pTransformCom->Get_Position();
			m_vecEffects[EFFECT_RING][m_iRingIndex]->Activate_Scaling(vDir, vPos, {0.05f, 0.05f});  

			m_iRingIndex++;
			m_iRingIndex %= MAX_RING_EFFECTS;
			m_fRingTimeAcc = 0.0f;
		}
	}
}
void CHunterArrow::Play_HitEffect(_float fTimeDelta)
{
	m_vecEffects[EFFECT_HIT][m_iHitIndex]->Activate_Reflecting(
		m_pTransformCom->Get_State(CTransform::STATE_LOOK), Get_ArrowHeadPos(), 80.0f);

	m_iHitIndex++;
	m_iHitIndex %= MAX_HIT_EFFECTS;
}

_float4 CHunterArrow::Get_ArrowHeadPos()
{
	return  XMVectorSetW(XMVector3TransformCoord(m_vColliderPivotPos, m_pTransformCom->Get_WorldMatrix()), 1.f);
}

HRESULT CHunterArrow::SetUp_Effects()
{
	for (_uint i = 0; i < MAX_TRAIL_EFFECTS; ++i)
	{
		CEffect_Base_S2* pEffect = nullptr;

		pEffect = static_cast<CEffect_Base_S2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Particle_Base",
			CUtile::Create_DummyString(), L"Particle_ArrowTrail"));

		if (pEffect != nullptr)
			m_vecEffects[EFFECT_TRAIL].push_back(pEffect);
		else
			return E_FAIL;
	}

	for (_uint i = 0; i < MAX_RING_EFFECTS; ++i)
	{
		CEffect_Base_S2* pEffect = nullptr;

		pEffect = static_cast<CEffect_Base_S2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Mesh_Base",
			CUtile::Create_DummyString(), L"Mesh_ShockRing"));

		if (pEffect != nullptr)
			m_vecEffects[EFFECT_RING].push_back(pEffect);
		else
			return E_FAIL;
	}

	for (_uint i = 0; i < MAX_HIT_EFFECTS; ++i)
	{
		CEffect_Base_S2* pEffect = nullptr;

		pEffect = static_cast<CEffect_Base_S2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Particle_Base",
			CUtile::Create_DummyString(), L"Particle_ArrowHit"));

		if (pEffect != nullptr)
			m_vecEffects[EFFECT_HIT].push_back(pEffect);
		else
			return E_FAIL;
	}

	return S_OK;
}

void CHunterArrow::Reset_Effects()
{
	for(_uint i=0;i < EFFECT_END;++i)
	{
		for (auto& eff : m_vecEffects[i])
		{
			Safe_Release(eff);
			m_vecEffects[i].clear();
		}

	}

	SetUp_Effects();
}

_int CHunterArrow::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (m_eArrowState == FIRE)
	{
		if (m_pHunter && m_pHunter->Get_AnimationIndex() == 29 /*SHOCK_ARROW_ATTACK */)
		{
			_float4 vArroaHeadPos = Get_ArrowHeadPos();
			vArroaHeadPos.y += 1.f;
			m_pSwipesCharged->Set_Effect(vArroaHeadPos, true);
		}

		m_eArrowState = FINISH;

		Play_HitEffect(0.0f);
		Play_FireFinishSound();
	}
	return 0;
}

void CHunterArrow::Play_FireFinishSound()
{
	_tchar* pSoundTable[FIRE_TYPE_END] = {
		m_pCopySoundKey[CSK_IMPACT],
		m_pCopySoundKey[CSK_HIT],
		m_pCopySoundKey[CSK_IMPACT2],
		m_pCopySoundKey[CSK_HIT],
	};

	m_pGameInstance->Play_Sound(pSoundTable[m_eArrowState], 0.5f);
}
