#include "stdafx.h"
#include "BombPlatform.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "Utile.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "RotBomb.h"

CBombPlatform::CBombPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnviromentObj(pDevice, pContext)
{
}

CBombPlatform::CBombPlatform(const CBombPlatform& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CBombPlatform::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CBombPlatform::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CBombPlatform::Late_Initialize(void* pArg)
{
	m_vInitPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector		vScale, vTrans;
	XMMatrixDecompose(&vScale, (_vector*)&m_vInitQuat, &vTrans, m_pTransformCom->Get_WorldMatrix());

	/* CloneTag로 m_vMovingPos 다르게 주면 돼 */
	m_vMovingPos = _float4(50.f, 1.f, 35.f, 1.f);
	m_vMovingQuat = XMQuaternionIdentity();
	m_fReturnTime = 5.f;

	_float3	vPos = m_vInitPos;
	_float3	vPivotScale = _float3(3.2f, 1.45f, 3.51f);
	_float3	vPivotPos = _float3(0.f, 0.f, 0.f);

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);

	CPhysX_Manager::PX_BOX_DESC PxBoxDesc;
	ZeroMemory(&PxBoxDesc, sizeof(CPhysX_Manager::PX_BOX_DESC));
	PxBoxDesc.pActortag = m_szCloneObjectTag;
	PxBoxDesc.eType = BOX_DYNAMIC;
	PxBoxDesc.vPos = vPos;
	PxBoxDesc.vSize = vPivotScale;
	PxBoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	PxBoxDesc.fDegree = 0.f;
	PxBoxDesc.isGravity = false;
	PxBoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
	PxBoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxBoxDesc.fDensity = 1.f;
	PxBoxDesc.fAngularDamping = 0.5f;
	PxBoxDesc.fMass = 0.1f;
	PxBoxDesc.fLinearDamping = 0.1f;
	PxBoxDesc.bCCD = false;
	PxBoxDesc.fDynamicFriction = 0.5f;
	PxBoxDesc.fStaticFriction = 0.5f;
	PxBoxDesc.fRestitution = 0.1f;
	PxBoxDesc.isTrigger = false;

	CPhysX_Manager::GetInstance()->Create_Box(PxBoxDesc, Create_PxUserData(this, false, COL_ENVIROMENT));
	m_pTransformCom->Add_Collider(m_szCloneObjectTag, matPivot);

	return S_OK;
}

void CBombPlatform::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_eCurState = Check_State();
	Update_State(fTimeDelta);

	m_pTransformCom->Tick(fTimeDelta);
}

void CBombPlatform::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_ePreState != m_eCurState)
		m_ePreState = m_eCurState;

	if (m_pRendererCom != nullptr && m_bRenderActive == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBombPlatform::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 4), E_FAIL);
	}

	return S_OK;
}

void CBombPlatform::Imgui_RenderProperty()
{
	ImGui_PhysXValueProperty();

	if (ImGui::Button("Reset Platform"))
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_vInitPos);
		m_fTimer = 0.f;
		m_eCurState = CBombPlatform::STATE_SLEEP;
	}
}

void CBombPlatform::ImGui_PhysXValueProperty()
{
	PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_DynamicCollider(m_szCloneObjectTag);

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

_int CBombPlatform::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (iColliderIndex == (_int)COL_PLAYER_BOMB && m_pDetectedBomb == nullptr)
		m_pDetectedBomb = dynamic_cast<CRotBomb*>(pTarget);

	if (iColliderIndex == (_int)COL_PLAYER_BOMB_EXPLOSION && m_eCurState == CBombPlatform::STATE_SLEEP)
		m_eCurState = CBombPlatform::STATE_OPEN;

	return 0;
}

_int CBombPlatform::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

_int CBombPlatform::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CBombPlatform::STATE CBombPlatform::Check_State()
{
	STATE	eState = m_ePreState;

	switch (eState)
	{
		case CBombPlatform::STATE_SLEEP:
		{
			if (m_pDetectedBomb != nullptr)
			{
				if (m_pDetectedBomb->Get_CurrentState() == CRotBomb::BOMB_BOOM)
				{
					eState = CBombPlatform::STATE_OPEN;
					m_pDetectedBomb = nullptr;
				}
			}

			break;
		}

		case CBombPlatform::STATE_OPEN:
		{
			_float4		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			if (vPos == m_vMovingPos)
			{
				eState = CBombPlatform::STATE_ACTIVATE;
				m_fTimer = 0.f;
			}

			break;
		}

		case CBombPlatform::STATE_ACTIVATE:
		{
			if (m_fTimer >= m_fReturnTime)
				eState = CBombPlatform::STATE_CLOSE;

			break;
		}

		case CBombPlatform::STATE_CLOSE:
		{
			_float4		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			if (vPos == m_vInitPos)
			{
				eState = CBombPlatform::STATE_SLEEP;
				m_fTimer = 0.f;
			}

			break;
		}
	}

	return eState;
}

void CBombPlatform::Update_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
		case CBombPlatform::STATE_SLEEP:
		{
			break;
		}

		case CBombPlatform::STATE_OPEN:
		{
			_float4		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			_float		fMaxLength = (m_vMovingPos - m_vInitPos).Length();
			_float		fCurLength = (vPos - m_vInitPos).Length();
			_float		fRatio = fCurLength / fMaxLength;
			//CUtile::Saturate<_float>(fRatio, 0.f, 1.f);

			if (fRatio <= 1.f)
			{
				_float4		vDir = XMVector3Normalize(m_vMovingPos - m_vInitPos);
				vPos += vDir;

				_vector		vScale, vQuat, vTrans;
				_vector		vCurQuat = XMQuaternionSlerp(m_vInitQuat, m_vMovingQuat, fRatio);
				_matrix		matWorld = m_pTransformCom->Get_WorldMatrix();
				XMMatrixDecompose(&vScale, &vQuat, &vTrans, matWorld);
				matWorld = XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), vCurQuat, vTrans);
				m_pTransformCom->Set_WorldMatrix(matWorld);
			}
			else
			{
				vPos = m_vMovingPos;
				fRatio = 1.f;

				_vector		vScale, vQuat, vTrans;
				_matrix		matWorld = m_pTransformCom->Get_WorldMatrix();
				XMMatrixDecompose(&vScale, &vQuat, &vTrans, matWorld);
				matWorld = XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), m_vMovingQuat, vTrans);
				m_pTransformCom->Set_WorldMatrix(matWorld);
			}

			//vPos = _float4::Lerp(m_vInitPos, m_vMovingPos, fRatio);

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

			break;
		}

		case CBombPlatform::STATE_ACTIVATE:
		{
			m_fTimer += fTimeDelta;
			break;
		}

		case CBombPlatform::STATE_CLOSE:
		{
			_float4		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			_float		fMaxLength = (m_vMovingPos - m_vInitPos).Length();
			_float		fCurLength = (m_vMovingPos - vPos).Length();
			_float		fRatio = fCurLength / fMaxLength;
			//CUtile::Saturate<_float>(fRatio, 0.f, 1.f);

			if (fRatio <= 1.f)
			{
				_float4		vDir = XMVector3Normalize(m_vInitPos - m_vMovingPos);
				vPos += vDir;

				_vector		vScale, vQuat, vTrans;
				_vector		vCurQuat = XMQuaternionSlerp(m_vMovingQuat, m_vInitQuat, fRatio);
				_matrix		matWorld = m_pTransformCom->Get_WorldMatrix();
				XMMatrixDecompose(&vScale, &vQuat, &vTrans, matWorld);
				matWorld = XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), vCurQuat, vTrans);
				m_pTransformCom->Set_WorldMatrix(matWorld);
			}
			else
			{
				vPos = m_vInitPos;
				fRatio = 1.f;

				_vector		vScale, vQuat, vTrans;
				_matrix		matWorld = m_pTransformCom->Get_WorldMatrix();
				XMMatrixDecompose(&vScale, &vQuat, &vTrans, matWorld);
				matWorld = XMMatrixAffineTransformation(vScale, XMQuaternionIdentity(), m_vInitQuat, vTrans);
				m_pTransformCom->Set_WorldMatrix(matWorld);
			}

			//vPos = _float4::Lerp(m_vMovingPos, m_vInitPos, fRatio);

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

			break;
		}
	}
}

HRESULT CBombPlatform::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = g_LEVEL;

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_RuinsKit_BombPlatForm", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	if (m_pModelCom->Get_IStancingModel())
	{
		FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModelInstance", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

		m_iShaderOption = 1;
	}
	else
	{
		FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModelTess", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

		m_iShaderOption = 4;
	}

	return S_OK;
}

HRESULT CBombPlatform::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBombPlatform* CBombPlatform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBombPlatform* pInstance = new CBombPlatform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBombPlatform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBombPlatform::Clone(void* pArg)
{
	CBombPlatform* pInstance = new CBombPlatform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBombPlatform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBombPlatform::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);
}
