#include "stdafx.h"
#include "..\public\BossRock_Pool.h"
#include "..\public\BossRock.h"

CBossRock_Pool::CBossRock_Pool(ID3D11Device* pDevice, ID3D11DeviceContext* p_context)
	:CGameObject(pDevice, p_context)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

CBossRock_Pool::CBossRock_Pool(const CBossRock_Pool& rhs)
	:CGameObject(rhs)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CBossRock_Pool::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CBossRock_Pool::Initialize(void* pArg)
{
	ZeroMemory(&m_Desc, sizeof(DESC));

	if (pArg) memcpy(&m_Desc, pArg, sizeof(DESC));
	else {		
		m_Desc.vCenterPos = { 0.f, 0.f, 0.f, 1.f };
		m_Desc.iRockCount = 30;
	}
	
	m_Rocks.reserve(m_Desc.iRockCount);

	CBossRock::DESC BossRockDesc;
	CBossRock* pRock = nullptr;
	for (_uint i = 0; i < (_uint)m_Desc.iRockCount; i++)
	{
		BossRockDesc.eType = (CBossRock::ROCK_TPYE)(i % CBossRock::ROCK_TPYE_END);
		BossRockDesc.vPosition = m_Desc.vCenterPos + _float4(CUtile::Get_RandomFloat(-7.f, 7.f), 0.01f, CUtile::Get_RandomFloat(-7.f, 7.f), 0.f);
		BossRockDesc.fUpTime = CUtile::Get_RandomFloat(1.f, 2.f);
		BossRockDesc.fSpeedY = CUtile::Get_RandomFloat(8.f, 10.f);

		pRock = (CBossRock*)m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BossRock"), CUtile::Create_DummyString(), &BossRockDesc);
		assert(pRock && "CBossRock_Pool::Initialize()");

		m_Rocks.push_back(pRock);
		pRock = nullptr;
	}

	return S_OK;
}

HRESULT CBossRock_Pool::Late_Initialize(void * pArg)
{	
	for (auto& pRock : m_Rocks)
		pRock->Late_Initialize(nullptr);

	return S_OK;
}

void CBossRock_Pool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& pRock : m_Rocks)
		pRock->Tick(fTimeDelta);
}

void CBossRock_Pool::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& pRock : m_Rocks)
		pRock->Late_Tick(fTimeDelta);
}

CBossRock_Pool* CBossRock_Pool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossRock_Pool* pInstance = new CBossRock_Pool(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBossRock_Pool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossRock_Pool::Clone(void* pArg)
{
	CBossRock_Pool* pInstance = new CBossRock_Pool(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBossRock_Pool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossRock_Pool::Free()
{
	__super::Free();

	for (auto& pRock : m_Rocks)
		Safe_Release(pRock);

	m_Rocks.clear();
}

void CBossRock_Pool::Execute_UpRocks()
{
	for (auto& pRock : m_Rocks)
		pRock->Exectue_Up();
}
