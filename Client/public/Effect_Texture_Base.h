#pragma once
#include "Effect_Base_S2.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Texture_Base final : public CEffect_Base_S2
{
public:
	enum	TEXTURE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_END };
private:
	CEffect_Texture_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Texture_Base(const CEffect_Texture_Base& rhs);
	virtual ~CEffect_Texture_Base() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg = nullptr) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual void					Imgui_RenderProperty() override;

public:
	virtual HRESULT					Save_Data() override;
	virtual HRESULT					Load_Data(_tchar* fileName) override;
	virtual	void					BackToNormal() override;
public:
	virtual	void					Activate(_float4 vPos)									override;
	virtual	void					Activate(CGameObject* pTarget)							override;
	virtual void					Activate(CGameObject* pTarget, _float2 vScaleSpeed)		override;
	virtual void					Activate(_float4 vPos, _float2 vScaleSpeed)				override;

	virtual void					DeActivate()					override;

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();
	HRESULT							SetUp_TextureInfo();

private: /* Tool Function */
	void							ColorCode(_int iTextureType);

private:
	CShader*						m_pShaderCom;
	CRenderer*						m_pRendererCom;
	CTexture*						m_pTextureCom[TEXTURE_END];
	CVIBuffer_Rect*					m_pVIBufferCom;

private:
	char*							m_ShaderVarName[TEXTURE_END];
	_tchar*							m_TextureComName[TEXTURE_END];
	_int							m_iTextureIndices[TEXTURE_END];
	char*							m_ShaderColorName[TEXTURE_END];
	_float4							m_vTextureColors[TEXTURE_END];

public:
	static CEffect_Texture_Base*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free()			override;
};
END