#include "stdafx.h"
#include "..\public\Dynamic_Stone.h"
#include "GameInstance.h"
#include "Dynamic_StoneCube.h"
#include "Monster.h"
#include "Kena.h"
CDynamic_Stone::CDynamic_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CDynamic_Stone::CDynamic_Stone(const CDynamic_Stone& rhs)
	: CEnviromentObj(rhs)
{
}


HRESULT CDynamic_Stone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamic_Stone::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bRenderActive = true;
	return S_OK;
}

HRESULT CDynamic_Stone::Late_Initialize(void* pArg)
{
	CGameInstance* pGameInst = CGameInstance::GetInstance();
	CGameObject* pCreateObject = nullptr;
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float3 vSize = _float3(1.f, 1.f, 1.f);

	CDynamic_StoneCube::tag_Dynamic_StoneCube StoneCubeDesc;

	_int iMaxIndex = 3, jMaxIndex = 3, kMaxIndex = 3;

	StoneCubeDesc.fMass = 50.f;
	StoneCubeDesc.fLinearDamping = 3.f;

	float	boxRadius = 0.20f; 
	int		boxCountPerLayer = 2; 
	float	layerHeight = 1.f;  //StoneCubeDesc.vSize.y; 
	float	layerRadiusStep = 0.0f; 
	int		layerCount = 5; 

	_float3 vColiderSize = { 0.f,0.f,0.f };

	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall03")
	{
		StoneCubeDesc.vSize = _float3(0.16f, 0.16f, 0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.1f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.8f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.3f;
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall03");
		vColiderSize = _float3(0.7f, 2.5f, 0.7f);
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall04")
	{
		StoneCubeDesc.vSize = _float3(0.16f, 0.16f, 0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.5f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.34f;
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall04");
		vColiderSize = _float3(1.13f, 2.88f, 1.13f); 
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall05")
	{
		StoneCubeDesc.vSize = _float3(0.16f, 0.16f, 0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.8f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.4f;
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall05");
		vColiderSize = _float3(0.9f, 3.3f, 0.9f); 
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall06")
	{
		StoneCubeDesc.vSize = _float3(0.16f, 0.16f, 0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.8f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.3f;
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall06");
		vColiderSize = _float3(1.f, 2.8f, 1.f); 

	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_MaskShrine_Rock_09")
	{
		StoneCubeDesc.vSize = _float3(0.09f, 0.09f, 0.09f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.4f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.3f;
		layerRadiusStep = 0.0f; 
		layerCount = 6; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_MaskShrine_Rock_09");
		vColiderSize = _float3( 0.26f,1.2f,0.26f ); 

	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockMedium04")
	{
		StoneCubeDesc.vSize = _float3(0.16f, 0.16f, 0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;


		boxRadius = 0.7f;
		boxCountPerLayer = 2; 
		layerHeight = 0.43f;
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockMedium04");
		vColiderSize = _float3(0.9f, 2.44f, 0.9f);
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockMedium06")
	{
		StoneCubeDesc.vSize = _float3(0.16f, 0.16f, 0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;
		
		boxRadius = 0.7f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.3f;
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockMedium06");
		vColiderSize = _float3(0.67f, 2.4f, 0.67f); 
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockMedium07")
	{
		StoneCubeDesc.vSize = _float3(0.16f,0.16f,0.16f);
		StoneCubeDesc.fMass = 10000.f;
		StoneCubeDesc.fLinearDamping = 0.01f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.7f; 
		boxCountPerLayer = 2; 
		layerHeight = 0.5f;		//StoneCubeDesc.vSize.y; 
		layerRadiusStep = 0.0f; 
		layerCount = 5; 
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockMedium07");
		vColiderSize = _float3(0.67f, 2.4f, 0.67f); 
	}

	vSize.x += 0.05f;
	vSize.y += 0.05f;
	vSize.z += 0.05f;
	/*for (_int i = 0; i < iMaxIndex; i++)
	{
		for (_int j = 0; j < jMaxIndex; j++)
		{
			for (_int k = 0; k < kMaxIndex; k++)
			{
				_int iIndex = (i * iMaxIndex * jMaxIndex) + (j * jMaxIndex) + k;

				pCreateObject = nullptr;

				const _tchar* NewName  = CUtile::Create_DummyString(m_szCloneObjectTag, iIndex);

				if (FAILED(pGameInst->Clone_GameObject(g_LEVEL,
					TEXT("Layer_Enviroment"),
					TEXT("Prototype_GameObject_Dynamic_StoneCube"),
					NewName, &m_EnviromentDesc, &pCreateObject)))
					assert(!"CImgui_MapEditor::CDynamic_Stone::Late_Initialize");

				pCreateObject->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION,
					XMVectorSet(vPos.x + _float(i * vSize.x), vPos.y+ _float(j + vSize.y), vPos.z + _float(k * vSize.z), 1.f));

				static_cast<CDynamic_StoneCube*>(pCreateObject)->Set_StoneCubeDesc(StoneCubeDesc);

			}
		}
	}*/

	
	
	_uint iTemp = 0;
	for (int layer = 0; layer < layerCount; ++layer) 
	{
		float layerRadius = boxRadius + layerRadiusStep * layer;
		int boxCount = boxCountPerLayer * (layer + 1);
		float angleStep = PxTwoPi / boxCount;

		for (int i = 0; i < boxCount; ++i)
		{
			PxVec3 Pxpos(layerRadius * std::cos(angleStep * i),
				layerHeight* layer,
				layerRadius * std::sin(angleStep *i ));

			StoneCubeDesc.vParentPos = vPos;
			Pxpos.x += vPos.x; StoneCubeDesc.vPos.x = Pxpos.x;
			Pxpos.y += vPos.y; StoneCubeDesc.vPos.y = Pxpos.y;
			Pxpos.z += vPos.z; StoneCubeDesc.vPos.z = Pxpos.z;
			
			pCreateObject = nullptr;

		
			const _tchar* NewName = CUtile::Create_DummyString(m_szCloneObjectTag, ++iTemp);

			pCreateObject = pGameInst->Clone_GameObject(
				TEXT("Prototype_GameObject_Dynamic_StoneCube"),
				NewName, &StoneCubeDesc);
			assert(nullptr != pCreateObject  && 
							"CImgui_MapEditor::CDynamic_Stone::Late_Initialize");
						
			pCreateObject->Late_Initialize();

			m_pDynamicObj_List.push_back(pCreateObject);
		}
	}

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;		
	BoxDesc.vPos = CUtile::Float_4to3(vPos);
	BoxDesc.vSize = vColiderSize;
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
	BoxDesc.bKinematic = true;
	//pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_ENVIROMENT));
	//m_pActor = pPhysX->Find_StaticActor(m_szCloneObjectTag);

	return S_OK;
}

void CDynamic_Stone::Tick(_float fTimeDelta)
{

#ifdef FOR_MAP_GIMMICK

#else
	if(m_bTestLateInit == false)
	{
		Late_Initialize();
		m_bTestLateInit = true;
	}
#endif

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	for (auto& pObj : m_pDynamicObj_List)
	{
		if (pObj != nullptr)
		{
#ifdef FOR_MAP_GIMMICK

#else
			static_cast<CDynamic_StoneCube*>(pObj)->Imgui_Move_Pos(vPos);
#endif
			pObj->Tick(fTimeDelta);
		}
	}

	__super::Tick(fTimeDelta);
}

void CDynamic_Stone::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bOnceColl && m_pActor)
	{
		CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
		pPhysX->Delete_Actor(m_pActor);
		m_pActor = nullptr;
	}

	if (m_bRenderActive == false)
		return;

	for (auto& pObj : m_pDynamicObj_List)
	{
		if (pObj != nullptr)
			pObj->Late_Tick(fTimeDelta);
	}


}

HRESULT CDynamic_Stone::Render()
{
	return S_OK;
}

HRESULT CDynamic_Stone::RenderShadow()
{
	return S_OK;
}

HRESULT CDynamic_Stone::RenderCine()
{
	return S_OK;
}

_int CDynamic_Stone::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget == nullptr || COL_DYNAMIC_ENVIOBJ == iColliderIndex)
		return 0;

	if (m_bOnceColl == true)		
		return 0;

	_bool bMonsterRealAttack = false;
	_bool bPlayerWeaponAttack = false;
	if ( ((iColliderIndex == (_int)COL_MONSTER_WEAPON && 
			(bMonsterRealAttack = ((CMonster*)pTarget)->IsRealAttack())))
		||  (iColliderIndex == (_int)COL_PLAYER_WEAPON && 
			(bPlayerWeaponAttack =  dynamic_cast<CKena*>(pTarget)->Get_State(CKena::STATE_ATTACK))))
	{
		for (auto& pObj : m_pDynamicObj_List)
		{
			if (pObj != nullptr)
			{
				static_cast<CDynamic_StoneCube*>(pObj)->Set_CollActive();
				// static_cast<CDynamic_StoneCube*>(pObj)->Execute_SleepEnd();
			}				
		}

		m_bOnceColl = true;
	}

	return 0;
}

void CDynamic_Stone::ImGui_PhysXValueProperty()
{
	//CPhysX_Manager::GetInstance()->Imgui_Render(m_szCloneObjectTag);

	//if(ImGui::Button("TestTest"))
	//{
	//	for (auto& pObj : m_pDynamicObj_List)
	//	{
	//		if (pObj != nullptr)
	//		{
	//			static_cast<CDynamic_StoneCube*>(pObj)->Set_CollActive();
	//			// static_cast<CDynamic_StoneCube*>(pObj)->Execute_SleepEnd();
	//		}
	//	}
	//}

}


CDynamic_Stone* CDynamic_Stone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamic_Stone* pInstance = new CDynamic_Stone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDynamic_Stone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDynamic_Stone::Clone(void* pArg)
{
	CDynamic_Stone* pInstance = new CDynamic_Stone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDynamic_Stone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_Stone::Free() 
{
	if(m_isCloned)
	{

		for (auto& pObj : m_pDynamicObj_List)
		{
			if (pObj != nullptr)
			{
				dynamic_cast<CDynamic_StoneCube*>(pObj)->Actor_Clear();
				Safe_Release(pObj);
			}
		}
		m_pDynamicObj_List.clear();
	}
	__super::Free();
}
