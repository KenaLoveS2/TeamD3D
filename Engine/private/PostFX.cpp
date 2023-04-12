#include "stdafx.h"
#include "..\public\PostFX.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPostFX)

CPostFX::CPostFX()
{
	m_bOn = true;
	m_fMiddleGrey = 10.f;
	m_fWhite = 2.f;
	m_fBloomThreshold = 0.8f;
	m_fBloomScale = 0.4f;
	m_fAdaptation = 0.5f;
	m_fDOFFarStart = 10000.f;
	m_fPreDOFFarStart = 10000.f;
	m_fDOFFarRange = 0.5f;
	m_fBokehBlurThreshold = 0.5f;
	m_fBokehLumThreshold = 0.5f;
	m_fBokehColorScale = 1.f;
	m_fBokehRadiusScale = 1.f;
}

HRESULT CPostFX::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	Clear();
	if (m_pDevice == nullptr)
	{
		m_pDevice = pDevice;
		Safe_AddRef(m_pDevice);
	}

	if (m_pContext == nullptr)
	{
		m_pContext = pContext;
		Safe_AddRef(m_pContext);
	}

	if (m_pDevice == nullptr || m_pContext == nullptr)
		return E_FAIL;

	D3D11_VIEWPORT	ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(ViewportDesc));

	_uint	iNumViewPorts = 1;

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	m_iWidth = static_cast<_uint>(ViewportDesc.Width);
	m_iHeight = static_cast<_uint>(ViewportDesc.Height);
	m_iDownScaleGroups = static_cast<_uint>(ceil(static_cast<_float>(m_iWidth * m_iHeight / 16) / 1024.f));

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the downscaled target
	D3D11_TEXTURE2D_DESC dtd = {
		m_iWidth / 4, //UINT Width;
		m_iHeight / 4, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R16G16B16A16_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};
	FAILED_CHECK(m_pDevice->CreateTexture2D(&dtd, NULL, &m_pDownScaleRT));
	SetDebugName(m_pDownScaleRT, "PostFX - Down Scaled RT");

	// Create the resource views
	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
	ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	dsrvd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dsrvd.Texture2D.MipLevels = 1;
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pDownScaleRT, &dsrvd, &m_pDownScaleSRV));
	SetDebugName(m_pDownScaleSRV, "PostFX - Down Scaled SRV");

	// Create the UAVs
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	DescUAV.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = m_iWidth * m_iHeight / 16;
	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pDownScaleRT, &DescUAV, &m_pDownScaleUAV));
	SetDebugName(m_pDownScaleUAV, "PostFX - Down Scaled UAV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate temporary target
	FAILED_CHECK(m_pDevice->CreateTexture2D(&dtd, NULL, &m_pTempRT[0]));
	SetDebugName(m_pTempRT[0], "PostFX - Temp 0 RT");

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pTempRT[0], &dsrvd, &m_pTempSRV[0]));
	SetDebugName(m_pTempSRV[0], "PostFX - Temp 0 SRV");

	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pTempRT[0], &DescUAV, &m_pTempUAV[0]));
	SetDebugName(m_pTempUAV[0], "PostFX - Temp 0 UAV");

	FAILED_CHECK(m_pDevice->CreateTexture2D(&dtd, NULL, &m_pTempRT[1]));
	SetDebugName(m_pTempRT[1], "PostFX - Temp 1 RT");

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pTempRT[1], &dsrvd, &m_pTempSRV[1]));
	SetDebugName(m_pTempSRV[1], "PostFX - Temp 1 SRV");

	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pTempRT[1], &DescUAV, &m_pTempUAV[1]));
	SetDebugName(m_pTempUAV[1], "PostFX - Temp 1 UAV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate bloom target
	FAILED_CHECK(m_pDevice->CreateTexture2D(&dtd, NULL, &m_pBloomRT));
	SetDebugName(m_pBloomRT, "PostFX - Bloom RT");

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pBloomRT, &dsrvd, &m_pBloomSRV));
	SetDebugName(m_pBloomSRV, "PostFX - Bloom SRV");

	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pBloomRT, &DescUAV, &m_pBloomUAV));
	SetDebugName(m_pBloomUAV, "PostFX - Bloom UAV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled luminance buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(float);
	bufferDesc.ByteWidth = m_iDownScaleGroups * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDownScale1DBuffer));
	SetDebugName(m_pDownScale1DBuffer, "PostFX - Luminance Down Scale 1D Buffer");

	ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = m_iDownScaleGroups;
	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pDownScale1DBuffer, &DescUAV, &m_pDownScale1DUAV));
	SetDebugName(m_pDownScale1DUAV, "PostFX - Luminance Down Scale 1D UAV");

	dsrvd.Format = DXGI_FORMAT_UNKNOWN;
	dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	dsrvd.Buffer.FirstElement = 0;
	dsrvd.Buffer.NumElements = m_iDownScaleGroups;
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pDownScale1DBuffer, &dsrvd, &m_pDownScale1DSRV));
	SetDebugName(m_pDownScale1DSRV, "PostFX - Luminance Down Scale 1D SRV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate average luminance buffer
	bufferDesc.ByteWidth = sizeof(float);
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pAvgLumBuffer));
	SetDebugName(m_pAvgLumBuffer, "PostFX - Average Luminance Buffer");

	DescUAV.Buffer.NumElements = 1;
	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pAvgLumBuffer, &DescUAV, &m_pAvgLumUAV));
	SetDebugName(m_pAvgLumUAV, "PostFX - Average Luminance UAV");

	dsrvd.Buffer.NumElements = 1;
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pAvgLumBuffer, &dsrvd, &m_pAvgLumSRV));
	SetDebugName(m_pAvgLumSRV, "PostFX - Average Luminance SRV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate previous frame average luminance buffer
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pPrevAvgLumBuffer));
	SetDebugName(m_pPrevAvgLumBuffer, "PostFX - Previous Average Luminance Buffer");

	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pPrevAvgLumBuffer, &DescUAV, &m_pPrevAvgLumUAV));
	SetDebugName(m_pPrevAvgLumUAV, "PostFX - Previous Average Luminance UAV");

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pPrevAvgLumBuffer, &dsrvd, &m_pPrevAvgLumSRV));
	SetDebugName(m_pPrevAvgLumSRV, "PostFX - Previous Average Luminance SRV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate bokeh buffer
	const UINT nMaxBokehInst = 4056;
	bufferDesc.StructureByteStride = 7 * sizeof(float);
	bufferDesc.ByteWidth = nMaxBokehInst * bufferDesc.StructureByteStride;
	//bufferDesc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pBokehBuffer));
	SetDebugName(m_pBokehBuffer, "PostFX - Bokeh Buffer");

	DescUAV.Buffer.NumElements = nMaxBokehInst;
	DescUAV.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pBokehBuffer, &DescUAV, &m_pBokehUAV));
	SetDebugName(m_pBokehUAV, "PostFX - Bokeh UAV");

	dsrvd.Buffer.NumElements = nMaxBokehInst;
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pBokehBuffer, &dsrvd, &m_pBokehSRV));
	SetDebugName(m_pBokehSRV, "PostFX - Bokeh SRV");

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	bufferDesc.ByteWidth = 16;

	D3D11_SUBRESOURCE_DATA initData;
	UINT bufferInit[4] = { 0, 1, 0, 0 };
	initData.pSysMem = bufferInit;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, &initData, &m_pBokehIndirectDrawBuffer));
	SetDebugName(m_pBokehIndirectDrawBuffer, "PostFX - Bokeh Indirect Draw Buffer");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate constant buffers
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(TDownScaleCB);
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDownScaleCB));
	SetDebugName(m_pDownScaleCB, "PostFX - Down Scale CB");

	bufferDesc.ByteWidth = sizeof(TFinalPassCB);
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pFinalPassCB));
	SetDebugName(m_pFinalPassCB, "PostFX - Final Pass CB");

	bufferDesc.ByteWidth = sizeof(TBlurCB);
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pBlurCB));
	SetDebugName(m_pBlurCB, "PostFX - Blur CB");

	bufferDesc.ByteWidth = sizeof(TBokehHightlightScanCB);
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pBokehHightlightScanCB));
	SetDebugName(m_pBokehHightlightScanCB, "PostFX - Bokeh Hightlight Scan CB");

	bufferDesc.ByteWidth = sizeof(TBokehRenderCB);
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pBokeRenderCB));
	SetDebugName(m_pBokeRenderCB, "PostFX - Bokeh Render CB");

	// Compile the shaders
	DWORD dwShaderFlags = 0;
#ifdef _DEBUG
	dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	dwShaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	WCHAR str[MAX_PATH] = L"../Bin/ShaderFiles/PostDownScaleFX.hlsl";
	ID3DBlob* pShaderBlob = NULL;
	FAILED_CHECK(CompileShader(str, NULL, "DownScaleFirstPass", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDownScaleFirstPassCS));
	SetDebugName(m_pDownScaleFirstPassCS, "Post FX - Down Scale First Pass CS");
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "DownScaleSecondPass", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDownScaleSecondPassCS));
	SetDebugName(m_pDownScaleSecondPassCS, "Post FX - Down Scale Second Pass CS");
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "BloomReveal", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pBloomRevealCS));
	SetDebugName(m_pBloomRevealCS, "Post FX - Bloom Reveal CS");
	Safe_Release(pShaderBlob);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// NVidia Gaussian Blur
	lstrcpy(str, L"../Bin/ShaderFiles/Blur.hlsl");
	FAILED_CHECK(CompileShader(str, NULL, "VerticalFilter", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_VerticalBlurCS));
	SetDebugName(m_VerticalBlurCS, "Post FX - Vertical Blur CS");
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "HorizFilter", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_HorizontalBlurCS));
	SetDebugName(m_HorizontalBlurCS, "Post FX - Horizontal Blur CS");
	Safe_Release(pShaderBlob);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Bokeh
	lstrcpy(str, L"../Bin/ShaderFiles/BokehCS.hlsl");
	FAILED_CHECK(CompileShader(str, NULL, "HighlightScan", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_BokehHighlightSearchCS));
	SetDebugName(m_BokehHighlightSearchCS, "Post FX - Bokeh Highlight Scan CS");
	Safe_Release(pShaderBlob);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Final pass
	lstrcpy(str, L"../Bin/ShaderFiles/PostFX.hlsl");
	FAILED_CHECK(CompileShader(str, NULL, "FullScreenQuadVS", "vs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pFullScreenQuadVS));
	SetDebugName(m_pFullScreenQuadVS, "Post FX - Full Screen Quad VS");
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "FinalPassPS", "ps_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pFinalPassPS));
	SetDebugName(m_pFinalPassPS, "Post FX - Final Pass PS");
	Safe_Release(pShaderBlob);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Bokeh render

	lstrcpy(str, L"../Bin/ShaderFiles/Bokeh.hlsl");
	FAILED_CHECK(CompileShader(str, NULL, "BokehVS", "vs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pBokehVS));
	SetDebugName(m_pBokehVS, "Post FX - Bokeh VS");
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "BokehGS", "gs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateGeometryShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pBokehGS));
	SetDebugName(m_pBokehGS, "Post FX - Bokeh GS");
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "BokehPS", "ps_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pBokehPS));
	SetDebugName(m_pBokehPS, "Post FX - Bokeh PS");
	Safe_Release(pShaderBlob);

	//lstrcpy(str, L"../Bin/ShaderFiles/Bokeh.dds");
	//FAILED_CHECK_RETURN(CreateDDSTextureFromFile(m_pDevice, str, nullptr, &m_pBokehTexView),E_FAIL);

	lstrcpy(str, L"../Bin/ShaderFiles/BokehA.png");
	FAILED_CHECK_RETURN(CreateWICTextureFromFile(m_pDevice, str, nullptr, &m_pBokehTexView), E_FAIL);
	SetDebugName(m_pBokehPS, "Post FX - Bokeh.dds");

	// Create the blend state for the bokeh highlights
	D3D11_BLEND_DESC descBlend;
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;
	const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		TRUE,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL,
	};
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		descBlend.RenderTarget[i] = defaultRenderTargetBlendDesc;
	FAILED_CHECK(m_pDevice->CreateBlendState(&descBlend, &m_pAddativeBlendState));
	SetDebugName(m_pAddativeBlendState, "Post FX - Addative Blending BS");

	// Create the two samplers
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	FAILED_CHECK(m_pDevice->CreateSamplerState(&samDesc, &m_pSampLinear));
	SetDebugName(m_pSampLinear, "Linear Sampler");

	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	FAILED_CHECK(m_pDevice->CreateSamplerState(&samDesc, &m_pSampPoint));
	SetDebugName(m_pSampPoint, "Point Sampler");

	return S_OK;
}

void CPostFX::Clear()
{
	Safe_Release(m_pDownScaleRT);
	Safe_Release(m_pDownScaleSRV);
	Safe_Release(m_pDownScaleUAV);
	Safe_Release(m_pTempRT[0]);
	Safe_Release(m_pTempSRV[0]);
	Safe_Release(m_pTempUAV[0]);
	Safe_Release(m_pTempRT[1]);
	Safe_Release(m_pTempSRV[1]);
	Safe_Release(m_pTempUAV[1]);
	Safe_Release(m_pBloomRT);
	Safe_Release(m_pBloomSRV);
	Safe_Release(m_pBloomUAV);
	Safe_Release(m_pDownScale1DBuffer);
	Safe_Release(m_pDownScale1DUAV);
	Safe_Release(m_pDownScale1DSRV);
	Safe_Release(m_pDownScaleCB);
	Safe_Release(m_pFinalPassCB);
	Safe_Release(m_pBlurCB);
	Safe_Release(m_pBokehHightlightScanCB);
	Safe_Release(m_pBokeRenderCB);
	Safe_Release(m_pAvgLumBuffer);
	Safe_Release(m_pAvgLumUAV);
	Safe_Release(m_pAvgLumSRV);
	Safe_Release(m_pPrevAvgLumBuffer);
	Safe_Release(m_pPrevAvgLumUAV);
	Safe_Release(m_pPrevAvgLumSRV);
	Safe_Release(m_pBokehBuffer);
	Safe_Release(m_pBokehUAV);
	Safe_Release(m_pBokehSRV);
	Safe_Release(m_pBokehIndirectDrawBuffer);
	Safe_Release(m_pDownScaleFirstPassCS);
	Safe_Release(m_pDownScaleSecondPassCS);
	Safe_Release(m_pBloomRevealCS);
	Safe_Release(m_HorizontalBlurCS);
	Safe_Release(m_VerticalBlurCS);
	Safe_Release(m_BokehHighlightSearchCS);
	Safe_Release(m_pFullScreenQuadVS);
	Safe_Release(m_pFinalPassPS);
	Safe_Release(m_pBokehVS);
	Safe_Release(m_pBokehGS);
	Safe_Release(m_pBokehPS);
	Safe_Release(m_pBokehTexView);
	Safe_Release(m_pAddativeBlendState);

	Safe_Release(m_pSampPoint);
	Safe_Release(m_pSampLinear);
}

void CPostFX::PostProcessing(ID3D11ShaderResourceView* pHDRSRV, ID3D11RenderTargetView* pLDRRTV, ID3D11ShaderResourceView* pDepthSRV)
{
	ID3D11RenderTargetView*	pBackBufferView = nullptr;
	ID3D11DepthStencilView*	pDepthStencilView = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferView, &pDepthStencilView);
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	float fAdaptationNorm = 0.f;

	static bool s_bFirstTime = true;

	if (s_bFirstTime)
	{
		// On the first frame we want to fully adapt the new value so use 0
		fAdaptationNorm = 0.0f;
		s_bFirstTime = false;
	}
	else
	{
		// Normalize the adaptation time with the frame time (all in seconds)
		// Never use a value higher or equal to 1 since that means no adaptation at all (keeps the old value)
		fAdaptationNorm = min(m_fAdaptation < 0.0001f ? 1.0f : (_float)TIMEDELTA / m_fAdaptation, 0.9999f);
	}

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = m_pContext->Map(m_pDownScaleCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	if(SUCCEEDED(hr))
	{
		TDownScaleCB* pDownScale = (TDownScaleCB*)MappedResource.pData;
		pDownScale->nWidth = m_iWidth / 4;
		pDownScale->nHeight = m_iHeight / 4;
		pDownScale->nTotalPixels = pDownScale->nWidth * pDownScale->nHeight;
		pDownScale->nGroupSize = m_iDownScaleGroups;
		pDownScale->fAdaptation = fAdaptationNorm;
		pDownScale->fBloomThreshold = m_fBloomThreshold;
		_float cameraFar = /**CGameInstance::GetInstance()->Get_CameraFar();*/ 500.f;
		_float cameraNear = /**CGameInstance::GetInstance()->Get_CameraNear();*/ 0.2f;
		float fQ = cameraFar / (cameraFar - cameraNear);
		pDownScale->ProjectionValues[0] = -cameraNear * fQ;
		pDownScale->ProjectionValues[1] = -fQ;
		m_pContext->Unmap(m_pDownScaleCB, 0);
	}

	ID3D11Buffer* arrConstBuffers[1] = { m_pDownScaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Down scale the HDR image
	ID3D11RenderTargetView* rt[1] = { NULL };
	m_pContext->OMSetRenderTargets(1, rt, NULL);
	DownScale(pHDRSRV);

	// Bloom
	Bloom();

	// Blur the bloom values
	Blur(m_pTempSRV[0], m_pBloomUAV);

	// Cleanup
	ZeroMemory(&arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Scan for the bokeh highlights
	//BokehHighlightScan(pHDRSRV, pDepthSRV);

	// Do the final pass
	rt[0] = pLDRRTV;
	m_pContext->OMSetRenderTargets(1, rt, NULL);
	FinalPass(pHDRSRV, pDepthSRV);

	// Draw the bokeh highlights on top of the image
	//BokehRender();

	// Swap the previous frame average luminance
	ID3D11Buffer* pTempBuffer = m_pAvgLumBuffer;
	ID3D11UnorderedAccessView* pTempUAV = m_pAvgLumUAV;
	ID3D11ShaderResourceView* p_TempSRV = m_pAvgLumSRV;
	m_pAvgLumBuffer = m_pPrevAvgLumBuffer;
	m_pAvgLumUAV = m_pPrevAvgLumUAV;
	m_pAvgLumSRV = m_pPrevAvgLumSRV;
	m_pPrevAvgLumBuffer = pTempBuffer;
	m_pPrevAvgLumUAV = pTempUAV;
	m_pPrevAvgLumSRV = p_TempSRV;

	m_pContext->OMSetRenderTargets(1, &pBackBufferView, pDepthStencilView);
	Safe_Release(pBackBufferView);
	Safe_Release(pDepthStencilView);
}

void CPostFX::Imgui_Render()
{
	ImGui::Checkbox("HDR On", &m_bOn); 

	static _float2 middleGreyMinMax{ 0.f, 20.f };
	ImGui::InputFloat2("MiddleGreyMinMax", (float*)&middleGreyMinMax);
	ImGui::DragFloat("MiddleGrey", &m_fMiddleGrey, 0.01f, middleGreyMinMax.x, middleGreyMinMax.y);

	static _float2 whiteMinMax{ 0.f, 5.f };
	ImGui::InputFloat2("WhiteMinMax", (float*)&whiteMinMax);
	ImGui::DragFloat("White", &m_fWhite, 0.01f, whiteMinMax.x, whiteMinMax.y);

	static _float2 adaptationMinMax{ 0.f, 10.f };
	ImGui::InputFloat2("adaptationMinMax", (float*)&adaptationMinMax);
	ImGui::DragFloat("Adaptation", &m_fAdaptation, 0.01f, adaptationMinMax.x, adaptationMinMax.y);

	static _float2 BloomThresholdMinMax{ 0.f, 2.5f };
	ImGui::InputFloat2("BoomThresholdMinMax", (float*)&BloomThresholdMinMax);
	ImGui::DragFloat("BoomThreshold", &m_fBloomThreshold, 0.01f, BloomThresholdMinMax.x, BloomThresholdMinMax.y);

	static _float2 BloomScaleMinMax{ 0.f, 2.f };
	ImGui::InputFloat2("BloomScaleMinMax", (float*)&BloomScaleMinMax);
	ImGui::DragFloat("BloomScale", &m_fBloomScale, 0.01f, BloomScaleMinMax.x, BloomScaleMinMax.y);

	static _float2 DOFFarStartMinMax{ 0.f, 100.f };
	ImGui::InputFloat2("DOFFarStartMinMax", (float*)&DOFFarStartMinMax);
	ImGui::DragFloat("DOFFarStart", &m_fDOFFarStart, 0.01f, DOFFarStartMinMax.x, DOFFarStartMinMax.y);

	static _float2 DOFFarRangeRcpMinMax{ 0.f, 1.f };
	ImGui::InputFloat2("DOFFarRangeRcpMinMax", (float*)&DOFFarRangeRcpMinMax);

	static _float2 BokehLumThresholdMinMax{ 0.f, 1.f };
	ImGui::InputFloat2("BokehLumThresholdMinMax", (float*)&BokehLumThresholdMinMax);
	ImGui::DragFloat("BokehLumThreshold", &m_fBokehLumThreshold, 0.01f, BokehLumThresholdMinMax.x, BokehLumThresholdMinMax.y);

	static _float2 BokehBlurThresholdMinMax{ 0.f, 1.f };
	ImGui::InputFloat2("BokehBlurThresholdMinMax", (float*)&BokehBlurThresholdMinMax);
	ImGui::DragFloat("BokehBlurThreshold", &m_fBokehBlurThreshold, 0.01f, BokehBlurThresholdMinMax.x, BokehBlurThresholdMinMax.y);

	static _float2 BokehRadiusScaleMinMax{ 0.f, 10.f };
	ImGui::InputFloat2("BokehRadiusScaleMinMax", (float*)&BokehRadiusScaleMinMax);
	ImGui::DragFloat("BokehRadiusScale", &m_fBokehRadiusScale, 0.01f, BokehRadiusScaleMinMax.x, BokehRadiusScaleMinMax.y);

	static _float2 BokehColorScaleMinMax{ 0.f, 10.f };
	ImGui::InputFloat2("BokehColorScaleMinMax", (float*)&BokehColorScaleMinMax);
	ImGui::DragFloat("BokehColorScale", &m_fBokehColorScale, 0.01f, BokehColorScaleMinMax.x, BokehColorScaleMinMax.y);
}

void CPostFX::DownScale(ID3D11ShaderResourceView * pHDRSRV)
{
	// Output
	ID3D11UnorderedAccessView* arrUAVs[2] = { m_pDownScale1DUAV, m_pDownScaleUAV };
	m_pContext->CSSetUnorderedAccessViews(0, 2, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pHDRSRV, NULL };
	m_pContext->CSSetShaderResources(0, 2, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pDownScaleFirstPassCS, NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch(m_iDownScaleGroups, 1, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - reduce to a single pixel

	// Outoput
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	arrUAVs[0] = m_pAvgLumUAV;
	m_pContext->CSSetUnorderedAccessViews(0, 2, arrUAVs, NULL);

	// Input
	arrViews[0] = m_pDownScale1DSRV;
	arrViews[1] = m_pPrevAvgLumSRV;
	m_pContext->CSSetShaderResources(0, 2, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pDownScaleSecondPassCS, NULL, 0);

	// Excute with a single group - this group has enough threads to process all the pixels
	m_pContext->Dispatch(1, 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 2, arrUAVs, (UINT*)(&arrUAVs));
}

void CPostFX::Bloom()
{
	// Input
	ID3D11ShaderResourceView* arrViews[2] = { m_pDownScaleSRV, m_pAvgLumSRV };
	m_pContext->CSSetShaderResources(0, 2, arrViews);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pTempUAV[0] };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Shader
	m_pContext->CSSetShader(m_pBloomRevealCS, NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch(m_iDownScaleGroups, 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
}

void CPostFX::Blur(ID3D11ShaderResourceView * pInput, ID3D11UnorderedAccessView * pOutput)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - horizontal gaussian filter

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pTempUAV[1] };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { pInput };
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader(m_HorizontalBlurCS, NULL, 0);

	// Execute the horizontal filter
	m_pContext->Dispatch((UINT)ceil((m_iWidth / 4.0f) / (128.0f - 12.0f)), (UINT)ceil(m_iHeight / 4.0f), 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// First pass - vertical gaussian filter

	// Output
	arrUAVs[0] = pOutput;
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	arrViews[0] = m_pTempSRV[1];
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader(m_VerticalBlurCS, NULL, 0);

	// Execute the vertical filter
	m_pContext->Dispatch((UINT)ceil(m_iWidth / 4.0f), (UINT)ceil((m_iHeight / 4.0f) / (128.0f - 12.0f)), 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
}

void CPostFX::BokehHighlightScan(ID3D11ShaderResourceView* pHDRSRV, ID3D11ShaderResourceView* pDepthSRV)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pBokehHightlightScanCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TBokehHightlightScanCB* pBokehHightlightScan = (TBokehHightlightScanCB*)MappedResource.pData;
	pBokehHightlightScan->nWidth = m_iWidth;
	pBokehHightlightScan->nHeight = m_iHeight;
	_float cameraFar = /**CGameInstance::GetInstance()->Get_CameraFar();*/ 500.f;
	_float cameraNear = /**CGameInstance::GetInstance()->Get_CameraNear();*/ 0.2f;
	float fQ = cameraFar / (cameraFar - cameraNear);
	pBokehHightlightScan->ProjectionValues[0] = -cameraNear * fQ;
	pBokehHightlightScan->ProjectionValues[1] = -fQ;
	pBokehHightlightScan->fDOFFarStart = m_fDOFFarStart;
	m_fDOFFarRangeRcp = 1.f / max(m_fDOFFarRange, 0.001f);
	pBokehHightlightScan->fDOFFarRangeRcp = m_fDOFFarRangeRcp;
	pBokehHightlightScan->fMiddleGrey = m_fMiddleGrey;
	pBokehHightlightScan->fLumWhiteSqr = m_fWhite;
	pBokehHightlightScan->fLumWhiteSqr *= pBokehHightlightScan->fMiddleGrey; // Scale by the middle gray value
	pBokehHightlightScan->fLumWhiteSqr *= pBokehHightlightScan->fLumWhiteSqr; // Square
	pBokehHightlightScan->fBokehBlurThreshold = m_fBokehBlurThreshold;
	pBokehHightlightScan->fBokehLumThreshold = m_fBokehLumThreshold;
	pBokehHightlightScan->fRadiusScale = m_fBokehRadiusScale;
	pBokehHightlightScan->fColorScale = m_fBokehColorScale;
	m_pContext->Unmap(m_pBokehHightlightScanCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pBokehHightlightScanCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pBokehUAV };
	UINT nCount = 0; // Indicate we want to flush everything already in the buffer
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, &nCount);

	// Input
	ID3D11ShaderResourceView* arrViews[3] = { pHDRSRV, pDepthSRV, m_pAvgLumSRV };
	m_pContext->CSSetShaderResources(0, 3, arrViews);

	// Shader
	m_pContext->CSSetShader(m_BokehHighlightSearchCS, NULL, 0);

	// Execute the horizontal filter
	m_pContext->Dispatch((UINT)ceil((float)(m_iWidth * m_iHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 3, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(&arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CPostFX::FinalPass(ID3D11ShaderResourceView * pHDRSRV, ID3D11ShaderResourceView* pDepthSRV)
{
	ID3D11ShaderResourceView* arrViews[6] = { pHDRSRV, m_pAvgLumSRV, m_pBloomSRV , m_pDownScaleSRV, pDepthSRV };
	m_pContext->PSSetShaderResources(0, 6, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = m_pContext->Map(m_pFinalPassCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (SUCCEEDED(hr))
	{
		TFinalPassCB* pFinalPass = (TFinalPassCB*)MappedResource.pData;
		pFinalPass->fMiddleGrey = m_fMiddleGrey;
		pFinalPass->fLumWhiteSqr = m_fWhite;
		pFinalPass->fLumWhiteSqr *= pFinalPass->fMiddleGrey; // Scale by the middle grey value
		pFinalPass->fLumWhiteSqr *= pFinalPass->fLumWhiteSqr; // Square
		pFinalPass->fBloomScale = m_fBloomScale;

		_float cameraFar = /**CGameInstance::GetInstance()->Get_CameraFar();*/ 500.f;
		_float cameraNear = /**CGameInstance::GetInstance()->Get_CameraNear();*/ 0.2f;

		float fQ = cameraFar / (cameraFar - cameraNear);
		pFinalPass->ProjectionValues[0] = -cameraNear * fQ;
		pFinalPass->ProjectionValues[1] = -fQ;
		pFinalPass->fDOFFarStart = m_fDOFFarStart;
		m_fDOFFarRangeRcp = 1.f / max(m_fDOFFarRange, 0.001f);
		pFinalPass->fDOFFarRangeRcp = m_fDOFFarRangeRcp;

		m_pContext->Unmap(m_pFinalPassCB, 0);
	}
	ID3D11Buffer* arrConstBuffers[1] = { m_pFinalPassCB };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	m_pContext->IASetInputLayout(NULL);
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[2] = { m_pSampPoint, m_pSampLinear };
	m_pContext->PSSetSamplers(0, 2, arrSamplers);

	// Set the shaders
	m_pContext->VSSetShader(m_pFullScreenQuadVS, NULL, 0);
	m_pContext->PSSetShader(m_pFinalPassPS, NULL, 0);

	m_pContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->PSSetShaderResources(0, 6, arrViews);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
}

void CPostFX::BokehRender()
{
	// Copy the amount of appended highlights so we can render them
	m_pContext->CopyStructureCount(m_pBokehIndirectDrawBuffer, 0, m_pBokehUAV);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pBokeRenderCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TBokehRenderCB* pBokehRender = (TBokehRenderCB*)MappedResource.pData;
	pBokehRender->AspectRatio[0] = 1.0f;
	pBokehRender->AspectRatio[1] = (float)m_iWidth / (float)m_iHeight;
	m_pContext->Unmap(m_pBokeRenderCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pBokeRenderCB };
	m_pContext->GSSetConstantBuffers(0, 1, arrConstBuffers);

	ID3D11BlendState* pPrevBlendState;
	FLOAT prevBlendFactor[4];
	UINT prevSampleMask;
	m_pContext->OMGetBlendState(&pPrevBlendState, prevBlendFactor, &prevSampleMask);
	m_pContext->OMSetBlendState(m_pAddativeBlendState, prevBlendFactor, prevSampleMask);

	ID3D11ShaderResourceView* arrViews[1] = { m_pBokehSRV };
	m_pContext->VSSetShaderResources(0, 1, arrViews);

	arrViews[0] = m_pBokehTexView;
	m_pContext->PSSetShaderResources(0, 1, arrViews);

	m_pContext->IASetInputLayout(NULL);
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	ID3D11SamplerState* arrSamplers[1] = { m_pSampLinear };
	m_pContext->PSSetSamplers(0, 1, arrSamplers);

	m_pContext->VSSetShader(m_pBokehVS, NULL, 0);
	m_pContext->GSSetShader(m_pBokehGS, NULL, 0);
	m_pContext->PSSetShader(m_pBokehPS, NULL, 0);

	m_pContext->DrawInstancedIndirect(m_pBokehIndirectDrawBuffer, 0);

	// Cleanup
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->GSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->VSSetShaderResources(0, 1, arrViews);
	m_pContext->PSSetShaderResources(0, 1, arrViews);
	m_pContext->OMSetBlendState(pPrevBlendState, prevBlendFactor, prevSampleMask);
	arrConstBuffers[0] = NULL;
	m_pContext->GSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CPostFX::Day()
{
	if (m_fMiddleGrey <= 10.f)
	{
		m_fMiddleGrey += TIMEDELTA * 2.f;
		if (m_fMiddleGrey > 10.f)
			m_fMiddleGrey = 10.f;
	}

	if(m_fWhite >= 2.f)
	{
		m_fWhite -= TIMEDELTA;
		if (m_fWhite < 2.f)
			m_fWhite = 2.f;
	}
}

void CPostFX::Night()
{
	if (m_fMiddleGrey > 1.f)
	{
		m_fMiddleGrey -= TIMEDELTA * 2.f;
		if (m_fMiddleGrey <= 0.5f)
			m_fMiddleGrey = 0.5f;
	}

	if (m_fWhite <= 5.f)
	{
		m_fWhite += TIMEDELTA;
		if (m_fWhite > 5.f)
			m_fWhite = 5.f;
	}
}

void CPostFX::BlurCapture()
{
	if(!m_bCapture)
	{
		m_fPreDOFFarStart = m_fDOFFarStart;
		m_bCapture = true;
	}
	
	m_fDOFFarStart = 0.f;
}

void CPostFX::CaptureOff()
{
	m_fDOFFarStart = m_fPreDOFFarStart;
	m_bCapture = false;
}

void CPostFX::SetDebugName(ID3D11DeviceChild * pObj, const char * pName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pName), pName);
}

HRESULT CPostFX::CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO * pMacros, char * strEntryPoint, char * strProfile, DWORD dwShaderFlags, ID3DBlob ** ppVertexShaderBuffer)
{
	return D3DCompileFromFile(strPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint, strProfile, dwShaderFlags, 0,
		ppVertexShaderBuffer, nullptr);
}

void CPostFX::Free()
{
	Clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
