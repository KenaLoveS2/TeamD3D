#include "stdafx.h"
#include "..\public\E_Warrior_PlaneRoot.h"
#include "GameInstance.h"

CE_Warrior_PlaneRoot::CE_Warrior_PlaneRoot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Warrior_PlaneRoot::CE_Warrior_PlaneRoot(const CE_Warrior_PlaneRoot & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_Warrior_PlaneRoot::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_Warrior_PlaneRoot::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_TextureComponents(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_Warrior_PlaneRoot::Late_Initialize(void * pArg)
{	
	//_float4 vPos;
	//XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	//m_pTriggerDAta = Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_PULSE, CUtile::Float_4to3(vPos), 1.f);
	//CPhysX_Manager::GetInstance()->Create_Trigger(m_pTriggerDAta);

	//_float3 vOriginPos = _float3(0.f, 0.f, 0.f);
	//_float3 vPivotScale = _float3(1.0f, 0.0f, 1.f);
	//_float3 vPivotPos = _float3(0.f, 0.f, 0.f);

	//// Capsule X == radius , Y == halfHeight
	//CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	//PxSphereDesc.eType = SPHERE_DYNAMIC;
	//PxSphereDesc.pActortag = m_szCloneObjectTag;
	//PxSphereDesc.vPos = vOriginPos;
	//PxSphereDesc.fRadius = vPivotScale.x;
	//PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	//PxSphereDesc.fDensity = 1.f;
	//PxSphereDesc.fAngularDamping = 0.5f;
	//PxSphereDesc.fMass = 59.f;
	//PxSphereDesc.fLinearDamping = 1.f;
	//PxSphereDesc.bCCD = true;
	//PxSphereDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	//PxSphereDesc.fDynamicFriction = 0.5f;
	//PxSphereDesc.fStaticFriction = 0.5f;
	//PxSphereDesc.fRestitution = 0.1f;

	//CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER));

	//_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	//m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	return S_OK;
}

void CE_Warrior_PlaneRoot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_eEFfectDesc.bActive == false)
		return;

	m_fTimeDelta += fTimeDelta;

	m_fDurationTime += fTimeDelta;
	if (m_fDurationTime > 1.f)
	{
		m_eEFfectDesc.bActive = false;
		m_fTimeDelta = 0.0f;
		m_fDurationTime = 0.0f;
	}

	// m_pTransformCom->Tick(fTimeDelta);
}

void CE_Warrior_PlaneRoot::Late_Tick(_float fTimeDelta)
{
 //  	if (m_eEFfectDesc.bActive == false)
	//{
	//	m_fTimeDelta = 0.0f;
	//	return;
	//}

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Warrior_PlaneRoot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_Warrior_PlaneRoot::Reset()
{
}

_int CE_Warrior_PlaneRoot::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_Warrior_PlaneRoot::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_Warrior_PlaneRoot::SetUp_TextureComponents()
{
	Edit_TextureComponent(2, 0);

	m_eEFfectDesc.fFrame[0] = 14.f; 
	m_eEFfectDesc.fFrame[1] = 15.f;

	m_eEFfectDesc.iPassCnt = 17;
	return S_OK;
}

HRESULT CE_Warrior_PlaneRoot::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Warrior_PlaneRoot::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, TEXT("Prototype_Component_Model_Plane_Root"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

void CE_Warrior_PlaneRoot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

CE_Warrior_PlaneRoot * CE_Warrior_PlaneRoot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Warrior_PlaneRoot * pInstance = new CE_Warrior_PlaneRoot(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Warrior_PlaneRoot Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Warrior_PlaneRoot::Clone(void * pArg)
{
	CE_Warrior_PlaneRoot * pInstance = new CE_Warrior_PlaneRoot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Warrior_PlaneRoot Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Warrior_PlaneRoot::Free()
{
	__super::Free();
}
