#include "stdafx.h"
#include "Chest_Anim.h"
#include "GameInstance.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "Kena_State.h"
#include "PhysX_Manager.h"

CChest_Anim::CChest_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CChest_Anim::CChest_Anim(const CChest_Anim & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CChest_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChest_Anim::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	//CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	m_bRenderActive = true;

	m_pModelCom->Set_AnimIndex((_uint)CURSED_ACTIVATE);

	return S_OK;
}

HRESULT CChest_Anim::Late_Initialize(void * pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Find_Component(L"Com_Transform"));
	NULL_CHECK_RETURN(m_pKenaTransform, E_FAIL);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.y += 0.4f;

	_float3 vSize = _float3(0.54f, 0.42f, 0.36f);

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();
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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_CHEST));
	
	pPhysX->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_CHEST, vPos, 5.f));

	return S_OK;
}

void CChest_Anim::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	/*Culling*/
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_vector camPos = XMLoadFloat4(&vCamPos);
	const _vector	 vDir = camPos - vPos;

	_float f = XMVectorGetX(XMVector4Length(vDir));
	if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
		m_bRenderCheck = false;
	if (m_bRenderCheck == true)
		m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 100.f);



	if (m_bOpened == true)
		return;

#ifdef FOR_MAP_GIMMICK
	m_eCurState = Check_State();
	Update_State(fTimeDelta);
#endif
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CChest_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if (m_pRendererCom && m_bRenderActive && false == m_bRenderCheck  )
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CChest_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CChest_Anim::Imgui_RenderProperty()
{
}

void CChest_Anim::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pModelCom->Imgui_RenderProperty();
}

void CChest_Anim::ImGui_PhysXValueProperty()
{
	PxRigidActor*		pActor = CPhysX_Manager::GetInstance()->Find_StaticActor(m_szCloneObjectTag);

	PxShape*			pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxBoxGeometry& Geometry = pShape->getGeometry().box();
	PxVec3&	fScale = Geometry.halfExtents;

	/* Scale */
	ImGui::BulletText("Scale Setting");
	ImGui::DragFloat("Scale X", &fScale.x, 0.01f);
	ImGui::DragFloat("Scale Y", &fScale.y, 0.01f);
	ImGui::DragFloat("Scale Z", &fScale.z, 0.01f);

	pShape->setGeometry(Geometry);
}

HRESULT CChest_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
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

_int CChest_Anim::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

_int CChest_Anim::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
	{
		m_bKenaDetected = true;
		m_pModelCom->ResetAnimIdx_PlayTime((_uint)CChest_Anim::CURSED_ACTIVATE);
		m_pKena->Set_ChestInteractable(true);
	}

	return 0;
}

_int CChest_Anim::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
	{
		m_bKenaDetected = false;
		m_pKena->Set_ChestInteractable(true);
	}

	return 0;
}

CChest_Anim::ANIMATION CChest_Anim::Check_State()
{
	ANIMATION	eState = m_ePreState;
	const _uint	iKenaState = m_pKena->Get_AnimationStateIndex();

	switch (eState)
	{
	case CChest_Anim::CURSED_ACTIVATE:
		{
			if (iKenaState == (_uint)CKena_State::INTERACT_STAFF)
			{
				eState = CChest_Anim::OPEN;
				m_pModelCom->Set_AnimIndex((_uint)CChest_Anim::OPEN);
			}

			break;
		}

	case CChest_Anim::CURSED_CLEARED:
		{
			if (m_bKenaDetected == true)
			{
				eState = CChest_Anim::CURSED_ACTIVATE;
				m_pModelCom->Set_AnimIndex((_uint)CChest_Anim::CURSED_ACTIVATE);
			}
			
			break;
		}

	case CChest_Anim::OPEN:
		{
			if (m_pModelCom->Get_AnimationFinish() == true)
				m_bOpened = true;

			break;
		}

	case CChest_Anim::ANIMATION_END:
		{
			eState = CChest_Anim::CURSED_CLEARED;
			m_pModelCom->Set_AnimIndex((_uint)CChest_Anim::CURSED_CLEARED);

			break;
		}
	}

	return eState;
}

void CChest_Anim::Update_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
		case CChest_Anim::CURSED_ACTIVATE:
		{
			break;
		}

		case CChest_Anim::CURSED_CLEARED:
		{
			break;
		}

		case CChest_Anim::OPEN:
		{
			break;
		}

		case CChest_Anim::ANIMATION_END:
		{
			break;
		}
	}
}

HRESULT CChest_Anim::SetUp_Components()
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
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_ChestAnim"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom, nullptr, this)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CChest_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CChest_Anim * CChest_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChest_Anim*		pInstance = new CChest_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Chest_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChest_Anim::Clone(void * pArg)
{
	CChest_Anim*		pInstance = new CChest_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Chest_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
