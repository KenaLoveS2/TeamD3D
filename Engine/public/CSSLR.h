#pragma once
#include "Base.h"

BEGIN(Engine)

class CSSLR : public CBase
{
	DECLARE_SINGLETON(CSSLR)
private:
	CSSLR();
	virtual ~CSSLR() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Clear();
	void	Render(ID3D11RenderTargetView* pLightAccumRTV, ID3D11ShaderResourceView* pMiniDepthSRV, const _float3& vSunDir, const _float3& vSunColor);
	void	SetTraceRay(_bool bRay) { m_bShowRayTraceRes = bRay; }
private:

	// Prepare the occlusion map
	void PrepareOcclusion(ID3D11ShaderResourceView* pMiniDepthSRV);

	// Ray trace the occlusion map to generate the rays
	void RayTrace(const _float2& vSunPosSS, const _float3& vSunColor);

	// Combine the rays with the scene
	void Combine(ID3D11RenderTargetView* pLightAccumRTV);

	void			SetDebugName(ID3D11DeviceChild* pObj, const char* pName);
	HRESULT			CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer);

	UINT m_nWidth;
	UINT m_nHeight;
	float m_fInitDecay;
	float m_fDistDecay;
	float m_fMaxDeltaLen;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	ID3D11Texture2D* m_pOcclusionTex = nullptr;
	ID3D11UnorderedAccessView* m_pOcclusionUAV = nullptr;
	ID3D11ShaderResourceView* m_pOcclusionSRV = nullptr;

	ID3D11Texture2D* m_pLightRaysTex = nullptr;
	ID3D11RenderTargetView* m_pLightRaysRTV = nullptr;
	ID3D11ShaderResourceView* m_pLightRaysSRV = nullptr;

	// Shaders
	ID3D11Buffer* m_pOcclusionCB = nullptr;
	ID3D11ComputeShader* m_pOcclusionCS = nullptr;
	ID3D11Buffer* m_pRayTraceCB = nullptr;
	ID3D11VertexShader* m_pFullScreenVS = nullptr;
	ID3D11PixelShader* m_pRayTracePS = nullptr;
	ID3D11PixelShader* m_pCombinePS = nullptr;

	// Additive blend state to add the light rays on top of the scene lights
	ID3D11BlendState* m_pAdditiveBlendState = nullptr;

	_bool			 m_bShowRayTraceRes = false;

public:
	virtual void Free() override;
};

END