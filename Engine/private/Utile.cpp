#include "..\public\Utile.h"

CUtile::CUtile() { }
CUtile::~CUtile() { }

POINT CUtile::GetClientCursorPos(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	return pt;
}

void CUtile::SetClientCursorPos(HWND hWnd, unsigned int iClientX, unsigned int iClientY)
{
	POINT pt = { (LONG)iClientX, (LONG)iClientY };
	ClientToScreen(hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

_tchar* CUtile::CharToWideChar(char* pCharStr)
{
	int iStrLen = (int)strlen(pCharStr) + 1;
	_tchar* pText = new _tchar[iStrLen];
	ZeroMemory(pText, sizeof(_tchar) * iStrLen);

	MultiByteToWideChar(CP_ACP, 0, pCharStr, iStrLen, pText, iStrLen);

	return pText;
}

char* CUtile::WideCharToChar(_tchar * pWideStr)
{	
	int iStrLen = wcslen(pWideStr) + 1;
	char* pText = new char[iStrLen];
	ZeroMemory(pText, sizeof(char) * iStrLen);

	WideCharToMultiByte(CP_ACP, 0, pWideStr, -1, pText, iStrLen, 0, 0);

	return pText;
}

void CUtile::CharToWideChar(const char * pCharStr, _tchar * pOut)
{
	int iStrLen = (int)strlen(pCharStr) + 1;
	MultiByteToWideChar(CP_ACP, 0, pCharStr, iStrLen, pOut, iStrLen);
}

void CUtile::WideCharToChar(const _tchar * pWideStr, char * pOut)
{
	_int len;
	_int slength = lstrlen(pWideStr) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, pWideStr, slength, 0, 0, 0, 0);
	::WideCharToMultiByte(CP_ACP, 0, pWideStr, slength, pOut, len, 0, 0);
}

_tchar* CUtile::Create_String(_tchar *pText)
{
	_int iSize = wcslen(pText) + 1;

	_tchar* pString = new _tchar[iSize];
	ZeroMemory(pString, sizeof(_tchar) * iSize);

	lstrcpy(pString, pText);

	return pString;
}

char* CUtile::Create_String(char * pText)
{
	_int iSize = strlen(pText) + 1;

	char* pString = new char[iSize];
	ZeroMemory(pString, sizeof(_tchar) * iSize);

	strcpy_s(pString, iSize, pText);

	return pString;
}

_float CUtile::Get_RandomFloat(_float fMinValue, _float fMaxValue)
{
	if (fMinValue >= fMaxValue)
		return fMinValue;

	float f = (rand() % 10000) * 0.0001f;

	return (f * (fMaxValue - fMinValue)) + fMinValue;
}

_float3 CUtile::Get_RandomVector(const _float3 & fMinValue, const _float3 & fMaxValue)
{
	return _float3(Get_RandomFloat(fMinValue.x, fMaxValue.x),
				   Get_RandomFloat(fMinValue.y, fMaxValue.y),
				   Get_RandomFloat(fMinValue.z, fMaxValue.z));
}
