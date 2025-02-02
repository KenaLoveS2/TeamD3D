#include "stdafx.h"
#include "CPortalPlane.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Camera_Player.h"
#include "ControlRoom.h"

CPortalPlane::CPortalPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CPortalPlane::CPortalPlane(const CPortalPlane& rhs)
	: CEnviromentObj(rhs)
{
}

CPortalPlane* CPortalPlane::Get_LinkedPortal()
{
	if (m_bRendaerPortal_Gimmick == false)
		return nullptr;

	return m_pLinkedPortal;
}

HRESULT CPortalPlane::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CPortalPlane::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);
	
	m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_bRendaerPortal_Gimmick = true;
	return S_OK;
}

HRESULT CPortalPlane::Late_Initialize(void* pArg)
{
	/* 테스트를 위한 주석  나중에 주석푸세요 */
	/*m_pKena = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	NULL_CHECK_RETURN(m_pKena, E_FAIL);

	m_pCamera = dynamic_cast<CCamera_Player*>(CGameInstance::GetInstance()->Get_WorkCameraPtr());
	NULL_CHECK_RETURN(m_pCamera, E_FAIL);*/

	if (FAILED(SetUp_DiffuseTexture()))
		return E_FAIL;


	if (!lstrcmp(m_szCloneObjectTag, L"3_Portal0"))
	{
		m_pLinkedPortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"3_Portal1"));
		NULL_CHECK_RETURN(m_pLinkedPortal, E_FAIL);
	}
		
	else if (!lstrcmp(m_szCloneObjectTag, L"3_Portal1"))
	{
		m_pLinkedPortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"3_Portal0"));
		NULL_CHECK_RETURN(m_pLinkedPortal, E_FAIL);
	}

	else if (!lstrcmp(m_szCloneObjectTag, L"3_BossDeadPortal_0"))
	{
		m_pLinkedPortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"3_BossDeadPortal_1"));
		NULL_CHECK_RETURN(m_pLinkedPortal, E_FAIL);
		m_bRendaerPortal_Gimmick = false;
		return S_OK;
	}

	else if (!lstrcmp(m_szCloneObjectTag, L"3_BossDeadPortal_1"))
	{
		m_pLinkedPortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Enviroment", L"3_BossDeadPortal_0"));
		NULL_CHECK_RETURN(m_pLinkedPortal, E_FAIL);
		m_bRendaerPortal_Gimmick = false;
		return S_OK;
	}

	Late_init_For_GimmickLevel();

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(6.f, 6.f, 0.5f);

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = vSize;
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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_PORTAL));


	

	return S_OK;
}

void CPortalPlane::Tick(_float fTimeDelta)
{
#ifdef FOR_MAP_GIMMICK
	
#else
	if (!m_bTestOnce)
	{
		SetUp_DiffuseTexture();
		m_bTestOnce = true;
}
#endif

	CGameObject::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CPortalPlane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/*NonCulling*/
	if (m_pRendererCom && m_bRenderActive && m_bRendaerPortal_Gimmick)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPortalPlane::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 1);
	}
	return S_OK;
}

void CPortalPlane::Imgui_RenderProperty()
{
	m_pTransformCom->Imgui_RenderProperty_ForJH();

	PxRigidActor* pActor = CPhysX_Manager::GetInstance()->Find_StaticActor(m_szCloneObjectTag);

	if (pActor == nullptr)
		return;

	PxShape* pShape = nullptr;
	pActor->getShapes(&pShape, sizeof(PxShape));
	PxBoxGeometry& Geometry = pShape->getGeometry().box();
	PxVec3& fScale = Geometry.halfExtents;

	/* Scale */
	ImGui::BulletText("Scale Setting");
	ImGui::DragFloat("Scale X", &fScale.x, 0.01f);
	ImGui::DragFloat("Scale Y", &fScale.y, 0.01f);
	ImGui::DragFloat("Scale Z", &fScale.z, 0.01f);

	pShape->setGeometry(Geometry);
}

void CPortalPlane::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CPortalPlane::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

HRESULT CPortalPlane::Late_init_For_GimmickLevel()
{

	if (!lstrcmp(m_szCloneObjectTag, L"MG_CrystalGimmick_Portal"))
	{
		m_pLinkedPortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
			L"Layer_Enviroment", L"MG_TeleFlower_Gimmick_StartPortal"));

		/* TO.Do*/
		m_bRendaerPortal_Gimmick = false;

		NULL_CHECK_RETURN(m_pLinkedPortal, E_FAIL);

	}
	else if (!lstrcmp(m_szCloneObjectTag, L"MG_TeleFlower_Gimmick_Goal_Portal"))
	{
		m_pLinkedPortal = dynamic_cast<CPortalPlane*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL,
			L"Layer_Enviroment", L"MG_Final_Portal"));

		NULL_CHECK_RETURN(m_pLinkedPortal, E_FAIL);
	}
	else
		return S_OK;


	return S_OK;
}

void CPortalPlane::HunterBoss_DeadPortalColliderOn()
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(6.f, 6.f, 0.5f);

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = vSize;
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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_PORTAL));
}

HRESULT CPortalPlane::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_WaterPlane", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_Water", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	return S_OK;
}

HRESULT CPortalPlane::SetUp_DiffuseTexture()
{

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (!lstrcmp(m_szCloneObjectTag, L"3_Portal0"))
		{
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Test.png"));
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"3_Portal1"))
		{
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/3_Portal1.png"));
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"3_BossDeadPortal_0"))
		{
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/3_BossDeadPortal_0.png"));
		
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"3_BossDeadPortal_1"))
		{
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/3_BossDeadPortal_1.png"));
		
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"MG_CrystalGimmick_Portal"))
		{
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Gimmick_MapCrystal.png"));
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"MG_TeleFlower_Gimmick_StartPortal"))
		{
			m_bRendaerPortal_Gimmick = false;
			//m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Test.png"));
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"MG_TeleFlower_Gimmick_Goal_Portal"))
		{
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/GimmickMap_Final.png"));
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"MG_Final_Portal"))
		{
			m_bRendaerPortal_Gimmick = false;
			//m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Test.png"));
		}
		else if (!lstrcmp(m_szCloneObjectTag, L"MG_StartPortal"))
		{
			m_bRendaerPortal_Gimmick = false;
			
		}
		else
			m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Test.png"));

	}



	return S_OK;
}

HRESULT CPortalPlane::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fTime", &m_fTimeDelta, sizeof(_float)), E_FAIL);
	return S_OK;
}

CPortalPlane* CPortalPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPortalPlane* pInstance = new CPortalPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPortalPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPortalPlane::Clone(void* pArg)
{
	CPortalPlane* pInstance = new CPortalPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPortalPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPortalPlane::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
