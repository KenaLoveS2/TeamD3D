#include "..\public\Utile.h"

#include "Texture.h"
#include "Json/json.hpp"
#include <fstream>
#include "GameInstance.h"
#include "String_Manager.h"
#include "Camera.h"

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

wstring CUtile::stringToWString(string str)
{
	wstring wstr;
	return wstr.assign(str.begin(), str.end());
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

_tchar* CUtile::Create_StringAuto(const _tchar *pText)
{
	_int iSize = (_int)wcslen(pText) + 1;

	_tchar* pString = new _tchar[iSize];
	ZeroMemory(pString, sizeof(_tchar) * iSize);

	lstrcpy(pString, pText);

	CString_Manager::GetInstance()->Add_String(pString);

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

_float CUtile::FloatLerp(_float fNum1, _float fNum2, _float fRatio)
{
	return fNum1 * (1.f - fRatio) + fNum2 * fRatio;
}
// PhysX to DirectX 11 conversion matrix
const DirectX::XMMATRIX g_PhysXToD3D11Matrix = DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);

// Transform a position vector from PhysX to DirectX 11 coordinate system
_float3 CUtile::ConvertPosition_PxToD3D(PxVec3& vPxPosition)
{	
	DirectX::XMVECTOR d3d11Pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(vPxPosition.x, vPxPosition.y, vPxPosition.z, 1.0f), g_PhysXToD3D11Matrix);

	return d3d11Pos;
}

// Transform a position vector from DirectX 11 to PhysX coordinate system
PxVec3 CUtile::ConvertPosition_D3DToPx(_float3& vD3dPosition)
{	
	physx::PxVec3 PxPos(
		g_PhysXToD3D11Matrix.r[0].m128_f32[0] * vD3dPosition.x + g_PhysXToD3D11Matrix.r[1].m128_f32[0] * vD3dPosition.y + g_PhysXToD3D11Matrix.r[2].m128_f32[0] * vD3dPosition.z,
		g_PhysXToD3D11Matrix.r[0].m128_f32[1] * vD3dPosition.x + g_PhysXToD3D11Matrix.r[1].m128_f32[1] * vD3dPosition.y + g_PhysXToD3D11Matrix.r[2].m128_f32[1] * vD3dPosition.z,
		g_PhysXToD3D11Matrix.r[0].m128_f32[2] * vD3dPosition.x + g_PhysXToD3D11Matrix.r[1].m128_f32[2] * vD3dPosition.y + g_PhysXToD3D11Matrix.r[2].m128_f32[2] * vD3dPosition.z);

	return PxPos;
}

PxMat44 CUtile::ConvertMatrix_D3DToPx(_float4x4 D3dXMatrix)
{
	PxMat44 PxMatrix;
	
	PxMatrix[0][0] = D3dXMatrix._11;
	PxMatrix[0][1] = D3dXMatrix._12;
	PxMatrix[0][2] = -D3dXMatrix._13;
	PxMatrix[0][3] = D3dXMatrix._14;

	PxMatrix[1][0] = D3dXMatrix._21;
	PxMatrix[1][1] = D3dXMatrix._22;
	PxMatrix[1][2] = -D3dXMatrix._23;
	PxMatrix[1][3] = D3dXMatrix._24;

	PxMatrix[2][0] = -D3dXMatrix._31;
	PxMatrix[2][1] = -D3dXMatrix._32;
	PxMatrix[2][2] = D3dXMatrix._33;
	PxMatrix[2][3] = -D3dXMatrix._34;

	PxMatrix[3][0] = D3dXMatrix._41;
	PxMatrix[3][1] = D3dXMatrix._42;
	PxMatrix[3][2] = -D3dXMatrix._43;
	PxMatrix[3][3] = D3dXMatrix._44;
	
	return PxMatrix;
}

_float4x4 CUtile::ConvertMatrix_PxToD3D(PxMat44 PxMatrix)
{	
	// Convert the matrix from right-handed to left-handed coordinates
	_float4x4 D3dXMatrix;
	D3dXMatrix._11 = PxMatrix[0][0];
	D3dXMatrix._12 = PxMatrix[0][1];
	D3dXMatrix._13 = -PxMatrix[0][2];
	D3dXMatrix._14 = PxMatrix[0][3];

	D3dXMatrix._21 = PxMatrix[1][0];
	D3dXMatrix._22 = PxMatrix[1][1];
	D3dXMatrix._23 = -PxMatrix[1][2];
	D3dXMatrix._24 = PxMatrix[1][3];

	D3dXMatrix._31 = -PxMatrix[2][0];
	D3dXMatrix._32 = -PxMatrix[2][1];
	D3dXMatrix._33 = PxMatrix[2][2];
	D3dXMatrix._34 = -PxMatrix[2][3];

	D3dXMatrix._41 = PxMatrix[3][0];
	D3dXMatrix._42 = PxMatrix[3][1];
	D3dXMatrix._43 = -PxMatrix[3][2];
	D3dXMatrix._44 = PxMatrix[3][3];

	return D3dXMatrix;
}

wstring CUtile::utf8_to_wstring(const string& str)
{
	int wstr_len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[wstr_len];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, wstr_len);
	wstring result(wstr);
	delete[] wstr;
	return result;
}

string CUtile::wstring_to_utf8(const wstring& wstr)
{
	int str_len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* str = new char[str_len];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str, str_len, NULL, NULL);
	string result(str);
	delete[] str;
	return result;
}

void CUtile::Execute_BillBoard(CTransform* pTransform, _float3 vScale)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	
	_matrix worldmatrix = _smatrix::CreateBillboard(
		pTransform->Get_State(CTransform::STATE_TRANSLATION), 
		pPipeLine->Get_CamPosition_Float3(), 
		pPipeLine->Get_CamUp_Float3(), 
		&pPipeLine->Get_CamLook_Float3());

	pTransform->Set_WorldMatrix(worldmatrix);
	pTransform->Set_Scaled(vScale);
}

void CUtile::Execute_BillBoardOrtho(CTransform * pTransform, _float3 vScale, _float fCamDist)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	//_matrix worldmatrix = _smatrix::CreateBillboard(
	//	pTransform->Get_State(CTransform::STATE_TRANSLATION),
	//	pPipeLine->Get_CamPosition_Float3(),
	//	pPipeLine->Get_CamUp_Float3(),
	//	&pPipeLine->Get_CamLook_Float3());

	pTransform->Set_Look(pPipeLine->Get_CamLook_Float4());

	//_float3 vLook = pTransform->Get_State(CTransform::STATE_LOOK);
	//_float3 vRight = pTransform->Get_State(CTransform::STATE_RIGHT);
	//_matrix worldmatrix = _smatrix::CreateConstrainedBillboard(
	//	pTransform->Get_State(CTransform::STATE_TRANSLATION),
	//	vRight,
	//	pPipeLine->Get_CamRight_Float3(),
	//	&pPipeLine->Get_CamLook_Float3(),
	//	&vLook);

	//pTransform->Set_WorldMatrix(worldmatrix);

	_float fFar = *CGameInstance::GetInstance()->Get_CameraFar();
	_float fNear = *CGameInstance::GetInstance()->Get_CameraNear();
	_float fFov = *CGameInstance::GetInstance()->Get_CameraFov();
	_float fWidth = tanf(fFov*0.5f / fNear);

	_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	_float4 vPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vDist = vPos - vCamPos;
	//_float  fDist = XMVectorGetX(XMVector3Length(vDist));
	_float  fDist = XMVectorGetX(XMVector3Dot(XMLoadFloat4(&vDist), XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamLook_Float4())));

	_smatrix matView = CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4 vViewPos = XMVector3TransformCoord(vPos, matView);

	_float3 vScaleOrtho =
	{
		fWidth / fNear * (fDist/fFar) * vScale.x,
		fWidth / fNear * (fDist/fFar) * vScale.y,
		1.f,
	};
	//_float3 vScaleOrtho = { fNear* vScale.y / fDist , fNear*vScale.y / fDist, fNear*vScale.z / fDist };
	//_float3 vScaleOrtho = { fDist * vScale.y / fNear , fDist*vScale.y / fNear, fDist*vScale.z / fNear };

	pTransform->Set_Scaled(vScaleOrtho);

	//pTransform->Set_Scaled(vScale);
}

_tchar* CUtile::Create_DummyString()
{
	static _uint iIndex = 0;
	static _tchar szBuf[MAX_PATH] = { 0, };

	swprintf_s(szBuf, TEXT("%d"), iIndex++);

	return Create_StringAuto(szBuf);
}

_tchar * CUtile::Create_DummyString(const _tchar * pBaseText, _uint iCount)
{
	_tchar		szBuf[MAX_PATH] = L"";
	swprintf_s(szBuf, L"%s_%d", pBaseText, iCount);	

	return Create_StringAuto(szBuf);
}

