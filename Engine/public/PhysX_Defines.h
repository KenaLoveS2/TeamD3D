#pragma once
#include "Base.h"
#include "GameObject.h"
#include "Utile.h"

#define COLLISON_DUMMY			-1
#define TRIGGER_DUMMY			-1



BEGIN(Engine)
#pragma region Struct, Enum, Global Function 

enum ACTOR_TYPE {
	BOX_STATIC, SPHERE_STATIC, CAPSULE_STATIC, TRIANGLE_MESH_STATIC,
	BOX_DYNAMIC, SPHERE_DYNAMIC, CAPSULE_DYNAMIC, TRIANGLE_MESH_DYNAMIC,
	TRIGGER,
	TYPE_END
};

enum PX_FILTER_TYPE { 
	FILTER_DEFULAT,
	PLAYER_BODY, PLAYER_WEAPON, 
	MONSTER_BODY, MONSTER_WEAPON,
	FILTER_GROUND, FILTER_WALL,
	PULSE_PLATE,  PULSE,
	FILTER_END,
};

typedef struct tagPhysXUserData
{
	ACTOR_TYPE	 eType;
	
	CGameObject* pOwner;

	_bool isGravity;
	_uint iColliderIndex;
} PX_USER_DATA;

static PX_USER_DATA* Create_PxUserData(class CGameObject* pOwner, _bool isGravity, _uint iColliderIndex)
{
	PX_USER_DATA* pData = new PX_USER_DATA;

	pData->pOwner = pOwner;
	pData->isGravity = isGravity;
	pData->iColliderIndex = iColliderIndex;
	
	return pData;
}


enum ON_TRIGGER_PARAM {
	ON_TRIGGER_PARAM_ACTOR,
	ON_TRIGGER_PARAM_TRIGGER,
	ON_TRIGGER_PARAM_END,
};

typedef struct tagPhysXTriggerData
{
	_tchar* pActortag;
	CGameObject* pOwner;
	_uint iTriggerIndex;
	_float3 vPos;
	_float fRadius;
	
	PxRigidStatic* pTriggerStatic;

} PX_TRIGGER_DATA;

static PX_TRIGGER_DATA* Create_PxTriggerData(const _tchar* pActortag, class CGameObject* pOwner, _uint iTriggerIndex, _float3 vPos, _float fRadius)
{
	PX_TRIGGER_DATA* pData = new PX_TRIGGER_DATA;
	ZeroMemory(pData, sizeof(PX_TRIGGER_DATA));

	pData->pActortag = CUtile::Create_StringAuto(pActortag);
	pData->pOwner = pOwner;
	pData->iTriggerIndex = iTriggerIndex;
	pData->vPos = vPos;
	pData->fRadius = fRadius;
		
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
				_float3 vCollisionPos;
				for (PxU32 j = 0; j < nbContacts; j++)
				{
					PxVec3 point = contacts[j].position;
					vCollisionPos = CUtile::ConvertPosition_PxToD3D(point);
					/*
					PxVec3 impulse = contacts[j].impulse;
					PxU32 internalFaceIndex0 = contacts[j].internalFaceIndex0;
					PxU32 internalFaceIndex1 = contacts[j].internalFaceIndex1;
					*/
				}

				PX_USER_DATA* pSourUserData = (PX_USER_DATA*)pairHeader.actors[0]->userData;
				PX_USER_DATA* pDestUserData = (PX_USER_DATA*)pairHeader.actors[1]->userData;

				CGameObject* pSourObject = pSourUserData ? pSourUserData->pOwner : nullptr;
				CGameObject* pDestObject = pDestUserData ? pDestUserData->pOwner : nullptr;

				pSourObject && pSourObject->Execute_Collision(pDestObject, vCollisionPos, pDestUserData ? pDestUserData->iColliderIndex : COLLISON_DUMMY);
				pDestObject && pDestObject->Execute_Collision(pSourObject, vCollisionPos, pSourUserData ? pSourUserData->iColliderIndex : COLLISON_DUMMY);
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
			
			// 모양이 삭제된 경우 쌍 무시 
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;
			
			PX_TRIGGER_DATA* pTriggerData = (PX_TRIGGER_DATA*)tp.triggerActor->userData;
			PX_USER_DATA* pActorUserData = (PX_USER_DATA*)tp.otherActor->userData;

			CGameObject* pTriggerObject = pTriggerData ? pTriggerData->pOwner : nullptr;
			CGameObject* pActorObject = pActorUserData ? pActorUserData->pOwner : nullptr;
			
			if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{				
				pTriggerObject && pTriggerObject->Execute_TriggerTouchFound(pActorObject, ON_TRIGGER_PARAM_TRIGGER, pActorUserData ? pActorUserData->iColliderIndex : TRIGGER_DUMMY);
				pActorObject && pActorObject->Execute_TriggerTouchFound(pTriggerObject, ON_TRIGGER_PARAM_ACTOR, pTriggerData ? pTriggerData->iTriggerIndex : TRIGGER_DUMMY);
			}
			else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				pTriggerObject && pTriggerObject->Execute_TriggerTouchLost(pActorObject, ON_TRIGGER_PARAM_TRIGGER, pActorUserData ? pActorUserData->iColliderIndex : TRIGGER_DUMMY);
				pActorObject && pActorObject->Execute_TriggerTouchLost(pTriggerObject, ON_TRIGGER_PARAM_ACTOR, pTriggerData ? pTriggerData->iTriggerIndex : TRIGGER_DUMMY);
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