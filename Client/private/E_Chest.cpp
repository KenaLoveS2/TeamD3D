#include "stdafx.h"
#include "..\public\E_Chest.h"
#include "GameInstance.h"

CE_Chest::CE_Chest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_Chest::CE_Chest(const CE_Chest & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_Chest::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_Chest::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = true;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(nullptr);

	return S_OK;
}

HRESULT CE_Chest::Late_Initialize(void* pArg)
{
	m_bTimer = true;
	return S_OK;
}

void CE_Chest::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	if (m_fShaderBindTime > 1.f)
		Reset();
}

void CE_Chest::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_pParent)
	{
		_matrix  matParent = m_pParent->Get_TransformCom()->Get_WorldMatrix();
		_matrix  matScaleSet = XMMatrixIdentity();
		_float4  vScale = m_pTransformCom->Get_Scaled();

		memcpy(&matScaleSet.r[0], &(XMVector3Normalize(matParent.r[0]) * vScale.x), sizeof(_vector));
		memcpy(&matScaleSet.r[1], &(XMVector3Normalize(matParent.r[1]) * vScale.y), sizeof(_vector));
		memcpy(&matScaleSet.r[2], &(XMVector3Normalize(matParent.r[2]) * vScale.z), sizeof(_vector));
		memcpy(&matScaleSet.r[3], &matParent.r[3], sizeof(_vector));
		matScaleSet.r[3] = XMVectorSetY(matScaleSet.r[3], XMVectorGetY(matScaleSet.r[3]) + 0.6f);

		m_WorldWithParentMatrix = m_InitWorldMatrix * matScaleSet;
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldWithParentMatrix));

		m_pTransformCom->RotationFromNow(m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.0f));
	}

	Update_childPosition();

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_Chest::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CE_Chest::Imgui_RenderProperty()
{
}

HRESULT CE_Chest::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_Chest::SetUp_ShaderResources()
{
	return S_OK;
}

void CE_Chest::Reset()
{
	m_fTimeDelta = 0.0f;
	m_fShaderBindTime = 0.0f;
	m_fHDRValue = 2.f;
	m_bTimer = false;
	m_eEFfectDesc.bActive = false;

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(false);
}

void CE_Chest::Update_childPosition()
{
	for (auto& pChild : m_vecChild)
		pChild->Set_Active(true);

	if (m_pParent) 
	{
		_vector vPos = m_pTransformCom->Get_Position();

		_vector vLook = XMVector3Normalize(m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));
		_float4 vChildPos0 = vPos + vLook * 0.2f;
		_float4 vChildPos1 = vPos - vLook * 0.2f;

		_float4x4 matWorld;
		XMStoreFloat4x4(&matWorld, m_pTransformCom->Get_WorldMatrix());

		m_vecChild[CHILD_0]->Set_WorldMatrix(matWorld);
		m_vecChild[CHILD_1]->Set_WorldMatrix(matWorld);

		m_vecChild[CHILD_0]->Set_Position(vChildPos0);
		m_vecChild[CHILD_1]->Set_Position(vChildPos1);
	}
}

CE_Chest * CE_Chest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Chest * pInstance = new CE_Chest(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Chest Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Chest::Clone(void * pArg)
{
	CE_Chest * pInstance = new CE_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Chest Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Chest::Free()
{
	__super::Free();
}
