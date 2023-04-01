#include "stdafx.h"
#include "CSSAO.h"
#include "GameInstance.h"
IMPLEMENT_SINGLETON(CSSAO)

CSSAO::CSSAO()
{
}

HRESULT CSSAO::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
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

	D3D11_VIEWPORT	ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(ViewportDesc));
	_uint	iNumViewPorts = 1;
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);
	m_nWidth = static_cast<_uint>(ViewportDesc.Width) / 2;
	m_nHeight = static_cast<_uint>(ViewportDesc.Height) / 2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate SSAO
	D3D11_TEXTURE2D_DESC t2dDesc = {
		m_nWidth, //UINT Width;
		m_nHeight, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R32_TYPELESS,//DXGI_FORMAT_R8_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};
	FAILED_CHECK(m_pDevice->CreateTexture2D(&t2dDesc, NULL, &m_pSSAO_RT));
	SetDebugName(m_pSSAO_RT, "SSAO - Final AO values");

	// Create the UAVs
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pSSAO_RT, &UAVDesc, &m_pSSAO_UAV));
	SetDebugName(m_pSSAO_UAV, "SSAO - Final AO values UAV");

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pSSAO_RT, &SRVDesc, &m_pSSAO_SRV));
	SetDebugName(m_pMiniDepthSRV, "SSAO - Final AO values SRV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled depth buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = 4 * sizeof(float);
	bufferDesc.ByteWidth = m_nWidth * m_nHeight * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	FAILED_CHECK(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pMiniDepthBuffer));
	SetDebugName(m_pMiniDepthBuffer, "SSAO - Downscaled Depth Buffer");

	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_nWidth * m_nHeight;
	FAILED_CHECK(m_pDevice->CreateUnorderedAccessView(m_pMiniDepthBuffer, &UAVDesc, &m_pMiniDepthUAV));
	SetDebugName(m_pMiniDepthUAV, "SSAO - Downscaled Depth UAV");

	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_nWidth * m_nHeight;
	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pMiniDepthBuffer, &SRVDesc, &m_pMiniDepthSRV));
	SetDebugName(m_pMiniDepthSRV, "SSAO - Downscaled Depth SRV");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scale depth constant buffer
	D3D11_BUFFER_DESC CBDesc;
	ZeroMemory(&CBDesc, sizeof(CBDesc));
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.ByteWidth = sizeof(TDownscaleCB);
	FAILED_CHECK(m_pDevice->CreateBuffer(&CBDesc, NULL, &m_pDownscaleCB));
	SetDebugName(m_pDownscaleCB, "SSAO - Downscale Depth CB");

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Compile the shaders
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;// | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	WCHAR str[MAX_PATH] = L"../Bin/ShaderFiles/SSAO.hlsl";
	ID3DBlob* pShaderBlob = NULL;
	FAILED_CHECK(CompileShader(str, NULL, "DepthDownscale", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDepthDownscaleCS));
	Safe_Release(pShaderBlob);

	FAILED_CHECK(CompileShader(str, NULL, "SSAOCompute", "cs_5_0", dwShaderFlags, &pShaderBlob));
	FAILED_CHECK(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pComputeCS));
	Safe_Release(pShaderBlob);

	return S_OK;
}

void CSSAO::Clear()
{
	Safe_Release(m_pDownscaleCB);
	Safe_Release(m_pSSAO_RT);
	Safe_Release(m_pSSAO_SRV);
	Safe_Release(m_pSSAO_UAV);
	Safe_Release(m_pMiniDepthBuffer);
	Safe_Release(m_pMiniDepthUAV);
	Safe_Release(m_pMiniDepthSRV);
	Safe_Release(m_pDepthDownscaleCS);
	Safe_Release(m_pComputeCS);
}

void CSSAO::Compute(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV)
{
	DownscaleDepth(pDepthSRV, pNormalsSRV);
	ComputeSSAO();
	Blur();
}

void CSSAO::DownscaleDepth(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pDownscaleCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TDownscaleCB* pDownscale = (TDownscaleCB*)MappedResource.pData;
	pDownscale->nWidth = m_nWidth;
	pDownscale->nHeight = m_nHeight;
	pDownscale->fHorResRcp = 1.0f / (float)pDownscale->nWidth;
	pDownscale->fVerResRcp = 1.0f / (float)pDownscale->nHeight;
	const _float4x4* pProj = &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	pDownscale->ProjParams.x = 1.0f / pProj->m[0][0];
	pDownscale->ProjParams.y = 1.0f / pProj->m[1][1];
	_float cameraFar = /**CGameInstance::GetInstance()->Get_CameraFar();*/ 500.f;
	_float cameraNear = /**CGameInstance::GetInstance()->Get_CameraNear();*/ 0.2f;
	float fQ = cameraFar / (cameraFar - cameraNear);
	pDownscale->ProjParams.z = -cameraNear * fQ;
	pDownscale->ProjParams.w = -fQ;
	pDownscale->ViewMatrix = XMMatrixTranspose(CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	pDownscale->fOffsetRadius = (float)m_iSSAOSampRadius;
	pDownscale->fRadius = m_fRadius;
	pDownscale->fMaxDepth = cameraFar;
	m_pContext->Unmap(m_pDownscaleCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownscaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pMiniDepthUAV };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pDepthSRV, pNormalsSRV };
	m_pContext->CSSetShaderResources(0, 2, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pDepthDownscaleCS, NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch((UINT)ceil((float)(m_nWidth * m_nHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CSSAO::ComputeSSAO()
{
	// Constants
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownscaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pSSAO_UAV };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pMiniDepthSRV };
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pComputeCS, NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch((UINT)ceil((float)(m_nWidth * m_nHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CSSAO::Blur()
{
}

void CSSAO::SetDebugName(ID3D11DeviceChild* pObj, const char* pName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pName), pName);
}

HRESULT CSSAO::CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile,
	DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer)
{
	return D3DCompileFromFile(strPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint, strProfile, dwShaderFlags, 0,
		ppVertexShaderBuffer, nullptr);
}

void CSSAO::Free()
{
	Clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
