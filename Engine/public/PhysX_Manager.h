#pragma once
#include "PhysX_Defines.h"

BEGIN(Engine)
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
		_bool isGravity;
		_float fStaticFriction, fDynamicFriction, fRestitution;

		PX_FILTER_TYPE eFilterType;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping, fMass, fLinearDamping;
		_bool bCCD, bKinematic;

		tagPhysxActorBoxDesc()
		{
			bCCD = true;
			bKinematic = false;
		}

	} PX_BOX_DESC;

	typedef struct tagPhysxActorSphereDesc
	{
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos;
		_float fRadius;
		_bool isGravity;
		_float fStaticFriction, fDynamicFriction, fRestitution;

		PX_FILTER_TYPE eFilterType;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping, fMass, fLinearDamping;
		_bool bCCD, bKinematic;

		tagPhysxActorSphereDesc()
		{
			bCCD = true;
			bKinematic = false;
		}
	} PX_SPHERE_DESC;

	typedef struct tagPhysxActorCapsuleDesc
	{
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos, vRotationAxis;
		_float fRadius, fHalfHeight;
		_float fDegree;
		_bool isGravity;
		_float fStaticFriction, fDynamicFriction, fRestitution;


		PX_FILTER_TYPE eFilterType;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping, fMass, fLinearDamping;
		_bool bCCD, bKinematic;

		tagPhysxActorCapsuleDesc()
		{
			bCCD = true;
			bKinematic = false;
		}

	} PX_CAPSULE_DESC;
private:
	PxDefaultAllocator m_PxDefaultAllocatorCallback;
	PxDefaultErrorCallback m_PxDefaultErrorCallback;
	
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	PxTolerancesScale			m_PxTolerancesScale;

	PxFoundation*					m_pFoundation = nullptr;
	PxPhysics*						m_pPhysics = nullptr;
	PxScene*							m_pScene = nullptr;
	PxMaterial*						m_pMaterial = nullptr;
	PxPvd*								m_pPvd = nullptr;
	PxCooking*						m_pCooking = nullptr;
	
	CustomSimulationEventCallback m_EventCallback;
	
private:
	map<const _tchar*, PxRigidActor*> m_StaticActors;
	map<const _tchar*, PxRigidActor*> m_DynamicActors;
	map<const _tchar*, PxRigidActor*> m_DynamicColliders;

	map<const _tchar*, PxRigidActor*> m_Triggers;

	list<PX_USER_DATA*> m_UserDataes;
	list<PX_TRIGGER_DATA*> m_TriggerDataes;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

#pragma region Render Variable
#ifdef _DEBUG
private:	
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*												m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;
	_float4														m_vColor;



#endif // _DEBUG
	wstring										wstrSelectedTag = L"";
	_int										iSelectColider_Index = 0;
public:
	void						Physx_Init() {
		wstrSelectedTag = L"";
		iSelectColider_Index = 0;
	}

#pragma endregion
	
private:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	virtual void Free() override;
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void Render();

	void	Imgui_Render();
	void	Imgui_Render(const _tchar* pActorName,vector<_float3>* vec_ColiderSize);

	PxVec3						Get_ScalingBox(PxRigidActor *pActor);
		
	PxRigidActor*				Find_StaticGameObject(_int iIndex);
	PxRigidActor*				Find_DynamicGameObject(_int iIndex);
	
	void Update_Trasnform(_float fTimeDelta);

	void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);
	void Clear();

	PxRigidStatic * Create_TriangleMeshActor_Static(PxTriangleMeshDesc& Desc, PX_USER_DATA* pUserData, _float fStaticFriction = 0.5f, _float fDynamicFriction = 0.5f, _float fRestitution = 0.1f, _float3 vScale=_float3(0.f,0.f,0.f));
	
	void Create_Trigger(PX_TRIGGER_DATA* pTriggerData);
	void Create_TriggerStatic(PX_TRIGGER_DATA* pTriggerData);
	
	void Create_Box(PX_BOX_DESC& Desc, PX_USER_DATA* pUserData);
	void Create_Sphere(PX_SPHERE_DESC& Desc, PX_USER_DATA* pUserData);
	void Create_Capsule(PX_CAPSULE_DESC& Desc, PX_USER_DATA* pUserData);

	_float4x4 Get_ActorMatrix(const _tchar *pActorTag);
	_float4x4 Get_ActorMatrix(PxRigidActor* pActor);
	void Set_ActorMatrix(const _tchar *pActorTag, _float4x4 Matrix);
	void Set_ActorMatrix(PxRigidActor* pActor, _float4x4 Matrix);
	void Set_ActorMatrixExecptTranslation(const _tchar *pActorTag, _float4x4 Matrix);
	void Set_ActorMatrixExecptTranslation(PxRigidActor* pActor, _float4x4 Matrix);
	void Set_GravityFlag(const _tchar *pActorTag, _bool bGravityFlag, _bool bNow = false);
	
	void Add_Force(const _tchar *pActorTag, _float3 vForce);
	void Add_Force(PxRigidActor* pActor, _float3 vForce);

	PxRigidActor* Find_StaticActor(const _tchar* pActorTag);
	PxRigidActor* Find_DynamicActor(const _tchar* pActorTag);
	PxRigidActor* Find_DynamicCollider(const _tchar* pActorTag);
	
	_bool Raycast_Collision(_float3 vRayPos, _float3 vRayDir, _float fRange, _float3* pPositionOut = nullptr, CGameObject** pObjectOut = nullptr);
	_bool IsMouseOver(HWND hWnd, CGameObject *pTargetObject, _float fRange, _float3* pPositionOut = nullptr);

	void Set_ActorPosition(const _tchar* pActorTag, _float3 vPosition);
	void Set_ActorRotation(const _tchar* pActorTag, _float fDegree, _float3 vAxis);
	void Set_ActorPosition(PxRigidActor* pActor, _float3 vPosition);
	void Set_ActorRotation(PxRigidActor* pActor, _float fDegree, _float3 vAxis);

	void Set_ActorScaling(const _tchar* pActorTag, _float3 vScale);
	void Set_ScalingBox(PxRigidActor* pActor, _float3 vScale);
	void Set_ScalingSphere(PxRigidActor* pActor, _float fRadius);
	void Set_ScalingCapsule(PxRigidActor* pActor, _float fRadius, _float fHalfHeight);

	void Set_DynamicParameter(PxRigidActor* pActor, _float fMass, _float fLinearDamping, _float3 vVelocity);
	void Set_DynamicParameter(const _tchar* pActorTag, _float fMass,  _float fLinearDamping, _float3 vVelocity);
	void Set_Velocity(PxRigidActor* pActor, _float3 vVelocity);

	void Init_Rendering();		

	void Set_ActorFlag_Simulation(const _tchar* pActorTag, _bool bFlag);
	void Set_ActorFlag_Simulation(PxRigidActor* pActor, _bool bFlag);
	PxRigidActor* Find_Actor(const _tchar* pActorTag);
	
	void Delete_Actor(PxActor* pActor);
	void Delete_Actor(class CGameObject* pObject);
	void Reset();
	
	void Delete_DynamicActor(PxRigidActor* pActor);

	void Set_Visualization(PxRigidActor *pActor, _bool bFlag);
};
END