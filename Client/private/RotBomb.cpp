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
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
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
	assert(m_pKena != nullptr && "CFireBullet::Initialize()");

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_eEFfectDesc.bActive = false;

	return S_OK;
}

HRESULT CRotBomb::Late_Initialize(void * pArg)
{
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(m_fScale * 0.25f, 0.f, 1.f);
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

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER_ARROW));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	Set_Child();
	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);
		pChild->Late_Initialize(nullptr);
	}

	return S_OK;
}

void CRotBomb::Tick(_float fTimeDelta)
{
	_float		fTimeRate = CGameInstance::GetInstance()->Get_TimeRate(L"Timer_60");
	if (fTimeRate != 1.f)
		fTimeDelta /= fTimeRate;
	
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	m_eCurState = Check_State();
	Update_State(fTimeDelta);
	
	PxRigidActor*		pActor = m_pTransformCom->FindActorData(m_szCloneObjectTag)->pActor;
	PxShape*			pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxSphereGeometry& Geometry = pShape->getGeometry().sphere();
	Geometry.radius = m_fScale * 0.25f;
	pShape->setGeometry(Geometry);

	m_pTransformCom->Tick(fTimeDelta);

	m_pAnimation->Play_Animation(fTimeDelta);
}

void CRotBomb::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if(m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}
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

		if (i == 1) //  1== inrot
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
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_eEFfectDesc.vColor;

	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	if (ImGui::Button("Bomb"))
	{
		m_bBoom = true;
		m_eEFfectDesc.bActive = true;
	}

	if (ImGui::Button("ChildActive false"))
	{
		m_bBoom = false;
		m_eEFfectDesc.bActive = false;
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);
	}

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
	m_fScale = m_fInitScale;
	m_bHit = false;
	m_pTarget = nullptr;
	m_vHitPosition = _float4::One;
	m_bBoom = false;
	m_fBoomTimer = 0.f;
	m_pAnimation->State_Animation("CHARGE");

	m_eEFfectDesc.bActive = false;
	m_fTimeDelta = 0.f;
}

void CRotBomb::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_vecChild.reserve(CHILD_END);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RotBombExplosion", L"RotBombExplosion"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);
	
	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Explosion_p", L"RotBombExplosion_P"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	for (auto& pChild : m_vecChild)
		pChild->Set_Position(vPos);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRotBomb::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_Rot_Bomb", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

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
		if (iKenaState == (_uint)CKena_State::BOMB_INTO_ADD)
		{
			eState = CRotBomb::BOMB_CHARGE;
			m_pAnimation->State_Animation("CHARGE");
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_CHARGE)
	{
		if (iKenaState == (_uint)CKena_State::BOMB_LOOP_ADD)
		{
			eState = CRotBomb::BOMB_READY;
			m_pAnimation->State_Animation("CHARGE_LOOP");
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_READY)
	{
		if (iKenaState == (_uint)CKena_State::BOMB_RELEASE_ADD)
		{
			eState = CRotBomb::BOMB_RELEASE;
			m_pAnimation->State_Animation("CHARGE_LOOP");
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

		if (m_fBoomTimer > 4.5f && m_pAnimation->Get_CurrentAnimName() == "LAND_LOOP")
			m_pAnimation->State_Animation("LAND_RUMBLE");

		if (m_bBoom == true)
		{
			eState = CRotBomb::BOMB_BOOM;
			m_pAnimation->State_Animation("LAND_RUMBLE");
		}
	}
	else if (m_eCurState == CRotBomb::BOMB_BOOM)
	{
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		for (auto& pChild : m_vecChild)
		{
			pChild->Set_Active(true);
			pChild->Set_Position(vPos);
		}

		Reset();
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

		m_fScale += fTimeDelta;
		CUtile::Saturate<_float>(m_fScale, m_fInitScale, m_fMaxScale);

		break;
		}
	case CRotBomb::BOMB_READY:
		{
		matSocket = pHand->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
		_vector	vHandPos = matSocket.r[3];
		vHandPos = XMVectorSetY(vHandPos, XMVectorGetY(vHandPos) + 0.2f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vHandPos);

		break;
		}
	case CRotBomb::BOMB_RELEASE:
		{
		m_pTransformCom->Go_Straight(fTimeDelta);

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

_int CRotBomb::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget == nullptr || iColliderIndex == (_int)COLLISON_DUMMY || iColliderIndex == (_int)COL_MONSTER || iColliderIndex == (_int)COL_ELETE_MONSTER || iColliderIndex == (_int)COL_BOSS_MONSTER)
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
}
