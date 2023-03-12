#include "stdafx.h"
#include "..\public\E_PulseObject.h"
#include "GameInstance.h"
#include "Effect_Trail.h"

CE_PulseObject::CE_PulseObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_PulseObject::CE_PulseObject(const CE_PulseObject & rhs)
	:CEffect_Mesh(rhs)
{
	
}

void CE_PulseObject::Set_InitMatrixScaled(_float3 vScale)
{
	_float4 vRight, vUp, vLook;

	XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[0]) * vScale.x);
	XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])  * vScale.y);
	XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])  *vScale.z);

	memcpy(&m_InitWorldMatrix.m[0][0], &vRight, sizeof vRight);
	memcpy(&m_InitWorldMatrix.m[1][0], &vUp, sizeof vUp);
	memcpy(&m_InitWorldMatrix.m[2][0], &vLook, sizeof vLook);
}

_float3 CE_PulseObject::Get_InitMatrixScaled()
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[0])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])));
}

HRESULT CE_PulseObject::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_PulseObject::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	m_eEFfectDesc.bActive = false;
	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));



	m_pTransformCom->Set_Scaled(_float3(3.f, 3.f, 3.f));

	return S_OK;
}

HRESULT CE_PulseObject::Late_Initialize(void * pArg)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_ePulseDesc.vResetPos);

	_float3 vScale = m_ePulseDesc.vResetSize;
	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_SPHERE_DESC PxSphereDesc;
	PxSphereDesc.eType = SPHERE_DYNAMIC;
	PxSphereDesc.pActortag = m_szCloneObjectTag;
	PxSphereDesc.vPos = vPos;
	PxSphereDesc.fRadius = vScale.x;
	PxSphereDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	PxSphereDesc.fDensity = 1.f;
	PxSphereDesc.fAngularDamping = 0.5f;
	PxSphereDesc.eFilterType = PX_FILTER_TYPE::PULSE;
	PxSphereDesc.fDynamicFriction = 0.5f;
	PxSphereDesc.fStaticFriction = 0.5f;
	PxSphereDesc.fRestitution = 0.1f;

	CPhysX_Manager::GetInstance()->Create_Sphere(PxSphereDesc, Create_PxUserData(this, false, COL_PULSE));
	CPhysX_Manager::GetInstance()->Set_ActorFlag_Simulation(m_szCloneObjectTag, false);
	m_pTransformCom->Add_Collider(m_szCloneObjectTag, m_pTransformCom->Get_WorldMatrixFloat4x4());
	m_pRendererCom->Set_PhysXRender(true);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_ePulseDesc.vResetPos));


	return S_OK;
}

void CE_PulseObject::Tick(_float fTimeDelta)
{
	// m_eEffectDesc.bActive == 현재 동작하고 있는거 판단함 
	if (m_eEFfectDesc.bActive == false)
		return;

	Type_Tick(fTimeDelta);

	__super::Tick(fTimeDelta);

}



void CE_PulseObject::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pParent != nullptr && dynamic_cast<CEffect_Trail*>(this) == false)
		Set_Matrix();
}

HRESULT CE_PulseObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

HRESULT CE_PulseObject::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

void CE_PulseObject::Type_Tick(_float TimeDelta)
{
	_float3 vScale = m_pTransformCom->Get_Scaled();

	//if (m_ePulseDesc.eObjType == PULSE_OBJ_DELIVER)
	//{
	//	m_ePulseDesc.fIncreseRatio = 1.02f;
	//	m_ePulseDesc.fPulseMaxSize = 10.f;
	//	m_ePulseDesc.vResetSize = _float3(2.f, 2.f, 2.f);
	//}
	//else if (m_ePulseDesc.eObjType == PULSE_OBJ_RECIVE)
	//{
	//	m_ePulseDesc.fIncreseRatio = 1.05f;
	//	m_ePulseDesc.fPulseMaxSize = 5.f;
	//	m_ePulseDesc.vResetSize = _float3(0.5f, 0.5f, 0.5f);
	//}
	vScale *= m_ePulseDesc.fIncreseRatio;

	if (vScale.x >= m_ePulseDesc.fPulseMaxSize)
	{
		m_eEFfectDesc.bActive = false;
		m_pTransformCom->Set_Scaled(m_ePulseDesc.vResetSize);	// 기믹에 실패했을때  보여주는용이고
		m_pTransformCom->Set_PxPivotScale(vScale);				// 피직스용이고 
		CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vScale);

		//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_ePulseDesc.vResetPos));

	}
	else
	{
		m_pTransformCom->Set_Scaled(vScale);
		m_pTransformCom->Set_PxPivotScale(vScale);
	}


}

void CE_PulseObject::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();

	_float3 vPxPivotScale = m_pTransformCom->Get_vPxPivotScale();

	float fScale[3] = { vPxPivotScale.x, vPxPivotScale.y, vPxPivotScale.z };
	ImGui::DragFloat3("PxScale", fScale, 0.01f, 0.1f, 100.0f);
	vPxPivotScale.x = fScale[0]; vPxPivotScale.y = fScale[1]; vPxPivotScale.z = fScale[2];
	CPhysX_Manager::GetInstance()->Set_ActorScaling(m_szCloneObjectTag, vPxPivotScale);
	m_pTransformCom->Set_PxPivotScale(vPxPivotScale);

	_float3 vPxPivot = m_pTransformCom->Get_vPxPivot();

	float fPos[3] = { vPxPivot.x, vPxPivot.y, vPxPivot.z };
	ImGui::DragFloat3("PxPivotPos", fPos, 0.01f, -100.f, 100.0f);
	vPxPivot.x = fPos[0]; vPxPivot.y = fPos[1]; vPxPivot.z = fPos[2];
	m_pTransformCom->Set_PxPivot(vPxPivot);
}



CE_PulseObject * CE_PulseObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_PulseObject * pInstance = new CE_PulseObject(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_PulseObject Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_PulseObject::Clone(void * pArg)
{
	CE_PulseObject * pInstance = new CE_PulseObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_PulseObject Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_PulseObject::Free()
{
	__super::Free();
}
