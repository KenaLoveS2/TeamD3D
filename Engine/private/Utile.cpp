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
	_int iStrLen = (_int)strlen(pCharStr) + 1;
	_tchar* pText = new _tchar[iStrLen];
	ZeroMemory(pText, sizeof(_tchar) * iStrLen);

	MultiByteToWideChar(CP_ACP, 0, pCharStr, iStrLen, pText, iStrLen);

	return pText;
}

char* CUtile::WideCharToChar(_tchar * pWideStr)
{	
	_int iStrLen = (_int)wcslen(pWideStr) + 1;
	char* pText = new char[iStrLen];
	ZeroMemory(pText, sizeof(char) * iStrLen);

	WideCharToMultiByte(CP_ACP, 0, pWideStr, -1, pText, iStrLen, 0, 0);

	return pText;
}

void CUtile::CharToWideChar(const char * pCharStr, _tchar * pOut)
{
	_int iStrLen = (_int)strlen(pCharStr) + 1;
	MultiByteToWideChar(CP_ACP, 0, pCharStr, iStrLen, pOut, iStrLen);
}

void CUtile::WideCharToChar(const _tchar * pWideStr, char * pOut)
{
	if (!lstrcmp(pWideStr,L""))
		return;
	_int len;
	_int slength = lstrlen(pWideStr) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, pWideStr, slength, 0, 0, 0, 0);
	::WideCharToMultiByte(CP_ACP, 0, pWideStr, slength, pOut, len, 0, 0);
}




_tchar * CUtile::StringToWideChar(string str)
{
	size_t origsize = 0, convertedChars = 0; // 원래 문자열 길이, 변환된 문자열 길이	
	origsize = strlen(str.c_str()) + 1;  // 변환시킬 문자열의 길이를 구함
	wchar_t* t = new wchar_t[origsize];
	mbstowcs_s(&convertedChars, t, origsize, str.c_str(), _TRUNCATE);
	return (_tchar*)t;
}

string CUtile::WstringToString(wstring wstr)
{
	string str;
	return str.assign(wstr.begin(), wstr.end());
}

char * CUtile::Split_String(char * pSour, char szSymbol)
{
	_uint	iSourLength = (_uint)strlen(pSour) + 1;

	char*	pOut = new char[iSourLength];
	ZeroMemory(pOut, iSourLength);

	_bool	bSplitPoint = false;
	for (_uint i = 0, j = 0; i < iSourLength; ++i)
	{
		if (bSplitPoint == false)
		{
			if (pSour[i] == szSymbol)
				bSplitPoint = true;
		}
		else
			pOut[j++] = pSour[i];
	}

	return pOut;
}

_tchar* CUtile::Create_String(const _tchar *pText)
{
	_int iSize = (_int)wcslen(pText) + 1;

	_tchar* pString = new _tchar[iSize];
	ZeroMemory(pString, sizeof(_tchar) * iSize);

	lstrcpy(pString, pText);

	return pString;
}

char* CUtile::Create_String(const char * pText)
{
	_int iSize = (_int)strlen(pText) + 1;

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

char * CUtile::SeparateText(char * pCharStr)
{
	char flag = 0;
	char Temp[128] = "";

	while (*pCharStr)
	{
		if (*pCharStr == '_')
			flag++;
		else if (flag == 2)
		{
			for (int i = 0; i < strlen(pCharStr) + 1; ++i)
			{
				if (pCharStr[i] == '_' || pCharStr[i] == '\0')
				{
					flag = 0;
					return Temp;
				}
				Temp[i] = pCharStr[i];
			}
		}
		else if (flag == 3 && *pCharStr == '_')
			flag = 0;

		pCharStr++;
	}
	return Temp;
}

_float CUtile::FloatLerp(_float fNum1, _float fNum2, _float fRatio)
{
	return fNum1 * (1.f - fRatio) + fNum2 * fRatio;
}
