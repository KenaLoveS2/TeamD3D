#include "stdafx.h"
#include "SceneChangePortal.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Kena.h"
#include "Camera_Player.h"
#include "ControlRoom.h"

CSceneChangePortal::CSceneChangePortal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CSceneChangePortal::CSceneChangePortal(const CSceneChangePortal& rhs)
	: CEnviromentObj(rhs)
{
}

CSceneChangePortal* CSceneChangePortal::Get_LinkedPortal()
{
	if (m_bRendaerPortal_Gimmick == false)
		return nullptr;

	return m_pLinkedPortal;
}

HRESULT CSceneChangePortal::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CSceneChangePortal::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_bRendaerPortal_Gimmick = true;
	return S_OK;
}

HRESULT CSceneChangePortal::Late_Initialize(void* pArg)
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

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_SCENECHANGE_PORTAL));

	return S_OK;
}

void CSceneChangePortal::Tick(_float fTimeDelta)
{

	CGameObject::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CSceneChangePortal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/*NonCulling*/
	if (m_pRendererCom && m_bRenderActive && m_bRendaerPortal_Gimmick)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSceneChangePortal::Render()
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

void CSceneChangePortal::Imgui_RenderProperty()
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

void CSceneChangePortal::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CSceneChangePortal::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}


HRESULT CSceneChangePortal::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_WaterPlane", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_Water", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_Material(i, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Textures/Test.png"));
	}

	return S_OK;
}

HRESULT CSceneChangePortal::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fTime", &m_fTimeDelta, sizeof(_float)), E_FAIL);
	return S_OK;
}

CSceneChangePortal* CSceneChangePortal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSceneChangePortal* pInstance = new CSceneChangePortal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSceneChangePortal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSceneChangePortal::Clone(void* pArg)
{
	CSceneChangePortal* pInstance = new CSceneChangePortal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSceneChangePortal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSceneChangePortal::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
