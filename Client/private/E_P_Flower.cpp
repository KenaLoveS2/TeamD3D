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
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Flower::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = true;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	RELEASE_INSTANCE(CGameInstance);

	m_pVIInstancingBufferCom->Set_PSize(_float2(0.5f, 0.5f)); //fix
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.5f, 2.f);
	return S_OK;
}

void CE_P_Flower::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	if(m_pKena == nullptr)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pKena = (CKena*)	pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
		RELEASE_INSTANCE(CGameInstance);
	}

	_float4 vKenaPos = m_pKena->Get_TransformCom()->Get_Position();
	_float4 vMyPos = m_pTransformCom->Get_Position();
	 
	_float vDistance = XMVectorGetX(XMVector3Length(vKenaPos - vMyPos));
	if(vDistance > 20.f)
	{
		vKenaPos.y = 1.0f;
		m_pTransformCom->Set_Position(vKenaPos);
		m_fLife = 0.0f;
		m_bTurn = true;
	}

	m_fLife += fTimeDelta * 0.2f;
	if (m_bTurn)
	{
		if (m_fLife > 2.f)
		{
			m_bTurn = false;
			m_fLife = 0.0f;
		}
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
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_Flower::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bTurn", &m_bTurn, sizeof(_bool)), E_FAIL);

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
