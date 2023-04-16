#include "stdafx.h"
#include "..\public\E_P_Level.h"
#include "GameInstance.h"

CE_P_Level::CE_P_Level(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Level::CE_P_Level(const CE_P_Level & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Level::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Level::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 6.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_fHDRValue = 2.5f;
	return S_OK;
}

HRESULT CE_P_Level::Late_Initialize(void* pArg)
{
	m_ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	Reset();

	return S_OK;
}

void CE_P_Level::Tick(_float fTimeDelta)
{
	if (m_bReset == false)
		Reset();

	if (m_eEFfectDesc.bActive == true)
		Tick_Rotation(fTimeDelta);

#ifdef _DEBUG
	//ImGui::Begin("CE_P_Level");
	//if (ImGui::Button("active"))
	//{
	//	m_eEFfectDesc.bActive = true;
	//}
	//if (ImGui::Button("recom"))
	//{
	//	m_pShaderCom->ReCompile();
	//}

	//if (ImGui::Button("re"))
	//{	
	//	m_ePointDesc->bRotation = true;

	//	m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 2.f);
	//	m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.2f));
	//}

	//ImGui::End();
#endif // _DEBUG

	__super::Tick(fTimeDelta);

	if (m_bTimer)
	{
		m_fLife += fTimeDelta;
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.f, 0.2f - m_fLife));

		if (m_fLife > 1.f)
			Reset();
	}
}

void CE_P_Level::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 2.f);
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.2f));

		m_bTurnOnfirst = true;
	}

 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Level::Render()
{
	FAILED_CHECK_RETURN(__super::Render(),E_FAIL);

	return S_OK;
}

void CE_P_Level::Tick_Rotation(_float fTimeDelta)
{
	_float4 vRandom = _float4(CUtile::Get_RandomFloat(0.1f, 1.0f), CUtile::Get_RandomFloat(0.1f, 1.0f), CUtile::Get_RandomFloat(0.1f, 1.0f), 0.0f);
	m_pTransformCom->Turn(XMVectorSet(vRandom.x, vRandom.y, vRandom.z, 0.0f), fTimeDelta);

	m_fDurationTime += fTimeDelta;
	if (m_fDurationTime > 1.5f)
	{
		m_ePointDesc->bRotation = false;

		if (m_fDurationTime > 2.5f)
			m_bTimer = true;
			// Reset();
	}
}

void CE_P_Level::Reset()
{
	m_bReset = true;
	m_ePointDesc->bRotation = true;
	m_ePointDesc->fRange = 3.f;

	m_pVIInstancingBufferCom->Set_RandomSpeeds(1.f, 2.f);
	m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.2f));
	m_eEFfectDesc.bActive = false;
	m_bTimer = false;
	m_fDurationTime = 0.0f;
	m_fLife = 0.0f;
}

CE_P_Level * CE_P_Level::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Level * pInstance = new CE_P_Level(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Level Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Level::Clone(void * pArg)
{
	CE_P_Level * pInstance = new CE_P_Level(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Level Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Level::Free()
{
	__super::Free();
}
