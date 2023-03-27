#include "stdafx.h"
#include "..\public\E_Warrior_ShockFronExtended_Plane.h"
#include "GameInstance.h"

CE_Warrior_ShockFronExtended_Plane::CE_Warrior_ShockFronExtended_Plane(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Warrior_ShockFronExtended_Plane::CE_Warrior_ShockFronExtended_Plane(const CE_Warrior_ShockFronExtended_Plane & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_Warrior_ShockFronExtended_Plane::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_Warrior_ShockFronExtended_Plane::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_Scaled(_float3(1.8f, 1.8f, 1.8f));
	m_eEFfectDesc.vColor = XMVectorSet(66.f, 23.f, 0.f, 0.f) / 255.f;
	return S_OK;
}

HRESULT CE_Warrior_ShockFronExtended_Plane::Late_Initialize(void * pArg)
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

void CE_Warrior_ShockFronExtended_Plane::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fTimeDelta = 0.0f;
		return;
	}

	__super::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;

	// m_pTransformCom->Tick(fTimeDelta);
}

void CE_Warrior_ShockFronExtended_Plane::Late_Tick(_float fTimeDelta)
{
   	if (m_eEFfectDesc.bActive == false)
   		return;

	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		vPos.y = vPos.y - 3.f;
		m_pTransformCom->Set_Position(vPos);
	}

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CE_Warrior_ShockFronExtended_Plane::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_Warrior_ShockFronExtended_Plane::Reset()
{
}

_int CE_Warrior_ShockFronExtended_Plane::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_Warrior_ShockFronExtended_Plane::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_Warrior_ShockFronExtended_Plane::SetUp_TextureComponent()
{

	return S_OK;
}

HRESULT CE_Warrior_ShockFronExtended_Plane::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Warrior_ShockFronExtended_Plane::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);

	Set_ModelCom(m_eEFfectDesc.eMeshType);

	return S_OK;
}

void CE_Warrior_ShockFronExtended_Plane::Imgui_RenderProperty()
{
	ImGui::InputFloat4("DiffuseTexture", (_float*)&m_eEFfectDesc.fFrame);
	ImGui::InputFloat("Pass", (_float*)&m_eEFfectDesc.iPassCnt);

	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
}

CE_Warrior_ShockFronExtended_Plane * CE_Warrior_ShockFronExtended_Plane::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Warrior_ShockFronExtended_Plane * pInstance = new CE_Warrior_ShockFronExtended_Plane(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Warrior_ShockFronExtended_Plane Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Warrior_ShockFronExtended_Plane::Clone(void * pArg)
{
	CE_Warrior_ShockFronExtended_Plane * pInstance = new CE_Warrior_ShockFronExtended_Plane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Warrior_ShockFronExtended_Plane Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Warrior_ShockFronExtended_Plane::Free()
{
	__super::Free();
}
