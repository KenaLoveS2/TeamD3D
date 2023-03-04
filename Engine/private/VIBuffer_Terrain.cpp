#include "stdafx.h"
#include "..\public\VIBuffer_Terrain.h"
#include "GameInstance.h"

#include "Frustum.h"

#include "QuadTree.h"
#include "Utile.h"
#include "PhysX_Manager.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
	
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)	
	, m_pVerticesPos(rhs.m_pVerticesPos)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pIndices(rhs.m_pIndices)
	, m_pQuadTree(rhs.m_pQuadTree)


{
	Safe_AddRef(m_pQuadTree);

}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER			fh;
	BITMAPINFOHEADER			ih;
	_ulong*						pPixel = nullptr;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;	
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	pPixel = new _ulong[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;	
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER
	VTXNORTEX*			pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

		//	11111111 11111011 11111011 11111011
		//& 00000000 00000000 00000000 11111111

			//pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.f, (_float)i);
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, 0.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion



#pragma region INDEX_BUFFER
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1, 
				iIndex + 1, 
				iIndex
			};

			_vector			vSour, vDest, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;
		}
	}

#pragma endregion


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);

	m_pQuadTree->Make_Neighbor();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg, CGameObject* pOwner)
{
	if (FAILED(__super::Initialize(pArg, pOwner)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	CFrustum*			pFrustum = GET_INSTANCE(CFrustum);

	pFrustum->Transform_ToLocalSpace(WorldMatrix);

	_uint			iNumFaces = 0;

#ifdef USE_QUADTREE
	m_pQuadTree->Culling(pFrustum, m_pVerticesPos, m_pIndices, &iNumFaces);

#else
	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};		

			_bool			isIn[4] = {
				pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])), 
				pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
				pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
				pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]))
			};

			/* 우상단 삼각형이 그려져야하니? */	
			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				m_pIndices[iNumFaces]._0 = iIndices[0];
				m_pIndices[iNumFaces]._1 = iIndices[1];
				m_pIndices[iNumFaces]._2 = iIndices[2];
				++iNumFaces;
			}

			/* 좌하단 삼각형이 그려져야하니? */		
			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				m_pIndices[iNumFaces]._0 = iIndices[0];
				m_pIndices[iNumFaces]._1 = iIndices[2];
				m_pIndices[iNumFaces]._2 = iIndices[3];
				++iNumFaces;
			}
		}
	}
#endif

	D3D11_MAPPED_SUBRESOURCE		SubResources;
	ZeroMemory(&SubResources, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResources);

	memcpy(SubResources.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumFaces * 3;

	RELEASE_INSTANCE(CFrustum);
}

_bool CVIBuffer_Terrain::PickingRetrunIndex_scale(HWND hWnd, CTransform * pTransform, _float3 & fIndexs)
{
	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPortDesc;
	_uint								iNumViewports = 1;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);


	vPoint.x = ptMouse.x / (ViewPortDesc.Width * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(ViewPortDesc.Height * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));

	_matrix		matWorld;
	matWorld = pTransform	->Get_WorldMatrix_Inverse();
	XMVector3TransformCoord(XMLoadFloat4(&vRayPos), matWorld);
	XMVector3TransformNormal(XMLoadFloat4(&vRayDir), matWorld);

	RELEASE_INSTANCE(CGameInstance);
	_ulong	dwVtxIdx[3]{};
	_float fDist;

	for (_ulong i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong dwIndex = i * (m_iNumVerticesX)+j;

			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + (m_iNumVerticesX)+1;
			dwVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat3(&m_pVerticesPos[dwVtxIdx[1]]),
				XMLoadFloat3(&m_pVerticesPos[dwVtxIdx[0]]),
				XMLoadFloat3(&m_pVerticesPos[dwVtxIdx[2]]),
				fDist))
			{
				// 여기서 바꿔야함
				dwIndex = i * (m_iNumVerticesX - 1) + j;

				fIndexs.x = _float(dwIndex + (m_iNumVerticesX - 1));
				fIndexs.y = _float(dwIndex + (m_iNumVerticesX - 1) + 1);
				fIndexs.z = _float(dwIndex + 1);
				return  true;
			}


			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat3(&m_pVerticesPos[dwVtxIdx[1]]),
				XMLoadFloat3(&m_pVerticesPos[dwVtxIdx[0]]),
				XMLoadFloat3(&m_pVerticesPos[dwVtxIdx[2]]),
				fDist))
			{
				dwIndex = i * (m_iNumVerticesX - 1) + j;
				fIndexs.x = _float(dwIndex + (m_iNumVerticesX - 1));
				fIndexs.y = _float(dwIndex + (m_iNumVerticesX - 1) + 1);
				fIndexs.z = _float(dwIndex + 1);

				return  true;
			}
		}
	}
	return false;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg, CGameObject* pOwner)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();
	
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pIndices);		


	}

	Safe_Release(m_pQuadTree);
}
