#include "stdafx.h"
#include "..\public\Layer.h"
#include "GameObject.h"
#include "EnviromentObj.h"
#include "PhysX_Manager.h"
#include "Renderer.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Late_Initialize()
{
	for (auto& Pair : m_GameObjects)
	{
		if (Pair.second != nullptr)
			Pair.second->Late_Initialize();
	}

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

void CLayer::Delete_AllObjLayer(_int iRoomIndex)
{
	/*for (auto& Pair : m_GameObjects)
	{
		if (dynamic_cast<CEnviromentObj*>(Pair.second) == nullptr)
			return;


		if (iRoomIndex == static_cast<CEnviromentObj*>(Pair.second)->Get_RoomIndex())
		{
			Safe_Release(Pair.second);
			m_GameObjects.erase(Pair.first);
		}
	}*/
	

	for (auto Iter = m_GameObjects.begin(); Iter != m_GameObjects.end();)
	{
		if (dynamic_cast<CEnviromentObj*>(Iter->second) == nullptr)
		{
			++Iter;
		}
		if (iRoomIndex == static_cast<CEnviromentObj*>(Iter->second)->Get_RoomIndex())
		{
			if(static_cast<CEnviromentObj*>(Iter->second)->Get_RendererCom() != nullptr)
				static_cast<CEnviromentObj*>(Iter->second)->Get_RendererCom()->EraseStaticShadowObject(Iter->second);

			Iter->second->Get_TransformCom()->Clear_Actor();
			Safe_Release(Iter->second);
			Iter->second = nullptr;
			Iter = m_GameObjects.erase(Iter);
		}
		else
			++Iter;
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
		
	CPhysX_Manager::GetInstance()->Delete_Actor(pBackup);
	
	Safe_Release(pBackup);

	return S_OK;
}
