#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {
		RENDER_PRIORITY,
		RENDER_STATIC_SHADOW,
		RENDER_SHADOW,
		RENDER_REFLECT,
		RENDER_NONALPHABLEND,
		RENDER_NONLIGHT,
		RENDER_ALPHABLEND,
		RENDER_EFFECT,
		RENDER_UI,
		RENDER_UILAST,		
		RENDER_VIEWER,
		RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Initialize_ShadowResources(_uint iWidth, _uint iHeight);
	virtual HRESULT Initialize_ReflectResources(_uint iWidth, _uint iHeight);

	HRESULT ReCompile();
	void			ShootStaticShadow() { m_bStaticShadow = true; };
	void			Imgui_Render();

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);

#ifdef _DEBUG
	HRESULT Add_DebugRenderGroup(class CComponent* pComponent);
	_bool								m_bDebugRender = false;
#endif

	HRESULT Draw_RenderGroup();

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

private:
#ifdef _DEBUG
	list<class CComponent*>				m_DebugObject;
	typedef list<class CComponent*>		DEBUGOBJECTS;
#endif
private:
	class CTarget_Manager*			m_pTarget_Manager = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;
	class CLevel_Manager*			m_pLevel_Manager = nullptr;
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	class CShader*							m_pShader = nullptr;
	class CShader*							m_pShader_PostProcess = nullptr;
	class CShader*							m_pShader_SSAO = nullptr;
	_float4x4									m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	ID3D11DepthStencilView*			m_pShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView*			m_pStaticShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView*			m_pReflectDepthStencilView = nullptr;

	_uint											m_iShadowWidth = 0, m_iShadowHeight = 0;
	_bool										m_bPhysXRenderFlag = false;
	_bool										m_bStaticShadow = false;

	_bool										m_bDynamicShadow = false;
	_bool										m_bSSAO = true;

	_float										m_fDistortTime = 0.f;

private:
	HRESULT Render_StaticShadow();
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_Reflect();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_SSAO();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_HDR();
	HRESULT Render_PostProcess();
	HRESULT Render_Effect();
	HRESULT Render_UI();
	HRESULT Render_UILast();
	HRESULT Render_Viewer();		// Model Viewer

	HRESULT PostProcess_Distort();
	_bool		m_bDistort = true;
	HRESULT PostProcess_GrayScale();
	_bool		m_bGrayScale = false;

#ifdef _DEBUG
private:
	HRESULT Render_DebugObject();
#endif

public:
	ID3D11ShaderResourceView*		Get_LDRTexture() { return m_pLDRTexture; }

private:
	ID3D11ShaderResourceView*		m_pLDRTexture = nullptr;

private:
	HRESULT CreateTexture(const _tchar* pTextureFilePath, ID3D11ShaderResourceView**& OUT pTexture);
	ID3D11ShaderResourceView** 	m_pFlareTexture = nullptr;
	ID3D11ShaderResourceView** 	m_pDistortionTexture = nullptr;
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRenderer* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	void Set_PhysXRender(_bool bFlag) { m_bPhysXRenderFlag = bFlag; }
};

END