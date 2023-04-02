#include "stdafx.h"
#include "HealthFlower_Anim.h"
#include "GameInstance.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "Kena_State.h"
#include "Kena_Status.h"
#include "Camera_Player.h"

CHealthFlower_Anim::CHealthFlower_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CHealthFlower_Anim::CHealthFlower_Anim(const CHealthFlower_Anim& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CHealthFlower_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHealthFlower_Anim::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	m_pTransformCom->Set_Position(XMVectorSet(-10.f, 0.f, -10.f, 1.f));

	m_pModelCom->Set_AnimIndex((_uint)CHealthFlower_Anim::OPEN_LOOP);

	return S_OK;
}

HRESULT CHealthFlower_Anim::Late_Initialize(void* pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.y += 0.4f;

	_float3 vSize = _float3(0.36f, 0.45f, 0.33f);

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = vSize;
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_HEALTHFLOWER));
	pPhysX->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_HEALTHFLOWER, vPos, 15.f));

	return S_OK;
}

void CHealthFlower_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_eCurState = Check_State();
	Update_State(fTimeDelta); 
	
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
}

void CHealthFlower_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHealthFlower_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (m_bUsed == true && i == 4)
			continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CHealthFlower_Anim::ImGui_AnimationProperty()
{
	if (ImGui::Button("Reset Anim"))
	{
		m_bUsed = false;
		m_eCurState = OPEN_LOOP;
	}

	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pModelCom->Imgui_RenderProperty();
}

void CHealthFlower_Anim::ImGui_PhysXValueProperty()
{
	PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_StaticActor(m_szCloneObjectTag);

	PxShape* pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxBoxGeometry& Geometry = pShape->getGeometry().box();
	PxVec3& fScale = Geometry.halfExtents;

	/* Scale */
	ImGui::BulletText("Scale Setting");
	ImGui::DragFloat("Scale X", &fScale.x, 0.01f);
	ImGui::DragFloat("Scale Y", &fScale.y, 0.01f);
	ImGui::DragFloat("Scale Z", &fScale.z, 0.01f);

	pShape->setGeometry(Geometry);
}

HRESULT CHealthFlower_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;
}

_int CHealthFlower_Anim::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
	{
		m_bKenaDetected = true;
		// m_pKena->Set_ChestInteractable(true);
	}

	return 0;
}

_int CHealthFlower_Anim::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
	{
		m_bKenaDetected = true;
		// m_pKena->Set_ChestInteractable(true);
	}

	return 0;
}

CHealthFlower_Anim::ANIMATION CHealthFlower_Anim::Check_State()
{
	ANIMATION	eState = m_ePreState;
	const _uint	iKenaState = m_pKena->Get_AnimationStateIndex();

	switch (eState)
	{
	case CHealthFlower_Anim::OPEN_LOOP:
		{
			if (m_bUsed == false && m_bInteractable == true)
			{
				if (iKenaState == (_uint)CKena_State::ROT_ACTION || iKenaState == (_uint)CKena_State::ROT_ACTION_AIM_STATE)
				{
					m_bUsed = true;
					eState = CHealthFlower_Anim::BIND;
					m_pModelCom->Set_AnimIndex((_uint)eState);
				}
			}

			break;
		}
	case CHealthFlower_Anim::BIND:
		{
			if (m_pModelCom->Get_AnimationFinish() == true)
			{
				eState = CHealthFlower_Anim::OPEN;
				m_pModelCom->Set_AnimIndex((_uint)eState);

				_int	iMaxHP = m_pKena->Get_Status()->Get_MaxHP();
				_int	iHalfAmount = _int(floor(iMaxHP / 2.f));
				m_pKena->Get_Status()->Add_HealAmount(iHalfAmount);

				/* PARTICLE */
			}

			break;
		}
	case CHealthFlower_Anim::OPEN:
		{
			if (m_pModelCom->Get_AnimationFinish() == true)
			{
				eState = CHealthFlower_Anim::OPEN_LOOP;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			break;
		}
	}

	return eState;
}

void CHealthFlower_Anim::Update_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case CHealthFlower_Anim::OPEN_LOOP:
		{
			if (m_bUsed == false)
			{
				m_bInteractable = Check_CameraRay();
				m_pKena->Set_RotActionPossible(m_bInteractable);
			}

			break;
		}
	case CHealthFlower_Anim::BIND:
		{
			break;
		}
	case CHealthFlower_Anim::OPEN:
		{
			break;
		}
	}
}

_bool CHealthFlower_Anim::Check_CameraRay()
{
	_float3		vCamPos = CGameInstance::GetInstance()->Get_CamPosition_Float3();
	_float3		vCamLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamLook_Float3());

	if (CPhysX_Manager::GetInstance()->Raycast_CollisionTarget(vCamPos, vCamLook, 15.f, nullptr, this))
	{
		m_pKena->Call_FocusRotIcon(this);
		return true;
	}

	return false;
}

HRESULT CHealthFlower_Anim::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = g_LEVEL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_HealthFlower"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom, nullptr, this)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CHealthFlower_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHealthFlower_Anim* CHealthFlower_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealthFlower_Anim* pInstance = new CHealthFlower_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHealthFlower_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHealthFlower_Anim::Clone(void* pArg)
{
	CHealthFlower_Anim* pInstance = new CHealthFlower_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHealthFlower_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHealthFlower_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
