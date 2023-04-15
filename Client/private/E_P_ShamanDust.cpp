#include "stdafx.h"
#include "..\public\E_P_ShamanDust.h"
#include "GameInstance.h"
#include "BossShaman.h"

CE_P_ShamanDust::CE_P_ShamanDust(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_ShamanDust::CE_P_ShamanDust(const CE_P_ShamanDust & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_ShamanDust::Initialize_Prototype(const _tchar * pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_P_ShamanDust::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);

	m_eEFfectDesc.bActive = true;
	return S_OK;
}

HRESULT CE_P_ShamanDust::Late_Initialize(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pShaman = (CBossShaman*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Monster"), TEXT("BossShaman_0"));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CE_P_ShamanDust::Tick(_float fTimeDelta)
{
	ImGui::Begin("CE_P_ShamanDust");
	if (ImGui::Button("re"))
		m_pShaderCom->ReCompile();
	ImGui::End();
	
	if (m_pShaman && m_pShaman->Get_MonsterStatusPtr()->Get_HP() < 1.0f && m_eEFfectDesc.bActive == false)
		return;

	__super::Tick(fTimeDelta);

	if(m_pShaman == nullptr)
		m_pShaman = (CBossShaman*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Monster"), TEXT("BossShaman_0"));

	_float4 vShamanPos = m_pShaman->Get_TransformCom()->Get_Position();
	_float4 vMyPos = m_pTransformCom->Get_Position();

	_float vDistance = XMVectorGetX(XMVector3Length(vShamanPos - vMyPos));
	if(vDistance > 20.f)
	{
		vShamanPos.y = m_pShaman->Get_TransformCom()->Get_Position().y + 1.5f;
		m_pTransformCom->Set_Position(vShamanPos);
		m_fLife = 0.0f;
		m_bTurn = true;
	}

	m_fLife += fTimeDelta * 0.7f;
	if (m_bTurn)
	{
		if (m_fLife > 2.f)
		{
			m_bTurn = false;
			m_fLife = 0.0f;
		}
	}
}

void CE_P_ShamanDust::Late_Tick(_float fTimeDelta)
{
	if (m_bTurnOnfirst == false)
	{
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(3.f, 10.f)); //fix
		m_pVIInstancingBufferCom->Set_RandomSpeeds(0.5f, 2.f);
		m_pTransformCom->Set_Position(m_pShaman->Get_TransformCom()->Get_Position());

		m_bTurnOnfirst = true;
	}

	if (m_pShaman && m_pShaman->Get_MonsterStatusPtr()->Get_HP() < 1.0f && m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_ShamanDust::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

HRESULT CE_P_ShamanDust::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bTurn", &m_bTurn, sizeof(_bool)), E_FAIL);
 	FAILED_CHECK_RETURN(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", CGameInstance::GetInstance()->Get_DepthTargetSRV()), E_FAIL);

	return S_OK;
}

CE_P_ShamanDust * CE_P_ShamanDust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_ShamanDust * pInstance = new CE_P_ShamanDust(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_ShamanDust Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_ShamanDust::Clone(void * pArg)
{
	CE_P_ShamanDust * pInstance = new CE_P_ShamanDust(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_ShamanDust Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_ShamanDust::Free()
{
	__super::Free();
}
