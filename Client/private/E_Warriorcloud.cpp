#include "stdafx.h"
#include "..\public\E_Warriorcloud.h"
#include "GameInstance.h"
#include "BossWarrior.h"
#include "E_P_Warrior.h"

CE_Warriorcloud::CE_Warriorcloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_Warriorcloud::CE_Warriorcloud(const CE_Warriorcloud & rhs)
	: CEffect(rhs)
{
}

HRESULT CE_Warriorcloud::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Warriorcloud::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	FAILED_CHECK_RETURN(SetUp_Child(), E_FAIL);
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

void CE_Warriorcloud::Tick(_float fTimeDelta)
{
	if (m_pWarrior == nullptr)
		m_pWarrior = (CBossWarrior*)CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Monster", L"BossWarrior_0");

	if (m_eEFfectDesc.bActive == false)
		return;

	m_vecChild[0]->Set_Active(true);

	m_fDurationTime += fTimeDelta;
	if (m_fDurationTime > 1.f)
	{
		m_eEFfectDesc.vScale = XMVectorSet(1.f, 1.f, 1.f, 1.f);
		dynamic_cast<CE_P_Warrior*>(m_vecChild[0])->Reset();
		m_eEFfectDesc.bActive = false;
		m_vecChild[0]->Set_Active(false);
		m_fTimeDelta = 0.0f;
		m_fDurationTime = 0.0f;
	}
	else
	{
		_float4 vScale = m_eEFfectDesc.vScale;
		vScale.x += fTimeDelta * 3.f + 0.2f;
		vScale.y += fTimeDelta * 3.f + 0.2f;
		vScale.z += fTimeDelta * 3.f + 0.2f;
		m_eEFfectDesc.vScale = vScale;

		if (m_pWarrior)
		{
			_vector vLook = XMVector3Normalize(m_pWarrior->Get_WorldMatrix().r[2]);
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_vector vMovePos = vPos + vLook * fTimeDelta;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMovePos);

		}
	}

	__super::Tick(fTimeDelta);
}

void CE_Warriorcloud::Late_Tick(_float fTimeDelta)
{

	if (m_pParent != nullptr)
		Set_Matrix();

	__super::Late_Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

}

HRESULT CE_Warriorcloud::Render()
{	
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CE_Warriorcloud::Imgui_RenderProperty()
{

}

HRESULT CE_Warriorcloud::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_Warriorcloud::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	return S_OK;
}

HRESULT CE_Warriorcloud::SetUp_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_Warrior_P", L"W_P_Explosion"));
	NULL_CHECK_RETURN(pEffectBase,E_FAIL );
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CE_Warriorcloud * CE_Warriorcloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,  const _tchar* pFilePath )
{
	CE_Warriorcloud * pInstance = new CE_Warriorcloud(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Warriorcloud Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Warriorcloud::Clone(void * pArg)
{
	CE_Warriorcloud * pInstance = new CE_Warriorcloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Warriorcloud Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Warriorcloud::Free()
{
	__super::Free();
}

