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


	static  string	  WstringToString(wstring wstr);
	static  wstring	  stringToWString(string str);


	static _tchar* Create_String(const _tchar *pText);
	static char* Create_String(const char *pText);
	static _tchar* Create_StringAuto(const _tchar *pText);
	
	static char* Split_String(char * pSour, char szSymbol);

	static _float  Get_RandomFloat(_float fMinValue, _float fMaxValue);
	static _float3 Get_RandomVector(const _float3& fMinValue, const _float3& fMaxValue);

	static char*   SeparateText(char* pCharStr);
	static _float  FloatLerp(_float fNum1, _float fNum2, _float fRatio);

	static _float3 ConvertPosition_PxToD3D(PxVec3& vPxPosition);
	static PxVec3 ConvertPosition_D3DToPx(_float3& vD3dPosition);

	static PxMat44 ConvertMatrix_D3DToPx(_float4x4 D3dXMatrix);
	static _float4x4 ConvertMatrix_PxToD3D(PxMat44 PxMatrix);
	
	static  void 	MODELMATERIAL_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,
		const char* jSonPath, vector<MODELMATERIAL>& Vec);

	static _float4 Calc_RayDirByMousePos(HWND hWnd, ID3D11DeviceContext *pContext, _float4 vRayPos);

	inline static _float3 Float_3to4(_float4& vValue) { return _float3(vValue.x, vValue.y, vValue.z); }
	inline static _float4 Float_4to3(_float3& vValue, _float fW) { return _float4(vValue.x, vValue.y, vValue.z, fW); }

	static void Execute_BillBoard(class CTransform* pTransform, _float3 vScale);

	static _tchar* Create_DummyString();

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
