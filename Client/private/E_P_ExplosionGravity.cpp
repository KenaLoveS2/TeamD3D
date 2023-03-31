#include "stdafx.h"
#include "..\public\E_P_ExplosionGravity.h"
#include "GameInstance.h"

CE_P_ExplosionGravity::CE_P_ExplosionGravity(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_ExplosionGravity::CE_P_ExplosionGravity(const CE_P_ExplosionGravity & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_ExplosionGravity::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if(pFilePath != nullptr)
	{
		if (FAILED(Load_E_Desc(pFilePath)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = false;
	//m_pVIInstancingBufferCom->Set_PSize(_float2(0.5f, 0.5f));
	return S_OK;
}

HRESULT CE_P_ExplosionGravity::Late_Initialize(void* pArg)
{
	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_EXPLOSION_GRAVITY;

	return S_OK;
}

void CE_P_ExplosionGravity::Tick(_float fTimeDelta)
{
	ImGui::Begin("ExplosionGravity");

	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData = m_pVIInstancingBufferCom->Get_InstanceData();

	ImGui::InputFloat4("Color", (_float*)&m_eEFfectDesc.vColor);
	ImGui::InputFloat4("DiffuseTexture", (_float*)&m_eEFfectDesc.fFrame);
	ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);

	_float fSpeed = m_pTransformCom->Get_TransformDesc().fSpeedPerSec;
	_float fRotation = m_pTransformCom->Get_TransformDesc().fRotationPerSec;
	ImGui::InputFloat("Speed", &fSpeed);
	ImGui::InputFloat("Rotation", &fRotation);

	ImGui::End();

	__super::Tick(fTimeDelta);
}

void CE_P_ExplosionGravity::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_ExplosionGravity::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::SetUp_Components()
{
	return S_OK;
}

void CE_P_ExplosionGravity::Set_Option()
{
	switch (m_eType)
	{
	case TYPE::TYPE_DAMAGE_MONSTER:
		break;
	}
}

CE_P_ExplosionGravity * CE_P_ExplosionGravity::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_ExplosionGravity * pInstance = new CE_P_ExplosionGravity(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_ExplosionGravity Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_ExplosionGravity::Clone(void * pArg)
{
	CE_P_ExplosionGravity * pInstance = new CE_P_ExplosionGravity(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_ExplosionGravity Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_ExplosionGravity::Free()
{
	__super::Free();
}
