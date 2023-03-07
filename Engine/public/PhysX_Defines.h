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

enum PX_FILTER_TYPE { 
	FILTER_DEFULAT,
	PLAYER_BODY, PLAYER_WEAPON, 
	MONSTER_BODY, MONSTER_WEAPON,
	FILTER_END,
};

typedef struct tagPhysXUserData
{
	ACTOR_TYPE	 eType;
	
	CGameObject* pOwner;
	bool					isGravity;
} PX_USER_DATA;

static PX_USER_DATA* Create_PxUserData(class CGameObject* pOwner, _bool isGravity = true)
{
	PX_USER_DATA* pData = new PX_USER_DATA;
	pData->pOwner = pOwner;
	pData->isGravity = isGravity;
	return pData;
}

#pragma endregion

#pragma region Collision Callback Event Class
class CustomSimulationEventCallback : public PxSimulationEventCallback
{
public:
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
	{
		const PxU32 bufferSize = 64;
		PxContactPairPoint contacts[bufferSize];


		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& Pair = pairs[i];

			// eNOTIFY_TOUCH_FOUND: 두 배우의 접촉이 처음 감지되면 애플리케이션에 알립니다.
			if (Pair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{	
				PxU32 nbContacts = Pair.extractContacts(contacts, bufferSize);
				for (PxU32 j = 0; j < nbContacts; j++)
				{
					PxVec3 point = contacts[j].position;
					PxVec3 impulse = contacts[j].impulse;
					PxU32 internalFaceIndex0 = contacts[j].internalFaceIndex0;
					PxU32 internalFaceIndex1 = contacts[j].internalFaceIndex1;
				}

				PX_USER_DATA* pSourUserData = (PX_USER_DATA*)pairHeader.actors[0]->userData;
				PX_USER_DATA* pDestUserData = (PX_USER_DATA*)pairHeader.actors[1]->userData;
								
				pSourUserData && pSourUserData->pOwner->Execute_Collision();
				pDestUserData && pDestUserData->pOwner->Execute_Collision();
			}
			
			/*
			// eDETECT_CCD_CONTACT: 연속 충돌 감지를 사용하여 두 액터 간의 접촉을 감지합니다.	
			if (cp.events & PxPairFlag::eDETECT_CCD_CONTACT) { int temp = 0; }
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_CCD) { int temp = 0; }
			if (cp.events & PxPairFlag::eNOTIFY_CONTACT_POINTS) { int temp = 0; }
			if (cp.events & PxPairFlag::eCONTACT_EVENT_POSE) { int temp = 0; }
			*/
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

	virtual void onWake(PxActor** actors, PxU32 count) override { int wake = 0;	};
	virtual void onSleep(PxActor** actors, PxU32 count) override { int sleep = 0; };
	virtual void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override { int adavance = 0; };
};
#pragma endregion
END