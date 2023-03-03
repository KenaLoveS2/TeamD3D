#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

#pragma region Struct, Enum, Global Function 
enum ACTOR_TYPE {
	BOX_STATIC, SPHERE_STATIC, CAPSULE_STATIC, TRIANGLE_MESH_STATIC,
	BOX_DYNAMIC, SPHERE_DYNAMIC, CAPSULE_DYNAMIC, TRIANGLE_MESH_DYNAMIC,
	TYPE_END
};

typedef struct ENGINE_DLL tagPhysXUserData
{
	ACTOR_TYPE eType;
	
	CGameObject* pOwner;
} PX_USER_DATA;

static PX_USER_DATA* Create_PxUserData(class CGameObject* pOwner)
{
	PX_USER_DATA* pData = new PX_USER_DATA;
	pData->pOwner = pOwner;

	return pData;
}

//dx11 constant buffer
typedef struct tagConstantBufferData
{
	XMMATRIX WVP_Matrix;
	XMMATRIX WorldMatrix;
	XMMATRIX WorldMatrix_Inverse_Transpose;
	XMFLOAT3 Cam_Position;
} CB_DATA;
#pragma endregion

#pragma region Collision Callback Event Class
class CustomSimulationEventCallback : public PxSimulationEventCallback
{
public:
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];

			// eNOTIFY_TOUCH_FOUND: 두 배우의 접촉이 처음 감지되면 애플리케이션에 알립니다.
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{	
				PX_USER_DATA* pSourUserData = (PX_USER_DATA*)pairHeader.actors[0]->userData;
				PX_USER_DATA* pDestUserData = (PX_USER_DATA*)pairHeader.actors[1]->userData;
								
				pSourUserData && pSourUserData->pOwner->Execute_Collision();
				pDestUserData && pDestUserData->pOwner->Execute_Collision();
			}
			// eDETECT_CCD_CONTACT: 연속 충돌 감지를 사용하여 두 액터 간의 접촉을 감지합니다.	
			if (cp.events & PxPairFlag::eDETECT_CCD_CONTACT)
			{
				int temp = 0;
			}

			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_CCD)
			{
				int temp = 0;
			}

			if (cp.events & PxPairFlag::eNOTIFY_CONTACT_POINTS)
			{
				int temp = 0;
			}

			if (cp.events & PxPairFlag::eCONTACT_EVENT_POSE)
			{
				int temp = 0;
			}
		}
	}

	// Implement the onTrigger callback function, which is called when a trigger volume is entered or exited
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override
	{
		// Print the trigger information for each trigger pair
		for (PxU32 i = 0; i < count; i++)
		{
			PxTriggerPair& tp = pairs[i];
			if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				tp.triggerActor->getName();
				tp.otherActor->getName();
			}
			else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				tp.triggerActor->getName();
				tp.otherActor->getName();
			}
		}
	}

	// Implement the onConstraintBreak callback function, which is called when a constraint breaks
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override
	{
		// Print the constraint information for each broken constraint
		for (PxU32 i = 0; i < count; i++)
		{
			PxConstraintInfo& ci = constraints[i];
			// std::cout << "Constraint broke between " << ci.actor0->getName() << " and " << ci.actor1->getName() << std::endl;
			// ci.actor0->getName();
			// ci.actor1->getName();
		}
	}

	virtual void onWake(PxActor** actors, PxU32 count) override {
		int wake = 0;
	};
	virtual void onSleep(PxActor** actors, PxU32 count) override {
		int sleep = 0;
	};
	virtual void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {
		int adavance = 0;
	};
};
#pragma endregion
END