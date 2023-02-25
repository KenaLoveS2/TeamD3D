#include "stdafx.h"
#include "..\public\Object_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Imgui_Manager.h"
#include "Utile.h"

#include "EnviromentObj.h"

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

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels, _uint iNumCopyPrototypes)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];
	m_mapAnimModel = new map<const _tchar*, class CGameObject*>[iNumLevels];
	m_mapShaderValueModel = new map<const _tchar*, class CGameObject*>[iNumLevels];

	m_iNumLevels = iNumLevels;
	
	m_iNumCopyPrototypes = iNumCopyPrototypes;	
	for (_uint i = 0; i < m_iNumCopyPrototypes; i++)
	{
		PROTOTYPES Prototypes;
		m_CopyPrototypes.push_back(Prototypes);
	}

	return S_OK;
}

HRESULT CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])	
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

 	for (auto& Pair : m_mapAnimModel[iLevelIndex])
 		Safe_Release(Pair.second);
 
 	m_mapAnimModel[iLevelIndex].clear();

	for (auto& Pair : m_mapShaderValueModel[iLevelIndex])
		Safe_Release(Pair.second);

	m_mapShaderValueModel[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);
	
	for (_uint i = 0; i < m_iNumCopyPrototypes; i++)
	{
		_tchar* pTag = CUtile::Create_String(pPrototypeTag);
		m_CopyTagList.push_back(pTag);
		
		CGameObject* pGameObject = pPrototype->Clone();
		m_CopyPrototypes[i].emplace(pTag, pGameObject);
	}

	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, 
	const _tchar * pCloneObjectTag, void * pArg, CGameObject** ppOut)
{
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer) return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pCloneObjectTag, pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pCloneObjectTag, pGameObject);

	/*Set_CloneTag*/
	if (pCloneObjectTag != nullptr)
		pGameObject->Set_CloneTag(pCloneObjectTag);

	/*Set_ProtoTag*/
	if (pPrototypeTag != nullptr)
		pGameObject->Set_ProtoTag(pPrototypeTag);

	if (ppOut)
	{
		*ppOut = pGameObject;
	}

	return S_OK;
}

CGameObject * CObject_Manager::Clone_GameObject(const _tchar * pPrototypeTag, const _tchar * pCloneObjectTag, void * pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	if (pCloneObjectTag != nullptr)
		pGameObject->Set_CloneTag(pCloneObjectTag);

	/*Set_ProtoTag*/
	if (pPrototypeTag != nullptr)
		pGameObject->Set_ProtoTag(pPrototypeTag);

	return pGameObject;	
}

HRESULT CObject_Manager::Clone_AnimObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, const _tchar * pCloneObjectTag, void * pArg, CGameObject ** ppOut)
{
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, E_FAIL);

	CGameObject*		pGameObject = pPrototype->Clone(pArg);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		NULL_CHECK_RETURN(pLayer, E_FAIL);

		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pCloneObjectTag, pGameObject), E_FAIL);

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pCloneObjectTag, pGameObject);

	m_mapAnimModel[iLevelIndex].emplace(pCloneObjectTag, pGameObject);
	Safe_AddRef(pGameObject);

	if (pCloneObjectTag != nullptr)
		pGameObject->Set_CloneTag(pCloneObjectTag);

	/*Set_ProtoTag*/
	if (pPrototypeTag != nullptr)
		pGameObject->Set_ProtoTag(pPrototypeTag);

	if (ppOut)
		*ppOut = pGameObject;

	return S_OK;
}

HRESULT CObject_Manager::Add_ClonedGameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar* pCloneObjectTag, CGameObject * pGameObject)
{	
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer) return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pCloneObjectTag, pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pCloneObjectTag, pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_ShaderValueObject(_uint iLevelIndex, CGameObject * pGameObject)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_mapShaderValueModel[iLevelIndex].emplace(pGameObject->Get_ObjectCloneName(), pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_AnimObject(_uint iLevelIndex, CGameObject * pGameObject)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_mapAnimModel[iLevelIndex].emplace(pGameObject->Get_ObjectCloneName(), pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
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

void CObject_Manager::SwitchOnOff_Shadow(_bool bSwitch)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->SwitchOnOff_Shadow(bSwitch);
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
		for (auto& Pair : m_mapAnimModel[i])
			Safe_Release(Pair.second);
		m_mapAnimModel[i].clear();

		for (auto& Pair : m_mapShaderValueModel[i])
			Safe_Release(Pair.second);
		m_mapShaderValueModel[i].clear();

		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
	Safe_Delete_Array(m_mapShaderValueModel);
	Safe_Delete_Array(m_mapAnimModel);
	

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
	for (_uint i = 0; i < m_iNumCopyPrototypes; i++)
	{
		for (auto& Pair : m_CopyPrototypes[i])
		{
			Safe_Release(Pair.second);
		}
	}

	for (auto& iter : m_CopyTagList)
		Safe_Delete_Array(iter);
	
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
				
void CObject_Manager::Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject)
{
	bool bFound = false;

	if (m_iNumLevels <= iLevel)
		return;

	const LAYERS& targetLevel = m_pLayers[iLevel];

	if (ImGui::TreeNode("ObjectViewer"))
	{
		for (auto& LayerPair : targetLevel) // for layer loop
		{
			char szLayerTag[128];
			char szobjectTag[128];
			CUtile::WideCharToChar(LayerPair.first, szLayerTag);
			if (ImGui::TreeNode(szLayerTag))  // for object loop listbox
			{
				if (ImGui::BeginListBox("##"))
				{
					CGameObject *pObj = nullptr;
					for (auto& Pair : LayerPair.second->GetGameObjects())
					{
						pObj = Pair.second;
						if (pObj != nullptr)
							CUtile::WideCharToChar((pObj->Get_ObjectCloneName()), szobjectTag);

						const bool bSelected = pSelectedObject == pObj;
						if (bSelected)
						{
							ImGui::SetItemDefaultFocus();
							bFound = true;
						}
						if (ImGui::Selectable(szobjectTag, bSelected))
						{
							m_wstrSelecteObject_LayerTag = LayerPair.first;
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

void CObject_Manager::Imgui_DeleteComponentOrObject(OUT class CGameObject*& pSelectedObject)
{
	if (nullptr == pSelectedObject)
		return;
	ImGui::InputText("Com_Tag", &m_strComponentTag);
	if (ImGui::Button("Delete_Object"))
	{
		Delete_Object(CGameInstance::GetInstance()->Get_CurLevelIndex(),
			m_wstrSelecteObject_LayerTag.c_str(), pSelectedObject->Get_ObjectCloneName());

		m_wstrSelecteObject_LayerTag = L"";
		pSelectedObject = nullptr;
	}
	ImGui::SameLine();			// Delete_Object		ImGui::SameLine(); 	Delete_Component
	if (ImGui::Button("Delete_Component"))	
	{
		if (nullptr == pSelectedObject)
			return;
		_tchar* pComponentTag = CUtile::StringToWideChar(m_strComponentTag);
		CGameInstance::GetInstance()->Add_String(pComponentTag);
		pSelectedObject->Delete_Component(pComponentTag);
	}
}

void CObject_Manager::Imgui_Push_Group(CGameObject * pSelectedObject)
{
	if (nullptr == pSelectedObject)
		return;

	if (ImGui::CollapsingHeader("Envirioments_Control_Components"))
	{
		if (ImGui::Button("Add Multi Group"))
		{
			Mulit_ObjectList.push_back(pSelectedObject);
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear Multi Group"))
			Mulit_ObjectList.clear();

		if (ImGui::Button("GroupObj_Delete_Component"))
		{
			_tchar* pComponentTag = CUtile::StringToWideChar(m_strComponentTag);
			CGameInstance::GetInstance()->Add_String(pComponentTag);

			for (auto &pObj : Mulit_ObjectList)
			{
				pObj->Delete_Component(pComponentTag);
				//Safe_Release(pComponentTag);
			}
		}

		Imgui_Add_For_EnviroMent_Component();

		ImGui::Separator();

		string str = "";
		wstring pObjectClontTag = L"";
		if (ImGui::BeginListBox("#T#"))
		{
			const bool bTestSelected = false;
			for (auto& pAddObj : Mulit_ObjectList)
			{
				char szobjectTag[128];
				CUtile::WideCharToChar(pAddObj->Get_ObjectCloneName(), szobjectTag);

				if (ImGui::Selectable(szobjectTag, bTestSelected))
				{
					pObjectClontTag = pAddObj->Get_ObjectCloneName();
				}
			}
			ImGui::EndListBox();
		}

		if (pObjectClontTag != L"")
		{
			Mulit_ObjectList.remove_if([&](CGameObject* pGameObject)->bool
			{
				if (!lstrcmp(pObjectClontTag.c_str(), pGameObject->Get_ObjectCloneName()))
					return true;
				return false;
			});
		}
	}
}

void CObject_Manager::Imgui_Add_For_EnviroMent_Component()
{
	static _int iOption = 0;
	ImGui::InputInt("Component Option", &iOption);

	if (ImGui::Button("GroupObj_Add_Component"))
	{
		_tchar* pComponentTag = CUtile::StringToWideChar(m_strComponentTag);
		CGameInstance::GetInstance()->Add_String(pComponentTag);

		for (auto &plistObj : Mulit_ObjectList)
		{
			if (dynamic_cast<CEnviromentObj*>(plistObj) == nullptr)
				continue;
			
			static_cast<CEnviromentObj*>(plistObj)->Add_AdditionalComponent(CGameInstance::GetInstance()->Get_CurLevelIndex(),
					pComponentTag, (CEnviromentObj::COMPONENTS_OPTION)iOption);
		}
	}
}

HRESULT CObject_Manager::Delete_Object(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pCloneObjectTag)
{	
	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (pLayer == nullptr) return E_FAIL;
	
	pLayer->Delete_GameObject(pCloneObjectTag);

	return S_OK;
}
