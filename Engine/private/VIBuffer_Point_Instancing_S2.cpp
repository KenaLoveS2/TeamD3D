#include "stdafx.h"
#include "..\public\VIBuffer_Point_Instancing_S2.h"
#include "Utile.h"

CVIBuffer_Point_Instancing_S2::CVIBuffer_Point_Instancing_S2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
	, m_pXSpeeds(nullptr)
	, m_pYSpeeds(nullptr)
	, m_pZSpeeds(nullptr)
{
	ZeroMemory(&m_tInfo, sizeof(m_tInfo));
}

CVIBuffer_Point_Instancing_S2::CVIBuffer_Point_Instancing_S2(const CVIBuffer_Point_Instancing_S2 & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_pXSpeeds(rhs.m_pXSpeeds)
	, m_pYSpeeds(rhs.m_pYSpeeds)
	, m_pZSpeeds(rhs.m_pZSpeeds)
	, m_tInfo(rhs.m_tInfo)
{
}

HRESULT CVIBuffer_Point_Instancing_S2::Initialize_Prototype(POINTINFO * tInfo)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (tInfo == nullptr)
		return E_FAIL;

	ZeroMemory(&m_tInfo, sizeof POINTINFO);
	memcpy(&m_tInfo, tInfo, sizeof POINTINFO);

	m_iNumInstance = m_tInfo.iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumPrimitive = m_iNumInstance;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(_ushort);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	m_pXSpeeds = new _float[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pXSpeeds[i] = CUtile::Get_RandomFloat(m_tInfo.vSpeedMin.x, m_tInfo.vSpeedMax.x);
	
	m_pYSpeeds = new _float[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pYSpeeds[i] = CUtile::Get_RandomFloat(m_tInfo.vSpeedMin.x, m_tInfo.vSpeedMax.x);
	
	m_pZSpeeds = new _float[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pZSpeeds[i] = CUtile::Get_RandomFloat(m_tInfo.vSpeedMin.x, m_tInfo.vSpeedMax.x);

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT*			pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = { 0.f, 0.f, 0.f };
	pVertices->vPSize = m_tInfo.vPSize;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort*		pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region INSTANCE_BUFFER

	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_tInfo.iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 행렬은 바뀐다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[m_tInfo.iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_int i = 0; i < m_tInfo.iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Initialize(void * pArg, CGameObject * pOwner)
{
	return E_NOTIMPL;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CVIBuffer_Point_Instancing_S2::Render()
{
	return E_NOTIMPL;
}

CVIBuffer_Point_Instancing_S2 * CVIBuffer_Point_Instancing_S2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, POINTINFO * tInfo)
{
	return nullptr;
}

CComponent * CVIBuffer_Point_Instancing_S2::Clone(void * pArg, CGameObject * pOwner)
{
	return nullptr;
}

void CVIBuffer_Point_Instancing_S2::Free()
{
}
