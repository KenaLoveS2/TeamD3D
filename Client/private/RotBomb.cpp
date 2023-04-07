#include "stdafx.h"
#include "..\public\RotBomb.h"
#include "GameInstance.h"
#include "Utile.h"
#include "AnimationState.h"
#include "Kena.h"
#include "Kena_State.h"
#include "Bone.h"
#include "E_KenaPulse.h"
#include "E_P_Explosion.h"
#include "E_BombTrail.h"
#include "Camera_Player.h"
#include "Kena_Status.h"

CRotBomb::CRotBomb(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CRotBomb::CRotBomb(const CRotBomb & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CRotBomb::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CRotBomb::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 10.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; 
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	assert(m_pKena != nullptr && "CRotBomb::Initialize()");

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_eEFfectDesc.bActive = false;

	return S_OK;
}

HRESULT CRotBomb::Late_Initialize(void * pArg)
{
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(m_fScale * 0.125f, 0.f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.f);

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

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER_BOMB));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	Set_Child();
	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);
		pChild->Late_Initialize(nullptr);
	}
	m_pPathTrail->Set_Parent(this);
	m_pPathTrail->Late_Initialize(nullptr);

	return S_OK;
}

void CRotBomb::Tick(_float fTimeDelta)
{
// 	if (m_szCloneObjectTag == L"RotBomb")
// 		ToolOption("CRotBomb");

	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
	if (fTimeRate != 1.f)
		fTimeDelta /= fTimeRate;

	if (m_eEFfectDesc.bActive == false)
		return;

	m_pPathTrail->Reset();
	m_eCurState = Check_State();
	Update_State(fTimeDelta);
	
	PxRigidActor*		pActor = m_pTransformCom->FindActorData(m_szCloneObjectTag)->pActor;
	PxShape*			pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxSphereGeometry& Geometry = pShape->getGeometry().sphere();
	Geometry.radius = m_fScale * 0.125f;
	pShape->setGeometry(Geometry);

	m_pTransformCom->Tick(fTimeDelta);
	m_pAnimation->Play_Animation(fTimeDelta);

	for (auto pChild : m_vecChild)
		pChild->Tick(fTimeDelta);

	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_pPathTrail->Set_Position(vPos);
	m_pPathTrail->Tick(fTimeDelta);
}

void CRotBomb::Late_Tick(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
	if (fTimeRate != 1.f)
		fTimeDelta /= fTimeRate;

	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	m_pPathTrail->Late_Tick(fTimeDelta);

	if(m_pRendererCom != nullptr && m_eCurState < CRotBomb::BOMB_BOOM)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}

	for (auto pChild : m_vecChild)
		pChild->Late_Tick(fTimeDelta);
}

HRESULT CRotBomb::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0 || i == 2) //  0 || 2 == Sphere
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_NoiseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_CloudTexture");

			if (i == 0)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		}

		if (i == 1 && m_bInject) //  1== inrot
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_AO_R_MTexture");

			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		}
	}
	return S_OK;
}

void CRotBomb::Imgui_RenderProperty()
{

}

void CRotBomb::ImGui_ShaderValueProperty()
{
}

void CRotBomb::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

void CRotBomb::ImGui_AnimationProperty()
{
	if (ImGui::Button("Reset"))
		Reset();

	m_pPathTrail->Imgui_RenderProperty();

	m_pTransformCom->Imgui_RenderProperty_ForJH();

	ImGui::Text("Current State : %d", (_int)m_eCurState);

	ImGui::BeginTabBar("RotBomb Animation & State");

	if (ImGui::BeginTabItem("Animation"))
	{
		m_pModelCom->Imgui_RenderProperty();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("State"))
	{
		m_pAnimation->ImGui_RenderProperty();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void CRotBomb::Reset()
{
	m_eCurState = BOMBSTATE_END;
	m_ePreState = BOMBSTATE_END;
	m_bInject = false;
	m_fScale = m_fInitScale;
	m_PathList.clear();
	m_bHit = false;
	m_pTarget = nullptr;
	m_vHitPosition = _float4::One;
	m_bBoom = false;
	m_fBoomTimer = 0.f;
	m_pAnimation->State_Animation("CHARGE");

	m_eEFfectDesc.bActive = false;
	m_fTimeDelta = 0.f;

	m_pPathTrail->Set_Active(false);
	m_pPathTrail->Reset();
}

void CRotBomb::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_vecChild.reserve(CHILD_END);

	wstring strCloneTag = m_szCloneObjectTag;
	strCloneTag += L"_RotBombExplosion";

	_tchar* pTag = CUtile::Create_StringAuto(strCloneTag.c_str());
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RotBombExplosion", pTag));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);
	
	strCloneTag = m_szCloneObjectTag;
	strCloneTag += L"_RotBombExplosion";
	pTag = CUtile::Create_StringAuto(strCloneTag.c_str());
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Explosion_p", pTag));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	for (auto& pChild : m_vecChild)
		pChild->Set_Position(vPos);

	m_pPathTrail = dynamic_cast<CE_BombTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_BombTrail", L"RotBombTrail"));
	NULL_CHECK_RETURN(m_pPathTrail, );
	m_pPathTrail->Set_Position(vPos);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRotBomb::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_Rot_Bomb", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	/* Texture */
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 1) // Rot
		{
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Rot Bomb/T_RotStreakWithEyes_D.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_NORMALS, TEXT("../Bin/Resources/Anim/Rot Bomb/T_RotStreak_N.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(1, WJTextureType_AMBIENT_OCCLUSION, TEXT("../Bin/Resources/Anim/Rot Bomb/T_RotStreak_AO_R_M.png")), E_FAIL);
		}

		if (i == 0 || i == 2)
		{
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Rot Bomb/Noise_cloudsmed.png")), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(i, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Rot Bomb/T_NoiseClassic01.png")), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CRotBomb::SetUp_State()
{
	m_pModelCom->Set_RootBone("bomb_rig");
	m_pModelCom->Set_BoneIndex(L"../Bin/Data/Animation/RotBomb BoneInfo.json");
	m_pAnimation = CAnimationState::Create(this, m_pModelCom, "bomb_rig", "../Bin/Data/Animation/RotBomb.json");
	
	return S_OK;
}

HRESULT CRotBomb::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

HRESULT CRotBomb::SetUp_ShadowShaderResources()
{
	return S_OK;
}

CRotBomb::BOMBSTATE CRotBomb::Check_State()
{
	BOMBSTATE	eState = m_ePreState;
	const _uint	iKenaState = m_pKena->Get_AnimationStateIndex();

	if (m_eCurState == CRotBomb::BOMBSTATE_END)
	{
		m_pPathTrail->Set_Active(false);

		if (iKenaState == (_uint)CKena_State::BOMB_INTO_ADD)
		{
			eState = CRotBomb::BOMB_CHARGE;
			m_pAnimation->State_Animation("CHARGE");
			m_pPathTrail->Set_Active(true);
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_CHARGE)
	{
		if (iKenaState == (_uint)CKena_State::BOMB_LOOP_ADD)
		{
			eState = CRotBomb::BOMB_READY;
			m_pAnimation->State_Animation("CHARGE_LOOP");
			m_pPathTrail->Set_Active(true);
		}
		else if (iKenaState == (_uint)CKena_State::BOMB_INJECT_ADD)
		{
			eState = CRotBomb::BOMB_INJECT_CHARGE;
			m_pAnimation->State_Animation("INJECT");
			m_bInject = true;
			m_pPathTrail->Set_Active(true);
		}
		else if (iKenaState == (_uint)CKena_State::BOMB_RELEASE_ADD)
		{
			eState = CRotBomb::BOMB_RELEASE;
			m_pAnimation->State_Animation("CHARGE_LOOP");
			m_fScale = m_fMaxScale;
			m_pPathTrail->Set_Active(false);
			m_pPathTrail->Copy_Path(m_PathList);
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_INJECT_CHARGE)
	{
		if (m_pAnimation->Get_AnimationFinish() == true)
		{
			eState = CRotBomb::BOMB_READY;
			m_pAnimation->State_Animation("CHARGE_LOOP");
			m_pPathTrail->Set_Active(true);
		}
		
		if (iKenaState == (_uint)CKena_State::BOMB_RELEASE_ADD)
		{
			eState = CRotBomb::BOMB_RELEASE;
			m_pAnimation->State_Animation("CHARGE_LOOP");
			m_fScale = m_fInjectScale;
			m_pPathTrail->Set_Active(false);
			m_pPathTrail->Copy_Path(m_PathList);
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_READY)
	{
		if (iKenaState == (_uint)CKena_State::BOMB_INJECT_ADD)
		{
			if (m_bInject == false)
			{
				eState = CRotBomb::BOMB_INJECT_CHARGE;
				m_pAnimation->State_Animation("INJECT");
				m_bInject = true;
				m_pPathTrail->Set_Active(true);
			}
		}
		else if (iKenaState == (_uint)CKena_State::BOMB_RELEASE_ADD)
		{
			eState = CRotBomb::BOMB_RELEASE;
			m_pAnimation->State_Animation("CHARGE_LOOP");
			m_pPathTrail->Set_Active(false);
			m_pPathTrail->Copy_Path(m_PathList);
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_RELEASE)
	{
		if (m_bHit == true)
		{
			eState = CRotBomb::BOMB_LAND;
			m_pAnimation->State_Animation("LAND");
			m_fBoomTimer = 0.f;
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_LAND)
	{
		if (m_pAnimation->Get_AnimationFinish() == true && m_pAnimation->Get_CurrentAnimName() == "LAND")
			m_pAnimation->State_Animation("LAND_LOOP");

		if (m_fBoomTimer > m_fBoomTime && m_pAnimation->Get_CurrentAnimName() == "LAND_LOOP")
		{
			m_pAnimation->State_Animation("LAND_RUMBLE");

			if (m_pKena->Get_Status()->Get_SkillState(CKena_Status::SKILL_BOMB, 2) == true)
				m_pAnimation->Set_AnimationSpeed(50.f);
		}

		if (m_bBoom == true)
		{
			eState = CRotBomb::BOMB_BOOM;

			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			for (auto& pChild : m_vecChild)
			{
				pChild->Set_Active(true);
				pChild->Set_Position(vPos);
			}
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_BOOM)
	{
		if (m_vecChild[CHILD_COVER]->Get_Active() == false)
		{
			eState = CRotBomb::BOMBSTATE_END;
			Reset();
		}
	}

	return eState;
}

void CRotBomb::Update_State(_float fTimeDelta)
{
	CModel*	pModel = dynamic_cast<CModel*>(m_pKena->Find_Component(L"Com_Model"));
	CBone*	pHand = pModel->Get_BonePtr("rt_hand_socket_jnt");
	_matrix	matSocket;
	
	switch (m_eCurState)
	{
	case CRotBomb::BOMB_CHARGE:
		{
		matSocket = pHand->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
		_vector	vHandPos = matSocket.r[3];
		vHandPos = XMVectorSetY(vHandPos, XMVectorGetY(vHandPos) + 0.2f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vHandPos);

		Calculate_Path(fTimeDelta);

		m_fScale += fTimeDelta;
		CUtile::Saturate<_float>(m_fScale, m_fInitScale, m_fMaxScale);

		break;
		}
	case CRotBomb::BOMB_INJECT_CHARGE:
		{
		matSocket = pHand->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
		_vector	vHandPos = matSocket.r[3];
		vHandPos = XMVectorSetY(vHandPos, XMVectorGetY(vHandPos) + 0.2f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vHandPos);

		Calculate_Path(fTimeDelta);

		m_fScale += fTimeDelta;
		CUtile::Saturate<_float>(m_fScale, m_fInitScale, m_fInjectScale);

		break;
		}
	case CRotBomb::BOMB_READY:
		{
		matSocket = pHand->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
		_vector	vHandPos = matSocket.r[3];
		vHandPos = XMVectorSetY(vHandPos, XMVectorGetY(vHandPos) + 0.2f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vHandPos);

		Calculate_Path(fTimeDelta);

		break;
		}
	case CRotBomb::BOMB_RELEASE:
		{
		Throw(fTimeDelta);

		break;
		}
	case CRotBomb::BOMB_LAND:
		{
		_matrix	matTargetWorld = m_pTarget->Get_WorldMatrix();
		_vector	vHitWorldPos = XMVector3TransformCoord(m_vHitPosition, matTargetWorld);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vHitWorldPos);

		m_fBoomTimer += fTimeDelta;

		if (m_pAnimation->Get_AnimationFinish() == true)
			m_bBoom = true;

		break;
		}
	case CRotBomb::BOMB_BOOM:
		{
		break;
		}
	}

	m_pTransformCom->Set_Scaled(_float3(m_fScale, m_fScale, m_fScale));
}

void CRotBomb::Calculate_Path(_float fTimeDelta)
{
	if (dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr()) == nullptr)
		return;

	m_matDummy = m_pTransformCom->Get_WorldMatrix();

	_float3	vCamPos = CGameInstance::GetInstance()->Get_CamPosition_Float3();
	_float3	vCamLook = CGameInstance::GetInstance()->Get_CamLook_Float3();
	_float3	vCamRight = CGameInstance::GetInstance()->Get_CamRight_Float3();
	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3	vScale = m_pTransformCom->Get_Scaled();
	_float4	vTargetPos;

	if (CPhysX_Manager::GetInstance()->Raycast_Collision(vCamPos, vCamLook, 10.f, &m_vAimPos))
	{
		vTargetPos = XMVectorSetW(m_vAimPos, 1.f);
		m_pTransformCom->LookAt(vTargetPos);
	}
	else
	{
		vCamLook.Normalize();
		vTargetPos = XMVectorSetW(vCamPos + vCamLook * 10.f, 1.f);
		m_pTransformCom->LookAt(vTargetPos);		
	}

 	m_pPathTrail->Add_BezierCurve(vPos, vTargetPos, fTimeDelta);
}

void CRotBomb::Throw(_float fTimeDelta)
{
	if (m_PathList.empty() == true)
	{
		m_pTransformCom->Projectile_Motion(XMConvertToRadians(170.f), fTimeDelta);
		m_pTransformCom->Go_Straight(fTimeDelta);
		return;
	}

	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	vTarget = m_PathList.front();
	_float4	vDir = vTarget - vPos;

	if (XMVectorGetX(XMVector3Length(vDir)) == 0.f)
	{
		m_PathList.pop_front();

		if (m_PathList.empty() == true)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
			return;
		}

		vTarget = m_PathList.front();
	}

	m_pTransformCom->LookAt(vTarget);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTarget);

// 	_float4	vDist = XMVector3Normalize(vDir) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta;
// 
// 	_float4	vMovedPos = vPos + vDist;
// 	_float4	vAfterDir = vTarget - vMovedPos;
// 
// 	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vDir, 0.f)), XMVector3Normalize(XMVectorSetY(vAfterDir, 0.f)))));
// 	if (fAngle == XM_PI)
// 	{
//  		m_PathList.pop_front();
// // 
// // 		Throw(fTimeDelta);
// 		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTarget);
// 	}
// 	else
// 		m_pTransformCom->Set_Translation(vMovedPos, vDist);
}

_int CRotBomb::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget == nullptr || iColliderIndex == (_int)COLLISON_DUMMY || iColliderIndex == (_int)COL_MONSTER || iColliderIndex == (_int)COL_ELETE_MONSTER || iColliderIndex == (_int)COL_BOSS_MONSTER || iColliderIndex == (_int)COL_ENVIROMENT)
	{
		if (m_bHit == false)
		{
			if (pTarget == nullptr)
			{
				Reset();
				return 0;
			}

			m_bHit = true;
			m_pTarget = pTarget;
			m_vHitPosition = XMVector3TransformCoord(XMVectorSetW(vCollisionPos, 1.f), pTarget->Get_TransformCom()->Get_WorldMatrix_Inverse());
		}
	}

	if (iColliderIndex == (_int)COL_PLAYER && m_eCurState == CRotBomb::BOMB_LAND && m_pKena->Get_Status()->Get_SkillState(CKena_Status::SKILL_BOMB, 1) == true)
		m_bBoom = true;

	return 0;
}

_int CRotBomb::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iTriggerIndex == (_int)ON_TRIGGER_PARAM_ACTOR)
	{
		if (iColliderIndex == (_int)TRIGGER_PULSE && m_eCurState == CRotBomb::BOMB_LAND)
			m_bBoom = true;
	}

	return 0;
}

CRotBomb * CRotBomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRotBomb*	pInstance = new CRotBomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRotBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotBomb::Clone(void * pArg)
{
	CRotBomb*	pInstance = new CRotBomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRotBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotBomb::Free()
{
	__super::Free();

	Safe_Release(m_pAnimation);
	Safe_Release(m_pPathTrail);
}
