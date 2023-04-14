#include "stdafx.h"
#include "Respawn_Trigger.h"
#include "GameInstance.h"
#include "Kena.h"

CRespawn_Trigger::CRespawn_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRespawn_Trigger::CRespawn_Trigger(const CRespawn_Trigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRespawn_Trigger::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CRespawn_Trigger::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	m_pTransformCom->Set_Position(XMVectorSet(9.89f, 28.3f, 155.25f, 1.f));

	return S_OK;
}

HRESULT CRespawn_Trigger::Late_Initialize(void* pArg)
{
	_float3		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float3 vSize = _float3(15.f, 1.f, 3.f);

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
	BoxDesc.isTrigger = true;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_CHEST));

	return S_OK;
}

void CRespawn_Trigger::ImGui_PhysXValueProperty()
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

_int CRespawn_Trigger::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (m_bUsed == true)
		return 0 ;

	if (pTarget != nullptr)
	{
		if (iColliderIndex == (_int)COL_PLAYER)
		{
			CKena* pKena = dynamic_cast<CKena*>(pTarget);
			if (pKena != nullptr)
			{
				pKena->Set_RespawnPoint(this);
				m_bUsed = true;
			}
		}
	}

	return 0;
}

_int CRespawn_Trigger::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CRespawn_Trigger* CRespawn_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRespawn_Trigger* pInstance = new CRespawn_Trigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRespawn_Trigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRespawn_Trigger::Clone(void* pArg)
{
	CRespawn_Trigger* pInstance = new CRespawn_Trigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRespawn_Trigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRespawn_Trigger::Free()
{
	__super::Free();
}
