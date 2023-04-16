#include "stdafx.h"
#include "..\public\E_PulseObject.h"
#include "GameInstance.h"
#include "Effect_Trail.h"
#include "BowTarget.h"

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

	m_fHDRValue = 3.f;
	m_pTransformCom->Set_Scaled(_float3(3.f, 3.f, 3.f));
	m_eEFfectDesc.vColor = XMVectorSet(0.0f, 125.f, 255.f, 16.f) / 255.f;
	return S_OK;
}

HRESULT CE_PulseObject::Late_Initialize(void * pArg)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_ePulseDesc.vResetPos);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_ePulseDesc.vResetPos));

	if (m_ePulseDesc.eObjType == PULSE_OBJ_RECIVE)
		return S_OK;

	m_pTriggerData = Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_PULSE, vPos, 1.f);
	CPhysX_Manager::GetInstance()->Create_Trigger(m_pTriggerData);

	return S_OK;
}

void CE_PulseObject::Tick(_float fTimeDelta)
{
	ToolOption("CE_PulseObject");

	if (m_bFinish == true )//&& m_ePulseDesc.eObjType == PULSE_OBJ_DELIVER)
	{
		m_fPulseResetTimer += fTimeDelta;
		m_eEFfectDesc.bActive = false;
	}

	if (m_fPulseResetTimer >= 6.f)
	{
		m_bFinish = false;
		m_fPulseResetTimer = 0.f;
	}
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

	if (m_pParent != nullptr)
		Set_Matrix();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND2, this);
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

	_bool bRecive = false;

	if (m_ePulseDesc.eObjType == PULSE_OBJ_RECIVE) {
		bRecive = true;
	}
	else
		bRecive = false;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bPulseRecive", &bRecive, sizeof(bRecive)), E_FAIL);
	return S_OK;
}

void CE_PulseObject::Type_Tick(_float TimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	_float3 vScale = m_pTransformCom->Get_Scaled();

	vScale *= m_ePulseDesc.fIncreseRatio;

	
	if (vScale.x >= m_ePulseDesc.fPulseMaxSize)
	{
		m_eEFfectDesc.bActive = false;
		m_pTransformCom->Set_Scaled(m_ePulseDesc.vResetSize );
		
		if(m_ePulseDesc.eObjType == PULSE_OBJ_DELIVER)
		{
			CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerData->pTriggerStatic, m_ePulseDesc.vResetSize.x);
		}
		m_bFinish = true;
	}
	else
	{
		if (m_ePulseDesc.eObjType == PULSE_OBJ_DELIVER)
		{
			CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerData->pTriggerStatic, vScale.x *3.5f);
		}

		m_pTransformCom->Set_Scaled(vScale);
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

_int CE_PulseObject::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	if (pTarget != nullptr)
	{
		if (iColliderIndex == (_int)COL_BOWTARGET)
			dynamic_cast<CBowTarget*>(pTarget)->TurnOn_Launch();
	}

	return 0;
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
