#include "stdafx.h"
#include "..\public\SpiritArrow.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Kena_Staff.h"
#include "Camera_Player.h"
#include "Bone.h"

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

	ImGui::Begin("Arrow");

	ImGui::InputFloat("Frame", (_float*)&m_eEFfectDesc.fFrame);
	ImGui::InputFloat("Mask", (_float*)&m_eEFfectDesc.fMaskFrame);

	if (ImGui::Button("DotConfirm"))
		m_pShaderCom->ReCompile();

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

	ImGui::End();

}

void CSpiritArrow::Late_Tick(_float fTimeDelta)
{
	if (m_bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	//if (m_pRendererCom != nullptr)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSpiritArrow::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0) // 메인 앞콕
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
		}
		else if (i == 1) // 손잡이
		{
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 7);
		}
		else if (i == 2) // 가운데 메쉬 
		{
			continue;
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 8);
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

	return S_OK;
}

HRESULT CSpiritArrow::SetUp_ShadowShaderResources()
{
	return S_OK;
}

CSpiritArrow::ARROWSTATE CSpiritArrow::Check_State()
{
	ARROWSTATE	eState = m_ePreState;
	const string&		strKenaState = m_pKena->Get_AnimationState();

	if (m_bActive == false)
		return eState;

	if (m_eCurState == CSpiritArrow::ARROWSTATE_END)
	{
		if (strKenaState == "BOW_CHARGE" || strKenaState == "BOW_RECHARGE")
			eState = CSpiritArrow::ARROW_CHARGE;
	}
	else if (m_eCurState == CSpiritArrow::ARROW_CHARGE)
	{
		if (strKenaState == "BOW_CHARGE_FULL" || strKenaState == "BOW_CHARGE_LOOP")
			eState = CSpiritArrow::ARROW_READY;
		else if (strKenaState == "BOW_RELEASE")
		{
			eState = CSpiritArrow::ARROW_FIRE;
			m_fScale = m_fMaxScale;
			m_bReachToAim = false;
			m_vFirePosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_vFireCamLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamLook_Float4());
			m_vFireCamPos = CGameInstance::GetInstance()->Get_CamPosition();
		}
	}
	else if (m_eCurState == CSpiritArrow::ARROW_READY)
	{
		if (strKenaState == "BOW_RELEASE")
		{
			eState = CSpiritArrow::ARROW_FIRE;
			m_fScale = m_fMaxScale;
			m_bReachToAim = false;
			m_vFirePosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_vFireCamLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamLook_Float4());
			m_vFireCamPos = CGameInstance::GetInstance()->Get_CamPosition();
		}
	}
	else if (m_eCurState == CSpiritArrow::ARROW_FIRE)
	{
		if (m_bHit == true)
			eState = CSpiritArrow::ARROW_HIT;
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

		break;
		}
	case CSpiritArrow::ARROW_READY:
		{
		matSocket = pStaffBone->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, matSocket.r[3]);

		matSocket = pStaffHead->Get_CombindMatrix() * pModel->Get_PivotMatrix() * m_pKena->Get_WorldMatrix();
		m_pTransformCom->LookAt(matSocket.r[3]);

		break;
		}
	case CSpiritArrow::ARROW_FIRE:
		{
		_float4	vTargetPos = m_vFireCamPos + m_vFireCamLook * m_fDistance;

		m_pTransformCom->Arrow(vTargetPos, m_vFirePosition, XMConvertToRadians(160.f), fTimeDelta, m_bReachToAim);
		m_pTransformCom->Go_Straight(fTimeDelta);

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

	if (pTarget == nullptr || iColliderIndex == COLLISON_DUMMY || iColliderIndex == COL_GROUND || iColliderIndex == COL_ENVIROMENT)
	{
		m_bHit = true;

		_vector	vPos = m_pKena->Get_WorldMatrix().r[3];
		_float	fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - vPos));
		int a = 0;
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
}

CSpiritArrow * CSpiritArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpiritArrow*	pInstance = new CSpiritArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpiritArrow::Clone(void * pArg)
{
	CSpiritArrow*	pInstance = new CSpiritArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKena");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpiritArrow::Free()
{
	__super::Free();
}
