#include "stdafx.h"
#include "..\public\E_Swipes_Charged.h"
#include "GameInstance.h"
#include "Monster.h"

CE_Swipes_Charged::CE_Swipes_Charged(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Swipes_Charged::CE_Swipes_Charged(const CE_Swipes_Charged & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_Swipes_Charged::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(__super::Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Swipes_Charged::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_Swipes_Charged::Late_Initialize(void * pArg)
{	
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_pTriggerData = Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_PULSE, CUtile::Float_4to3(vPos), 1.f);
	CPhysX_Manager::GetInstance()->Create_Trigger(m_pTriggerData);

	_float3 vOriginPos = _float3(0.f, 0.f, 0.f);
	_float3 vPivotScale = _float3(1.0f, 0.0f, 1.f);
	_float3 vPivotPos = _float3(0.f, 0.f, 0.f);

	// Capsule X == radius , Y == halfHeight
	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vOriginPos;
	PxSphereDesc.fRadius = vPivotScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.fMass = 59.f;
	PxSphereDesc.fLinearDamping = 1.f;
	PxSphereDesc.bCCD = true;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PLAYER_BODY;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PLAYER));

	_smatrix	matPivot = XMMatrixTranslation(vPivotPos.x, vPivotPos.y, vPivotPos.z);
	m_pTransformCom->Add_Collider(PxSphereDesc.pActortag, matPivot);

	return S_OK;
}

void CE_Swipes_Charged::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTimeDelta += fTimeDelta;
	m_pTransformCom->Tick(fTimeDelta);
}

void CE_Swipes_Charged::Late_Tick(_float fTimeDelta)
{
  	//if (m_eEFfectDesc.bActive == false)
  	//	return;

	if (m_pParent != nullptr)
		Set_Matrix();
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Swipes_Charged::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

_int CE_Swipes_Charged::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	_bool bRealAttack = false;
	if (iColliderIndex == (_uint)COL_MONSTER_WEAPON && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()))
	{

	}
	return 0;
}

void CE_Swipes_Charged::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_Swipes_Charged::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Swipes_Charged::SetUp_Components()
{	
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_SPHERE;

	return S_OK;
}

void CE_Swipes_Charged::Imgui_RenderProperty()
{
}

CE_Swipes_Charged * CE_Swipes_Charged::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Swipes_Charged * pInstance = new CE_Swipes_Charged(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Swipes_Charged Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Swipes_Charged::Clone(void * pArg)
{
	CE_Swipes_Charged * pInstance = new CE_Swipes_Charged(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Swipes_Charged Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Swipes_Charged::Free()
{
	__super::Free();
}
