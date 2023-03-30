#include "stdafx.h"
#include "PhysX_Manager.h"
#include "Utile.h"
#include "String_Manager.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#include "GameInstance.h"

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

/*
Mass: The mass of the object, specified in kilograms.
질량: 킬로그램으로 지정된 물체의 질량.

Center of mass: The center of mass of the object, specified as a vector in local space.
질량 중심: 로컬 공간에서 벡터로 지정된 개체의 질량 중심입니다.

Inertia tensor: The 3x3 inertia tensor of the object, specified in local space.
관성 텐서: 로컬 공간에서 지정된 개체의 3x3 관성 텐서입니다.

Linear velocity: The linear velocity of the object, specified as a vector in world space.
선형 속도: 월드 공간에서 벡터로 지정된 개체의 선형 속도입니다.

Angular velocity: The angular velocity of the object, specified as a vector in local space.
각속도: 로컬 공간에서 벡터로 지정된 객체의 각속도.

Linear damping: The damping applied to the linear motion of the object. This value should be between 0 (no damping) and 1 (complete damping).
선형 감쇠: 객체의 직선 운동에 적용되는 감쇠입니다. 이 값은 0(제동 없음)과 1(완전한 제동) 사이여야 합니다.

Angular damping: The damping applied to the angular motion of the object. This value should be between 0 (no damping) and 1 (complete damping).
각도 감쇠: 개체의 각 운동에 적용되는 감쇠입니다. 이 값은 0(제동 없음)과 1(완전한 제동) 사이여야 합니다.

Max angular velocity: The maximum angular velocity that the object can reach, specified in radians per second.
최대 각속도: 객체가 도달할 수 있는 최대 각속도로, 초당 라디안으로 지정됩니다.

Sleep threshold: The amount of motion required to wake the object up from sleep. This value should be between 0 (never sleep) and a large number (sleep immediately).
수면 임계값: 개체를 절전 상태에서 깨우는 데 필요한 동작의 양입니다. 이 값은 0(잠자지 않음)과 큰 숫자(즉시 잠자기) 사이여야 합니다.

CCD threshold: The distance threshold at which continuous collision detection (CCD) is triggered for the object. This value should be greater than 0.
CCD 임계값: 개체에 대해 CCD(연속 충돌 감지)가 트리거되는 거리 임계값입니다. 이 값은 0보다 커야 합니다.

Collision filtering: The collision filter data used to determine which objects can collide with the rigid body.
충돌 필터링: 강체와 충돌할 수 있는 객체를 결정하는 데 사용되는 충돌 필터 데이터입니다.

Kinematic target: If the rigid body is set to kinematic, this specifies the target transform that the body will interpolate towards.
운동학적 대상: 강체가 운동학적으로 설정된 경우 신체가 보간할 대상 변환을 지정합니다.
*/


PxFilterFlags CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
                                 PxFilterObjectAttributes attributes1, PxFilterData filterData1,
                                 PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}


	if ((filterData0.word0 == (PxU32)PLAYER_BODY && filterData1.word0 == (PxU32)PLAYER_WEAPON)			||
		(filterData0.word0 == (PxU32)PLAYER_WEAPON && filterData1.word0 == (PxU32)PLAYER_BODY)			||

		(filterData0.word0 == (PxU32)MONSTER_BODY && filterData1.word0 == (PxU32)MONSTER_WEAPON)		||
		(filterData0.word0 == (PxU32)MONSTER_WEAPON && filterData1.word0 == (PxU32)MONSTER_BODY)		||

		(filterData0.word0 == (PxU32)MONSTER_WEAPON  &&  filterData1.word0 == (PxU32)MONSTER_WEAPON)	||
		
		(filterData0.word0 == (PxU32)PLAYER_BODY &&  filterData1.word0 == (PxU32)PLAYER_BODY)			||
				
		(filterData0.word0 == (PxU32)PLAYER_WEAPON && filterData1.word0 == (PxU32)MONSTER_WEAPON)		||
		(filterData0.word0 == (PxU32)MONSTER_WEAPON && filterData1.word0 == (PxU32)PLAYER_WEAPON)		||

		(filterData0.word0 == (PxU32)MONSTER_PARTS && filterData1.word0 == (PxU32)MONSTER_PARTS)		||
		(filterData0.word0 == (PxU32)MONSTER_PARTS && filterData1.word0 == (PxU32)MONSTER_BODY)			||
		(filterData0.word0 == (PxU32)MONSTER_PARTS && filterData1.word0 == (PxU32)MONSTER_WEAPON)		||		
		(filterData0.word0 == (PxU32)MONSTER_PARTS && filterData1.word0 == (PxU32)PLAYER_BODY)			||

		(filterData0.word0 == (PxU32)MONSTER_BODY && filterData1.word0 == (PxU32)MONSTER_PARTS )		||
		(filterData0.word0 == (PxU32)MONSTER_WEAPON && filterData1.word0 == (PxU32)MONSTER_PARTS )		||		
		(filterData0.word0 == (PxU32)PLAYER_BODY && filterData1.word0 == (PxU32)MONSTER_PARTS)			)
	{
		return PxFilterFlag::eSUPPRESS;
	}
	
	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eDETECT_CCD_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_CCD
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eCONTACT_EVENT_POSE;
		
	/*
	eCONTACT_DEFAULT: 연락처 생성을 위한 기본 동작입니다. 이것은 다른 모든 플래그를 기본값으로 설정하는 것과 같습니다.
	eNOTIFY_TOUCH_FOUND: 두 배우의 접촉이 처음 감지되면 애플리케이션에 알립니다.
	eNOTIFY_TOUCH_LOST: 두 배우의 연락이 끊어졌을 때 어플리케이션에 알립니다.
	eNOTIFY_THRESHOLD_FORCE_FOUND: 두 액터 사이의 힘이 지정된 임계값을 초과하면 애플리케이션에 알립니다.
	eNOTIFY_THRESHOLD_FORCE_LOST: 두 액터 사이의 힘이 지정된 임계값 아래로 떨어지면 애플리케이션에 알립니다.
	eNOTIFY_CONTACT_POINTS: 두 배우의 접점 적용을 알립니다.
	eSOLVE_CONTACT: 충돌 감지 시 두 행위자 간의 접촉을 생성합니다.
	eMODIFY_CONTACTS: 애플리케이션이 두 배우 사이의 접점을 수정할 수 있도록 허용합니다.
	eDETECT_DISCRETE_CONTACT: 불연속 충돌 감지를 사용하여 두 액터 간의 접촉을 감지합니다.
	eDETECT_CCD_CONTACT: 연속 충돌 감지를 사용하여 두 액터 간의 접촉을 감지합니다.
	ePRE_SOLVER_VELOCITY: 솔버가 실행되기 전에 액터에 속도 변경을 적용합니다.
	ePOST_SOLVER_VELOCITY: 솔버 실행 후 액터에 속도 변경을 적용합니다.
	eCONTACT_EVENT_POSE: 두 배우의 접점 포즈 적용을 알립니다.
	eNEXT_FREE: 값을 반복하는 데 사용됩니다 PxPairFlag.
	*/

	return PxFilterFlag::eDEFAULT;
}


IMPLEMENT_SINGLETON(CPhysX_Manager)

CPhysX_Manager::CPhysX_Manager()
{
}

void CPhysX_Manager::Free()
{
	Clear();
#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
#endif // _DEBUG
}

HRESULT CPhysX_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxDefaultAllocatorCallback, m_PxDefaultErrorCallback);
	assert(m_pFoundation != nullptr && "CPhysX_Manager::InitWorld()");
	
	m_pPvd = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL);
		
	m_PxTolerancesScale.length = 100;
	m_PxTolerancesScale.speed = 981;
	
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
	assert(m_pPhysics != nullptr && "CPhysX_Manager::InitWorld()");

	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	
	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());		
	SceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	SceneDesc.cpuDispatcher = m_pDispatcher;	
	SceneDesc.filterShader = CustomFilterShader;
	SceneDesc.simulationEventCallback = &m_EventCallback;
	SceneDesc.broadPhaseType = PxBroadPhaseType::eABP;
	m_pScene = m_pPhysics->createScene(SceneDesc);	
	assert(m_pScene != nullptr && "CPhysX_Manager::InitWorld()");
	
	m_pScene->setFlag(PxSceneFlag::eENABLE_CCD, true);

#ifdef _DEBUG
	PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
	assert(pPvdClient != nullptr && "CPhysX_Manager::InitWorld()");
	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
#endif // _DEBUG
		
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));
	assert(m_pCooking != nullptr && "CPhysX_Manager::InitWorld()");
	
	PxCookingParams params(m_PxTolerancesScale);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	m_pCooking->setParams(params);
	
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	// 기본 땅 생성	
	PxRigidStatic* pGroundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 1), *m_pMaterial);		
	m_pScene->addActor(*pGroundPlane);
	
	m_pControlllerManager = PxCreateControllerManager(*m_pScene);

	Init_Rendering();
		
	return S_OK;
}

void CPhysX_Manager::Init_Rendering()
{	
#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);
	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout);

	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eWORLD_AXES, 1.0f);
	// m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	// m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_EDGES, 1);
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_POINT, 2.f);
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_NORMAL, 2.f);

#endif
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{	
	static const _float fPxTimeDelta = 1.f / 60.f;
	// PX_UNUSED(false);

	m_pScene->simulate(fPxTimeDelta);
	m_pScene->fetchResults(true);

	Update_Trasnform(fTimeDelta);
}

void CPhysX_Manager::Render()
{	
#ifdef _DEBUG
	const PxRenderBuffer &RenderBuffer = m_pScene->getRenderBuffer();

	PxU32 NbTriangles = RenderBuffer.getNbTriangles();
	PxU32 NbLines = RenderBuffer.getNbLines();
	PxU32 NbTexts = RenderBuffer.getNbTexts();
	PxU32 NbPoints = RenderBuffer.getNbPoints();

	m_pEffect->SetWorld(XMMatrixIdentity());
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	static const PxU32 PxSkipCount= 1;
	for (PxU32 i = 0; i < NbLines; i += PxSkipCount)
	{
		if (i >= NbLines) break;

		const PxDebugLine& pose = RenderBuffer.getLines()[i];

		PxVec3 PxPos_0 = pose.pos0;
		PxVec3 PxPos_1 = pose.pos1;

		DX::DrawLine(m_pBatch, CUtile::ConvertPosition_PxToD3D(PxPos_0), CUtile::ConvertPosition_PxToD3D(PxPos_1), _float4(0.f,1.f,0.f,1.f));
	}

	m_pBatch->End();
#endif // _DEBUG
}

void CPhysX_Manager::Update_Trasnform(_float fTimeDelta)
{
	PX_USER_DATA* pUserData = nullptr;
	PxRigidDynamic* pActor = nullptr;
	PxController* pController = nullptr;
	/*
	for (auto Pair : m_Controllers)
	{
		pController = (PxController*)Pair.second;
		pUserData = (PX_USER_DATA*)pController->getUserData();

		if (pUserData)
		{	
			_float3 vPos = CUtile::ConvertPosition_PxToD3D(pController->getActor()->getGlobalPose().p);			
			pUserData->pOwner->Set_Position(vPos);
		}
	}
	*/	
	for (auto Pair : m_DynamicActors)
	{
		pActor = (PxRigidDynamic*)Pair.second;
		pUserData = (PX_USER_DATA*)pActor->userData;

		if(pUserData && pUserData->isGravity)
		{
			PxTransform ActorTrasnform = pActor->getGlobalPose();
			_float3 vObjectPos = CUtile::ConvertPosition_PxToD3D(ActorTrasnform.p);

			pUserData->pOwner->Set_Position(vObjectPos);
		}
	}	
}

void CPhysX_Manager::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*	m_pPhysics, t, geometry, *m_pMaterial, 1.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	m_pScene->addActor(*dynamic);
}

void CPhysX_Manager::Clear()
{	
	Px_Safe_Release(m_pControlllerManager);
	Px_Safe_Release(m_pCooking);	
	Px_Safe_Release(m_pScene);
	Px_Safe_Release(m_pDispatcher);
	Px_Safe_Release(m_pPhysics);
		
	PxPvdTransport* pTransport = m_pPvd->getTransport();
	Px_Safe_Release(m_pPvd);
	Px_Safe_Release(pTransport);	
	Px_Safe_Release(m_pFoundation);

	for (auto &iter : m_UserDataes)
	{
		Safe_Delete(iter);
	}

	for (auto &iter : m_TriggerDataes)
	{
		Safe_Delete(iter);
	}
}


PxRigidStatic * CPhysX_Manager::Create_TriangleMeshActor_Static(PxTriangleMeshDesc& Desc, PX_USER_DATA* pUserData, _float fStaticFriction, _float fDynamicFriction,
	_float fRestitution,_float3 vScale )
{
	PxDefaultMemoryOutputStream WriteBuffer;
	m_pCooking->cookTriangleMesh(Desc, WriteBuffer);

	PxDefaultMemoryInputData ReadBuffer(WriteBuffer.getData(), WriteBuffer.getSize());
	PxTriangleMesh* pMesh = m_pPhysics->createTriangleMesh(ReadBuffer);

	PxTransform Transform(PxIdentity);
	PxRigidStatic *pBody = m_pPhysics->createRigidStatic(Transform);

	PxMaterial *pMaterial = m_pPhysics->createMaterial(fStaticFriction, fDynamicFriction, fRestitution);

	PxShape* shape = nullptr;

	if (vScale.x != 0)
	{
		shape = m_pPhysics->createShape(PxTriangleMeshGeometry(pMesh,PxMeshScale(PxVec3(vScale.x,vScale.y,vScale.z))),
			*pMaterial, true);
	}
	else
	{
		shape = m_pPhysics->createShape(PxTriangleMeshGeometry(pMesh), *pMaterial, true);
	}
	assert(shape != nullptr && "CPhysX_Manager::Create_TriangleMeshActor_Static");
	shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

	PxFilterData FilterData;
	FilterData.word0 = PX_FILTER_TYPE::FITLER_ENVIROMNT;
	shape->setSimulationFilterData(FilterData);

	pBody->attachShape(*shape);

	if (pUserData)
	{	
		pUserData->eType = TRIANGLE_MESH_STATIC;
		pBody->userData = pUserData;
		m_UserDataes.push_back(pUserData);
	}

	m_pScene->addActor(*pBody);
	pMaterial->release();
	shape->release();
	return pBody;
}


void CPhysX_Manager::Create_Box(PX_BOX_DESC& Desc, PX_USER_DATA* pUserData)
{
	if (Desc.eType == BOX_STATIC)
	{	
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));

		/*
		PxQuat Rotation = PxQuat(XMConvertToRadians(Desc.fDegree), CUtile::ConvertPosition_D3DToPx(Desc.vRotationAxis));
		Transform.q = Rotation;
		*/
		// transform.q = Rotation;
		
		PxRigidStatic* pBox = m_pPhysics->createRigidStatic(Transform);		
		PxMaterial *pMaterial = m_pPhysics->createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
		//PxShape* pShape = m_pPhysics->createShape(PxBoxGeometry(Desc.vSize.x, Desc.vSize.y, Desc.vSize.z), *pMaterial, false);	
		PxShape* pShape = m_pPhysics->createShape(PxBoxGeometry(Desc.vSize.x, Desc.vSize.y, Desc.vSize.z), *pMaterial, true);
	
		PxTransform relativePose(PxVec3(0, 0, 0));
		pShape->setLocalPose(relativePose);
		pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

		PxFilterData FilterData;
		FilterData.word0 = Desc.eFilterType;
		pShape->setSimulationFilterData(FilterData);

		pBox->attachShape(*pShape);

		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pBox->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_StaticActors.emplace(pTag, pBox);
		
		m_pScene->addActor(*pBox);
		pShape->release();
		pMaterial->release();
	}
	else if (Desc.eType == BOX_DYNAMIC)
	{	
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidDynamic* pBox = m_pPhysics->createRigidDynamic(Transform);
		PxMaterial *pMaterial = m_pPhysics->createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
		PxShape* pShape = m_pPhysics->createShape(PxBoxGeometry(Desc.vSize.x, Desc.vSize.y, Desc.vSize.z), *pMaterial, true);
		PxTransform relativePose(PxVec3(0, 0, 0));
		pShape->setLocalPose(relativePose);
		pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		
		PxFilterData FilterData;
		FilterData.word0 = Desc.eFilterType;
		pShape->setSimulationFilterData(FilterData);
		
		pBox->attachShape(*pShape);
		pBox->setMass(Desc.fMass);
		pBox->setLinearDamping(Desc.fLinearDamping);
		pBox->setAngularDamping(Desc.fAngularDamping);
		pBox->setLinearVelocity(PxVec3(Desc.vVelocity.x, Desc.vVelocity.y, Desc.vVelocity.z));
		pBox->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, Desc.bKinematic);
		pBox->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, Desc.bCCD);
		
		PxRigidBodyExt::updateMassAndInertia(*pBox, Desc.fDensity);
		
		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pBox->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		if (pUserData->isGravity)
		{
			_tchar *pTag = CUtile::Create_String(Desc.pActortag);
			CString_Manager::GetInstance()->Add_String(pTag);
			m_DynamicActors.emplace(pTag, pBox);
		}
		else
		{
			_tchar *pTag = CUtile::Create_String(Desc.pActortag);
			CString_Manager::GetInstance()->Add_String(pTag);
			m_DynamicColliders.emplace(pTag, pBox);
		}

	
		m_pScene->addActor(*pBox);
		pShape->release();
		pMaterial->release();
	}	
}

void CPhysX_Manager::Create_Sphere(PX_SPHERE_DESC & Desc, PX_USER_DATA * pUserData)
{
	if (Desc.eType == SPHERE_STATIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidStatic* pSphere = m_pPhysics->createRigidStatic(Transform);		
		PxMaterial *pMaterial = m_pPhysics->createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
		PxShape* pShape = m_pPhysics->createShape(PxSphereGeometry(Desc.fRadius), *pMaterial, true);
		pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
	
		PxFilterData FilterData;
		FilterData.word0 = Desc.eFilterType;
		pShape->setSimulationFilterData(FilterData);
		
		pSphere->attachShape(*pShape);
		
		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pSphere->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_StaticActors.emplace(pTag, pSphere);

		m_pScene->addActor(*pSphere);
		pShape->release();
		pMaterial->release();
	}
	else if (Desc.eType == SPHERE_DYNAMIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidDynamic* pSphere = m_pPhysics->createRigidDynamic(Transform);
		PxMaterial *pMaterial = m_pPhysics->createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
		PxShape* pShape = m_pPhysics->createShape(PxSphereGeometry(Desc.fRadius), *pMaterial, true);
		PxTransform relativePose(PxVec3(0, 0, 0));
		pShape->setLocalPose(relativePose);
		pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

		PxFilterData FilterData;
		FilterData.word0 = Desc.eFilterType;
		pShape->setSimulationFilterData(FilterData);

		pSphere->attachShape(*pShape);
		pSphere->setMass(Desc.fMass);
		pSphere->setLinearDamping(Desc.fLinearDamping);
		pSphere->setAngularDamping(Desc.fAngularDamping);
		pSphere->setLinearVelocity(PxVec3(Desc.vVelocity.x, Desc.vVelocity.y, Desc.vVelocity.z));		
		pSphere->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, Desc.bCCD);
		pSphere->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, Desc.bKinematic);
		PxRigidBodyExt::updateMassAndInertia(*pSphere, Desc.fDensity);

		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pSphere->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		if (pUserData->isGravity)
		{
			_tchar *pTag = CUtile::Create_String(Desc.pActortag);
			CString_Manager::GetInstance()->Add_String(pTag);
			m_DynamicActors.emplace(pTag, pSphere);
		}
		else
		{
			_tchar *pTag = CUtile::Create_String(Desc.pActortag);
			CString_Manager::GetInstance()->Add_String(pTag);
			m_DynamicColliders.emplace(pTag, pSphere);
		}

		m_pScene->addActor(*pSphere);
		pShape->release();
		pMaterial->release();
	}	
}

void CPhysX_Manager::Create_Capsule(PX_CAPSULE_DESC& Desc, PX_USER_DATA* pUserData)
{
	if (Desc.eType == CAPSULE_STATIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidStatic *pCapsule = m_pPhysics->createRigidStatic(Transform);
		PxMaterial *pMaterial = m_pPhysics->createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
		PxShape* pShape = m_pPhysics->createShape(PxCapsuleGeometry(Desc.fRadius, Desc.fHalfHeight), *pMaterial, true);
		
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
		
		PxFilterData FilterData;
		FilterData.word0 = Desc.eFilterType;
		pShape->setSimulationFilterData(FilterData);

		pCapsule->attachShape(*pShape);

		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pCapsule->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_StaticActors.emplace(pTag, pCapsule);
	
		m_pScene->addActor(*pCapsule);
		pShape->release();
		pMaterial->release();
	}
	else if (Desc.eType == CAPSULE_DYNAMIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidDynamic *pCapsule = m_pPhysics->createRigidDynamic(Transform);		
		PxMaterial *pMaterial = m_pPhysics->createMaterial(Desc.fStaticFriction, Desc.fDynamicFriction, Desc.fRestitution);
		PxShape* pShape = m_pPhysics->createShape(PxCapsuleGeometry(Desc.fRadius, Desc.fHalfHeight), *pMaterial, true);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);

		PxFilterData FilterData;
		FilterData.word0 = Desc.eFilterType;
		pShape->setSimulationFilterData(FilterData);

		pCapsule->attachShape(*pShape);
		pCapsule->setMass(Desc.fMass);
		pCapsule->setLinearDamping(Desc.fLinearDamping);
		pCapsule->setAngularDamping(Desc.fAngularDamping);
		pCapsule->setLinearVelocity(PxVec3(Desc.vVelocity.x, Desc.vVelocity.y, Desc.vVelocity.z));
		pCapsule->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, Desc.bCCD);
		pCapsule->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, Desc.bKinematic);
		PxRigidBodyExt::updateMassAndInertia(*pCapsule, Desc.fDensity);
		
		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pCapsule->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		if (pUserData->isGravity)
		{
			_tchar *pTag = CUtile::Create_String(Desc.pActortag);
			CString_Manager::GetInstance()->Add_String(pTag);
			m_DynamicActors.emplace(pTag, pCapsule);
		}
		else
		{
			_tchar *pTag = CUtile::Create_String(Desc.pActortag);
			CString_Manager::GetInstance()->Add_String(pTag);
			m_DynamicColliders.emplace(pTag, pCapsule);
		}

		m_pScene->addActor(*pCapsule);
		pShape->release();
		pMaterial->release();
	}	
}

_float4x4 CPhysX_Manager::Get_ActorMatrix(const _tchar *pActorTag)
{
	PxRigidDynamic* pActor = (PxRigidDynamic*)Find_DynamicActor(pActorTag);
	if (pActor == nullptr) 	{
		pActor = (PxRigidDynamic*)Find_StaticActor(pActorTag);
	}
	assert(pActor != nullptr && "CPhysX_Manager::Get_ActorMatrix");

	return Get_ActorMatrix(pActor);	
}

_float4x4 CPhysX_Manager::Get_ActorMatrix(PxRigidActor* pActor)
{
	PxMat44 PxMatrix(pActor->getGlobalPose());
	
	return CUtile::ConvertMatrix_PxToD3D(PxMatrix);
}

void CPhysX_Manager::Set_ActorMatrix(const _tchar *pActorTag, _float4x4 Matrix)
{
	PxRigidDynamic* pActor = (PxRigidDynamic*)Find_DynamicActor(pActorTag);
	if (pActor == nullptr) {
		pActor = (PxRigidDynamic*)Find_StaticActor(pActorTag);
	}
	if (pActor == nullptr) return;

	return Set_ActorMatrix(pActor, Matrix);
}

void CPhysX_Manager::Set_ActorMatrix(PxRigidActor* pActor, _float4x4 Matrix)
{	
	PxTransform NewTransform(CUtile::ConvertMatrix_D3DToPx(Matrix));

	pActor->setGlobalPose(NewTransform);
}

void CPhysX_Manager::Set_ActorMatrixExecptTranslation(const _tchar *pActorTag, _float4x4 Matrix)
{
	PxRigidDynamic* pActor = (PxRigidDynamic*)Find_DynamicActor(pActorTag);
	if (pActor == nullptr) {
		pActor = (PxRigidDynamic*)Find_StaticActor(pActorTag);
	}
	if (pActor == nullptr) return;

	return Set_ActorMatrixExecptTranslation(pActor, Matrix);
}

void CPhysX_Manager::Set_ActorMatrixExecptTranslation(PxRigidActor* pActor, _float4x4 Matrix)
{
	PxTransform NewTransform(CUtile::ConvertMatrix_D3DToPx(Matrix));

	NewTransform.p = pActor->getGlobalPose().p;
		
	pActor->setGlobalPose(NewTransform);
}

void CPhysX_Manager::Set_GravityFlag(const _tchar *pActorTag, _bool bGravityFlag, _bool bNow)
{	
	PxRigidDynamic* pActor = (PxRigidDynamic*)Find_DynamicActor(pActorTag);
	if (pActor == nullptr) return;

	pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !bGravityFlag);
	if (bNow && bGravityFlag)
	{
		Add_Force(pActor, _float3(0.f, 0.1f, 0.f));
	}
}

void CPhysX_Manager::Add_Force(const _tchar *pActorTag, _float3 vForce)
{
	PxRigidDynamic* pActor = (PxRigidDynamic*)Find_DynamicActor(pActorTag);
	if (pActor == nullptr) return;

	Add_Force(pActor, vForce);
}

void CPhysX_Manager::Add_Force(PxRigidActor* pActor, _float3 vForce)
{
	// pActor->addForce(PxVec3(v.x, v.y, v.z));
	PxVec3 displacement = CUtile::ConvertPosition_D3DToPx(vForce);
	PxTransform newTransform = pActor->getGlobalPose();
	newTransform.p += displacement;
	pActor->setGlobalPose(newTransform);
}

PxRigidActor* CPhysX_Manager::Find_StaticActor(const _tchar* pActorTag)
{
	auto Pair = find_if(m_StaticActors.begin(), m_StaticActors.end(), CTag_Finder(pActorTag));
	if (Pair == m_StaticActors.end()) return nullptr;

	return Pair->second;
}

PxRigidActor* CPhysX_Manager::Find_DynamicActor(const _tchar* pActorTag)
{
	auto Pair = find_if(m_DynamicActors.begin(), m_DynamicActors.end(), CTag_Finder(pActorTag));
	if (Pair == m_DynamicActors.end()) return nullptr;

	return Pair->second;
}

PxRigidActor * CPhysX_Manager::Find_DynamicCollider(const _tchar * pActorTag)
{
	auto Pair = find_if(m_DynamicColliders.begin(), m_DynamicColliders.end(), CTag_Finder(pActorTag));
	if (Pair == m_DynamicColliders.end()) return nullptr;

	return Pair->second;
}

_bool CPhysX_Manager::Raycast_Collision(_float3 vRayPos, _float3 vRayDir, _float fRange, _float3* pPositionOut, CGameObject** pObjectOut)
{
	PxRaycastBuffer hit;
	PxReal distance = fRange;
	PxVec3 origin = CUtile::ConvertPosition_D3DToPx(vRayPos);
	PxVec3 direction = CUtile::ConvertPosition_D3DToPx(vRayDir);
	direction.normalize();	
	
	_bool hitResult = m_pScene->raycast(origin, direction, distance, hit);
	if (hitResult)
	{
		PxRigidActor*		pActor = hit.block.actor;
		PxShape*			pShape = nullptr;
		pActor->getShapes(&pShape, sizeof(PxShape));
		PxShapeFlags Flags = pShape->getFlags();

		if (Flags.isSet(PxShapeFlag::eTRIGGER_SHAPE))
			return false;

		if(pPositionOut)
			*pPositionOut = CUtile::ConvertPosition_PxToD3D(hit.block.position);
		
		if (pObjectOut)
		{
			PX_USER_DATA* pUserData = (PX_USER_DATA*)pActor->userData;
			pUserData && (*pObjectOut = pUserData->pOwner);
		}
	}
		
	return hitResult;
}

_bool CPhysX_Manager::IsMouseOver(HWND hWnd, CGameObject* pTargetObject, _float fRange,  _float3* pPositionOut)
{
	POINT tMouse = CUtile::GetClientCursorPos(hWnd);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_matrix WorldlMatrix = pTargetObject->Get_TransformCom()->Get_WorldMatrix();
	_matrix WorldlMatrixInverse = pTargetObject->Get_TransformCom()->Get_WorldMatrix_Inverse();
	_matrix ViewMatrixInverse = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrixInverse = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);

	D3D11_VIEWPORT ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);
	_uint iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_float4 vMousePos;
	vMousePos.x = tMouse.x / (ViewportDesc.Width * 0.5f) - 1.f;
	vMousePos.y = tMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;
	
	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInverse);
	_float4 vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float4 vRayDir = vMousePos - vRayPos;
	
	vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrixInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInverse);
	vRayDir = XMVector3Normalize(vRayDir);

	PxRaycastBuffer hit;
	PxReal distance = fRange;
	PxVec3 origin = CUtile::ConvertPosition_D3DToPx(_float3(vRayPos.x, vRayPos.y, vRayPos.z));
	PxVec3 direction = CUtile::ConvertPosition_D3DToPx(_float3(vRayDir.x, vRayDir.y, vRayDir.z));
	direction.normalize();

	_bool hitResult = m_pScene->raycast(origin, direction, distance, hit);
	if (hitResult)
	{
		PX_USER_DATA* pUserData = (PX_USER_DATA*)hit.block.actor->userData;
		if (pUserData)
		{
			if (pPositionOut)
				*pPositionOut = CUtile::ConvertPosition_PxToD3D(hit.block.position);
									
			if(pUserData->pOwner == pTargetObject)
				return true;
		}
	}

	return false;
}

void CPhysX_Manager::Set_ActorPosition(const _tchar* pActorTag, _float3 vPosition)
{
	PxRigidActor* pActor = Find_StaticActor(pActorTag);
	if(pActor == nullptr)
		pActor = Find_DynamicActor(pActorTag);

	assert(pActor != nullptr && "CPhysX_Manager::Set_ActorPosition");

	Set_ActorPosition(pActor, vPosition);
}

void CPhysX_Manager::Set_ActorRotation(const _tchar* pActorTag, _float fDegree, _float3 vAxis)
{
	PxRigidActor* pActor = Find_DynamicActor(pActorTag);
	assert(pActor != nullptr && "CPhysX_Manager::Set_ActorRotation");

	Set_ActorRotation(pActor, fDegree, vAxis);
}

void CPhysX_Manager::Set_ActorPosition(PxRigidActor* pActor, _float3 vPosition)
{	
	PxTransform transform = pActor->getGlobalPose();
	transform.p = CUtile::ConvertPosition_D3DToPx(vPosition);
	pActor->setGlobalPose(transform);
}

void CPhysX_Manager::Set_ActorRotation(PxRigidActor* pActor, _float fDegree, _float3 vAxis)
{
	PxQuat Rotation = PxQuat(XMConvertToRadians(fDegree), CUtile::ConvertPosition_D3DToPx(vAxis));
	
	PxTransform transform = pActor->getGlobalPose();
	transform.q = Rotation * transform.q;
	// transform.q = Rotation;
	pActor->setGlobalPose(transform);
}

void CPhysX_Manager::Set_ActorScaling(const _tchar* pActorTag, _float3 vScale)
{
	PxRigidActor* pActor = Find_StaticActor(pActorTag);
	if (pActor == nullptr)
		pActor = Find_DynamicActor(pActorTag);
	if (pActor == nullptr)
		pActor = Find_DynamicCollider(pActorTag);

	assert(pActor != nullptr && "CPhysX_Manager::Set_ActorScaling");

	PX_USER_DATA* p = (PX_USER_DATA*)pActor->userData;

	if (p->eType == BOX_STATIC || p->eType == BOX_DYNAMIC)
		Set_ScalingBox(pActor, vScale);
	else if (p->eType == SPHERE_STATIC || p->eType == SPHERE_DYNAMIC)
		Set_ScalingSphere(pActor, vScale.x);
	else if (p->eType == CAPSULE_STATIC || p->eType == CAPSULE_DYNAMIC)
		Set_ScalingCapsule(pActor, vScale.x, vScale.y);
}

void CPhysX_Manager::Set_ScalingBox(PxRigidActor* pActor, _float3 vScale)
{
	PxShape* shape;
	pActor->getShapes(&shape, sizeof(PxShape));

	PxBoxGeometry newGeometry(PxVec3(vScale.x, vScale.y, vScale.z));
	shape->setGeometry(newGeometry);
}

void CPhysX_Manager::Set_ScalingSphere(PxRigidActor* pActor, _float fRadius)
{
	PxShape* shapes[16];
	const PxU32 nbShapes = pActor->getShapes(shapes, 16);

	PxShape* shape;
	pActor->getShapes(&shape, 1);
	
	PxSphereGeometry sphere;
	shape->getSphereGeometry(sphere);

	sphere.radius = fRadius;

	shape->setGeometry(sphere);
}

void CPhysX_Manager::Set_ScalingCapsule(PxRigidActor* pActor, _float fRadius, _float fHalfHeight)
{
	PxShape* shapes[16];
	const PxU32 nbShapes = pActor->getShapes(shapes, 16);
	
	PxShape* shape;
	pActor->getShapes(&shape, 1);

	PxCapsuleGeometry Capsule;
	shape->getCapsuleGeometry(Capsule);

	Capsule.radius = fRadius;
	Capsule.halfHeight = fHalfHeight;

	shape->setGeometry(Capsule);	
}

void CPhysX_Manager::Imgui_Render()
{
	_uint	 nStaticObjectCount = static_cast<_uint>(m_StaticActors.size());
	_uint nDynamicObjectCount = static_cast<_uint>(m_DynamicActors.size());

	ImGui::BulletText("Object to change PhyX variables");

	{
		static _int iStaticSelectObject = -1;
		char** ppObjectTag = new char*[nStaticObjectCount];

		_uint iTagLength = 0;
		_uint i = 0;

		for (auto& Pair : m_StaticActors)
			ppObjectTag[i++] = CUtile::WideCharToChar(const_cast<_tchar*>(Pair.first));

		ImGui::ListBox("StaticObject List", &iStaticSelectObject, ppObjectTag, nStaticObjectCount);

		if (iStaticSelectObject != -1)
		{
			PxRigidActor*	pRigidActor = Find_StaticGameObject(iStaticSelectObject);

			ImGui::BulletText("Current Static Object : ");
			ImGui::SameLine();
			ImGui::Text(ppObjectTag[iStaticSelectObject]);
			PX_USER_DATA* pUserData = (PX_USER_DATA*)pRigidActor->userData;

			if (pUserData && pUserData->pOwner != nullptr)
				pUserData->pOwner->ImGui_PhysXValueProperty();
		}

		for (_uint i = 0; i < nStaticObjectCount; ++i)
			Safe_Delete_Array(ppObjectTag[i]);
		Safe_Delete_Array(ppObjectTag);
	}

	{
		static _int iDynamicSelectObject = -1;
		char** ppObjectTag = new char*[nDynamicObjectCount];

		_uint iTagLength = 0;
		_uint i = 0;

		for (auto& Pair : m_DynamicActors)
			ppObjectTag[i++] = CUtile::WideCharToChar(const_cast<_tchar*>(Pair.first));

		ImGui::ListBox("DynamicObject List", &iDynamicSelectObject, ppObjectTag, nDynamicObjectCount);

		if (iDynamicSelectObject != -1)
		{
			PxRigidActor*	pRigidActor = Find_DynamicGameObject(iDynamicSelectObject);

			ImGui::BulletText("Current Dynamic Object : ");
			ImGui::SameLine();
			ImGui::Text(ppObjectTag[iDynamicSelectObject]);
			PX_USER_DATA* pUserData = (PX_USER_DATA*)pRigidActor->userData;

			if (pUserData)
				pUserData->pOwner->ImGui_PhysXValueProperty();
		}

		for (_uint i = 0; i < nDynamicObjectCount; ++i)
			Safe_Delete_Array(ppObjectTag[i]);
		Safe_Delete_Array(ppObjectTag);
	}
}

PxRigidActor* CPhysX_Manager::Find_StaticGameObject(_int iIndex)
{
	auto	iter = m_StaticActors.begin();

	for (_int i = 0; i < iIndex; ++i)
		++iter;

	return iter->second;
}

PxRigidActor* CPhysX_Manager::Find_DynamicGameObject(_int iIndex)
{
	auto iter = m_DynamicActors.begin();

	for (_int i = 0; i < iIndex; ++i)
		++iter;

	return iter->second;
}

void CPhysX_Manager::Set_DynamicParameter(const _tchar* pActorTag, _float fMass, _float fLinearDamping, _float3 vVelocity)
{
	PxRigidActor* pActor  = Find_DynamicActor(pActorTag);

	if (pActor == nullptr)
		return;

	((PxRigidDynamic*)pActor)->setMass(fMass);
	((PxRigidDynamic*)pActor)->setLinearDamping(fLinearDamping);
	PxVec3 v = CUtile::ConvertPosition_D3DToPx(vVelocity);
	((PxRigidDynamic*)pActor)->setLinearVelocity(v);
}

void  CPhysX_Manager::Set_DynamicParameter(PxRigidActor * pActor, _float fMass, _float fLinearDamping, _float3 vVelocity)
{
	if (pActor == nullptr)
		return;

	((PxRigidDynamic*)pActor)->setMass(fMass);
	((PxRigidDynamic*)pActor)->setLinearDamping(fLinearDamping);
	PxVec3 v = CUtile::ConvertPosition_D3DToPx(vVelocity);
	((PxRigidDynamic*)pActor)->setLinearVelocity(v);
}

void CPhysX_Manager::Set_Velocity(PxRigidActor * pActor, _float3 vVelocity)
{
	if (pActor == nullptr)
		return;

	PxVec3 v = CUtile::ConvertPosition_D3DToPx(vVelocity);
	((PxRigidDynamic*)pActor)->setLinearVelocity(v);
}

void CPhysX_Manager::Set_ActorFlag_Simulation(const _tchar* pActorTag, _bool bFlag)
{
	PxRigidActor *pActor = Find_Actor(pActorTag);
	if (pActor == nullptr) return;

	Set_ActorFlag_Simulation(pActor, bFlag);
}

void CPhysX_Manager::Set_ActorFlag_Simulation(PxRigidActor* pActor, _bool bFlag)
{
	pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !bFlag);
}

PxRigidActor* CPhysX_Manager::Find_Actor(const _tchar * pActorTag)
{
	PxRigidActor *pActor = Find_StaticActor(pActorTag);
	if (pActor == nullptr) 
	{
		pActor = Find_DynamicActor(pActorTag);
		if (pActor == nullptr) 
		{
			pActor = Find_DynamicCollider(pActorTag);
			if (pActor == nullptr)
			{
				return nullptr;
			}
		}
	}

	return pActor;
}

void CPhysX_Manager::Create_Trigger(PX_TRIGGER_DATA* pTriggerData)
{
	if (pTriggerData == nullptr) return;

	PxTransform Transform(CUtile::ConvertPosition_D3DToPx(pTriggerData->vPos));

	pTriggerData->pTriggerStatic = m_pPhysics->createRigidStatic(Transform);

	PxShape* pShape = m_pPhysics->createShape(PxSphereGeometry(pTriggerData->fRadius), *m_pMaterial, true);
	pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	
	pTriggerData->pTriggerStatic->attachShape(*pShape);
	pTriggerData->pTriggerStatic->userData = pTriggerData;

	m_TriggerDataes.push_back(pTriggerData);
	m_Triggers.emplace(CUtile::Create_StringAuto(pTriggerData->pActortag), pTriggerData->pTriggerStatic);

	m_pScene->addActor(*pTriggerData->pTriggerStatic);
	pShape->release();
}

void CPhysX_Manager::Delete_Actor(PxActor* pActor)
{	
	m_pScene->removeActor(*pActor);
}

void CPhysX_Manager::Delete_Actor(CGameObject* pObject)
{
	PX_USER_DATA* pUserData = nullptr;	
	for (auto Pair = m_DynamicActors.begin(); Pair != m_DynamicActors.end();)
	{
		pUserData = (PX_USER_DATA*)(*Pair).second->userData;
		if (pUserData->pOwner == pObject) 
		{	
			m_pScene->removeActor(*(*Pair).second);
			Pair = m_DynamicActors.erase(Pair);
		}
		else Pair++;
	}

	for (auto Pair = m_DynamicColliders.begin(); Pair != m_DynamicColliders.end();)
	{
		pUserData = (PX_USER_DATA*)(*Pair).second->userData;
		if (pUserData->pOwner == pObject)
		{
			m_pScene->removeActor(*(*Pair).second);
			Pair = m_DynamicColliders.erase(Pair);
		}
		else Pair++;
	}	
}

void CPhysX_Manager::Reset()
{
	PxActorTypeFlags actorTypes = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;

	PxU32 numActors = m_pScene->getNbActors(actorTypes);
	PxActor** actors = nullptr;
	m_pScene->getActors(actorTypes, actors, numActors, 0);

	for (PxU32 i = 0; i < numActors; i++) 
	{
		m_pScene->removeActor(*actors[i]);
		actors[i]->release();
	}
}

void CPhysX_Manager::Delete_DynamicActor(PxRigidActor* pActor)
{	
	for (auto Pair = m_DynamicActors.begin(); Pair != m_DynamicActors.end();)
	{
		if (Pair->second == pActor)
		{
			m_pScene->removeActor(*(*Pair).second);
			Pair->second = nullptr;
			Pair = m_DynamicActors.erase(Pair);
			break;
		}
		else Pair++;
	}
}

PxVec3 CPhysX_Manager::Get_ScalingBox(PxRigidActor *pActor)
{
	PxShape* shape;
	pActor->getShapes(&shape, 1);
	
	PxGeometryHolder newGeometry = shape->getGeometry();
	newGeometry.getType();

	PxVec3 Temp = newGeometry.box().halfExtents;

	return Temp;
}
#ifdef _DEBUG
void CPhysX_Manager::Imgui_Render(const _tchar * pActorName, vector<_float3>* vec_ColiderSize)
{
	if (pActorName == nullptr)
		return;

	wstring wstrActorParentName = pActorName;
	
	if (ImGui::BeginListBox("##"))
	{
		string Actor_ParentName = CUtile::WstringToString(wstrActorParentName);

		char szViewName[256];
		const bool bColider_Selected = false;
		_int iIncreaseIndex = 0;
		for (auto& ProtoPair : m_StaticActors)
		{
			if (ProtoPair.second == nullptr)
				continue;

			wstring  Temp = ProtoPair.first;
			string str = CUtile::WstringToString(Temp);

			if (str.find(Actor_ParentName, 0) != std::string::npos)
			{
				CUtile::WideCharToChar(ProtoPair.first, szViewName);

				if (ImGui::Selectable(szViewName, bColider_Selected))
				{
					wstrSelectedTag = ProtoPair.first;
					
					size_t size = Actor_ParentName.size() + 1;
					string NewTemp = szViewName;
					string Sour = "";
					Sour = NewTemp.substr(size);
					iSelectColider_Index = atoi(Sour.c_str());
				}
			}
		}
		ImGui::EndListBox();
	}

	if (wstrSelectedTag != L"")
	{
		PxRigidActor*	pRigidActor = Find_StaticActor(wstrSelectedTag.c_str());

		ImGui::BulletText("Current Static Actor : ");
		ImGui::SameLine();
		string Temp = CUtile::WstringToString(wstrSelectedTag);
		ImGui::Text(Temp.c_str());

		_float3 vScale = (*vec_ColiderSize)[iSelectColider_Index];
		float fScale[3] = { vScale.x, vScale.y, vScale.z };

		ImGui::DragFloat3("Px_Scale", fScale, 0.01f, 0.1f, 100.0f);

		vScale.x = fScale[0];  vScale.y = fScale[1];  vScale.z = fScale[2];

		Set_ScalingBox(pRigidActor, vScale);
		(*vec_ColiderSize)[iSelectColider_Index] = vScale;
	}
}
#endif

void CPhysX_Manager::Set_Visualization(PxRigidActor *pActor, _bool bFlag)
{
	PxShape* shape;
	pActor->getShapes(&shape, 1);

	PxGeometryHolder newGeometry = shape->getGeometry();
	shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, bFlag);
}

_bool CPhysX_Manager::IsFalling(PxRigidDynamic* pActor)
{	
	const PxVec3& velocity = pActor->getLinearVelocity();

	return velocity.y < -0.5f;
}

void CPhysX_Manager::Create_Controller(PX_CONTROLLER_CAPSULE_DESC& ControllerCapsuleDesc, PX_USER_DATA* pUserData)
{
	if (pUserData)
		m_UserDataes.push_back(pUserData);

	PxCapsuleControllerDesc PxContollerDesc;	

	PxContollerDesc.position = PxExtendedVec3(ControllerCapsuleDesc.vPos.x, ControllerCapsuleDesc.vPos.y, ControllerCapsuleDesc.vPos.z);
	PxContollerDesc.density = ControllerCapsuleDesc.fDensity;
	PxContollerDesc.radius = ControllerCapsuleDesc.fRadius;
	PxContollerDesc.height = ControllerCapsuleDesc.fHeight;
	PxContollerDesc.upDirection = PxVec3(ControllerCapsuleDesc.vUpDirection.x, ControllerCapsuleDesc.vUpDirection.y, ControllerCapsuleDesc.vUpDirection.z);
	PxContollerDesc.material = m_pPhysics->createMaterial(ControllerCapsuleDesc.fStaticFriction, ControllerCapsuleDesc.fDynamicFriction, ControllerCapsuleDesc.fRestitution);
	PxContollerDesc.userData = pUserData;
	PxContollerDesc.contactOffset = ControllerCapsuleDesc.fContactOffset;
	PxContollerDesc.stepOffset = ControllerCapsuleDesc.fStepOffset;	
	PxContollerDesc.slopeLimit = ControllerCapsuleDesc.fSlopeLimit;
	
	PxController* pController = m_pControlllerManager->createController(PxContollerDesc);
	pController->getActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	
	m_Controllers.emplace(CUtile::Create_StringAuto(ControllerCapsuleDesc.pTag), pController);
}

void CPhysX_Manager::Move_Controller(PxController* pController, _float3 vDist)
{	
	PxVec3 vPxDist = CUtile::ConvertPosition_D3DToPx(vDist);

	PxControllerFilters filters;
	pController->move(vPxDist, 0.01f, 1.0f / 60.0f, filters);
}

PxController* CPhysX_Manager::Find_Controller(const _tchar* pTag)
{
	auto Pair = find_if(m_Controllers.begin(), m_Controllers.end(), CTag_Finder(pTag));
	if (Pair == m_Controllers.end()) return nullptr;

	return Pair->second;	
}