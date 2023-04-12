#pragma once
#include "Base.h"

BEGIN(Engine)

class CSSAO : public CBase
{
	DECLARE_SINGLETON(CSSAO)

public:
	CSSAO();
	virtual ~CSSAO() = default;

	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Clear();

	void Compute(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV);
	void SetParameters(int iSSAOSampRadius, float fRadius) { m_iSSAOSampRadius = iSSAOSampRadius; m_fRadius = fRadius; }

	ID3D11ShaderResourceView* GetSSAOSRV() { return m_pSSAO_SRV; }
	ID3D11ShaderResourceView* GetMiniDepthSRV() { return m_pSSAO_SRV; }

private:

	void DownscaleDepth(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV);

	void ComputeSSAO();

	void Blur();

	void					SetDebugName(ID3D11DeviceChild* pObj, const char* pName);
	HRESULT			CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer);

	UINT m_nWidth;
	UINT m_nHeight;
	int m_iSSAOSampRadius;
	float m_fRadius;

	typedef struct
	{
		UINT nWidth;
		UINT nHeight;
		float fHorResRcp;
		float fVerResRcp;
		_float4 ProjParams;
		_smatrix ViewMatrix;
		float fOffsetRadius;
		float fRadius;
		float fMaxDepth;
		UINT pad;
	} TDownscaleCB;
	ID3D11Buffer* m_pDownscaleCB = nullptr;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	// SSAO values for usage with the directional light
	ID3D11Texture2D* m_pSSAO_RT = nullptr;
	ID3D11UnorderedAccessView* m_pSSAO_UAV = nullptr;
	ID3D11ShaderResourceView* m_pSSAO_SRV = nullptr;

	// Downscaled depth buffer (1/4 size)
	ID3D11Buffer* m_pMiniDepthBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pMiniDepthUAV = nullptr;
	ID3D11ShaderResourceView* m_pMiniDepthSRV = nullptr;

	// Shaders
	ID3D11ComputeShader* m_pDepthDownscaleCS = nullptr;
	ID3D11ComputeShader* m_pComputeCS = nullptr;

public:
	virtual void Free() override;
};

END
