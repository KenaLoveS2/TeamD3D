#include "stdafx.h"
#include "BowTarget_Trigger.h"
#include "GameInstance.h"
#include "BowTarget_Manager.h"
#include "BowTarget.h"
#include "Kena.h"
#include "E_KenaPulse.h"

CBowTarget_Trigger::CBowTarget_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBowTarget_Trigger::CBowTarget_Trigger(const CBowTarget_Trigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBowTarget_Trigger::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CBowTarget_Trigger::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	if (g_LEVEL == (_int)LEVEL_GIMMICK)
	{
		m_pTransformCom->Set_Position(XMVectorSet(3.267f, 37.316f, 14.191f, 1.f));
		m_wstrTargetGroup = L"MINIGAME_GROUP_0";
	}
	else if (g_LEVEL == (_int)LEVEL_FINAL)
	{
		m_pTransformCom->Set_Position(XMVectorSet(25.424f, 15.908f, 980.447f, 1.f));
		m_wstrTargetGroup = L"MAP4_GROUP_0";
	}

	return S_OK;
}

HRESULT CBowTarget_Trigger::Late_Initialize(void* pArg)
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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_TRIGGER));

	return S_OK;
}

void CBowTarget_Trigger::ImGui_PhysXValueProperty()
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

_int CBowTarget_Trigger::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget != nullptr)
	{
		if (iColliderIndex == (_int)COL_PLAYER)
		{
			CE_KenaPulse* pKenaPulse = dynamic_cast<CE_KenaPulse*>(pTarget);
			if (CGameInstance::GetInstance()->Get_CurLevelIndex() == (_uint)LEVEL_GIMMICK)
			{
				FAILED_CHECK_RETURN(CBowTarget_Manager::GetInstance()->Group_Active(m_wstrTargetGroup), E_FAIL);
			
				CUI_ClientManager::UI_PRESENT tag = CUI_ClientManager::MINIGAME_START;
				_float ff = 0.f;
				CKena* pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
				if (pKena != nullptr)
					pKena->m_Delegator.broadcast(tag, ff);

			}
			else if (CGameInstance::GetInstance()->Get_CurLevelIndex() == (_uint)LEVEL_FINAL && pKenaPulse != nullptr)
			{
				FAILED_CHECK_RETURN(CBowTarget_Manager::GetInstance()->Group_Active(m_wstrTargetGroup), E_FAIL);
			}
		}
	}

	return 0;
}

_int CBowTarget_Trigger::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return 0;
}

CBowTarget_Trigger* CBowTarget_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBowTarget_Trigger* pInstance = new CBowTarget_Trigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBowTarget_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBowTarget_Trigger::Clone(void* pArg)
{
	CBowTarget_Trigger* pInstance = new CBowTarget_Trigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBowTarget_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBowTarget_Trigger::Free()
{
	__super::Free();
}
