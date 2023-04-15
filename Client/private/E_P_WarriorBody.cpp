#include "stdafx.h"
#include "..\public\E_P_WarriorBody.h"
#include "GameInstance.h"
#include "BossWarrior.h"

CE_P_WarriorBody::CE_P_WarriorBody(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_WarriorBody::CE_P_WarriorBody(const CE_P_WarriorBody & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_WarriorBody::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_WarriorBody::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = true;
	return S_OK;
}

void CE_P_WarriorBody::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);
}

void CE_P_WarriorBody::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false  )
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(8.f, 10.f));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(0.5f, 1.f);
		m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.0f));
		m_eEFfectDesc.vColor = XMVectorSetW(m_eEFfectDesc.vColor, 0.05f);
		m_bTurnOnfirst = true;
	}

	if (dynamic_cast<CBossWarrior*>(m_pParent)->Get_MonsterStatusPtr()->Get_HP() < 1.0f || m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_Position();
		vPos.y += 1.5f;
		m_pTransformCom->Set_Position(vPos);
	}

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_WarriorBody::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_P_WarriorBody::Imgui_RenderProperty()
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

void CE_P_WarriorBody::Reset()
{
}

CE_P_WarriorBody * CE_P_WarriorBody::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_WarriorBody * pInstance = new CE_P_WarriorBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_WarriorBody Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_WarriorBody::Clone(void * pArg)
{
	CE_P_WarriorBody * pInstance = new CE_P_WarriorBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_WarriorBody Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_WarriorBody::Free()
{
	__super::Free();
}
