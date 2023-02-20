#include "String_Manager.h"

IMPLEMENT_SINGLETON(CString_Manager)

CString_Manager::CString_Manager() 
{ 
}

void CString_Manager::Free()
{
	for (auto& vecLevel : m_LevelModelTags)
	{
		for (auto& pStr : vecLevel)
		{
			Safe_Delete_Array(pStr);
		}
	}
}

void CString_Manager::Initalize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels + 1;
	m_iStaticIndex = iNumLevels;
	m_LevelModelTags.reserve(m_iNumLevels);

	for (_uint i = 0; i < m_iNumLevels; i++)
	{
		vector<_tchar*> vecLevel;
		m_LevelModelTags.push_back(vecLevel);
	}
}

HRESULT CString_Manager::Add_String(_uint iLevelIndex, _tchar * pStr)
{
	/*
	_tchar *pFindString = Find_String(iLevelIndex, pTag);
	if (pFindString) return E_FAIL;
	*/
	
	vector<_tchar*> &vecLevel = m_LevelModelTags[iLevelIndex];
	vecLevel.push_back(pStr);

	return S_OK;
}

HRESULT CString_Manager::Add_String(_tchar * pStr)
{
	/*
	_tchar *pFindString = Find_String(iLevelIndex, pTag);
	if (pFindString) return E_FAIL;
	*/

	vector<_tchar*> &vecLevel = m_LevelModelTags[m_iStaticIndex];
	vecLevel.push_back(pStr);

	return S_OK;
}

_tchar* CString_Manager::Find_String(_uint iLevelIndex, _tchar * pStr)
{
	if (iLevelIndex >= m_iNumLevels) return nullptr;

	vector<_tchar*> &vecLevel = m_LevelModelTags[iLevelIndex];
	
	auto iter = find_if(vecLevel.begin(), vecLevel.end(), [&](_tchar* pModelTag)->_bool
	{
		return !lstrcmp(pStr, pModelTag);
	});

	if (iter == vecLevel.end()) return nullptr;

	return *iter;
}
