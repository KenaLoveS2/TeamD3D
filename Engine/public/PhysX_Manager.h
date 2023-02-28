#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

enum ACTOR_TYPE {
	BOX_STATIC, SPHERE_STATIC, CAPSULE_STATIC, TRIANGLE_MESH_STATIC,
	BOX_DYNAMIC, SPHERE_DYNAMIC, CAPSULE_DYNAMIC, TRIANGLE_MESH_DYNAMIC,
	TYPE_END
};

typedef struct ENGINE_DLL tagPhysXUserData
{
	CGameObject* pOwner;	
	ACTOR_TYPE eType;
} PX_USER_DATA;

static PX_USER_DATA* Create_PxUserData(class CGameObject* pOwner)
{
	PX_USER_DATA* pData = new PX_USER_DATA;
	pData->pOwner = pOwner;

	return pData;
}

class ENGINE_DLL CustomSimulationEventCallback : public PxSimulationEventCallback
{
public:	
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
	{	
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];


			// eNOTIFY_TOUCH_FOUND: 두 배우의 접촉이 처음 감지되면 애플리케이션에 알립니다.
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{					const char* pSourName = pairHeader.actors[0]->getName();
				const char* pDestName = pairHeader.actors[1]->getName();

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

class ENGINE_DLL CPhysX_Manager : public CBase
{
	DECLARE_SINGLETON(CPhysX_Manager)

public:	
	typedef struct tagPhysxActorBoxDesc
	{	
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos, vSize, vRotationAxis;
		_float fDegree;
		
		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping;
	} PX_BOX_DESC;

	typedef struct tagPhysxActorSphereDesc
	{
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos;
		_float fRadius;
		
		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping;
	} PX_SPHERE_DESC;

	typedef struct tagPhysxActorCapsuleDesc
	{
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos, vRotationAxis;
		_float fRadius, fHalfHeight;
		_float fDegree;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping;		
	} PX_CAPSULE_DESC;
		
private:
	PxDefaultAllocator m_PxDefaultAllocatorCallback;
	PxDefaultErrorCallback m_PxDefaultErrorCallback;
	
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	PxTolerancesScale m_PxTolerancesScale;

	PxFoundation*	m_pFoundation = nullptr;
	PxPhysics*		m_pPhysics = nullptr;
	PxScene*		m_pScene = nullptr;
	PxMaterial*		m_pMaterial = nullptr;
	PxPvd*			m_pPvd = nullptr;
	PxCooking*		m_pCooking = nullptr;
	
	CustomSimulationEventCallback m_EventCallback;
	
private:
	map<const _tchar*, PxRigidActor*> m_StaticActors;
	map<const _tchar*, PxRigidActor*> m_DynamicActors;

	list<PX_USER_DATA*> m_UserDataes;

private:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	virtual void Free() override;
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Render() {};
	void Update_Trasnform(_float fTimeDelta);

	void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);
	void Clear();

	void Create_TriangleMeshActor_Static(PxTriangleMeshDesc& Desc);
	
	void Create_Box(PX_BOX_DESC& Desc, PX_USER_DATA* pUserData);
	void Create_Sphere(PX_SPHERE_DESC& Desc, PX_USER_DATA* pUserData);
	void Create_Capsule(PX_CAPSULE_DESC& Desc, PX_USER_DATA* pUserData);

	void Get_ActorMatrix(_uint iPxActorIndex);
	void Set_GravityFlag(const _tchar *pActorTag, _bool bGravityFlag, _bool bNow);
	
	void Add_Force(const _tchar *pActorTag, _float3 vForce);
	void Add_Force(PxRigidActor* pActor, _float3 vForce);
	PxRigidActor* Find_StaticActor(const _tchar* pActorTag);
	PxRigidActor* Find_DynamicActor(const _tchar* pActorTag);
	_bool Raycast_Collision(_float3 vRayPos, _float3 vRayDir, _float fRange, _float3* pOut = nullptr);
		
	void Set_ActorPosition(const _tchar* pActorTag, _float3 vPosition);
	void Set_ActorRotation(const _tchar* pActorTag, _float fDegree, _float3 vAxis);
	void Set_ActorPosition(PxRigidActor* pActor, _float3 vPosition);
	void Set_ActorRotation(PxRigidActor* pActor, _float fDegree, _float3 vAxis);

	void Set_ActorScaling(const _tchar* pActorTag, _float3 vScale);
	void Set_ScalingBox(PxRigidActor* pActor, _float3 vScale);
	void Set_ScalingSphere(PxRigidActor* pActor, _float fRadius);
	void Set_ScalingCapsule(PxRigidActor* pActor, _float fRadius, _float fHalfHeight);
};
END