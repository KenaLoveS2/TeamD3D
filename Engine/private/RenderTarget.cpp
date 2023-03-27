#include "stdafx.h"
#include "..\public\RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 * pClearColor)
{
	m_ePixelFormat = ePixelFormat;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	/* 장치에 바인딩하여 셰이더를 통해 바인딩된 이 텍스쳐에 픽셀을 기록한다.*/
	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	/* 이 텍스쳐를 셰이더 전역으로 전달하여 픽셀셰이더안에서 이 텍스쳐에 저장된 픽셀의 값(Diffuse, Normal, Shade)을 읽어온다. */
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = *pClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Resize(_uint iWidth, _uint iHeight)
{
#ifdef _DEBUG
	m_WorldMatrix._11 = m_vSize.x;
	m_WorldMatrix._22 = m_vSize.y;
	m_WorldMatrix._41 = m_vPos.x - iWidth * 0.5f;
	m_WorldMatrix._42 = -m_vPos.y + iHeight * 0.5f;
#endif

	if (m_bResizable == false)
		return S_OK;

	if (m_pTexture2D != nullptr)
		Safe_Release(m_pTexture2D);

	if (m_pRTV != nullptr)
		Safe_Release(m_pRTV);

	if (m_pSRV != nullptr)
		Safe_Release(m_pSRV);

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = m_ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D));

	FAILED_CHECK(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV));

	FAILED_CHECK(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV));

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_vPos.x = fX;
	m_vPos.y = fY;
	m_vSize.x = fSizeX;
	m_vSize.y = fSizeY;
	m_WorldMatrix._11 = m_vSize.x;
	m_WorldMatrix._22 = m_vSize.y;
	m_WorldMatrix._41 = m_vPos.x - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -m_vPos.y + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix);
	pShader->Set_ShaderResourceView("g_Texture", m_pSRV);

	pShader->Begin(0);
	pVIBuffer->Render();
	return S_OK;
}
#endif // _DEBUG

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 * pClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, pClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);
}
