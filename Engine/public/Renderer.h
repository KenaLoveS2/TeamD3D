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
		RENDER_CINE,
		RENDER_NONALPHABLEND,
		RENDER_NONLIGHT,
		RENDER_ALPHABLEND,
		RENDER_UIHDR,
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
	virtual HRESULT Initialize_CineResources(_uint iWidth, _uint iHeight);

	HRESULT ReCompile();
	void			ShootStaticShadow() { m_bStaticShadow = true; };
	void			Imgui_Render();


	void			EraseStaticShadowObject(class CGameObject* pObject);
	void			EraseAllStaticShadowObject();
public:
	HRESULT			Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	void			Set_Flare(_bool bFlare) { m_bFlare = bFlare; }
	void			Set_MotionBlur(_bool bBlur) { m_bMotionBlur = bBlur; }
	void			Set_Fog(bool bFog) { m_bFog = bFog; }
	void			Set_FogValue(_float4 vColor, _float fFogRange) { m_vFogColor = vColor, m_fFogRange = fFogRange; }

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
	list<class CComponent*>					m_DebugObject;
	typedef list<class CComponent*>		DEBUGOBJECTS;
#endif
private:
	class CTarget_Manager*		m_pTarget_Manager = nullptr;
	class CLight_Manager*			m_pLight_Manager = nullptr;
	class CLevel_Manager*		m_pLevel_Manager = nullptr;
	class CVIBuffer_Rect*			m_pVIBuffer = nullptr;
	class CShader*						m_pShader = nullptr;
	class CShader*						m_pShader_PostProcess = nullptr;
	class CShader*						m_pShader_SSAO = nullptr;
	_float4x4								m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	ID3D11DepthStencilView*			m_pShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView*			m_pStaticShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView*			m_pCineDepthStencilView = nullptr;

	_uint								m_iShadowWidth = 0, m_iShadowHeight = 0;
	_bool							m_bPhysXRenderFlag = false;
	_bool							m_bStaticShadow = false;
	_bool							m_bDynamicShadow = false;
	_bool							m_bSSAO = true;

	_float							m_fDistortTime = 0.f;
	_float							m_fPrevCaptureTime = 0.f;
	_bool							m_bCine = false;

private:
	void Increase_Time();

	HRESULT Render_StaticShadow();
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_SSAO();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UIHDR();
	HRESULT Render_HDR();
	HRESULT Render_PostProcess();
	HRESULT Render_Effect();
	HRESULT Render_UI();
	HRESULT Render_UILast();
	HRESULT Render_Viewer();		// Model Viewer
	HRESULT Render_PrevFrame();
	

	HRESULT PostProcess_Distort();
	_bool		m_bDistort = true;
	HRESULT PostProcess_GrayScale();
	_bool		m_bGrayScale = false;
	HRESULT PostProcess_MotionBlur();
	_bool		m_bMotionBlur = false;
	HRESULT PostProcess_Flare();
	_bool		m_bFlare = false;
	HRESULT PostProcess_LightShaft();
	_bool		m_bLightShaft = false;

	_bool		m_bFog = false;
	_float4		m_vFogColor;
	_float		m_fFogStart = 0.f;
	_float		m_fFogRange = 30.f;

private:
	HRESULT Render_DebugObject();
	HRESULT Render_Cine();

public:
	ID3D11ShaderResourceView*		Get_LDRTexture() { return m_pLDRTexture; }
	const _bool&		Get_Fog() { return m_bFog; }
	const _float4& Get_FogColor() { return m_vFogColor; }
	void Set_CaptureMode(_bool bCapture)
	{
		m_bCaptureMode = bCapture;
		m_bBlurCapture = false;
	}

private:
	ID3D11ShaderResourceView*		m_pLDRTexture = nullptr;
	_bool											m_bCaptureMode = false;
	_bool											m_bBlurCapture = false;
	_float4											m_vLightShaftValue = _float4(0.3f, 0.95f,0.5f,0.5f);

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