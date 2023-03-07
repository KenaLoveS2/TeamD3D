#include "stdafx.h"
#include "..\public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pBlendState(nullptr)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar * pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	m_pBatch = new SpriteBatch(m_pContext);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pBlendState = nullptr;
	m_pDevice->CreateBlendState(&blendDesc, &m_pBlendState);

	return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _float fRadian, _float2 vScale, _fvector vColor)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin(SpriteSortMode_Immediate, m_pBlendState);

	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRadian, _float2(0.f, 0.f), vScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontFilePath)
{
	CCustomFont*		pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Release(m_pBlendState);

	Safe_Delete(m_pFont);
	Safe_Delete(m_pBatch);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
