#include "stdafx.h"
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

	for (auto pair : m_mapUIString)
		pair.second.clear();
	m_mapUIString.clear();

	for (auto pair : m_mapUIWString)
		pair.second.clear();
	m_mapUIWString.clear();
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

void CString_Manager::Add_UIString(_uint iKey, string str)
{
	auto iter = m_mapUIString.find(iKey);
	if (iter == m_mapUIString.end())
	{
		vector<string> vec;
		vec.push_back(str);
		m_mapUIString.emplace(iKey, vec);
	}
	else
	{
		for (auto s : iter->second)
		{
			if (0 == s.compare(str))
			{
				MSG_BOX("Already Exists");
				return;
			}
		}
		iter->second.push_back(str);
	}
}

vector<string>* CString_Manager::Get_UIString(_uint iKey)
{
	auto iter = m_mapUIString.find(iKey);
	if (iter == m_mapUIString.end())
		return nullptr;

	return &iter->second;
}

void CString_Manager::Add_UIWString(_uint iKey, const wstring & str)
{
	auto iter = m_mapUIWString.find(iKey);
	if (iter == m_mapUIWString.end())
	{
		vector<wstring> vec;
		vec.push_back(str);
		m_mapUIWString.emplace(iKey, vec);
	}
	else
	{
		for (auto s : iter->second)
		{
			if (0 == s.compare(str))
			{
				MSG_BOX("Already Exists");
				return;
			}
		}
		iter->second.push_back(str);
	}
}

vector<wstring>* CString_Manager::Get_UIWString(_uint iKey)
{
	auto iter = m_mapUIWString.find(iKey);
	if (iter == m_mapUIWString.end())
		return nullptr;

	return &iter->second;
}
