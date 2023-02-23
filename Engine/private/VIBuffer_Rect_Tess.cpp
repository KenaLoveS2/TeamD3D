#include "stdafx.h"
#include "..\Public\VIBuffer_Rect_Tess.h"

CVIBuffer_Rect_Tess::CVIBuffer_Rect_Tess(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_Tess::CVIBuffer_Rect_Tess(const CVIBuffer_Rect_Tess & rhs)
	: CVIBuffer(rhs)
	
{

}

HRESULT CVIBuffer_Rect_Tess::Initialize_Prototype(D3D11_PRIMITIVE_TOPOLOGY ePrimitiveTopology)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;		//
	m_eIndexFormat = DXGI_FORMAT_UNKNOWN;
	m_iIndicesSizePerPrimitive = 0;
	m_iNumIndicesPerPrimitive = 0;			// 인덱스 버퍼가없다?
																// 버텍스 버퍼만 생성하고 던진다.

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTEX*		pVertices = new VTXTEX[4];
	ZeroMemory(pVertices, sizeof(VTXTEX) * 4);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Tess::Initialize(void * pArg, CGameObject* pOwner)
{
	return S_OK;
}

HRESULT CVIBuffer_Rect_Tess::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
	};

	_uint			iStrides[] = {
		m_iStride
	};

	_uint			iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->Draw(m_iNumVertices, 0);

	return S_OK;

	return S_OK;
}

CVIBuffer_Rect_Tess * CVIBuffer_Rect_Tess::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, D3D11_PRIMITIVE_TOPOLOGY ePrimitiveTopology)
{
	CVIBuffer_Rect_Tess*			pInstance = new CVIBuffer_Rect_Tess(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ePrimitiveTopology)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect_Tess::Clone(void * pArg, CGameObject* pOwner)
{
	CVIBuffer_Rect_Tess*			pInstance = new CVIBuffer_Rect_Tess(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Rect_Tess");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Tess::Free()
{
	__super::Free();
}
