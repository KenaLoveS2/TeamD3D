#include "stdafx.h"
#include "..\Public\VIBuffer_Terrain_Tess.h"

CVIBuffer_Terrain_Tess::CVIBuffer_Terrain_Tess(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain_Tess::CVIBuffer_Terrain_Tess(const CVIBuffer_Terrain_Tess & rhs)
	: CVIBuffer(rhs)
	//, m_pIndices(rhs.m_pIndices)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pVerticesPos(rhs.m_pVerticesPos)
{

}

HRESULT CVIBuffer_Terrain_Tess::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;		// 여기가 다른부분

	

	
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;

	 // m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
	// 이부분이 없음

#pragma region VERTEXBUFFER
	
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, rand() % 3, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}
#pragma endregion

#pragma region INDEXBUFFER


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	/*m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);*/

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;
		}
	}

	//for (_uint i = 0; i < m_iNumVertices; ++i)
	//	XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain_Tess::Initialize(void * pArg, CGameObject* pOwner)
{
	return S_OK;
}

CVIBuffer_Terrain_Tess * CVIBuffer_Terrain_Tess::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	CVIBuffer_Terrain_Tess*			pInstance = new CVIBuffer_Terrain_Tess(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain_Tess");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain_Tess::Clone(void * pArg, CGameObject* pOwner)
{
	CVIBuffer_Terrain_Tess*			pInstance = new CVIBuffer_Terrain_Tess(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain_Tess");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain_Tess::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		//Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pVerticesPos);
	}
}
