#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
protected:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	_uint											    Get_TextureIdx() { return m_iNumTextures; }
	ID3D11ShaderResourceView*      Get_Texture(_uint iTextureIdx = 0) const { return m_pTextures[iTextureIdx]; }

	const	_int	Get_SelectedTextureNum()const { return m_iSelectedTextureNum; }


public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures, _bool bddsLoad);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;

public:	
	HRESULT Bind_ShaderResources(class CShader* pShaderCom, const char* pConstantName);
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);

public:
	void			Imgui_ImageViewer();

private:
	_uint															m_iNumTextures = 0;
	ID3D11ShaderResourceView**					m_pTextures = nullptr;
	_int															m_iSelectedTextureNum = -1;
	_bool														m_bCloneTexture = false;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1,_bool bddsLoad=false);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END