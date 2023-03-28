#include "stdafx.h"
#include "..\public\E_P_Enrage.h"
#include "GameInstance.h"

CE_P_Enrage::CE_P_Enrage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Enrage::CE_P_Enrage(const CE_P_Enrage & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Enrage::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Enrage::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;

	m_pVIInstancingBufferCom->Set_PSize(_float2(0.5f, 0.5f));
	// m_pVIInstancingBufferCom->Set_RandomSpeeds(1.0f, 5.0f);
	return S_OK;
}

void CE_P_Enrage::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
	{
		m_fLife = 0.0f;
		return;
	}

	__super::Tick(fTimeDelta);
	m_fLife += fTimeDelta;
}

void CE_P_Enrage::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Enrage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_P_Enrage::Imgui_RenderProperty()
{
	if (ImGui::Button("Active"))
	{
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;
		m_pVIInstancingBufferCom->Set_GravityTimeZero();
	}

	if (ImGui::Button("Set_Shape"))
		m_pVIInstancingBufferCom->Set_ShapePosition();

	if (ImGui::Button("Set_Speed"))
		m_pVIInstancingBufferCom->Set_RandomSpeeds(1.0f, 10.0f);

	CVIBuffer_Point_Instancing::POINTDESC* ePoint = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstance = m_pVIInstancingBufferCom->Get_InstanceData();

	ImGui::Checkbox("UseGravity", &ePoint->bUseGravity);
	ImGui::InputFloat("fRange", &ePoint->fRange);
	ImGui::InputFloat("fCreateRange", &ePoint->fCreateRange);
}

void CE_P_Enrage::Reset()
{
	// m_pVIInstancingBufferCom->Set_ShapePosition();
	m_pVIInstancingBufferCom->Set_GravityTimeZero();
	m_pVIInstancingBufferCom->Set_ResetOriginPos();
}

CE_P_Enrage * CE_P_Enrage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Enrage * pInstance = new CE_P_Enrage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Enrage Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Enrage::Clone(void * pArg)
{
	CE_P_Enrage * pInstance = new CE_P_Enrage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Enrage Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Enrage::Free()
{
	__super::Free();
}
