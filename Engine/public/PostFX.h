#pragma once
#include "Base.h"

BEGIN(Engine)

class CPostFX : public CBase
{
	DECLARE_SINGLETON(CPostFX)
private:
	CPostFX();
	virtual	~CPostFX() = default;

public:
	HRESULT Initilaize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Clear();

	// Entry Point for Post Processing
	void	PostProcessing(ID3D11ShaderResourceView* pHDRSRV, ID3D11RenderTargetView* pLDRRTV);
	void	SetParameter(_float fMiddleGrey, _float fWhite);

private:
	void	DownScale(ID3D11ShaderResourceView* pHDRSRV);
	void	FinalPass(ID3D11ShaderResourceView* pHDRSRV);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	// Downscaled scene texture
	ID3D11Texture2D* m_pDownScaleRT = nullptr;
	ID3D11ShaderResourceView* m_pDownScaleSRV = nullptr;
	ID3D11UnorderedAccessView* m_pDownScaleUAV = nullptr;

	// Temporary texture
	ID3D11Texture2D* m_pTempRT[2]{ nullptr, nullptr };
	ID3D11ShaderResourceView* m_pTempSRV[2]{ nullptr, nullptr };
	ID3D11UnorderedAccessView* m_pTempUAV[2]{ nullptr, nullptr };

	// Bloom texture
	ID3D11Texture2D* m_pBloomRT = nullptr;
	ID3D11ShaderResourceView* m_pBloomSRV = nullptr;
	ID3D11UnorderedAccessView* m_pBloomUAV = nullptr;

	// 1D intermediate storage for the down scale operation
	ID3D11Buffer*				m_pDownScale1DBuffer = nullptr;
	ID3D11UnorderedAccessView*  m_pDownScale1DUAV = nullptr;
	ID3D11ShaderResourceView*   m_pDownScale1DSRV = nullptr;

	// Average luminance
	ID3D11Buffer*				m_pAvgLumBuffer = nullptr;
	ID3D11UnorderedAccessView*	m_pAvgLumUAV = nullptr;
	ID3D11ShaderResourceView*	m_pAvgLumSRV = nullptr;

	_uint						m_iWidth = 0;
	_uint						m_iHeight = 0;
	_uint						m_iDownScaleGroups = 0;
	_float						m_fMiddleGrey = 0.f;
	_float						m_fWhite = 0.f;
	_float						m_fAdaptation = 0.f;
	_float						m_fBloomThreshold = 0.f;
	_float						m_fBloomScale = 0.f;

	typedef struct
	{
		UINT nWidth;
		UINT nHeight;
		UINT nTotalPixels;
		UINT nGroupSize;
		float fAdaptation;
		float fBloomThreshold;
		UINT pad[2];
	} TDownScaleCB;
	ID3D11Buffer* m_pDownScaleCB = nullptr;

	typedef struct
	{
		float fMiddleGrey;
		float fLumWhiteSqr;
		float fBloomScale;
		UINT pad;
	} TFinalPassCB;
	ID3D11Buffer* m_pFinalPassCB = nullptr;

	typedef struct
	{
		UINT numApproxPasses;
		float fHalfBoxFilterWidth;			// w/2
		float fFracHalfBoxFilterWidth;		// frac(w/2+0.5)
		float fInvFracHalfBoxFilterWidth;	// 1-frac(w/2+0.5)
		float fRcpBoxFilterWidth;			// 1/w
		UINT pad[3];
	} TBlurCB;
	ID3D11Buffer* m_pBlurCB = nullptr;

	// Shaders
	ID3D11ComputeShader* m_pBloomRevealCS = nullptr;
	ID3D11ComputeShader* m_HorizontalBlurCS = nullptr;
	ID3D11ComputeShader* m_VerticalBlurCS = nullptr;

	ID3D11ComputeShader* m_pDownScaleFirstPassCS = nullptr;
	ID3D11ComputeShader* m_pDownScaleSecondPassCS = nullptr;
	ID3D11VertexShader* m_pFullScreenQuadVS = nullptr;
	ID3D11PixelShader* m_pFinalPassPS = nullptr;

	ID3D11SamplerState* m_pSampPoint = nullptr;
	ID3D11SamplerState* m_pSampLinear = nullptr;

public:
	virtual void Free() override;
};

END