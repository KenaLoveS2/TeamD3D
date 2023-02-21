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

	m_vecUITextureProtoTag.clear();
	m_vecUITextureName.clear();

	for (auto pair : m_mapUIStrings)
		pair.second.clear();
	m_mapUIStrings.clear();
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

void CString_Manager::Add_UITextureTag(wstring wstr)
{
	for (auto tag : m_vecUITextureProtoTag)
	{
		if (0 == tag.compare(wstr))
		{
			MSG_BOX("Already Exists : String_Manager");
			return;
		}
	}

	m_vecUITextureProtoTag.push_back(wstr);
	wstring tag = L"Prototype_Component_Texture_";
	size_t tagLength = tag.length();
	size_t length = 0;
	length = wstr.length() - tagLength;
	wstr = wstr.substr(tagLength, length);

	string str;
	str.assign(wstr.begin(), wstr.end());
	m_vecUITextureName.push_back(str);
}

void CString_Manager::Add_UIString(_tchar * tag, string str)
{
	auto iter = find_if(m_mapUIStrings.begin(), m_mapUIStrings.end(), CTag_Finder(tag));
	if (iter == m_mapUIStrings.end())
	{
		vector<string> vec;
		vec.push_back(str);
		m_mapUIStrings.emplace(tag, vec);
	}
	else
	{
		for (auto t : iter->second)
		{
			if (0 == t.compare(str))
			{
				MSG_BOX("Already Exists");
				return;
			}
		}

		iter->second.push_back(str);
	}
}

vector<string>* CString_Manager::Get_UIString(_tchar * tag)
{
	auto iter = find_if(m_mapUIStrings.begin(), m_mapUIStrings.end(), CTag_Finder(tag));
	if (iter == m_mapUIStrings.end())
		return nullptr;

	return &iter->second;
}
