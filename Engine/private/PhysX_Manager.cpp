#include "stdafx.h"
#include "..\public\PhysX_Manager.h"
#include "Utile.h"
#include "String_Manager.h"

PxFilterFlags CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eDETECT_CCD_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_CCD
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eCONTACT_EVENT_POSE;

	/*
	eCONTACT_DEFAULT: ����ó ������ ���� �⺻ �����Դϴ�. �̰��� �ٸ� ��� �÷��׸� �⺻������ �����ϴ� �Ͱ� �����ϴ�.
	eNOTIFY_TOUCH_FOUND: �� ����� ������ ó�� �����Ǹ� ���ø����̼ǿ� �˸��ϴ�.
	eNOTIFY_TOUCH_LOST: �� ����� ������ �������� �� ���ø����̼ǿ� �˸��ϴ�.
	eNOTIFY_THRESHOLD_FORCE_FOUND: �� ���� ������ ���� ������ �Ӱ谪�� �ʰ��ϸ� ���ø����̼ǿ� �˸��ϴ�.
	eNOTIFY_THRESHOLD_FORCE_LOST: �� ���� ������ ���� ������ �Ӱ谪 �Ʒ��� �������� ���ø����̼ǿ� �˸��ϴ�.
	eNOTIFY_CONTACT_POINTS: �� ����� ���� ������ �˸��ϴ�.
	eSOLVE_CONTACT: �浹 ���� �� �� ������ ���� ������ �����մϴ�.
	eMODIFY_CONTACTS: ���ø����̼��� �� ��� ������ ������ ������ �� �ֵ��� ����մϴ�.
	eDETECT_DISCRETE_CONTACT: �ҿ��� �浹 ������ ����Ͽ� �� ���� ���� ������ �����մϴ�.
	eDETECT_CCD_CONTACT: ���� �浹 ������ ����Ͽ� �� ���� ���� ������ �����մϴ�.
	ePRE_SOLVER_VELOCITY: �ֹ��� ����Ǳ� ���� ���Ϳ� �ӵ� ������ �����մϴ�.
	ePOST_SOLVER_VELOCITY: �ֹ� ���� �� ���Ϳ� �ӵ� ������ �����մϴ�.
	eCONTACT_EVENT_POSE: �� ����� ���� ���� ������ �˸��ϴ�.
	eNEXT_FREE: ���� �ݺ��ϴ� �� ���˴ϴ� PxPairFlag.
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
}

HRESULT CPhysX_Manager::Initialize()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxDefaultAllocatorCallback, m_PxDefaultErrorCallback);
	assert(m_pFoundation != nullptr && "CPhysX_Manager::InitWorld()");
	
	m_pPvd = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL);
	
	m_PxTolerancesScale.length = 100;
	m_PxTolerancesScale.speed = 981;
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, m_PxTolerancesScale, true, m_pPvd);
	assert(m_pPhysics != nullptr && "CPhysX_Manager::InitWorld()");

	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	
	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());		
	SceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	SceneDesc.cpuDispatcher = m_pDispatcher;	
	SceneDesc.filterShader = CustomFilterShader;
	SceneDesc.simulationEventCallback = &m_EventCallback;
	m_pScene = m_pPhysics->createScene(SceneDesc);	
	assert(m_pScene != nullptr && "CPhysX_Manager::InitWorld()");
	
	/* Release������ 5�� �ּ�? */
	PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
	assert(pPvdClient != nullptr && "CPhysX_Manager::InitWorld()");		
	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));
	assert(m_pCooking != nullptr && "CPhysX_Manager::InitWorld()");
	
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// �⺻ �� ����	
	PxRigidStatic* pGroundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 1), *m_pMaterial);	
	m_pScene->addActor(*pGroundPlane);
	
	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{	
	static const PxF32 const PxTimeDelta = 1.0f / 60.0f;
	
	PX_UNUSED(false);
	m_pScene->simulate(PxTimeDelta);
	m_pScene->fetchResults(true);

	Update_Trasnform(fTimeDelta);
}

void CPhysX_Manager::Update_Trasnform(_float fTimeDelta)
{
	PX_USER_DATA* pUserData = nullptr;
	PxRigidDynamic* pActor = nullptr;

	for (auto Pair : m_DynamicActors)
	{
		pActor = (PxRigidDynamic*)Pair.second;
		pUserData = (PX_USER_DATA*)pActor->userData;

		PxTransform ActorTrasnform = pActor->getGlobalPose();
		_float3 vObjectPos = CUtile::ConvertPosition_PxToD3D(ActorTrasnform.p);

		pUserData->pOwner->Set_Position(vObjectPos);
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
}

void CPhysX_Manager::Create_TriangleMeshActor_Static(PxTriangleMeshDesc& Desc)
{	
	_float halfExtent = 0.1f;
	PxU32 size = 1;
	
	PxDefaultMemoryOutputStream WriteBuffer;
	m_pCooking->cookTriangleMesh(Desc, WriteBuffer);

	PxDefaultMemoryInputData ReadBuffer(WriteBuffer.getData(), WriteBuffer.getSize());
	PxTriangleMesh* pMesh = m_pPhysics->createTriangleMesh(ReadBuffer);

	PxTransform Transform(PxIdentity);
	PxRigidStatic *pBody = m_pPhysics->createRigidStatic(Transform);
	PxShape* shape = m_pPhysics->createShape(PxTriangleMeshGeometry(pMesh), *m_pMaterial, true);

	pBody->attachShape(*shape);
	m_pScene->addActor(*pBody);
}

void CPhysX_Manager::Create_Box(PX_BOX_DESC& Desc, PX_USER_DATA* pUserData)
{
	if (Desc.eType == BOX_STATIC)
	{	
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidStatic* pBox = m_pPhysics->createRigidStatic(Transform);
	
		PxShape* pShpae = m_pPhysics->createShape(PxBoxGeometry(Desc.vSize.x, Desc.vSize.y, Desc.vSize.z), *m_pMaterial, true);
		pBox->attachShape(*pShpae);
		
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

	}
	else if (Desc.eType == BOX_DYNAMIC)
	{	
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidDynamic* pBox = m_pPhysics->createRigidDynamic(Transform);

		PxShape* pShape = m_pPhysics->createShape(PxBoxGeometry(Desc.vSize.x, Desc.vSize.y, Desc.vSize.z), *m_pMaterial, true);
		
		pBox->attachShape(*pShape);
		pBox->setAngularDamping(Desc.fAngularDamping);
		pBox->setLinearVelocity(PxVec3(Desc.vVelocity.x, Desc.vVelocity.y, Desc.vVelocity.z));
		PxRigidBodyExt::updateMassAndInertia(*pBox, Desc.fDensity);
			
		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pBox->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_DynamicActors.emplace(pTag, pBox);
		
		m_pScene->addActor(*pBox);
	}	
}

void CPhysX_Manager::Create_Sphere(PX_SPHERE_DESC & Desc, PX_USER_DATA * pUserData)
{
	if (Desc.eType == SPHERE_STATIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidStatic* pSphere = m_pPhysics->createRigidStatic(Transform);

		PxShape* pShape = PxRigidActorExt::createExclusiveShape(*pSphere, PxSphereGeometry(Desc.fRadius), *m_pMaterial);		
		// pSphere->attachShape(*pShape);

		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pSphere->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_DynamicActors.emplace(pTag, pSphere);

		m_pScene->addActor(*pSphere);
	}
	else if (Desc.eType == SPHERE_DYNAMIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidDynamic* pSphere = m_pPhysics->createRigidDynamic(Transform);

		PxShape* pShape = PxRigidActorExt::createExclusiveShape(*pSphere, PxSphereGeometry(Desc.fRadius), *m_pMaterial);		
		// pSphere->attachShape(*pShape);
		pSphere->setAngularDamping(Desc.fAngularDamping);
		pSphere->setLinearVelocity(PxVec3(Desc.vVelocity.x, Desc.vVelocity.y, Desc.vVelocity.z));		
		PxRigidBodyExt::updateMassAndInertia(*pSphere, Desc.fDensity);

		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pSphere->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_DynamicActors.emplace(pTag, pSphere);

		m_pScene->addActor(*pSphere);
	}	
}

void CPhysX_Manager::Create_Capsule(PX_CAPSULE_DESC& Desc, PX_USER_DATA* pUserData)
{
	if (Desc.eType == CAPSULE_STATIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidStatic *pCapsule = m_pPhysics->createRigidStatic(Transform);
		
		PxShape* pShape = PxRigidActorExt::createExclusiveShape(*pCapsule, PxCapsuleGeometry(Desc.fRadius, Desc.fHalfHeight), *m_pMaterial);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
		// pCapsule->attachShape(*pShape);
		
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
	}
	else if (Desc.eType == CAPSULE_DYNAMIC)
	{
		PxTransform Transform(CUtile::ConvertPosition_D3DToPx(Desc.vPos));
		PxRigidDynamic *pCapsule = m_pPhysics->createRigidDynamic(Transform);
		
		PxShape* pShape = PxRigidActorExt::createExclusiveShape(*pCapsule, PxCapsuleGeometry(Desc.fRadius, Desc.fHalfHeight), *m_pMaterial);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
		
		// pCapsule->attachShape(*pShape);
		pCapsule->setAngularDamping(Desc.fAngularDamping);
		pCapsule->setLinearVelocity(PxVec3(Desc.vVelocity.x, Desc.vVelocity.y, Desc.vVelocity.z));
		PxRigidBodyExt::updateMassAndInertia(*pCapsule, Desc.fDensity);
		
		if (pUserData)
		{
			pUserData->eType = Desc.eType;
			pCapsule->userData = pUserData;
			m_UserDataes.push_back(pUserData);
		}

		_tchar *pTag = CUtile::Create_String(Desc.pActortag);
		CString_Manager::GetInstance()->Add_String(pTag);
		m_DynamicActors.emplace(pTag, pCapsule);

		m_pScene->addActor(*pCapsule);		
	}	
}

void CPhysX_Manager::Get_ActorMatrix(_uint iPxActorIndex)
{
	/*
	PxRigidActor* pActor = Get_ActorPtr(iPxActorIndex);
	const PxU32 nbShapes = pActor->getNbShapes();
	PX_ASSERT(nbShapes <= 128);

	PxShape* shapes[128];
	pActor->getGlobalPose();
	pActor->getShapes(shapes, nbShapes);

	PxTransform actorTransform = pActor->getGlobalPose();
	PxVec3 actorPosition = actorTransform.p;
	PxQuat actorOrientation = actorTransform.q;

	// pActor->getGlobalPose().transform.p;

	for (PxU32 j = 0; j < nbShapes; j++)
	{
		const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *pActor));
		PxGeometryHolder h = shapes[j]->getGeometry();

		if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
			continue;

		// render object
		_float4x4 mw;
		mw(0, 0) = shapePose(0, 0); mw(0, 1) = shapePose(0, 1); mw(0, 2) = shapePose(0, 2); mw(0, 3) = shapePose(0, 3);
		mw(1, 0) = shapePose(1, 0); mw(1, 1) = shapePose(1, 1); mw(1, 2) = shapePose(1, 2); mw(1, 3) = shapePose(1, 3);
		mw(2, 0) = shapePose(2, 0); mw(2, 1) = shapePose(2, 1); mw(2, 2) = shapePose(2, 2); mw(2, 3) = shapePose(2, 3);
		mw(3, 0) = shapePose(3, 0); mw(3, 1) = shapePose(3, 1); mw(3, 2) = shapePose(3, 2); mw(3, 3) = shapePose(3, 3);
	}
	*/
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

_bool CPhysX_Manager::Raycast_Collision(_float3 vRayPos, _float3 vRayDir, _float fRange, _float3* pOut)
{
	PxRaycastBuffer hit;
	PxReal distance = fRange;
	PxVec3 origin = CUtile::ConvertPosition_D3DToPx(vRayPos);
	PxVec3 direction = CUtile::ConvertPosition_D3DToPx(vRayDir);
	direction.normalize();	
	
	_bool hitResult = m_pScene->raycast(origin, direction, distance, hit);
	if (hitResult && pOut)
		*pOut = CUtile::ConvertPosition_PxToD3D(hit.block.position);

	return hitResult;
}

void CPhysX_Manager::Set_ActorPosition(const _tchar* pActorTag, _float3 vPosition)
{
	PxRigidActor* pActor = Find_StaticActor(pActorTag);
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
	{
		pActor = Find_DynamicActor(pActorTag);
	}
	assert(pActor != nullptr && "CPhysX_Manager::Set_ActorPosition");

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
	pActor->getShapes(&shape, 1);

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

