#include "stdafx.h"
#include "TelePort_Flower.h"
#include "GameInstance.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "PhysX_Manager.h"

CTelePort_Flower::CTelePort_Flower(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CTelePort_Flower::CTelePort_Flower(const CTelePort_Flower & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CTelePort_Flower::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTelePort_Flower::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	//CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);

	m_bRenderActive = true;

	CTransform::TRANSFORMDESC		Desc = m_pTransformCom->Get_TransformDesc();
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = XMConvertToRadians(45.f);
	m_pTransformCom->Set_TransformDesc(Desc);	

	_vector	vScale, vTrans;
	XMMatrixDecompose(&vScale, &m_vInitQuternion, &vTrans, m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Set_AnimIndex((_uint)CLOSE_LOOP);

	return S_OK;
}

HRESULT CTelePort_Flower::Late_Initialize(void * pArg)
{
	m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pKenaTransform = dynamic_cast<CTransform*>(m_pKena->Find_Component(L"Com_Transform"));
	NULL_CHECK_RETURN(m_pKenaTransform, E_FAIL);

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(0.8f, 0.95f, 1.2f);

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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_TELEPORT_FLOWER));
	pPhysX->Create_Trigger(Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_TELEPORT_FLOWER, vPos, 30.f));

	return S_OK;
}

void CTelePort_Flower::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

/*Culling*/
	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	//_vector camPos = XMLoadFloat4(&vCamPos);
	//const _vector	 vDir = camPos - vPos;
	//
	//_float f = XMVectorGetX(XMVector4Length(vDir));
	//if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
	//	m_bRenderCheck = false;
	//if(m_bRenderCheck ==true)
	//	m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 15.f);
	//if (m_bRenderCheck)
/*~Culling*/

	m_eCurState = Check_State();
	Update_State(fTimeDelta);
	
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTelePort_Flower::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if (m_pRendererCom )//&& m_bRenderActive && m_bRenderCheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTelePort_Flower::Render()
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

void CTelePort_Flower::Imgui_RenderProperty()
{
}

void CTelePort_Flower::ImGui_AnimationProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();
	m_pModelCom->Imgui_RenderProperty();
}

void CTelePort_Flower::ImGui_PhysXValueProperty()
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

_int CTelePort_Flower::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (m_bKenaDetected == false)
		return 0;

	if (iColliderIndex == (_uint)COL_PLAYER_ARROW)
		m_bArrowHit = true;

	return 0;
}

_int CTelePort_Flower::Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
		m_bKenaDetected = true;

	return 0;
}

_int CTelePort_Flower::Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (iColliderIndex == (_uint)COL_PLAYER)
		m_bKenaDetected = false;

	return 0;
}

HRESULT CTelePort_Flower::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

CTelePort_Flower::ANIMATION CTelePort_Flower::Check_State()
{
	ANIMATION	eState = m_ePreState;

	switch (eState)
	{
	case CTelePort_Flower::ACTIVATE:
		{
			if (m_pModelCom->Get_AnimationFinish())
			{
				eState = CTelePort_Flower::ACTIVATE_LOOP;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			if (m_bKenaDetected == false)
			{
				eState = CTelePort_Flower::CLOSE;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			if (m_bArrowHit == true)
			{
				eState = CTelePort_Flower::INTERACT;
				m_pModelCom->Set_AnimIndex((_uint)eState);
				m_pKena->TurnOn_TeleportFlower();
				m_pKenaTransform->LookAt_NoUpDown(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
				m_pTransformCom->LookAt(m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION));
				m_fLerpRatio = 0.f;
			}

			break;
		}

	case CTelePort_Flower::CLOSE:
		{
			if (m_pModelCom->Get_AnimationFinish())
			{
				eState = CTelePort_Flower::CLOSE_LOOP;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			if (m_bKenaDetected == true)
			{
				eState = CTelePort_Flower::ACTIVATE;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			break;
		}

	case CTelePort_Flower::CLOSE_LOOP:
		{
			if (m_bKenaDetected == true)
			{
				eState = CTelePort_Flower::ACTIVATE;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			break;
		}

	case CTelePort_Flower::INTERACT:
		{
			if (m_pModelCom->Get_AnimationFinish())
			{
				eState = CTelePort_Flower::ACTIVATE_LOOP;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			break;
		}

	case CTelePort_Flower::ACTIVATE_LOOP:
		{
			if (m_bKenaDetected == false)
			{
				eState = CTelePort_Flower::CLOSE;
				m_pModelCom->Set_AnimIndex((_uint)eState);
			}

			if (m_bArrowHit == true)
			{
				eState = CTelePort_Flower::INTERACT;
				m_pModelCom->Set_AnimIndex((_uint)eState);
				m_pKena->TurnOn_TeleportFlower();
				m_pKenaTransform->LookAt_NoUpDown(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
				m_pTransformCom->LookAt(m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION));
				m_fLerpRatio = 0.f;
			}

			break;
		}

	case CTelePort_Flower::ANIMATION_END:
		{
			eState = CTelePort_Flower::CLOSE_LOOP;
			m_pModelCom->Set_AnimIndex((_uint)eState);

			break;
		}
	}

	return eState;
}

void CTelePort_Flower::Update_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case CTelePort_Flower::ACTIVATE:
	{
		
		break;
	}

	case CTelePort_Flower::CLOSE:
	{
		

		break;
	}

	case CTelePort_Flower::CLOSE_LOOP:
	{
		

		break;
	}

	case CTelePort_Flower::INTERACT:
	{
		m_bArrowHit = false;

		/*_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector	vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
		_vector	vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_vector	vDir = m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vDir, 0.f)), XMVector3Normalize(XMVectorSetY(vLook, 0.f)))));

		if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vRight, 0.f)), XMVector3Normalize(XMVectorSetY(vDir, 0.f)))) < 0)
			fAngle = (XM_PI - fAngle) * -1.f;

		m_pTransformCom->Turn(vUp, fAngle * fTimeDelta);

		vLook = XMVector3Normalize(XMVectorSet(0.f, XMVectorGetY(vLook), 0.f, 0.f));
		vDir = XMVector3Normalize(XMVectorSet(0.f, XMVectorGetY(vDir), 0.f, 0.f));

		fAngle = acosf(XMVectorGetX(XMVector3Dot(vDir, vLook)));

		m_pTransformCom->Turn(vRight, fAngle * fTimeDelta);

		m_pTransformCom->LookAt(m_pKenaTransform->Get_State(CTransform::STATE_TRANSLATION));*/

		break;
	}

	case CTelePort_Flower::ACTIVATE_LOOP:
	{

		break;
	}
	}

	if (m_fLerpRatio < 1.f && m_eCurState != CTelePort_Flower::INTERACT)
	{
		_smatrix		matWorld = m_pTransformCom->Get_WorldMatrix();
		_vector		vScale, vRotate, vTrans;

		XMMatrixDecompose(&vScale, &vRotate, &vTrans, matWorld);

		vRotate = XMQuaternionSlerp(vRotate, m_vInitQuternion, m_fLerpRatio);

		m_fLerpRatio += fTimeDelta * 0.2f;
		CUtile::Saturate<_float>(m_fLerpRatio, 0.f, 1.f);

		m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), vRotate, vTrans));
	}
}

HRESULT CTelePort_Flower::SetUp_Components()
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
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_TeleportFlowerAnim"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom, nullptr, this)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CTelePort_Flower::SetUp_ShaderResources()
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

CTelePort_Flower * CTelePort_Flower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTelePort_Flower*		pInstance = new CTelePort_Flower(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTelePort_Flower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTelePort_Flower::Clone(void * pArg)
{
	CTelePort_Flower*		pInstance = new CTelePort_Flower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTelePort_Flower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTelePort_Flower::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
