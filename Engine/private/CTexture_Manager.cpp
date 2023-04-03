#include "stdafx.h"
#include "..\public\CTexture_Manager.h"
#include "Texture.h"

IMPLEMENT_SINGLETON(CTexture_Manager)

CTexture_Manager::CTexture_Manager()
{
}

HRESULT CTexture_Manager::Add_Texture(const _tchar* pTextureFilePath, class CTexture* p_teuxtre)
{
	m_Textures.emplace(pTextureFilePath, p_teuxtre);
	return S_OK;
}

CTexture* CTexture_Manager::Find_Texture(const _tchar* pFilePath)
{
	auto	iter = find_if(m_Textures.begin(), m_Textures.end(), CTag_Finder(pFilePath));
	if (iter == m_Textures.end())
		return nullptr;
	return iter->second;
}

void CTexture_Manager::Free()
{
	m_Textures.clear();
}
