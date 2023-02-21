#include "stdafx.h"
#include "..\public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& Pair : m_GameObjects)
	{
		Pair.second ? Pair.second->Tick(fTimeDelta) : 0;
	}	
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_GameObjects)
	{
		Pair.second ? Pair.second->Late_Tick(fTimeDelta) : 0;
	}
}

void CLayer::SwitchOnOff_Shadow(_bool bSwitch)
{
	for (auto& Pair : m_GameObjects)
	{
		Pair.second ? Pair.second->SwitchOnOff_Shadow(bSwitch) : 0;
	}
}

CComponent * CLayer::Get_ComponentPtr(const _tchar* pCloneObjectTag, const _tchar * pComponentTag)
{	
	CGameObject* pObject = Find_GameObject(pCloneObjectTag);
	if (pObject == nullptr) return nullptr;

	return pObject->Find_Component(pComponentTag);
}

CGameObject * CLayer::Get_GameObjectPtr(const _tchar* pCloneObjectTag)
{
	CGameObject* pObject = Find_GameObject(pCloneObjectTag);
	if (pObject == nullptr) return nullptr;

	return pObject;
}

HRESULT CLayer::Add_GameObject(const _tchar* pCloneObjectTag, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace(pCloneObjectTag, pGameObject);

	return S_OK;
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& Pair : m_GameObjects)
		Safe_Release(Pair.second);

	m_GameObjects.clear();
}

CGameObject* CLayer::Find_GameObject(const _tchar * pCloneObjectTag)
{
	auto	Pair = find_if(m_GameObjects.begin(), m_GameObjects.end(), CTag_Finder(pCloneObjectTag));

	if (Pair == m_GameObjects.end())
		return nullptr;

	return Pair->second;
}

HRESULT CLayer::Delete_GameObject(const _tchar * pCloneObjectTag)
{
	auto Pair = find_if(m_GameObjects.begin(), m_GameObjects.end(), CTag_Finder(pCloneObjectTag));

	if (Pair == m_GameObjects.end()) return E_FAIL;

	CGameObject* pBackup = Pair->second;
	m_GameObjects.erase(Pair);
	
	Safe_Release(pBackup);

	return S_OK;
}
