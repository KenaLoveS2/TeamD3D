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

		PX_FILTER_TYPE eFilterType;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping, fMass, fDamping;
		_bool bCCD;
	} PX_BOX_DESC;

	typedef struct tagPhysxActorSphereDesc
	{
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos;
		_float fRadius;
		_bool isGravity;

		PX_FILTER_TYPE eFilterType;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping, fMass, fDamping;
		_bool bCCD;
	} PX_SPHERE_DESC;

	typedef struct tagPhysxActorCapsuleDesc
	{
		ACTOR_TYPE eType;
		const _tchar* pActortag;
		_float3 vPos, vRotationAxis;
		_float fRadius, fHalfHeight;
		_float fDegree;
		_bool isGravity;

		PX_FILTER_TYPE eFilterType;

		// Dynamic Parameter
		_float3 vVelocity;
		_float fDensity, fAngularDamping, fMass, fDamping;
		_bool bCCD;
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

	list<PX_USER_DATA*> m_UserDataes;

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
	PxRigidActor*				Find_StaticGameObject(_int iIndex);
	PxRigidActor*				Find_DynamicGameObject(_int iIndex);
	
	void Update_Trasnform(_float fTimeDelta);

	void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);
	void Clear();

	PxRigidStatic * Create_TriangleMeshActor_Static(PxTriangleMeshDesc& Desc);
	
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

	void Init_Rendering();		

	/*
	eVISUALIZATION: 이 플래그는 PhysX 비주얼 디버거에서 액터를 시각화해야 하는지 여부를 나타냅니다.
	eDISABLE_GRAVITY: 이 플래그는 액터의 중력을 비활성화하는 데 사용됩니다.
	eSEND_SLEEP_NOTIFIES: 이 플래그는 액터에 대한 수면 알림을 활성화/비활성화하는 데 사용됩니다. 액터가 잠들거나 깨어날 때 사용자에게 알림을 보낼 수 있습니다.
	eDISABLE_SIMULATION: 이 플래그는 액터에 대한 시뮬레이션을 비활성화하는 데 사용됩니다.
	eCONTACT_REPORTING: 이 플래그는 액터에 대한 연락처 보고를 활성화/비활성화하는 데 사용됩니다. 활성화되면 액터는 모든 접점을 사용자에게 보고합니다.
	eCONTACT_PERFORMANCE_RBD: 이 플래그는 액터에 대한 접촉 성능 기능을 활성화/비활성화하는 데 사용됩니다. 활성화되면 액터는 성능을 향상시키는 단순화된 접촉 생성 알고리즘을 사용하지만 접촉의 정확도가 떨어질 수 있습니다.
	eCONTACT_PERFORMANCE_ARTICULATION: 이 플래그는 다관절 몸체에 대한 접촉 성능 기능을 활성화/비활성화하는 데 사용됩니다. 활성화되면 다관절 몸체는 성능을 향상시키는 단순화된 접촉 생성 알고리즘을 사용하지만 접촉의 정확도가 떨어질 수 있습니다.
	eCONTACT_FORCE_THRESHOLD: 이 플래그는 액터에 대한 접촉력 임계값을 설정하는 데 사용됩니다. 접촉력이 이 임계값을 초과하면 접촉 보고서가 생성됩니다.
	eDISABLE_CONTACT_REPORT_BUFFER_RESIZE: 이 플래그는 연락처 보고서 버퍼가 액터에 대해 자동으로 크기가 조정되는 것을 방지하는 데 사용됩니다.
	eLOCK_COM: 이 플래그는 액터의 질량 중심을 고정하는 데 사용됩니다.
	*/
	void Set_ActorFlag_Simulation(const _tchar* pActorTag, _bool bFlag);
	void Set_ActorFlag_Simulation(PxRigidActor* pActor, _bool bFlag);
	PxRigidActor* Find_Actor(const _tchar* pActorTag);
	void Delete_Actor(PxActor& pActor);
};
END