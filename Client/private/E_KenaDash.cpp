#include "stdafx.h"
#include "..\public\E_KenaDash.h"
#include "GameInstance.h"
#include "E_P_ExplosionGravity.h"
#include "E_KenaDashRing.h"

CE_KenaDash::CE_KenaDash(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_KenaDash::CE_KenaDash(const CE_KenaDash & rhs)
	: CEffect(rhs)
{
	
}

HRESULT CE_KenaDash::Initialize_Prototype(const _tchar* pFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	if (pFilePath != nullptr)
		FAILED_CHECK_RETURN(Load_E_Desc(pFilePath), E_FAIL);

	return S_OK;
}

HRESULT CE_KenaDash::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Child(), E_FAIL);

	m_fHDRValue = 2.5f; 
	m_eEFfectDesc.bActive = false;
	m_pTransformCom->Set_WorldMatrix_float4x4(m_InitWorldMatrix);
	return S_OK;
}

void CE_KenaDash::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < CHILD_RING_5; ++i)
		m_vecChild[i]->Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == false)
		return;

	//__super::Tick(fTimeDelta);

	for (auto pChild : m_vecChild)
		pChild->Tick(fTimeDelta);

	m_fShaderBindTime += fTimeDelta;
	
	_bool bResult = TurnOffSystem(m_fTurnOffTime, 0.5f, fTimeDelta);
	if (bResult == true)	Reset();

	_bool bMyTurnOff = TurnOffSystem(fTime, 1.f, fTimeDelta);
	if (bMyTurnOff == true)	m_eEFfectDesc.bActive = false;
}

void CE_KenaDash::Tick_State(_float4 vPos)
{
	m_eEFfectDesc.bActive = true;

	m_eEFfectDesc.vScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	m_pTransformCom->Set_Scaled(_float3(0.01f, 0.01f, 0.01f));

	_float4 vMyPos = vPos;
	vMyPos.y = 0.0f;	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);

	for (auto& pChild : m_vecChild)
	{
		if (dynamic_cast<CE_KenaDashRing*>(pChild) == nullptr)
			pChild->Set_Active(true);
	}
		
	m_vecChild[CHILD_RING_0]->Set_Active(true);
	CGameInstance::GetInstance()->Play_Sound(L"SFX_Kena_Dash_Attack_Effect_Ring.ogg", 1.f, false);

	Tick_RingState(vMyPos); /* Ring */
	m_vecChild[CHILD_CONE]->Set_Position(vPos);
	dynamic_cast<CE_P_ExplosionGravity*>(m_vecChild[CHILD_P])->UpdateParticle(vMyPos);
}

void CE_KenaDash::Tick_RingState(_float4 vPos)
{
	auto& pChildRing = m_vecChild.begin();
	_vector vDir = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	_float4 vFinalPos = XMLoadFloat4(&vPos) - vDir * 0.2f;

	m_vecChild[CHILD_RING_0]->Set_Position(vFinalPos);
	m_vecChild[CHILD_RING_0]->Get_TransformCom()->LookAt(XMLoadFloat4(&vFinalPos) + vDir);
	dynamic_cast<CE_KenaDashRing*>(m_vecChild[CHILD_RING_0])->Set_CurrentScale(0.01f);
	//m_vecChild[CHILD_RING_0]->Get_TransformCom()->Set_Scaled(_float3(1.8f, 1.8f, 1.f));

	//for (_uint i = 0; i < CHILD_RING_4 + 1; i++, pChildRing++)
	//{
	//	//(*pChildRing)->Get_TransformCom()->Set_State(CTransform::STATE_LOOK, vDir);
	//	
	//	//vFinalPos.x += (i * 2.f);
	//	//vFinalPos.y += (i * 2.f);
	//	//vFinalPos.z += (i * 2.f);

	//	vFinalPos = XMLoadFloat4(&vFinalPos) + vDir * (i + 1) * 0.5f;

	//	(*pChildRing)->Set_Position(vFinalPos);
	//	(*pChildRing)->Get_TransformCom()->LookAt(XMLoadFloat4(&vFinalPos) + vDir);

	//	(*pChildRing)->Get_TransformCom()->Set_Scaled(_float3(1.8f, 1.8f, 1.f));
	//}
}

void CE_KenaDash::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_KenaDash::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CE_KenaDash::Reset()
{
	// m_eEFfectDesc.bActive = false;
	m_fShaderBindTime = 0.0f;
	m_bFinishSprite = false;

	for (auto& pChild : m_vecChild)
		pChild->Set_Active(false);
}

void CE_KenaDash::Imgui_RenderProperty()
{
}

HRESULT CE_KenaDash::SetUp_Child()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Base*	pEffectBase = nullptr;
	_tchar*			pCloneTag = nullptr;

	string		strMapTag = "";
	for (_uint i = 0; i < 6; ++i)
	{
		pCloneTag = CUtile::Create_DummyString(L"KenaDash_ring", i);
		pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_DashRing", pCloneTag));
		NULL_CHECK_RETURN(pEffectBase, E_FAIL);
		pEffectBase->Set_Parent(this);
		m_vecChild.push_back(pEffectBase);
	}

	for (_uint i = 0; i < (_uint)m_vecChild.size(); ++i)
	{
		if (i != (_uint)m_vecChild.size() - 1)
			dynamic_cast<CE_KenaDashRing*>(m_vecChild[i])->Set_NextRing(dynamic_cast<CE_KenaDashRing*>(m_vecChild[i + 1]));
	}

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_DashCone", L"KenaDash_Cone"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this); 
	m_vecChild.push_back(pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_ExplosionGravity", L"KenaDash_P"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	dynamic_cast<CE_P_ExplosionGravity*>(pEffectBase)->Set_Option(CE_P_ExplosionGravity::TYPE::TYPE_KENA_ATTACK2);
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CE_KenaDash * CE_KenaDash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaDash * pInstance = new CE_KenaDash(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaDash Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaDash::Clone(void * pArg)
{
	CE_KenaDash * pInstance = new CE_KenaDash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaDash Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaDash::Free()
{
	__super::Free();
}
