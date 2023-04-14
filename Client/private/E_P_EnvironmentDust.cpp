#include "stdafx.h"
#include "..\public\E_P_EnvironmentDust.h"
#include "GameInstance.h"

CE_P_EnvironmentDust::CE_P_EnvironmentDust(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_EnvironmentDust::CE_P_EnvironmentDust(const CE_P_EnvironmentDust & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_EnvironmentDust::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_EnvironmentDust::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

HRESULT CE_P_EnvironmentDust::Late_Initialize(void* pArg)
{
	return S_OK;
}

void CE_P_EnvironmentDust::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(ImGui::Button("True"))
	{
		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y = 13.330f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	}


	if(	!lstrcmp(m_pParent->Get_ObjectCloneName() , L"1_GimmickPlatForm"))
	{
		if (m_eEFfectDesc.bActive == true)
		{
			m_pTransformCom->Go_AxisY(fTimeDelta * 0.23f);
		}

		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		if(vPos.y >= 1.249f)
		{
			m_fLife += fTimeDelta;
			m_bTimer = true;
			m_bDissolve = true;
			_bool bResult = TurnOffSystem(m_fLife, 2.f, fTimeDelta);
			if (bResult) {
				Reset();
				m_pTransformCom->Set_PositionY(0.0f);
			}
		}
		

	}
	else if (!lstrcmp(m_pParent->Get_ObjectCloneName(), L"4_Gimmick_Wall_B"))
	{
		if (m_eEFfectDesc.bActive == true)
		{
			m_pTransformCom->Go_AxisY(fTimeDelta * 0.6f);
		}

		_float4	vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		if (vPos.y >= 17.5f)
		{
			m_fLife += fTimeDelta;
			m_bTimer = true;
			m_bDissolve = true;
			_bool bResult = TurnOffSystem(m_fLife, 1.f, fTimeDelta);
			if (bResult) Reset();
		}
	}
}

void CE_P_EnvironmentDust::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
// 		CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
// 		ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_BOX;
// 		ePointDesc->eRotXYZ = CVIBuffer_Point_Instancing::tagPointDesc::DIR_Y;
// 		ePointDesc->fMin = _float3(-10.f, 0.f, -10.f);
// 		ePointDesc->fMax = _float3(10.f, 8.f, 10.f);
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(3.f, 5.f));
		m_bTurnOnfirst = true;
	}

	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_EnvironmentDust::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_EnvironmentDust::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_EnvironmentDust::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);

	return S_OK;
}

void CE_P_EnvironmentDust::Reset()
{
	m_fLife = 0.0f;
	m_eEFfectDesc.bActive = false;
	m_bDissolve = false;
	m_bTimer = false;
}

CE_P_EnvironmentDust* CE_P_EnvironmentDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath)
{
	CE_P_EnvironmentDust * pInstance = new CE_P_EnvironmentDust(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_EnvironmentDust Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_EnvironmentDust::Clone(void * pArg)
{
	CE_P_EnvironmentDust * pInstance = new CE_P_EnvironmentDust(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_EnvironmentDust Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_EnvironmentDust::Free()
{
	__super::Free();
}
