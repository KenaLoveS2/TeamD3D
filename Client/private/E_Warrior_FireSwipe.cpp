#include "stdafx.h"
#include "..\public\E_Warrior_FireSwipe.h"
#include "GameInstance.h"

CE_Warrior_FireSwipe::CE_Warrior_FireSwipe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_Warrior_FireSwipe::CE_Warrior_FireSwipe(const CE_Warrior_FireSwipe & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_Warrior_FireSwipe::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CE_Warrior_FireSwipe::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 15.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_SwipeTexture(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_Warrior_FireSwipe::Late_Initialize(void * pArg)
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

void CE_Warrior_FireSwipe::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pTransformCom->Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
   		return;

	m_fTimeDelta += fTimeDelta;

 	m_fDurationTime += fTimeDelta;
 	if (m_fDurationTime > 1.f)
 		m_fDurationTime = 0.0f;
 	else
 		m_pTransformCom->Go_Backward(fTimeDelta);
}

void CE_Warrior_FireSwipe::Late_Tick(_float fTimeDelta)
{
   	if (m_eEFfectDesc.bActive == false)
   		return;

	//if (m_pParent != nullptr)
	//	Set_Matrix();
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Warrior_FireSwipe::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0) // 외부
		{
			m_pModelCom->Render(m_pShaderCom, 0, nullptr, 14);
		}
		if (i == 1) // 내부
		{
			m_pModelCom->Render(m_pShaderCom, 0, nullptr, 15);
		}
	}

	return S_OK;
}

void CE_Warrior_FireSwipe::Reset()
{
}

_int CE_Warrior_FireSwipe::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_Warrior_FireSwipe::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

HRESULT CE_Warrior_FireSwipe::SetUp_SwipeTexture()
{
	Edit_TextureComponent(5, 0);

	m_eEFfectDesc.fFrame[0] = 27.f;
	m_eEFfectDesc.fFrame[1] = 54.f;
	m_eEFfectDesc.fFrame[2] = 4.f;
	m_eEFfectDesc.fFrame[3] = 57.f;
	m_eEFfectDesc.fFrame[4] = 16.f;

	return S_OK;
}

HRESULT CE_Warrior_FireSwipe::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Warrior_FireSwipe::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Model_FireSwipe"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

void CE_Warrior_FireSwipe::Imgui_RenderProperty()
{
}

CE_Warrior_FireSwipe * CE_Warrior_FireSwipe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Warrior_FireSwipe * pInstance = new CE_Warrior_FireSwipe(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Warrior_FireSwipe Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Warrior_FireSwipe::Clone(void * pArg)
{
	CE_Warrior_FireSwipe * pInstance = new CE_Warrior_FireSwipe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Warrior_FireSwipe Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Warrior_FireSwipe::Free()
{
	__super::Free();
}
