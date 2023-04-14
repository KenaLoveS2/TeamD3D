#include "stdafx.h"
#include "Village_Trigger.h"
#include "GameInstance.h"
#include "Kena.h"
#include "BGM_Manager.h"

CVillage_Trigger::CVillage_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CVillage_Trigger::CVillage_Trigger(const CVillage_Trigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CVillage_Trigger::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CVillage_Trigger::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	return S_OK;
}

HRESULT CVillage_Trigger::Late_Initialize(void* pArg)
{
	if (!lstrcmp(m_szCloneObjectTag, L"IntoVillage_Trigger"))
	{
		m_bIntoVillage = true;
		m_pTransformCom->Set_Position(XMVectorSet(33.434f, 15.16f, 790.27f, 1.f));
	}
	else if (!lstrcmp(m_szCloneObjectTag, L"FromVillage_Trigger"))
	{
		m_bIntoVillage = false;
		m_pTransformCom->Set_Position(XMVectorSet(32.67f, 15.f, 849.f, 1.f));
	}

	_float3		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float3 vSize;

	if (!lstrcmp(m_szCloneObjectTag, L"IntoVillage_Trigger"))
		vSize = _float3(5.f, 1.f, 3.f);
	else if (!lstrcmp(m_szCloneObjectTag, L"FromVillage_Trigger"))
		vSize = _float3(4.f, 1.f, 3.f);

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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_TRIGGER));

	return S_OK;
}

void CVillage_Trigger::ImGui_PhysXValueProperty()
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

_int CVillage_Trigger::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget != nullptr)
	{
		if (iColliderIndex == (_int)COL_PLAYER)
		{
			CKena* pKena = dynamic_cast<CKena*>(pTarget);
			if (pKena != nullptr)
			{
				if (m_bIntoVillage == true)
				{
					CBGM_Manager::GetInstance()->Change_FieldState(CBGM_Manager::FIELD_VILLAGE);
				}
				else
				{
					CBGM_Manager::GetInstance()->Change_FieldState(CBGM_Manager::FIELD_IDLE);
				}
			}
		}
	}

	return 0;
}

_int CVillage_Trigger::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CVillage_Trigger* CVillage_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVillage_Trigger* pInstance = new CVillage_Trigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVillage_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVillage_Trigger::Clone(void* pArg)
{
	CVillage_Trigger* pInstance = new CVillage_Trigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVillage_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillage_Trigger::Free()
{
	__super::Free();
}
