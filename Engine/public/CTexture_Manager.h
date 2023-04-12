#pragma once

#include "Base.h"

BEGIN(Engine)

class CTexture_Manager final : public CBase
{
	DECLARE_SINGLETON(CTexture_Manager)
private:
	CTexture_Manager();
	virtual ~CTexture_Manager() = default;

public:
	HRESULT Add_Texture(const _tchar* pTextureFilePath, class CTexture* p_teuxtre);
	class CTexture* Find_Texture(const _tchar* pFilePath);

	HRESULT Scene_Chane_Clear();
	HRESULT Texture_Erase(const _tchar* pTextureFilePath);
private:
	map<const _tchar*, class CTexture*>			m_Textures;

public:
	virtual void Free() override;
};

END