#include "..\public\PostFX.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPostFX)

CPostFX::CPostFX()
{
}

HRESULT CPostFX::Initilaize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
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

	if (m_pDevice == nullptr || m_pContext == nullptr)
		return E_FAIL;

	D3D11_VIEWPORT	ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(ViewportDesc));

	_uint	iNumViewPorts = 1;

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	m_iWidth = static_cast<_uint>(ViewportDesc.Width);
	m_iHeight = static_cast<_uint>(ViewportDesc.Height);
	m_iDownScaleGroups = static_cast<_uint>(ceil(static_cast<_float>(m_iWidth * m_iHeight / 16) / 1024.f));

	return S_OK;
}

void CPostFX::Clear()
{
	Safe_Release(m_pDownScale1DBuffer);
	Safe_Release(m_pDownScale1DUAV);
	Safe_Release(m_pDownScale1DSRV);
	Safe_Release(m_pDownScaleCB);
	Safe_Release(m_pFinalPassCB);
	Safe_Release(m_pAvgLumBuffer);
	Safe_Release(m_pAvgLumUAV);
	Safe_Release(m_pAvgLumSRV);
	Safe_Release(m_pDownScaleFirstPassCS);
	Safe_Release(m_pDownScaleSecondPassCS);
	Safe_Release(m_pFullScreenQuadVS);
	Safe_Release(m_pFinalPassPS);
}

void CPostFX::PostProcessing(ID3D11ShaderResourceView* pHDRSRV, ID3D11RenderTargetView* pLDRRTV)
{
}

void CPostFX::SetParameter(_float fMiddleGrey, _float fWhite)
{
}

void CPostFX::DownScale(ID3D11ShaderResourceView * pHDRSRV)
{
}

void CPostFX::FinalPass(ID3D11ShaderResourceView * pHDRSRV)
{
}

void CPostFX::Free()
{
	Clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
