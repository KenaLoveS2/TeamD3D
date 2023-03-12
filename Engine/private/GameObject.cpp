#include "stdafx.h"
#include "..\public\GameObject.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Utile.h"

const _tchar* CGameObject::m_pTransformComTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isCloned(false)
	, m_bRotable(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs) 
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
	, m_bRotable(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	GAMEOBJECTDESC	GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	if (nullptr != pArg)	
		GameObjectDesc = *(GAMEOBJECTDESC*)pArg;	

	if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(), CGameInstance::m_pPrototypeTransformTag, m_pTransformComTag, (CComponent**)&m_pTransformCom, &GameObjectDesc.TransformDesc)))
		return E_FAIL;

	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::RenderShadow()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void * pArg, CGameObject* pOwner)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent*	pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg, pOwner);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	Safe_AddRef(pComponent);

	*ppOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Compute_CamDistance()
{
	_vector		vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_vector		vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	RELEASE_INSTANCE(CPipeLine);

}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Delete_Component(const _tchar * pComponentTag)
{
	for (auto& iter = m_Components.begin(); iter != m_Components.end();)
	{
		if (iter->second == Find_Component(pComponentTag))
		{
			Safe_Release(iter->second);
			m_Components.erase(iter);
			break;
		}
		else
			iter++;
	}
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

void CGameObject::Imgui_RenderComponentProperties()
{
	for (const auto& com : m_Components)
	{
		ImGui::Separator();
		char szName[128];
		CUtile::WideCharToChar(com.first, szName);
		ImGui::Text("%s", szName);
		com.second->Imgui_RenderProperty();
	}
}

void CGameObject::Set_Position(_float4& vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPosition));
}

void CGameObject::Set_Position(_float3& vPosition)
{
	_float3 vPivot = m_pTransformCom->Get_vPxPivot();
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition - vPivot, 1.f));
}

void CGameObject::Set_WorldMatrix(_float4x4& vPosition)
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&vPosition));
}
