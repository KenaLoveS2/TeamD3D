#include "stdafx.h"
#include "..\public\E_KenaDamage.h"
#include "GameInstance.h"

CE_KenaDamage::CE_KenaDamage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaDamage::CE_KenaDamage(const CE_KenaDamage & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_KenaDamage::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaDamage::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	for (auto& pChild : m_vecChild)
	{
		pChild->Set_Parent(this);
		if (!lstrcmp(pChild->Get_ObjectCloneName(), L"KenaDamageCircleR_2"))
		{
			_matrix matrchildworld = XMMatrixIdentity();
			_matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(90.0f));

			_float3	vScale = _float3(XMVectorGetX(matrchildworld.r[0]),
				XMVectorGetX(XMVector3Length(matrchildworld.r[1])),
				XMVectorGetX(XMVector3Length(matrchildworld.r[2])));

			_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
			_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
			_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;
			
			matrchildworld.r[0] = XMVector4Transform(vRight, RotationMatrix);
			matrchildworld.r[1] = XMVector4Transform(vUp, RotationMatrix);
			matrchildworld.r[2] = XMVector4Transform(vLook, RotationMatrix);

			pChild->Set_InitMatrix(matrchildworld);
		}
	}
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_KenaDamage::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

 	if (m_eEFfectDesc.bActive == false)
 		return;

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(m_eEFfectDesc.bActive);

	Set_AddScale(fTimeDelta + 1.0f);
	_bool bResult = TurnOffSystem(m_fScaleTime, 0.2f, fTimeDelta);
	if (bResult == true)
	{
		for (auto& pChild : m_vecChild)
			pChild->ResetSprite();

		m_fAddValue = 0.0f;
		m_eEFfectDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	}
}

void CE_KenaDamage::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaDamage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_KenaDamage::Imgui_RenderProperty()
{
	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;
}

CE_KenaDamage * CE_KenaDamage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_KenaDamage * pInstance = new CE_KenaDamage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaDamage Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaDamage::Clone(void * pArg)
{
	CE_KenaDamage * pInstance = new CE_KenaDamage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaDamage Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaDamage::Free()
{
	__super::Free();
}
