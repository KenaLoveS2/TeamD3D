#include "stdafx.h"
#include "..\public\E_P_SpiritArrow.h"
#include "GameInstance.h"

CE_P_SpiritArrow::CE_P_SpiritArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_SpiritArrow::CE_P_SpiritArrow(const CE_P_SpiritArrow & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_SpiritArrow::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_SpiritArrow::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.7f, 1.0f);
	m_eEFfectDesc.bActive = false;

	return S_OK;
}

void CE_P_SpiritArrow::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == true)
		__super::Tick(fTimeDelta);
}

void CE_P_SpiritArrow::Late_Tick(_float fTimeDelta)
{
	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_SpiritArrow::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

void CE_P_SpiritArrow::Imgui_RenderProperty()
{
	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData = m_pVIInstancingBufferCom->Get_InstanceData();

	ImGui::InputDouble("Speed", &eInstanceData->pSpeeds);

	ImGui::InputFloat("fLife", &m_fLife);
	ImGui::InputFloat("CreateRange", &ePointDesc->fCreateRange);
	ImGui::InputFloat("fTimeDelta", &ePointDesc->fTimeDelta);
	ImGui::InputFloat4("OriginPos", (_float*)&ePointDesc->vOriginPos);
	ImGui::InputFloat4("MyPos", (_float*)&m_pTransformCom->Get_Position());

	ImGui::InputFloat4("Diffuse", (_float*)&m_eEFfectDesc.fFrame);
	ImGui::InputFloat4("vColor", (_float*)&m_eEFfectDesc.vColor);

	_int iType = (_int)m_eEFfectDesc.eTextureRenderType;
	ImGui::InputInt("iPass", &m_eEFfectDesc.iPassCnt);
	ImGui::InputInt("Type", &iType);

	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);
	ImGui::Checkbox("Spread", &ePointDesc->bSpread);

	if (ImGui::Button("RefLife"))
		m_fLife = 0.0f;

	if (ImGui::Button("Re Shape"))
		Set_ShapePosition();
}

HRESULT CE_P_SpiritArrow::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

CE_P_SpiritArrow * CE_P_SpiritArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_SpiritArrow * pInstance = new CE_P_SpiritArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_SpiritArrow Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_SpiritArrow::Clone(void * pArg)
{
	CE_P_SpiritArrow * pInstance = new CE_P_SpiritArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_SpiritArrow Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_SpiritArrow::Free()
{
	__super::Free();
}
