#pragma once
#include "Base.h"

BEGIN(Engine)
class CString_Manager :	public CBase
{
	DECLARE_SINGLETON(CString_Manager)
private:
	_uint m_iNumLevels = 0;
	_uint m_iStaticIndex = 0;
	vector<vector<_tchar*>> m_LevelModelTags;

private: /* Only For UI */
	map<_uint, vector<string>>		m_mapUIString;
	map<_uint, vector<wstring>>		m_mapUIWString;
	 	
private:
	CString_Manager();
	virtual ~CString_Manager() = default;

public:
	virtual void Free();

	void Initalize(_uint iNumLevels);
	HRESULT Add_String(_uint iLevelIndex, _tchar* pStr);
	HRESULT Add_String(_tchar* pStr);
	_tchar* Find_String(_uint iLevelIndex, _tchar* pStr);

	/* Only For UI */
	void				Add_UIString(_uint iKey, string str);
	vector<string>*		Get_UIString(_uint iKey);
	void				Add_UIWString(_uint iKey, const wstring& str);
	vector<wstring>*	Get_UIWString(_uint iKey);


};
END
