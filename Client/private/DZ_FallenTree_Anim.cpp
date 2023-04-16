#include "stdafx.h"
#include "DZ_FallenTree_Anim.h"
#include "GameInstance.h"

#include "ControlRoom.h"


CDZ_FallenTree_Anim::CDZ_FallenTree_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CDZ_FallenTree_Anim::CDZ_FallenTree_Anim(const CDZ_FallenTree_Anim & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CDZ_FallenTree_Anim::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDZ_FallenTree_Anim::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	

	m_pModelCom->Set_AnimIndex(1);

	Push_EventFunctions();
	return S_OK;
}

HRESULT CDZ_FallenTree_Anim::Late_Initialize(void * pArg)
{
	CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));

	pCtrlRoom->Add_Gimmick_TrggerObj(m_szCloneObjectTag, this);

	

	return S_OK;
}

void CDZ_FallenTree_Anim::Tick(_float fTimeDelta)
{

#ifdef FOR_MAP_GIMMICK

#else
	if (!m_bOnlyTest) /*Test*/
	{
		Late_Initialize();
		m_bOnlyTest = true;
		CGameInstance::GetInstance()->Add_AnimObject(g_LEVEL, this);
	}
#endif
	__super::Tick(fTimeDelta);

	if(m_bBossClear)
	{
		m_pModelCom->Set_AnimIndex(0);
		m_bBossClear = false;
	}

	

	//
	if((false==m_bColiderOn && m_pModelCom->Get_AnimIndex() == 0 && m_pModelCom->Get_AnimationFinish()))
	{
		m_pModelCom->Set_AnimIndex(2);
	

#ifdef FOR_MAP_GIMMICK	
		Create_Colider();
#endif

		m_bColiderOn = true;
	}

	/*Culling*/
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_vector camPos = XMLoadFloat4(&vCamPos);
	const _vector	 vDir = camPos - vPos;

	_float f = XMVectorGetX(XMVector4Length(vDir));
	if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
		m_bRenderCheck = false;
	if (m_bRenderCheck == true)
		m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 100.f);

	/*~Culling*/

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CDZ_FallenTree_Anim::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bRenderActive && false == m_bRenderCheck)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CDZ_FallenTree_Anim::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CDZ_FallenTree_Anim::RenderShadow()
{
	if (FAILED(__super::RenderShadow())) return E_FAIL;
	if (FAILED(SetUp_ShadowShaderResources())) return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 11);

	return S_OK;
}

void CDZ_FallenTree_Anim::Create_Colider()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	// 0번
	{
		ZeroMemory(&BoxDesc, sizeof(BoxDesc));
		BoxDesc.pActortag = CUtile::Create_DummyString(m_szCloneObjectTag, 0);
		BoxDesc.eType = BOX_STATIC;
		BoxDesc.vPos = CUtile::Float_4to3(vPos);
		BoxDesc.vSize = _float3(2.1f, 5.85f, 2.1f);
		BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
		BoxDesc.fDegree = 0.f;
		BoxDesc.isGravity = false;
		BoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
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
		pPhysX->Create_Box(BoxDesc, Create_PxUserData(nullptr, false, COL_ENVIROMENT));
	}

	// 1번
	{
		_float4 vPos2 = vPos;
		vPos2.x += 3.35f;
		vPos2.y += 6.84f;
		vPos2.z -= 0.2f;

		ZeroMemory(&BoxDesc, sizeof(BoxDesc));
		BoxDesc.pActortag = CUtile::Create_DummyString(m_szCloneObjectTag, 1);
		BoxDesc.eType = BOX_STATIC;
		BoxDesc.vPos = CUtile::Float_4to3(vPos2);
		BoxDesc.vSize = _float3(1.06f, 2.1f, 1.61f);
		BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
		BoxDesc.fDegree = 0.f;
		BoxDesc.isGravity = false;
		BoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
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

		pPhysX->Create_Box(BoxDesc, Create_PxUserData(nullptr, false, COL_ENVIROMENT));
		PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_Actor(BoxDesc.pActortag);
		CPhysX_Manager::GetInstance()->Set_ActorRotation(pActor, 72.f, _float3(0.f, 0.f, 1.f));
	}

	// 2번
	{
		_float4 vPos3 = vPos;
		vPos3.x += 9.31f;
		vPos3.y += 8.3f;
		vPos3.z -= 0.06f;

		ZeroMemory(&BoxDesc, sizeof(BoxDesc));
		BoxDesc.pActortag = CUtile::Create_DummyString(m_szCloneObjectTag, 2);
		BoxDesc.eType = BOX_STATIC;
		BoxDesc.vPos = CUtile::Float_4to3(vPos3);
		BoxDesc.vSize = _float3(0.92f, 4.03f, 1.61f);
		BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
		BoxDesc.fDegree = 0.f;
		BoxDesc.isGravity = false;
		BoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
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

		pPhysX->Create_Box(BoxDesc, Create_PxUserData(nullptr, false, COL_ENVIROMENT));
		PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_Actor(BoxDesc.pActortag);
		CPhysX_Manager::GetInstance()->Set_ActorRotation(pActor, 72.f, _float3(0.f, 0.f, 1.f));
	}

	// 3번
	{
		_float4 vPos4 = vPos;
		vPos4.x += 19.11f;
		vPos4.y += 11.76f;
		vPos4.z -= 0.43f;

		ZeroMemory(&BoxDesc, sizeof(BoxDesc));
		BoxDesc.pActortag = CUtile::Create_DummyString(m_szCloneObjectTag, 3);
		BoxDesc.eType = BOX_STATIC;
		BoxDesc.vPos = CUtile::Float_4to3(vPos4);
		BoxDesc.vSize = _float3(1.f, 7.02f, 1.2f);
		BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
		BoxDesc.fDegree = 0.f;
		BoxDesc.isGravity = false;
		BoxDesc.eFilterType = PX_FILTER_TYPE::FITLER_ENVIROMNT;
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

		pPhysX->Create_Box(BoxDesc, Create_PxUserData(nullptr, false, COL_ENVIROMENT));
		PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_Actor(BoxDesc.pActortag);
		CPhysX_Manager::GetInstance()->Set_ActorRotation(pActor, 68.f, _float3(0.f, 0.f, 1.f));
	}
}

void CDZ_FallenTree_Anim::Push_EventFunctions()
{
	TreeSound(true, 0.f);
}

void CDZ_FallenTree_Anim::TreeSound(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CDZ_FallenTree_Anim::TreeSound);
		return;
	}

	CGameInstance::GetInstance()->Play_Sound(L"Tree.ogg", 0.3f, false);
}

void CDZ_FallenTree_Anim::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();

}

void CDZ_FallenTree_Anim::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();
	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	static _float3 FalltreePos = _float3(0.f, 0.f, 0.f);
	float fPos[3] = { FalltreePos.x, FalltreePos.y, FalltreePos.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	FalltreePos.x = fPos[0]; FalltreePos.y = fPos[1]; FalltreePos.z = fPos[2];

	_float3 Temp;
	XMStoreFloat3(&Temp, vPos + XMVectorSetW(XMLoadFloat3(&FalltreePos), 1.f));
	CPhysX_Manager::GetInstance()->Set_ActorPosition(
		m_szCloneObjectTag, Temp);

	static _float Degree = 0.f;

	ImGui::DragFloat("Rotation ", &Degree, 1.f, -90.f, 90.f);

	if (ImGui::Button("Rotation_GOOO!"))
	{
		static _float Degree2 = Degree;
		PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_Actor(m_szCloneObjectTag);
		//CPhysX_Manager::GetInstance()->Set_ActorRotation(pActor, -Degree2, _float3(0.f, 0.f, 1.f));

		CPhysX_Manager::GetInstance()->Set_ActorRotation(pActor, Degree, _float3(0.f, 0.f, 1.f));
	}
}

HRESULT CDZ_FallenTree_Anim::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	return S_OK;
}

HRESULT CDZ_FallenTree_Anim::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_DeadZoneTree"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom,nullptr,this)))
		return E_FAIL;

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		L"Prototype_Component_Shader_VtxAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CDZ_FallenTree_Anim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CDZ_FallenTree_Anim::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

CDZ_FallenTree_Anim * CDZ_FallenTree_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDZ_FallenTree_Anim*		pInstance = new CDZ_FallenTree_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDZ_FallenTree_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDZ_FallenTree_Anim::Clone(void * pArg)
{
	CDZ_FallenTree_Anim*		pInstance = new CDZ_FallenTree_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDZ_FallenTree_Anim");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDZ_FallenTree_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
