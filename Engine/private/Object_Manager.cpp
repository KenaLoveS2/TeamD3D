#include "..\public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include "Imgui_Manager.h"
#include "Utile.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, const _tchar * pCloneObjectTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ComponentPtr(pComponentTag, pCloneObjectTag);
}

CGameObject* CObject_Manager::Get_GameObjectPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pCloneObjectTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObjectPtr(pCloneObjectTag);
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])	
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);
	
	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, const _tchar * pCloneObjectTag, void * pArg, CGameObject** ppOut)
{
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pCloneObjectTag, pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pCloneObjectTag, pGameObject);

	if (ppOut)
	{
		*ppOut = pGameObject;
	}

	return S_OK;
}

CGameObject * CObject_Manager::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;	
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if(nullptr != Pair.second)
				Pair.second->Tick(fTimeDelta);			
		}
	}	
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));
	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
	

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();

	
}

void CObject_Manager::Imgui_ProtoViewer(_uint iLevel, OUT const _tchar *& szSelectedProto)
{
	if (m_iNumLevels <= iLevel)
		return;

	if (ImGui::CollapsingHeader("ProtoViewer"))
	{
		if (ImGui::BeginListBox("##"))
		{
			for (auto& ProtoPair : m_Prototypes)
			{
				
				const bool bSelected = (szSelectedProto != nullptr) && (0 == lstrcmp(ProtoPair.first, szSelectedProto));
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				char szViewName[512], szProtoName[256];
				CUtile::WideCharToChar(ProtoPair.first, szProtoName);
				sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());

				if (ImGui::Selectable(szViewName, bSelected))
					szSelectedProto = ProtoPair.first;
			}

			ImGui::EndListBox();
		}
	}
}

void CObject_Manager::Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject)
{
	bool bFound = false;

	if (m_iNumLevels <= iLevel)
		return;

	const LAYERS& targetLevel = m_pLayers[iLevel];

	if (ImGui::TreeNode("ObjectViewer"))
	{
		for (auto& Pair : targetLevel) // for layer loop
		{
			char szLayerTag[128];
			char szobjectTag[128];
			CUtile::WideCharToChar(Pair.first, szLayerTag);
			if (ImGui::TreeNode(szLayerTag))  // for object loop listbox
			{
				if (ImGui::BeginListBox("##"))
				{
					CGameObject *pObj = nullptr;
					for (auto& Pair : Pair.second->GetGameObjects())
					{
						pObj = Pair.second;
						if (pObj != nullptr)
							CUtile::WideCharToChar((pObj->Get_ObjectName()), szobjectTag);

						const bool bSelected = pSelectedObject == pObj;
						if (bSelected)
						{
							ImGui::SetItemDefaultFocus();
							bFound = true;
						}

						char szAddressName[256];
						sprintf_s(szAddressName, "%s[%p]", typeid(*pObj).name(), pObj);

						if (ImGui::Selectable(szAddressName, bSelected))
						{
							pSelectedObject = pObj;
							bFound = true;
						}

					}
					ImGui::EndListBox();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (bFound == false)
		pSelectedObject = nullptr;
}
