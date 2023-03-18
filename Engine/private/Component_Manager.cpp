#include "stdafx.h"
#include "..\public\Component_Manager.h"
#include "Component.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pPrototypes || 
		iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
	{
		_tchar szTemp[128] = L"";
		swprintf_s(szTemp, L"%s__Prototype Add Fail", pPrototypeTag);
		MessageBoxW(CGameInstance::GetInstance()->GetHWND(), L"FAIL", szTemp, MB_ICONERROR);
		return E_FAIL;
	}
		

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg, CGameObject * pOwner)
{
	CComponent*		pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent*		pComponent = pPrototype->Clone(pArg, pOwner);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

void CComponent_Manager::Delete_ProtoComponent(_uint iLevelIndex, const _tchar * pProtoTypeTag)
{
	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pProtoTypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return;

	Safe_Release(iter->second);
	iter = m_pPrototypes[iLevelIndex].erase(iter);
}


CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);

}
