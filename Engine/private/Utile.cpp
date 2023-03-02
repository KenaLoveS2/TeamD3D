#include "..\public\Utile.h"

#include "Texture.h"
#include "Json/json.hpp"
#include <fstream>
#include "GameInstance.h"

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

void  CUtile::MODELMATERIAL_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,
	const char* jSonPath,vector<MODELMATERIAL>& Vec)
{
	///*json 필요한 데이터  , 파일 패쓰 , 파일 */
	//_uint iNumTextures = 0;

	ifstream      file(jSonPath);
	Json			  jLoadMatrial;
	file >> jLoadMatrial;
	file.close();

	int				iMeshNum = 0;
	jLoadMatrial["0_MeshNum"].get_to<int>(iMeshNum);


	string FilePath = "";

	_tchar* pTextureFilePath = L""; 
	

	for (int i = 0; i < iMeshNum; ++i)
	{
		MODELMATERIAL ModelMatrial;
		if (i == 0)
		{
			for (auto jLoadChild : jLoadMatrial["1_First"])
			{
				jLoadChild["0_None_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NONE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NONE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["1_DIFFUSE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_DIFFUSE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_DIFFUSE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["2_SPECULAR_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPECULAR] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPECULAR] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["3_AMBIENT_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["4_EMISSIVE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["5_EMISSIVEMASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVEMASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVEMASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["6_NORMALS_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NORMALS] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NORMALS] = nullptr;


				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["7_MASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_MASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_MASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["8_SSS_MASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SSS_MASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SSS_MASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["9_SPRINT_EMISSIVE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPRINT_EMISSIVE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPRINT_EMISSIVE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["10_HAIR_DEPTH_Path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_DEPTH] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_DEPTH] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["11_ALPHA_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ALPHA] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ALPHA] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["12_HAIR_ROOT_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_ROOT] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_ROOT] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["13_COMP_MSK_CURV_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_MSK_CURV] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_MSK_CURV] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["14_COMP_H_R_AO_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_H_R_AO] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_H_R_AO] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["15_COMP_E_R_AO_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_E_R_AO] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_E_R_AO] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["16_ROUGHNESS_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ROUGHNESS] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ROUGHNESS] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["17_AMBIENT_OCCLUSION_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT_OCCLUSION] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT_OCCLUSION] = nullptr;

				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
			}
		}
		else if (i == 1)
		{
			for (auto jLoadChild : jLoadMatrial["2_Second"])
			{
				FilePath = "";
				pTextureFilePath = L"";
				jLoadChild["0_None_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NONE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NONE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["1_DIFFUSE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_DIFFUSE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_DIFFUSE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["2_SPECULAR_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPECULAR] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPECULAR] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["3_AMBIENT_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["4_EMISSIVE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["5_EMISSIVEMASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVEMASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVEMASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["6_NORMALS_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NORMALS] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NORMALS] = nullptr;


				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["7_MASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_MASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_MASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["8_SSS_MASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SSS_MASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SSS_MASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["9_SPRINT_EMISSIVE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPRINT_EMISSIVE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPRINT_EMISSIVE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["10_HAIR_DEPTH_Path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_DEPTH] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_DEPTH] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["11_ALPHA_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ALPHA] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ALPHA] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["12_HAIR_ROOT_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_ROOT] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_ROOT] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["13_COMP_MSK_CURV_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_MSK_CURV] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_MSK_CURV] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["14_COMP_H_R_AO_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_H_R_AO] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_H_R_AO] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["15_COMP_E_R_AO_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_E_R_AO] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_E_R_AO] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["16_ROUGHNESS_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ROUGHNESS] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ROUGHNESS] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["17_AMBIENT_OCCLUSION_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT_OCCLUSION] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT_OCCLUSION] = nullptr;

				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
			}
		}
		else if (i == 2)
		{
			for (auto jLoadChild : jLoadMatrial["3_Third"])
			{
				FilePath = "";
				pTextureFilePath = L"";
				jLoadChild["0_None_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NONE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NONE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["1_DIFFUSE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_DIFFUSE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_DIFFUSE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["2_SPECULAR_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPECULAR] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPECULAR] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["3_AMBIENT_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["4_EMISSIVE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["5_EMISSIVEMASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVEMASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_EMISSIVEMASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["6_NORMALS_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NORMALS] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_NORMALS] = nullptr;


				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["7_MASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_MASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_MASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["8_SSS_MASK_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SSS_MASK] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SSS_MASK] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["9_SPRINT_EMISSIVE_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPRINT_EMISSIVE] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_SPRINT_EMISSIVE] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["10_HAIR_DEPTH_Path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_DEPTH] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_DEPTH] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["11_ALPHA_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ALPHA] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ALPHA] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["12_HAIR_ROOT_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_ROOT] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_HAIR_ROOT] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["13_COMP_MSK_CURV_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_MSK_CURV] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_MSK_CURV] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["14_COMP_H_R_AO_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_H_R_AO] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_H_R_AO] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["15_COMP_E_R_AO_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_E_R_AO] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_COMP_E_R_AO] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["16_ROUGHNESS_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ROUGHNESS] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_ROUGHNESS] = nullptr;

				FilePath = "";
				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
				jLoadChild["17_AMBIENT_OCCLUSION_path"].get_to<string>(FilePath);
				pTextureFilePath = CUtile::StringToWideChar(FilePath);
				if (lstrcmp(pTextureFilePath, L"NONE"))
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT_OCCLUSION] = CTexture::Create(pDevice, pContext, pTextureFilePath, 1);
				else
					ModelMatrial.pTexture[aiTextureType::WJTextureType_AMBIENT_OCCLUSION] = nullptr;

				Safe_Delete_Array(pTextureFilePath);
				pTextureFilePath = L"";
			}
		}
		Vec.push_back(ModelMatrial);
	}


}