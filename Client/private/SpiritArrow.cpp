#include "stdafx.h"
#include "..\public\SpiritArrow.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Kena_Staff.h"
#include "Kena_State.h"
#include "Camera_Player.h"
#include "Bone.h"
#include "E_SpiritArrowTrail.h"

CSpiritArrow::CSpiritArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CSpiritArrow::CSpiritArrow(const CSpiritArrow & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CSpiritArrow::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSpiritArrow::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GaemObjectDesc;
	ZeroMemory(&GaemObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	GaemObjectDesc.TransformDesc.fSpeedPerSec = 20.f;
	GaemObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(45.f);

	FAILED_CHECK_RETURN(__super::Initialize(&GaemObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pStaff = dynamic_cast<CKena_Staff*>(m_pKena->Get_KenaPart(L"Kena_Staff"));	NULL_CHECK_RETURN(m_pStaff, E_FAIL);
	 
	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Find_Camera(L"PLAYER_CAM"));
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);

	Push_EventFunctions();

	m_iObjectProperty = OP_PLAYER_ARROW;
	m_eEFfectDesc.bActive = true;
	m_eEFfectDesc.iPassCnt = 5; // Effect_SpritArrow
	m_eEFfectDesc.fFrame[0] = 128.f;

	return S_OK;
}

HRESULT CSpiritArrow::Late_Initialize(void * pArg)
{
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(0.05f, 0.5f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.35f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.fMass = 59.f;
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PLAYER_WEAPON;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER_ARROW));

	_smatrix	matPivot = XMMatrixTranslation(0.f, 0.f, m_fScale * m_fScalePosRate);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);
	m_pRendererCom->Set_PhysXRender(true);

	Set_Child();

	for (auto& pChild : m_vecChild)
		pChild->Late_Initialize();

	return S_OK;
}

void CSpiritArrow::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActive == false)
		return;

	m_eCurState = Check_State();
	Update_State(fTimeDelta);

	_smatrix	matPivot = XMMatrixTranslation(0.f, 0.f, m_fScale * m_fScalePosRate);
	m_pTransformCom->FindActorData(m_szCloneObjectTag)->PivotMatrix = matPivot;
	
	m_pTransformCom->Tick(fTimeDelta);
}

void CSpiritArrow::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;
}

HRESULT CSpiritArrow::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0)  // 메인 앞콕
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
		else if (i == 1) // 손잡이
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 7);
		}
		else if (i == 2)   // 가운데 메쉬
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
	}

	return S_OK;
}

HRESULT CSpiritArrow::RenderShadow()
{
	return S_OK;
}

void CSpiritArrow::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_bActive);

	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		Reset();

	char	szState[16];

	if (m_eCurState == CSpiritArrow::ARROW_CHARGE)
		strcpy_s(szState, "CHARGE");
	else if (m_eCurState == CSpiritArrow::ARROW_READY)
		strcpy_s(szState, "READY");
	else if (m_eCurState == CSpiritArrow::ARROW_FIRE)
		strcpy_s(szState, "FIRE");
	else if (m_eCurState == CSpiritArrow::ARROW_HIT)
		strcpy_s(szState, "HIT");
	else
		strcpy_s(szState, "SLEEP");

	ImGui::Text("Current State : %s", szState);

	ImGui::DragFloat("Arrow Scale", &m_fScale, 0.01f, 1.f, 2.f);
	ImGui::DragFloat("Scale to Position Rate", &m_fScalePosRate, 0.001f, 0.f, 10.f);

	// Scale 1.f == Rate 0.35f
	// Scale 2.f == Rate 0.188f
	// 1.f				0.162f
}

void CSpiritArrow::ImGui_ShaderValueProperty()
{
}

void CSpiritArrow::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

void CSpiritArrow::Push_EventFunctions()
{
}

HRESULT CSpiritArrow::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_SpiritArrow", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	return S_OK;
}

HRESULT CSpiritArrow::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WaveHeight", &m_fWaveHeight, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Speed", &m_fSpeed, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WaveFrequency", &m_fWaveFrequency, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVSpeed", &m_fUVSpeed, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpiritArrow::SetUp_ShadowShaderResources()
{
	return S_OK;
}

CSpiritArrow::ARROWSTATE CSpiritArrow::Check_State()
{
	ARROWSTATE	eState = m_ePreState;
	const _uint		iKenaState = m_pKena->Get_AnimationStateIndex();

	if (m_bActive == false)
		return eState;

	if (m_eCurState == CSpiritArrow::ARROWSTATE_END)
	{
		m_vecChild[EFFECT_TRAIL]->Set_Active(false);

		if (iKenaState == (_uint)CKena_State::BOW_CHARGE_ADD ||
			iKenaState == (_uint)CKena_State::BOW_RECHARGE_ADD ||
			iKenaState == (_uint)CKena_State::BOW_AIR_CHARGE_ADD ||
			iKenaState == (_uint)CKena_State::BOW_AIR_RECHARGE_ADD)
		{
			eState = CSpiritArrow::ARROW_CHARGE;
			m_vecChild[EFFECT_POSITION]->Set_Active(true);
			dynamic_cast<CE_SpiritArrowTrail*>(m_vecChild[EFFECT_TRAIL])->ResetInfo();
		}
	}
	else if (m_eCurState == CSpiritArrow::ARROW_CHARGE)
	{
		if (iKenaState == (_uint)CKena_State::BOW_CHARGE_FULL_ADD ||
			iKenaState == (_uint)CKena_State::BOW_CHARGE_LOOP_ADD ||
			iKenaState == (_uint)CKena_State::BOW_AIR_CHARGE_LOOP_ADD)
			eState = CSpiritArrow::ARROW_READY;

		else if (iKenaState == (_uint)CKena_State::BOW_RELEASE_ADD ||
			iKenaState == (_uint)CKena_State::BOW_AIR_RELEASE_ADD)
		{
			eState = CSpiritArrow::ARROW_FIRE;
			m_fScale = m_fMaxScale;
			m_bReachToAim = false;
			m_vFirePosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_vFireCamLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamLook_Float4());
			m_vFireCamPos = CGameInstance::GetInstance()->Get_CamPosition();

			vector<CEffect_Base*>* pChilds = m_vecChild[EFFECT_POSITION]->Get_vecChild();
			for (auto pChild : *pChilds)
				pChild->Set_Active(false);

			m_vecChild[EFFECT_TRAIL]->Set_Active(true);
		}
		dynamic_cast<CE_SpiritArrowTrail*>(m_vecChild[EFFECT_TRAIL])->ResetInfo();
	}
	else if (m_eCurState == CSpiritArrow::ARROW_READY)
	{
		if (iKenaState == (_uint)CKena_State::BOW_RELEASE_ADD ||
			iKenaState == (_uint)CKena_State::BOW_AIR_RELEASE_ADD)
		{
			eState = CSpiritArrow::ARROW_FIRE;
			m_fScale = m_fMaxScale;
			m_bReachToAim = false;
			m_vFirePosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_vFireCamLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamLook_Float4());
			m_vFireCamPos = CGameInstance::GetInstance()->Get_CamPosition();

			vector<CEffect_Base*>* pChilds = m_vecChild[EFFECT_POSITION]->Get_vecChild();
			for (auto pChild : *pChilds)
				pChild->Set_Active(false);

			m_vecChild[EFFECT_TRAIL]->Set_Active(true);
		}
	}
	else if (m_eCurState == CSpiritArrow::ARROW_FIRE)
	{
		if (m_bHit == true)
		{
			eState = CSpiritArrow::ARROW_HIT;
			m_vecChild[EFFECT_TRAIL]->Set_Active(false);
			m_vecChild[EFFECT_POSITION]->Set_Active(false);
			m_vecChild[EFFECT_HIT]->Set_Active(true);
		}
	}
	else if (m_eCurState == CSpiritArrow::ARROW_HIT)
	{
		Reset();
	}

	return eState;
}

void CSpiritArrow::Update_State(_float fTimeDelta)
{
	CModel*	pModel = dynamic_cast<CModel*>(m_pKena->Find_Component(L"Com_Model"));
	CBone*	pHand = pModel->Get_BonePtr("bow_string_jnt_grip");
	CBone*	pStaffBone = pModel->Get_BonePtr("bow_string_jnt_grip");
	CBone*	pStaffHead = pModel->Get_BonePtr("staff_skin6_jnt");
	_matrix	matSocket;

	CUtile::Saturate<_float>(m_fScale, 1.f, m_fMaxScale);
	CUtile::Saturate<_float>(m_fScalePosRate, 0.188f, 0.35f);
	CUtile::Saturate<_float>(m_fDistance, m_fMinDistance, m_fMaxDistance);
	m_pTransformCom->Set_Scaled(CTransform::STATE_LOOK, m_fScale);

	switch (m_eCurState)
	{
	case CSpiritArrow::ARROW_CHARGE:
		{		
			matSocket = pStaffBone->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, matSocket.r[3]);

			matSocket = pStaffHead->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
			m_pTransformCom->LookAt(matSocket.r[3]);

			m_fScale += fTimeDelta;
			m_fScalePosRate -= fTimeDelta * 0.162f;
			m_fDistance = m_fScale * 10.f;

			CTransform::ActorData*	pActorData = m_pTransformCom->FindActorData(m_szCloneObjectTag);

 			_smatrix	matPivot = pActorData->PivotMatrix;
 			_float4	vColliderPos = (matPivot * m_pTransformCom->Get_WorldMatrix()).Translation();
 			vColliderPos.w = 1.f;

			/* Effects */
			m_vecChild[EFFECT_POSITION]->Set_Position(vColliderPos);

			_matrix matrWorld = m_vecChild[EFFECT_TRAIL]->Get_WorldMatrix();
			matrWorld.r[3] = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_vecChild[EFFECT_TRAIL]->Get_TransformCom()->Set_WorldMatrix(matrWorld);

			break;
		}
	case CSpiritArrow::ARROW_READY:
		{
			matSocket = pStaffBone->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, matSocket.r[3]);

			matSocket = pStaffHead->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
			m_pTransformCom->LookAt(matSocket.r[3]);

			CTransform::ActorData*	pActorData = m_pTransformCom->FindActorData(m_szCloneObjectTag);

			_smatrix	matPivot = pActorData->PivotMatrix;
			_float4	vColliderPos = (matPivot * m_pTransformCom->Get_WorldMatrix()).Translation();
			vColliderPos.w = 1.f;

			/* Effects */
			m_vecChild[EFFECT_POSITION]->Set_Position(vColliderPos);
			break;
		}
	case CSpiritArrow::ARROW_FIRE:
		{
			_float4	vTargetPos = m_vFireCamPos + m_vFireCamLook * m_fDistance;

			m_pTransformCom->Arrow(vTargetPos, m_vFirePosition, XMConvertToRadians(160.f), fTimeDelta, m_bReachToAim);
			m_pTransformCom->Go_Straight(fTimeDelta);

			CTransform::ActorData*	pActorData = m_pTransformCom->FindActorData(m_szCloneObjectTag);

			_smatrix	matPivot = pActorData->PivotMatrix;
			_float4	vColliderPos = (matPivot * m_pTransformCom->Get_WorldMatrix()).Translation();
			vColliderPos.w = 1.f;

			/* Effects */
			m_vecChild[EFFECT_POSITION]->Set_Position(vColliderPos);

			_matrix matrWorld = m_vecChild[EFFECT_TRAIL]->Get_WorldMatrix();
			matrWorld.r[3] = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_vecChild[EFFECT_TRAIL]->Get_TransformCom()->Set_WorldMatrix(matrWorld);
			break;
		}
	case CSpiritArrow::ARROW_HIT:
		{
			Reset();
			break;
		}
	}
}

_int CSpiritArrow::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (m_eCurState < CSpiritArrow::ARROW_FIRE)
		return 0;

	if (pTarget == nullptr || iColliderIndex == COLLISON_DUMMY || iColliderIndex == (_int)COL_GROUND || iColliderIndex == (_int)COL_ENVIROMENT)
	{
		m_bHit = true;

		_vector	vPos = m_pKena->Get_WorldMatrix().r[3];
		_float	fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - vPos));
		int a = 0;
	}

	/* Collision Test */
	if (pTarget == nullptr || iColliderIndex == COLLISON_DUMMY || iColliderIndex == (_int)COL_GROUND || iColliderIndex == (_int)COL_ENVIROMENT || iColliderIndex == (_int)COL_MONSTER)
	{
		m_bHit = true;
		m_vecChild[EFFECT_HIT]->Set_Position(vCollisionPos);
	}

	return 0;
}

void CSpiritArrow::Reset()
{
	m_bActive = false;
	m_bHit = false;
	m_bReachToAim = false;
	m_fScale = 1.f;
	m_fScalePosRate = 0.35f;
	m_eCurState = CSpiritArrow::ARROWSTATE_END;
	m_ePreState = CSpiritArrow::ARROWSTATE_END;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -3.f, 0.f, 1.f));

	for (auto pEffect : m_vecChild)
		pEffect->Set_Active(false);

	dynamic_cast<CE_SpiritArrowTrail*>(m_vecChild[EFFECT_TRAIL])->ResetInfo();
}

void CSpiritArrow::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	wstring strArrowPosCloneTag = L"";
	strArrowPosCloneTag = m_szCloneObjectTag;
	strArrowPosCloneTag += L"_Pos";

	_tchar* pPosCloneTag = CUtile::Create_StringAuto(strArrowPosCloneTag.c_str());

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_SpiritArrowPosition", pPosCloneTag));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	wstring strArrowHitCloneTag = L"";
	strArrowHitCloneTag = m_szCloneObjectTag;
	strArrowHitCloneTag += L"_Hit";

	_tchar* pHitCloneTag = CUtile::Create_StringAuto(strArrowHitCloneTag.c_str());

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_SpiritArrowHit", pHitCloneTag));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	/* Trail */
	wstring strArrowTrailCloneTag = L"";
	strArrowTrailCloneTag = m_szCloneObjectTag;
	strArrowTrailCloneTag += L"_Trail";

	_tchar* pTrailCloneTag = CUtile::Create_StringAuto(strArrowTrailCloneTag.c_str());

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_SpiritArrowTrail", pTrailCloneTag));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

CSpiritArrow * CSpiritArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpiritArrow*	pInstance = new CSpiritArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSpiritArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpiritArrow::Clone(void * pArg)
{
	CSpiritArrow*	pInstance = new CSpiritArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSpiritArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpiritArrow::Free()
{
	__super::Free();
}
