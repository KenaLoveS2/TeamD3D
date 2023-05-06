#include "stdafx.h"
#include "..\public\Dynamic_StoneCube.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"

CDynamic_StoneCube::CDynamic_StoneCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CDynamic_StoneCube::CDynamic_StoneCube(const CDynamic_StoneCube& rhs)
	: CGameObject(rhs)
{
}


void CDynamic_StoneCube::Set_CollActive()
{
	m_pTransformCom->Set_PxActorActive(true);
	m_pTransformCom->Set_Position(m_StoneCubeDesc.vPos);


	// m_pTransformCom->Set_PxActorSleep(false);
	// CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	// PxRigidActor* pCurActor = (pPhysX->Find_DynamicActor(m_szCloneObjectTag));
	// assert(nullptr != pCurActor && "CDynamic_StoneCube::Set_CollActive");
	// pPhysX->WakeUp(reinterpret_cast<PxRigidDynamic*>(pCurActor));
	//reinterpret_cast<PxRigidDynamic*>(pCurActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	//pPhysX->WakeUp(reinterpret_cast<PxRigidDynamic*>(pCurActor));

}

HRESULT CDynamic_StoneCube::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamic_StoneCube::Initialize(void* pArg)
{
	if(pArg!= nullptr)
	{
		memcpy(&m_StoneCubeDesc, pArg, sizeof(m_StoneCubeDesc));
	}

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_StoneCubeDesc.vPos.x, m_StoneCubeDesc.vPos.y, m_StoneCubeDesc.vPos.z, 1.f));
	m_pTransformCom->Set_Position(m_StoneCubeDesc.vPos);

	return S_OK;
}

HRESULT CDynamic_StoneCube::Late_Initialize(void* pArg)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_DYNAMIC;
	BoxDesc.vPos = _float3(-100.f, 0.f, -100.f);
	BoxDesc.vSize = m_StoneCubeDesc.vSize;
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = m_StoneCubeDesc.fMass;
	BoxDesc.fLinearDamping = 3.f;
	BoxDesc.fAngularDamping = m_StoneCubeDesc.fLinearDamping;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;
	BoxDesc.bKinematic = false;
	
	//pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, true, COL_DYNAMIC_ENVIOBJ, true, false)); //뒤에 트루 써주면된다.   _bool bRightUpLookSync == true 쓰면됀다. _bool isActive == true 쓰면됀다.
	//m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag,_float3(0.f,0.f,0.f), true); // Pivot 안슬거면 000 하면된다.   _bool bRightUpLookSync == true 로 주면된다. 

	/*
	PxRigidActor* pCurActor = (pPhysX->Find_DynamicActor(m_szCloneObjectTag));
	assert(nullptr != pCurActor && "CDynamic_StoneCube::Late_Initialize");
	pPhysX->PutToSleep(reinterpret_cast<PxRigidDynamic*>(pCurActor));
	*/
	return S_OK;
}

void CDynamic_StoneCube::Tick(_float fTimeDelta)
{
	/*
		if (m_bTestOnce == false)
		{
			Late_Initialize();
			m_bTestOnce = true;
		}

	*/

	
	__super::Tick(fTimeDelta);
}

void CDynamic_StoneCube::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
	{
#ifdef _DEBUG
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_CINE, this);
#endif
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CDynamic_StoneCube::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);

			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr && (*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_EMISSIVE] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				float f = 1.f;
				FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &f, sizeof(float)), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 4), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
			}
			else
			{
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
			}
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			
			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 17), E_FAIL);
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 18), E_FAIL);
			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 16), E_FAIL);
			}
			else
			{
				FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
			}
		}
	}

	return S_OK;
}

HRESULT CDynamic_StoneCube::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 21), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CDynamic_StoneCube::RenderCine()
{
	if (FAILED(__super::RenderCine()))
		return E_FAIL;

	//if (FAILED(__super::SetUp_CineShaderResources()))
	//	return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 12), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 20), E_FAIL);
		}
	}

	return S_OK;
}

void CDynamic_StoneCube::Actor_Clear()
{
	m_pTransformCom->Clear_Actor();
}

void CDynamic_StoneCube::Imgui_Move_Pos(_float4 vParentPos)
{
	_vector vChangePos = 
		XMVectorSetW(XMLoadFloat3(&m_StoneCubeDesc.vPos),1.f) - XMLoadFloat4(&m_StoneCubeDesc.vParentPos);

	vChangePos += XMLoadFloat4(&vParentPos);

	m_pTransformCom->Set_Position(vChangePos);

}


//void CDynamic_StoneCube::ImGui_PhysXValueProperty()
//{
//	__super::ImGui_PhysXValueProperty();
//
//	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();
//
//	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
//	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
//	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
//	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
//	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);
//
//	_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();
//
//	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
//	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
//	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
//	m_pTransformCom->Set_PxPivot(vPxPivot);
//}

HRESULT CDynamic_StoneCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_StoneCubeDesc.pModelName, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom,this)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamic_StoneCube::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CDynamic_StoneCube::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CDynamic_StoneCube* CDynamic_StoneCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamic_StoneCube* pInstance = new CDynamic_StoneCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDynamic_StoneCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDynamic_StoneCube::Clone(void* pArg)
{
	CDynamic_StoneCube* pInstance = new CDynamic_StoneCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDynamic_StoneCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_StoneCube::Free()
{
	
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

void CDynamic_StoneCube::Execute_SleepEnd()
{	
	m_pTransformCom->Connect_PxActor_Gravity(m_szCloneObjectTag, _float3(0.f, 0.f, 0.f), true);
	m_pTransformCom->Set_Position(m_StoneCubeDesc.vPos);
}