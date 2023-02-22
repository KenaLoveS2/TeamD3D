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
	vector<wstring> m_vecUITextureProtoTag;
	vector<string> m_vecUITextureName;
	map<_tchar*, vector<string>> m_mapUIStrings;
	 	
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
	void				Add_UITextureTag(wstring wstr);
	vector<wstring>*	Get_UITextureProtoTagsPtr() { return &m_vecUITextureProtoTag; }
	vector<string>*		Get_UITextureNamesPtr() { return &m_vecUITextureName; }
	void				Add_UIString(_tchar* tag, string str);
	vector<string>*		Get_UIString(_tchar* tag);


};
END
