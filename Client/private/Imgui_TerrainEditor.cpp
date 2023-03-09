#include "stdafx.h"
#include "..\public\Imgui_TerrainEditor.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Utile.h"
#include "Texture.h"

#include "Terrain.h"

#include "Json/json.hpp"
#include <fstream>

CImgui_TerrainEditor::CImgui_TerrainEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CImguiObject(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

void CImgui_TerrainEditor::LoadFilterData(string jsonFileName)
{
	string      strLoadDirectory = "../Bin/Data/Terrain/";

	strLoadDirectory += jsonFileName;

	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadTerrain;

	CGameObject*	pLoadTerrain = nullptr;

	file >> jLoadTerrain;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		szLayerTag = "";
	string		szCloneTag;
	string		szDiffuseTag;
	string		szNormalTag;
	string		szFillterTag;

	_tchar*		wszCloneTag = L"";
	_tchar*		wszDiffuseTag = L"";
	_tchar*		wszNormalTag = L"";
	_tchar*		wszFillterTag = L"";
	_int			iDiffuseNum = 0, iFillterOne_TextureNum = 0, iFillterTwo_TextureNum = 0, iFillterThree_TextureNum = 0, iHeightBmpNum = 0;

	jLoadTerrain["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	CGameInstance::GetInstance()->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadTerrain["Terrain_Data"])
	{
		jLoadChild["1_CloneTag"].get_to<string>(szCloneTag);
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		CGameInstance::GetInstance()->Add_String(wszCloneTag);

		jLoadChild["2_DiffuseTag"].get_to<string>(szDiffuseTag);
		wszDiffuseTag = CUtile::StringToWideChar(szDiffuseTag);
		CGameInstance::GetInstance()->Add_String(wszDiffuseTag);

		jLoadChild["3_FillterTag"].get_to<string>(szFillterTag);
		wszFillterTag = CUtile::StringToWideChar(szFillterTag);
		CGameInstance::GetInstance()->Add_String(wszFillterTag);

		jLoadChild["4_NormalTag"].get_to<string>(szNormalTag);
		wszNormalTag = CUtile::StringToWideChar(szNormalTag);
		CGameInstance::GetInstance()->Add_String(wszNormalTag);

		jLoadChild["5_Diffuse_TextureNum"].get_to<int>(iDiffuseNum);
		jLoadChild["6_FillterOne_TextureNum"].get_to<int>(iFillterOne_TextureNum);
		jLoadChild["7_FillterTwo_TextureNum"].get_to<int>(iFillterTwo_TextureNum);
		jLoadChild["8_FillterThree_TextureNum"].get_to<int>(iFillterThree_TextureNum);
		jLoadChild["9_Height_TextureNum"].get_to<int>(iHeightBmpNum);
		CTerrain::tag_TerrainDesc terrainDesc;

		terrainDesc.wstrDiffuseTag = wszDiffuseTag;
		terrainDesc.wstrNormalTag = wszNormalTag;
		terrainDesc.wstrFilterTag = wszFillterTag;
		terrainDesc.iBaseDiffuse = iDiffuseNum;
		terrainDesc.iFillterOne_TextureNum = iFillterOne_TextureNum;
		terrainDesc.iFillterTwo_TextureNum = iFillterTwo_TextureNum;
		terrainDesc.iFillterThree_TextureNum = iFillterThree_TextureNum;
		terrainDesc.iHeightBmpNum = iHeightBmpNum;
		
		float	fElement = 0.f;
		int		k = 0;
		for (float fElement : jLoadChild["10_Transform State"])
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
		}

		if (FAILED(CGameInstance::GetInstance()->Clone_GameObject(g_LEVEL,
			wszLayerTag,
			TEXT("Prototype_GameObject_Terrain"),
			wszCloneTag, &terrainDesc, &pLoadTerrain)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		static_cast<CTransform*>(pLoadTerrain->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		pLoadTerrain->Late_Initialize();	/*머지후 삭제 요망*/

		szCloneTag = "";
		pLoadTerrain = nullptr;

	}

}

HRESULT CImgui_TerrainEditor::Initialize(void * pArg)
{
	Ready_FilterBuffer();

	m_pHeightTexture = static_cast<CTexture*>(m_pGameInstance->
		Clone_Component(LEVEL_MAPTOOL, L"Prototype_Component_Terrain_HeightMaps"));

	assert(m_pHeightTexture != nullptr  &&" CImgui_TerrainEditor::Initialize");

	return S_OK;
}

void CImgui_TerrainEditor::Imgui_FreeRender()
{
	ImGui::Text("<Terrain_Tool>");
	
	if (ImGui::CollapsingHeader("Terrain_Option"))
	{
		Terrain_Selecte();
		Create_Terrain();
		Imgui_FilterControl();
		
		Imgui_Change_HeightBmp();
		Imgui_Save_Load();
		
	
	
	}
	ImGui::End();

}

void CImgui_TerrainEditor::Terrain_Selecte()
{
	m_pSelectedTerrain	=dynamic_cast<CTerrain*>(m_pGameInstance->Get_SelectObjectPtr());

	if (m_pSelectedTerrain == nullptr)
	{
		m_pSelectedTerrain = nullptr;
		return;
	}

	m_pPickingTerrain = m_pSelectedTerrain;

	m_pSelected_Buffer = nullptr;  m_pSelected_Tranform = nullptr; // 초기화

	m_pSelected_Buffer = dynamic_cast<CVIBuffer_Terrain*>(m_pSelectedTerrain->Find_Component(L"Com_VIBuffer"));
	m_pSelected_Tranform = dynamic_cast<CTransform*>(m_pSelectedTerrain->Find_Component(L"Com_Transform"));
	
	assert(nullptr != m_pSelectedTerrain && "CImgui_TerrainEditor::Terrain_Selecte");
	assert(nullptr != m_pSelected_Tranform && "CImgui_TerrainEditor::Terrain_Selecte");


	ImGui::InputInt("Selecte Filter Case", &m_iFilterCaseNum);

	if (m_iFilterCaseNum >= _int(FLTER_END))		// 예외처리
		m_iFilterCaseNum = _int(FLTER_END) - 1;
	
	if (m_iFilterCaseNum < _int(FILTER_FIRST))	// 예외처리
		m_iFilterCaseNum = _int(FILTER_FIRST);


}

void CImgui_TerrainEditor::Imgui_Save_Load()
{
	ImGui::NewLine();  ImGui::Checkbox("Write File ", &m_bSaveWrite);

	if (m_bSaveWrite)
		ImGui::InputText("Save_Name : ", &m_strFileName);

	if (ImGui::Button("Terrain_Save"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Save_Terrain Folder", "Select Save_Terrain Folder", ".json", "../Bin/Save Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Save_Terrain Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Save_Terrain();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}
	ImGui::SameLine();

	if (ImGui::Button("Terrain_Load"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Load_Terrain Folder", "Select Load_Terrain Folder", ".json", "../Bin/Load Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Load_Terrain Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Load_Terrain();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}
	ImGui::NewLine();
	if (ImGui::Button("Confirm_Fillter_Pixels_Save"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Pixel_Save Folder", "Select Pixel_Save Folder", ".Pixeldat", "../Bin/Save Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Pixel_Save Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Imgui_FilterPixel_Save();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}


	ImGui::SameLine();

	if (ImGui::Button("Confirm_Fillter_Pixels_Load"))
		ImGuiFileDialog::Instance()->OpenDialog("Select Pixel Load Folder", "Select Pixel Load Folder", ".Pixeldat", "../Bin/Load Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("Select Pixel Load Folder"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
		{
			Imgui_FilterPixel_Load();
			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
			ImGuiFileDialog::Instance()->Close();
	}
	

}

void CImgui_TerrainEditor::Imgui_FilterPixel_Save()
{
	string      strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
	if (m_bSaveWrite == true)
	{
		char   szDash[128] = "\\";
		strcat_s(szDash, m_strFileName.c_str());
		strSaveDirectory += string(szDash);
		strSaveDirectory += ".Pixeldat";
	}
	else
	{
		string	   strSaveFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
		char   szDash[128] = "\\";
		strcat_s(szDash, strSaveFileName.c_str());
		strSaveDirectory += string(szDash);
	}

	_tchar* wstSaveDirectory = CUtile::StringToWideChar(strSaveDirectory);
	m_pGameInstance->Add_String(wstSaveDirectory);
	HANDLE      hFile = CreateFile(wstSaveDirectory,
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}
	DWORD   dwByte = 0;

	for (_uint i = 0; i < 3; ++i)
	{
		WriteFile(hFile, m_pPixel[i], sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height, &dwByte, nullptr);
	}
	CloseHandle(hFile);
	MSG_BOX("Save_Complete");

}

void CImgui_TerrainEditor::Imgui_FilterPixel_Load()
{
	Clear_Filter_Pixel();

	string      strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
	if (m_bSaveWrite == true)
	{
		char   szDash[128] = "\\";
		strcat_s(szDash, m_strFileName.c_str());
		strSaveDirectory += string(szDash);
		strSaveDirectory += ".Pixeldat";
	}
	else
	{
		string	   strSaveFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
		char   szDash[128] = "\\";
		strcat_s(szDash, strSaveFileName.c_str());
		strSaveDirectory += string(szDash);
	}

	_tchar* wstSaveDirectory = CUtile::StringToWideChar(strSaveDirectory);
	m_pGameInstance->Add_String(wstSaveDirectory);

	HANDLE      hFile = CreateFile(wstSaveDirectory,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}
	DWORD   dwByte = 0;

	for (_uint i = 0; i < 3; ++i)
	{
		ReadFile(hFile, m_pPixel[i], sizeof(_ulong) * 256 * 256, &dwByte, nullptr);
	}
	CloseHandle(hFile);


	m_pSelectedTerrain->Erase_FilterCom();
	for (_uint iPixelIndex = 0; iPixelIndex < 3; ++iPixelIndex)
	{
		for (_uint i = 0; i < 256; ++i)
		{
			for (_uint j = 0; j < 256; ++j)
			{
				_uint iIndex = i * 256 + j;

				if (m_pPixel[iPixelIndex][iIndex] == D3DCOLOR_ARGB(255, 0, 0, 0))
				{
					m_FilterIndexSet[iPixelIndex].insert(_ulong(iIndex));
				}
			}
		}
	
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = 256;
		TextureDesc.Height = 256;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
			assert(!"issue");

		for (auto iter = m_FilterIndexSet[m_iFilterCaseNum].begin(); iter != m_FilterIndexSet[m_iFilterCaseNum].end(); ++iter)
		{
			m_pPixel[m_iFilterCaseNum][(*iter)] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}

		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);

		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
		
		memcpy(SubResource.pData, m_pPixel[m_iFilterCaseNum], (sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height));

		m_pContext->Unmap(pTexture2D, 0);

		


		wstring wstr = TEXT("../Bin/Resources/Terrain_Texture/Filter/");

		if (iPixelIndex == 0)
			wstr += TEXT("Filter_0.dds");
		else if (iPixelIndex == 1)
			wstr += TEXT("Filter_1.dds");
		else if (iPixelIndex == 2)
			wstr += TEXT("Filter_2.dds");

		DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, wstr.c_str());
		Safe_Release(pTexture2D);
	}


	m_pGameInstance->Delete_ProtoComponent(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"));

	m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3));

	m_pSelectedTerrain->Imgui_Tool_Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"));

}

void CImgui_TerrainEditor::Imgui_FilterControl()
{
	ImGui::Checkbox("Create_FilterMap", &m_bFilterStart); ImGui::SameLine();
	ImGui::Checkbox("Eraser_FilterMap", &m_bFilterErase);

	if (m_bFilterStart == true)
		m_bFilterErase = false;

	if (m_bFilterErase == true)
		m_bFilterStart = false;

	Draw_FilterTexture();
	UnDraw_FilterTexture();
}

void CImgui_TerrainEditor::Draw_FilterTexture()
{
	if (m_pSelected_Tranform == nullptr ||  false == m_bFilterStart|| m_pSelected_Buffer == nullptr)
		return;

	_float3 iIndex = _float3(1.f, 1.f, 1.f);

	m_CurFilterIndexSize =m_OldFilterIndexSize = (_int)m_FilterIndexSet[m_iFilterCaseNum].size();
 

	if (ImGui::IsMouseDragging(0))
	{
		if (m_pSelected_Buffer->PickingFilter_Pixel(g_hWnd, m_pSelected_Tranform, iIndex))
		{
			m_FilterIndexSet[m_iFilterCaseNum].insert(_ulong(iIndex.x));
			m_FilterIndexSet[m_iFilterCaseNum].insert(_ulong(iIndex.y));
			m_FilterIndexSet[m_iFilterCaseNum].insert(_ulong(iIndex.z));

			m_CurFilterIndexSize = (_int)m_FilterIndexSet[m_iFilterCaseNum].size();
		}
	}

	if (m_CurFilterIndexSize == m_OldFilterIndexSize)
		return;

	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
		assert(!"issue");

	for (auto iter = m_FilterIndexSet[m_iFilterCaseNum].begin(); iter != m_FilterIndexSet[m_iFilterCaseNum].end(); ++iter)
	{
		m_pPixel[m_iFilterCaseNum][(*iter)] = D3DCOLOR_ARGB(255, 0, 0, 0);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pPixel[m_iFilterCaseNum], (sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height));

	m_pContext->Unmap(pTexture2D, 0);

	m_pSelectedTerrain->Erase_FilterCom();

	wstring wstr = TEXT("../Bin/Resources/Terrain_Texture/Filter/");

	if (m_iFilterCaseNum == 0)
		wstr += TEXT("Filter_0.dds");
	else if(m_iFilterCaseNum == 1)
		wstr += TEXT("Filter_1.dds");
	else if (m_iFilterCaseNum == 2)
		wstr += TEXT("Filter_2.dds");

	 DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, wstr.c_str());
	Safe_Release(pTexture2D);

	m_pGameInstance->Delete_ProtoComponent(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"));

	m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3));

	m_pSelectedTerrain->Imgui_Tool_Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"));
}

void CImgui_TerrainEditor::UnDraw_FilterTexture()
{
	if (m_pSelected_Tranform == nullptr || false == m_bFilterErase || m_pSelected_Buffer == nullptr)
		return;

	_float3 iIndex = _float3(1.f, 1.f, 1.f);

	m_CurFilterIndexSize = m_OldFilterIndexSize = (_int)m_FilterIndexSet[m_iFilterCaseNum].size();

	if (ImGui::IsMouseDragging(0))
	{
		if (m_pSelected_Buffer->PickingFilter_Pixel(g_hWnd, m_pSelected_Tranform, iIndex))
		{
			auto std_SetIter = find_if(m_FilterIndexSet[m_iFilterCaseNum].begin(), m_FilterIndexSet[m_iFilterCaseNum].end(), [&](_ulong Value)->bool
			{
				return Value == iIndex.x;
			});

			if (std_SetIter != m_FilterIndexSet[m_iFilterCaseNum].end())
			{
				m_pPixel[m_iFilterCaseNum][(*std_SetIter)] = D3DCOLOR_ARGB(255, 255, 255, 255);
				m_FilterIndexSet[m_iFilterCaseNum].erase(std_SetIter);
			}
			std_SetIter = find_if(m_FilterIndexSet[m_iFilterCaseNum].begin(), m_FilterIndexSet[m_iFilterCaseNum].end(), [&](_ulong Value)->bool
			{
				return Value == iIndex.y;
			});
			if (std_SetIter != m_FilterIndexSet[m_iFilterCaseNum].end())
			{
				m_pPixel[m_iFilterCaseNum][(*std_SetIter)] = D3DCOLOR_ARGB(255, 255, 255, 255);
				m_FilterIndexSet[m_iFilterCaseNum].erase(std_SetIter);
			}
			std_SetIter = find_if(m_FilterIndexSet[m_iFilterCaseNum].begin(), m_FilterIndexSet[m_iFilterCaseNum].end(), [&](_ulong Value)->bool
			{
				return Value == iIndex.z;
			});
			if (std_SetIter != m_FilterIndexSet[m_iFilterCaseNum].end())
			{
				m_pPixel[m_iFilterCaseNum][(*std_SetIter)] = D3DCOLOR_ARGB(255, 255, 255, 255);
				m_FilterIndexSet[m_iFilterCaseNum].erase(std_SetIter);
			}
		}

		m_CurFilterIndexSize = (_int)m_FilterIndexSet[m_iFilterCaseNum].size();
	}

	if (m_CurFilterIndexSize == m_iFilterCaseNum)
		return;

	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
		assert(!"issue");

	for (auto iter = m_FilterIndexSet[m_iFilterCaseNum].begin(); iter != m_FilterIndexSet[m_iFilterCaseNum].end(); ++iter)
	{
		m_pPixel[m_iFilterCaseNum][(*iter)] = D3DCOLOR_ARGB(0, 0, 0, 0);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pPixel[m_iFilterCaseNum], (sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height));

	m_pContext->Unmap(pTexture2D, 0);

	m_pSelectedTerrain->Erase_FilterCom();

	wstring wstr = TEXT("../Bin/Resources/Terrain_Texture/Filter/");

	if (m_iFilterCaseNum == 0)
		wstr += TEXT("Filter_0.dds");
	else if (m_iFilterCaseNum == 1)
		wstr += TEXT("Filter_1.dds");
	else if (m_iFilterCaseNum == 2)
		wstr += TEXT("Filter_2.dds");

	DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, wstr.c_str());
	Safe_Release(pTexture2D);

	m_pGameInstance->Delete_ProtoComponent(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"));

	m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3));

	m_pSelectedTerrain->Imgui_Tool_Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"));

}

void CImgui_TerrainEditor::Imgui_Change_HeightBmp()
{
	if (m_pSelectedTerrain == nullptr)
		return;

	m_pHeightTexture->Imgui_ImageViewer();

	if (ImGui::Button("Height Chnage"))
	{
		_int iSelctedNum =			m_pHeightTexture->Get_SelectedTextureNum();

		wstring wstrFilePath = TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_");

		wstrFilePath += to_wstring(iSelctedNum);
		wstrFilePath += TEXT(".bmp");

		m_pSelectedTerrain->Change_HeightMap(wstrFilePath.c_str());
	}

}

void CImgui_TerrainEditor::Create_Terrain()
{

	static string			strCloneTag = "";
	ImGui::InputText("Clone Tag", &strCloneTag);

	if (ImGui::Button("Create_Terrain"))
	{
		CGameObject* pCreateObject = nullptr;
	
		_tchar *pCloneName = CUtile::StringToWideChar(strCloneTag);
		CGameInstance::GetInstance()->Add_String(pCloneName);

		if (FAILED(m_pGameInstance->Clone_GameObject(m_pGameInstance->Get_CurLevelIndex(),
			TEXT("Layer_BackGround"),
			TEXT("Prototype_GameObject_Terrain"),
			pCloneName, nullptr, &pCreateObject)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

	}
}

void CImgui_TerrainEditor::Clear_Filter_Pixel()
{
	for (_uint Filter_index = 0; Filter_index < (_uint)FLTER_END; ++Filter_index)
	{
		for (_uint i = 0; i < 256; ++i)
		{
			for (_uint j = 0; j < 256; ++j)
			{
				_uint iIndex = i * 256 + j;
				m_pPixel[Filter_index][iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
		}
		m_FilterIndexSet[Filter_index].clear();
	}


}

void CImgui_TerrainEditor::Save_Terrain()
{
	string      strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
	if (m_bSaveWrite == true)
	{
		char   szDash[128] = "\\";
		strcat_s(szDash, m_strFileName.c_str());
		strSaveDirectory += string(szDash);
		strSaveDirectory += ".json";
	}
	else
	{
		string	   strSaveFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
		char   szDash[128] = "\\";
		strcat_s(szDash, strSaveFileName.c_str());
		strSaveDirectory += string(szDash);
	}

	ofstream      file(strSaveDirectory.c_str());
	Json	jTerrainSave;


	_float4x4	fWroldMatrix;
	_float		fElement = 0.f;
	string		szLayerTag = "Layer_BackGround";
	string		szProtoObjTag = "";
	string		strDiffuseTag = "";
	string		strFillter = "";
	string		strNormal = "";

	wstring			wstrCloneTag = L"";
	string		szCloneTag;
	wstring		wstrDiffuseTag = L"";
	wstring		wstrFillterTag = L"";
	wstring		wstrNormalTag = L"";
	_int			iDiffuseNum = 0, iFillterOne_TextureNum = 0, iFillterTwo_TextureNum = 0, iFillterThree_TextureNum = 0, iHeightBmpNum = 0;
	CTerrain::TERRAIN_DESC* SaveDesc = nullptr;

	jTerrainSave["0_LayerTag"] = szLayerTag;

	for (auto& pObject : m_pGameInstance->Find_Layer(CGameInstance::GetInstance()->Get_CurLevelIndex(), L"Layer_BackGround")->GetGameObjects())
	{
		if (dynamic_cast<CTerrain*>(pObject.second) == nullptr)
			continue;

		SaveDesc = dynamic_cast<CTerrain*>(pObject.second)->Get_TerrainDesc();

		wstrDiffuseTag = SaveDesc->wstrDiffuseTag;
		wstrFillterTag = SaveDesc->wstrFilterTag;
		wstrNormalTag = SaveDesc->wstrNormalTag;
		iDiffuseNum = SaveDesc->iBaseDiffuse;
		iFillterOne_TextureNum = SaveDesc->iFillterOne_TextureNum;
		iFillterTwo_TextureNum = SaveDesc->iFillterTwo_TextureNum;
		iFillterThree_TextureNum = SaveDesc->iFillterThree_TextureNum;
		iHeightBmpNum = SaveDesc->iHeightBmpNum;
		Json jChild;

		wstrCloneTag =(pObject.second->Get_ObjectCloneName());
		szCloneTag =	CUtile::WstringToString(wstrCloneTag);
		jChild["1_CloneTag"] = szCloneTag;
		strDiffuseTag = CUtile::WstringToString(wstrDiffuseTag);
		jChild["2_DiffuseTag"] = strDiffuseTag;
		strFillter = CUtile::WstringToString(wstrFillterTag);
		jChild["3_FillterTag"] = strFillter;
		strNormal = CUtile::WstringToString(wstrNormalTag);
		jChild["4_NormalTag"] = strNormal;
		jChild["5_Diffuse_TextureNum"] = iDiffuseNum;
		jChild["6_FillterOne_TextureNum"] = iFillterOne_TextureNum;
		jChild["7_FillterTwo_TextureNum"] = iFillterTwo_TextureNum;
		jChild["8_FillterThree_TextureNum"] = iFillterThree_TextureNum;
		jChild["9_Height_TextureNum"] = iHeightBmpNum;

		CTransform* pTransform = static_cast<CTransform*>(pObject.second->Find_Component(L"Com_Transform"));
		assert(pTransform != nullptr && "CImgui_MapEditor::Imgui_Save_Func()");
		XMStoreFloat4x4(&fWroldMatrix, pTransform->Get_WorldMatrix());

		for (int i = 0; i < 16; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&fWroldMatrix + i, sizeof(float));
			jChild["10_Transform State"].push_back(fElement);		// 배열 저장. 컨테이너의 구조랑 비슷합니다. 이렇게 하면 Transform State에는 16개의 float 값이 저장됩니다.
		}

		jTerrainSave["Terrain_Data"].push_back(jChild);
	}

	file << jTerrainSave;
	file.close();
	MSG_BOX("Save_jTerrain");
}

void CImgui_TerrainEditor::Load_Terrain()
{
	string      strLoadDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();   // GetCurrentPath F12로 들가면 비슷한 다른 함수 더 있음.
	string	   strLoadFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
	char   szDash[128] = "\\";
	strcat_s(szDash, strLoadFileName.c_str());
	strLoadDirectory += string(szDash);
	
	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadTerrain;

	CGameObject*	pLoadTerrain = nullptr;

	file >> jLoadTerrain;
	file.close();

	_float4x4	fWroldMatrix;
	_tchar*		wszLayerTag = L"";
	string		szLayerTag = "";
	string		szCloneTag;
	string		szDiffuseTag;
	string		szNormalTag;
	string		szFillterTag;

	_tchar*		wszCloneTag = L"";
	_tchar*		wszDiffuseTag = L"";
	_tchar*		wszNormalTag = L"";
	_tchar*		wszFillterTag = L"";
	_int			iDiffuseNum = 0, iFillterOne_TextureNum = 0, iFillterTwo_TextureNum = 0, iFillterThree_TextureNum = 0, iHeightBmpNum = 0;

	jLoadTerrain["0_LayerTag"].get_to<string>(szLayerTag);
	wszLayerTag = CUtile::StringToWideChar(szLayerTag);
	m_pGameInstance->Add_String(wszLayerTag);

	for (auto jLoadChild : jLoadTerrain["Terrain_Data"])
	{
		jLoadChild["1_CloneTag"].get_to<string>(szCloneTag);
		wszCloneTag = CUtile::StringToWideChar(szCloneTag);
		m_pGameInstance->Add_String(wszCloneTag);
	
		jLoadChild["2_DiffuseTag"].get_to<string>(szDiffuseTag);
		wszDiffuseTag = CUtile::StringToWideChar(szDiffuseTag);
		m_pGameInstance->Add_String(wszDiffuseTag);
		
		jLoadChild["3_FillterTag"].get_to<string>(szFillterTag);
		wszFillterTag = CUtile::StringToWideChar(szFillterTag);
		m_pGameInstance->Add_String(wszFillterTag);
		
		jLoadChild["4_NormalTag"].get_to<string>(szNormalTag);
		wszNormalTag = CUtile::StringToWideChar(szNormalTag);
		m_pGameInstance->Add_String(wszNormalTag);

		jLoadChild["5_Diffuse_TextureNum"].get_to<int>(iDiffuseNum);
		jLoadChild["6_FillterOne_TextureNum"].get_to<int>(iFillterOne_TextureNum);
		jLoadChild["7_FillterTwo_TextureNum"].get_to<int>(iFillterTwo_TextureNum);
		jLoadChild["8_FillterThree_TextureNum"].get_to<int>(iFillterThree_TextureNum);
		jLoadChild["9_Height_TextureNum"].get_to<int>(iHeightBmpNum);
		CTerrain::tag_TerrainDesc terrainDesc;

		terrainDesc.wstrDiffuseTag = wszDiffuseTag;
		terrainDesc.wstrNormalTag = wszNormalTag;
		terrainDesc.wstrFilterTag = wszFillterTag;
		terrainDesc.iBaseDiffuse = iDiffuseNum;
		terrainDesc.iFillterOne_TextureNum = iFillterOne_TextureNum;
		terrainDesc.iFillterTwo_TextureNum = iFillterTwo_TextureNum;
		terrainDesc.iFillterThree_TextureNum = iFillterThree_TextureNum;
		terrainDesc.iHeightBmpNum = iHeightBmpNum;

		float	fElement = 0.f;
		int		k = 0;
		for (float fElement : jLoadChild["10_Transform State"])
		{
			memcpy(((float*)&fWroldMatrix) + (k++), &fElement, sizeof(float));
		}

		if (FAILED(m_pGameInstance->Clone_GameObject(g_LEVEL,
			wszLayerTag,
			TEXT("Prototype_GameObject_Terrain"),
			wszCloneTag, &terrainDesc, &pLoadTerrain)))
			assert(!"CImgui_MapEditor::Imgui_CreateEnviromentObj");

		static_cast<CTransform*>(pLoadTerrain->Find_Component(L"Com_Transform"))->Set_WorldMatrix_float4x4(fWroldMatrix);
		pLoadTerrain->Late_Initialize();	/*머지후 삭제 요망*/

		szCloneTag = "";
		pLoadTerrain = nullptr;

	}

}

void CImgui_TerrainEditor::Ready_FilterBuffer()
{
	for (_uint Filter_index = 0; Filter_index < (_uint)FLTER_END; ++Filter_index)
	{
		m_pPixel[Filter_index] = new _ulong[256 * 256];
		for (_uint i = 0; i < 256; ++i)
		{
			for (_uint j = 0; j < 256; ++j)
			{
				_uint iIndex = i * 256 + j;
				m_pPixel[Filter_index][iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
		}
	}
}

CImgui_TerrainEditor * CImgui_TerrainEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CImgui_TerrainEditor*	pInstance = new CImgui_TerrainEditor(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CImgui_TerrainEditor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_TerrainEditor::Free()
{
	__super::Free();
	for (_uint i = 0; i < (_uint)FLTER_END; ++i)
	{
		Safe_Delete_Array(m_pPixel[i]);
	}

	for (_uint i = 0; i < (_uint)FLTER_END; ++i)
	{
		m_FilterIndexSet[i].clear();
	}

	Safe_Release(pTexture2D);
	Safe_Release(m_pHeightTexture);
}