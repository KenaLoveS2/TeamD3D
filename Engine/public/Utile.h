#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class ENGINE_DLL CUtile
{
public:
	CUtile();
	~CUtile();

public:	
	static POINT GetClientCursorPos(HWND hWnd);
	static void SetClientCursorPos(HWND hWnd, unsigned int iClientX, unsigned int iClientY);

	static _tchar* CharToWideChar(char* pCharStr);
	static char* WideCharToChar(_tchar* pWideStr);
	static void CharToWideChar(const char* pCharStr, _tchar* pOut);
	static void WideCharToChar(const _tchar* pWideStr, char* pOut);

	static  _tchar* StringToWideChar(string str);

	static _tchar* Create_String(const _tchar *pText);
	static char* Create_String(const char *pText);

	static _float  Get_RandomFloat(_float fMinValue, _float fMaxValue);
	static _float3 Get_RandomVector(const _float3& fMinValue, const _float3& fMaxValue);

public:
	template<typename T> static void Swap(T& Sour, T&Dest)
	{
		T Temp = Sour;
		Sour = Dest;
		Dest = Temp;
	}

	template<typename T> static void Saturate(T& Value, T Min, T Max)
	{
		if (Value < Min)
			Value = Min;
		if (Value > Max)
			Value = Max;
	}
};
END
