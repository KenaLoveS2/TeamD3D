#include "stdafx.h"
#include "..\public\E_P_Warrior.h"
#include "GameInstance.h"

CE_P_Warrior::CE_P_Warrior(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Warrior::CE_P_Warrior(const CE_P_Warrior & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Warrior::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Warrior::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_pVIInstancingBufferCom->Set_Gravity(true);
	m_pVIInstancingBufferCom->Set_RandomSpeeds(1.0f, 10.0f);
	return S_OK;
}

void CE_P_Warrior::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);
}

void CE_P_Warrior::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Warrior::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_P_Warrior::Imgui_RenderProperty()
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

void CE_P_Warrior::Reset()
{
	m_pVIInstancingBufferCom->Set_ShapePosition();
}

CE_P_Warrior * CE_P_Warrior::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Warrior * pInstance = new CE_P_Warrior(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Warrior Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Warrior::Clone(void * pArg)
{
	CE_P_Warrior * pInstance = new CE_P_Warrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Warrior Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Warrior::Free()
{
	__super::Free();
}
