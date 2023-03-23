#include "stdafx.h"
#include "..\public\E_P_Flower.h"
#include "GameInstance.h"
#include "Kena.h"

CE_P_Flower::CE_P_Flower(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_Flower::CE_P_Flower(const CE_P_Flower & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_Flower::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_Flower::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = true;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	RELEASE_INSTANCE(CGameInstance);

	m_pVIInstancingBufferCom->Set_ShapePosition();
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.5f, 2.f);
	return S_OK;
}

void CE_P_Flower::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	_vector vPos = m_pKena->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Set_Position(vPos);

	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta > 60.f)
	{
		m_pVIInstancingBufferCom->Set_ShapePosition();
		m_fTimeDelta = 0.0f;
	}
}

void CE_P_Flower::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_Flower::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CE_P_Flower * CE_P_Flower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_Flower * pInstance = new CE_P_Flower(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_Flower Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_Flower::Clone(void * pArg)
{
	CE_P_Flower * pInstance = new CE_P_Flower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_Flower Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_Flower::Free()
{
	__super::Free();
}
