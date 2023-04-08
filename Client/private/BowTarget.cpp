#include "stdafx.h"
#include "..\public\BowTarget.h"
#include "GameInstance.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "BowTarget.h"
#include "AnimationState.h"

CBowTarget::CBowTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnviromentObj(pDevice, pContext)
{
}

CBowTarget::CBowTarget(const CBowTarget& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CBowTarget::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CBowTarget::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);

	m_bRenderActive = true;

	CTransform::TRANSFORMDESC		Desc = m_pTransformCom->Get_TransformDesc();
	Desc.fSpeedPerSec = 2.5f;
	Desc.fRotationPerSec = XMConvertToRadians(90.f);
	m_pTransformCom->Set_TransformDesc(Desc);

	return S_OK;
}

HRESULT CBowTarget::Late_Initialize(void* pArg)
{
	m_vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vPivotScale = _float3(0.2f, 0.f, 1.f);
	_float3 vPivotPos = _float3(0.03f, 0.13f, 0.01f);

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
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_BOWTARGET));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	return S_OK;
}

void CBowTarget::Tick(_float fTimeDelta)
{
	if (m_bDead == true)
		return;

	__super::Tick(fTimeDelta);

	m_eCurState = Check_State();
	Update_State(fTimeDelta);

	m_pAnimation->Play_Animation(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);

// 	if (CGameInstance::GetInstance()->Key_Down(DIK_E))
// 		m_bLaunch = true;
// 	if (CGameInstance::GetInstance()->Key_Down(DIK_R))
// 		Reset();
// 	if (CGameInstance::GetInstance()->Key_Down(DIK_T))
// 		m_bArrowHit = true;
}

void CBowTarget::Late_Tick(_float fTimeDelta)
{
	if (m_bDead == true)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if (m_pRendererCom != nullptr && m_bRenderActive == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBowTarget::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_AO_R_MTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 34);
	}

	return S_OK;
}

void CBowTarget::Imgui_RenderProperty()
{
}

void CBowTarget::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pAnimation->ImGui_RenderProperty();
}

void CBowTarget::ImGui_PhysXValueProperty()
{
	PxRigidActor* pActor = m_pTransformCom->Get_ActorList()->front().pActor;
	PxShape* pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxSphereGeometry& Geometry = pShape->getGeometry().sphere();
	_float& fScaleX = Geometry.radius;
	ImGui::BulletText("Radius Setting");
	ImGui::DragFloat("Radius", &fScaleX, 0.01f);
	pShape->setGeometry(Geometry);

	CTransform::ActorData* pActorData = m_pTransformCom->FindActorData(m_szCloneObjectTag);
	_smatrix		matPivot = pActorData->PivotMatrix;
	_float4		vScale, vRot, vTrans;
	ImGuizmo::DecomposeMatrixToComponents((_float*)&matPivot, (_float*)&vTrans, (_float*)&vRot, (_float*)&vScale);

	ImGui::DragFloat3("Rotate", (_float*)&vRot, 0.01f);
	ImGui::DragFloat3("Translation", (_float*)&vTrans, 0.01f);

	ImGuizmo::RecomposeMatrixFromComponents((_float*)&vTrans, (_float*)&vRot, (_float*)&vScale, (_float*)&matPivot);

	pActorData->PivotMatrix = matPivot;
}

HRESULT CBowTarget::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

void CBowTarget::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

_int CBowTarget::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget == nullptr)
	{

	}
	else
	{
		_bool	bWork = (m_eCurState == CBowTarget::LAUNCH || m_eCurState == CBowTarget::LOOP || m_eCurState == CBowTarget::FAIL_LOOP);
		if (iColliderIndex == (_int)COL_PLAYER_ARROW && bWork == true)
			m_bArrowHit = true;
	}

	return 0;
}

_int CBowTarget::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

_int CBowTarget::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CBowTarget::ANIMATION CBowTarget::Check_State()
{
	ANIMATION	eState = m_ePreState;

	switch (eState)
	{
		case CBowTarget::REST:
		{
			if (m_bLaunch == true)
			{
				eState = CBowTarget::LAUNCH;
				m_pAnimation->State_Animation("LAUNCH");
			}

			break;
		}

		case CBowTarget::LAUNCH:
		{
			if (m_pAnimation->Get_AnimationFinish() == true)
			{
				eState = CBowTarget::LOOP;
				m_pAnimation->State_Animation("LOOP", 0.f);
			}

			if (m_bArrowHit == true)
			{
				eState = CBowTarget::HIT;
				m_pAnimation->State_Animation("HIT");
			}

			break;
		}

		case CBowTarget::LOOP:
		{
			if (m_fFlyTime <= 0.f)
			{
				eState = CBowTarget::FAIL_LOOP;
				m_pAnimation->State_Animation("FAIL_LOOP");
			}

			if (m_bArrowHit == true)
			{
				eState = CBowTarget::HIT;
				m_pAnimation->State_Animation("HIT");
			}

			break;
		}

		case CBowTarget::HIT:
		{
			if (m_fDissolveTime >= 1.f)
			{
				m_bDead = true;
				m_pTransformCom->Clear_Actor();
				//Reset();
			}

			break;
		}

		case CBowTarget::FAIL_LOOP:
		{
			if (m_pAnimation->Get_AnimationFinish() == true)
			{
				Reset();
			}

			if (m_bArrowHit == true)
			{
				eState = CBowTarget::HIT;
				m_pAnimation->State_Animation("HIT");
			}

			break;
		}
	}

	return eState;
}

void CBowTarget::Update_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
		case CBowTarget::REST:
		{
			break;
		}

		case CBowTarget::LAUNCH:
		{
			break;
		}

		case CBowTarget::LOOP:
		{
			m_fFlyTime -= fTimeDelta;
			break;
		}

		case CBowTarget::HIT:
		{
			m_fDissolveTime = m_pAnimation->Get_AnimationProgress();
			CUtile::Saturate<_float>(m_fDissolveTime, 0.f, 1.f);
			break;
		}

		case CBowTarget::FAIL_LOOP:
		{
			_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			if (XMVectorGetY(vPos) > m_vInitPosition.y)
				m_pTransformCom->Go_AxisNegY(fTimeDelta);
			else
				m_pTransformCom->Set_Position(m_vInitPosition);

			break;
		}
	}
}

void CBowTarget::Reset()
{
	m_bKenaDetected = false;
	m_bLaunch = false;
	m_bArrowHit = false;
	m_fFlyTime = 4.f;

	m_eCurState = CBowTarget::REST;
	m_ePreState = CBowTarget::REST;

	m_fDissolveTime = 0.f;

	m_pTransformCom->Set_Position(m_vInitPosition);
}

HRESULT CBowTarget::SetUp_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	/* For.Com_Shader */
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = g_LEVEL;

	/* For.Com_Model */
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_BowTarget", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_COMP_E_R_AO, TEXT("../Bin/Resources/Anim/BowTarget/T_BowTarget_Comp_E_R_AO.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_EMISSIVE, TEXT("../Bin/Resources/Anim/BowTarget/T_BowTarget_E.png")), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	/* For.Com_DISSOLVETEX */
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Component_Texture_Dissolve_BowTarget", L"COM_DISSOLVETEX", (CComponent**)&m_pDissolveTexture), E_FAIL);
	

	return S_OK;
}

HRESULT CBowTarget::SetUp_State()
{
	m_pModelCom->Set_RootBone("WhirlyBird");
	m_pAnimation = CAnimationState::Create(this, m_pModelCom, "WhirlyBird", "../Bin/Data/Animation/BowTarget.json");
	m_pAnimation->State_Animation("REST", 0.f);

	return S_OK;
}

HRESULT CBowTarget::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bArrowHit, sizeof(_bool)), E_FAIL);

	m_bArrowHit && Bind_Dissolve(m_pShaderCom);

	return S_OK;
}

HRESULT CBowTarget::Bind_Dissolve(CShader* pShader)
{
	FAILED_CHECK_RETURN(pShader->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pDissolveTexture->Bind_ShaderResource(pShader, "g_DissolveTexture"), E_FAIL);

	return S_OK;
}

CBowTarget* CBowTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBowTarget* pInstance = new CBowTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBowTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBowTarget::Clone(void* pArg)
{
	CBowTarget* pInstance = new CBowTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBowTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBowTarget::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAnimation);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
