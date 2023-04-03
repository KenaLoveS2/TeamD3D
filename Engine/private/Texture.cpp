#include "stdafx.h"
#include "..\public\Texture.h"
#include "Shader.h"
#include "Utile.h"
#include "CTexture_Manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_pTextures(rhs.m_pTextures)
	, m_iNumTextures(rhs.m_iNumTextures)
{
	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_AddRef(m_pTextures[i]);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures, _bool bddsLoad)
{
	m_pTextures = new ID3D11ShaderResourceView*[iNumTextures];

	m_wstrFilePath = pTextureFilePath;
	m_iNumTextures = iNumTextures;
	
	for (_uint i = 0; i < m_iNumTextures; ++i)
	{
		_tchar	szTexturePath[MAX_PATH] = TEXT("");

		wsprintf(szTexturePath, pTextureFilePath, i);

		/* 문자열 분해하기. */
		_tchar			szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT		hr = 0;

		/*	if (bddsLoad == true)
			{
				_tchar	szDirectory[MAX_PATH] = TEXT("");
				_tchar	szFileName[MAX_PATH] = TEXT("");
				_tchar	szdds[MAX_PATH] = TEXT(".dds");

				_wsplitpath_s(pTextureFilePath, nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr,0);

				lstrcat(szDirectory, szFileName);
				lstrcat(szDirectory, szdds);

				hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szDirectory, nullptr, &m_pTextures[i]);

				if (hr == E_FAIL)
					return E_FAIL;
				return hr;
			}*/

		if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;
		else if (!lstrcmp(szExt, TEXT(".dds")))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);
		else if (!lstrcmp(szExt, TEXT(".hdr")))
		{
			int width, height, channels;
			char* pTexturePath = new char[MAX_PATH];
			CUtile::WideCharToChar(szTexturePath, pTexturePath);
			float* pData = stbi_loadf(pTexturePath, &width, &height, &channels, STBI_rgb_alpha);
			if (pData != nullptr)
			{
				D3D11_TEXTURE2D_DESC texDesc = {};
				texDesc.Width = width;
				texDesc.Height = height;
				texDesc.MipLevels = 1;
				texDesc.ArraySize = 1;
				texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				texDesc.SampleDesc.Count = 1;
				texDesc.Usage = D3D11_USAGE_DEFAULT;
				texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				D3D11_SUBRESOURCE_DATA initData = {};
				initData.pSysMem = pData;
				initData.SysMemPitch = width * sizeof(float) * 4;

				ID3D11Texture2D* pTexture = nullptr;
				ID3D11ShaderResourceView* pSRV = nullptr;
				hr = m_pDevice->CreateTexture2D(&texDesc, &initData, &pTexture);
				if (SUCCEEDED(hr))
				{
					hr = m_pDevice->CreateShaderResourceView(pTexture, nullptr, &pSRV);
					if (SUCCEEDED(hr))
					{
						m_pTextures[i] = pSRV;
					}
					pTexture->Release();
				}

				stbi_image_free(pData);
				Safe_Delete_Array(pTexturePath);
			}
		}
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);

		if (FAILED(hr))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

HRESULT CTexture::Bind_ShaderResources(CShader * pShaderCom, const char * pConstantName)
{
	return pShaderCom->Set_ShaderResourceViewArray(pConstantName, m_pTextures, m_iNumTextures);
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pShaderCom || 
		iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShaderCom->Set_ShaderResourceView(pConstantName, m_pTextures[iTextureIndex]);	
}

void CTexture::Imgui_ImageViewer()
{
	if (ImGui::CollapsingHeader("Select HeightBmp", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (_uint i = 0; i < m_iNumTextures; ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextures[i], ImVec2(32.f, 32.f)))
			{
				m_iSelectedTextureNum = i;
			}
			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}
	
		ImGui::Text(" Selected Number : %d", m_iSelectedTextureNum);
	}
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures, _bool bddsLoad)
{
	// 여기서 pTextureFilePath 가 있다면 클론
	// 없으면 만들고 맵에 저장
	CTexture* pInstance = CTexture_Manager::GetInstance()->Find_Texture(pTextureFilePath);

	if (pInstance)
	{
		pInstance = (CTexture*)pInstance->Clone();
		pInstance->Set_Clone(true);
		Safe_AddRef(pInstance);
		return  pInstance;
	}
	else
	{
		pInstance = new CTexture(pDevice, pContext);

		if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures, bddsLoad)))
		{
			MSG_BOX("Failed to Created : CTexture");
			Safe_Release(pInstance);
		}
		CTexture_Manager::GetInstance()->Add_Texture(pTextureFilePath, pInstance);
		return pInstance;
	}
}

CComponent * CTexture::Clone(void * pArg, CGameObject * pOwner)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
 	__super::Free();

	if(!m_bCloneTexture)
	{
		for (_uint i = 0; i < m_iNumTextures; ++i)
			Safe_Release(m_pTextures[i]);
	}

	if(false == m_isCloned)
		Safe_Delete_Array(m_pTextures);
}
