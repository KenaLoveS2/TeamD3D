#include "stdafx.h"
#include "..\public\VIBuffer_Point_Instancing_S2.h"
#include "Utile.h"

CVIBuffer_Point_Instancing_S2::CVIBuffer_Point_Instancing_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
	, m_pXSpeeds(nullptr)
	, m_pYSpeeds(nullptr)
	, m_pZSpeeds(nullptr)
	, m_pPositions(nullptr)
	, m_bFinished(false)
{
	ZeroMemory(&m_tInfo, sizeof(m_tInfo));
}

CVIBuffer_Point_Instancing_S2::CVIBuffer_Point_Instancing_S2(const CVIBuffer_Point_Instancing_S2& rhs)
	: CVIBuffer_Instancing(rhs)
	, m_pXSpeeds(nullptr)
	, m_pYSpeeds(nullptr)
	, m_pZSpeeds(nullptr)
	, m_pPositions(nullptr)
	, m_bFinished(false)
{
}

HRESULT CVIBuffer_Point_Instancing_S2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Initialize(void* pArg, CGameObject* pOwner)
{
	if (pArg == nullptr)
		return E_FAIL;

	ZeroMemory(&m_tInfo, sizeof POINTINFO);
	memcpy(&m_tInfo, pArg, sizeof POINTINFO);
	m_tInfo.fTermAcc = 0.f;

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
		m_pYSpeeds[i] = CUtile::Get_RandomFloat(m_tInfo.vSpeedMin.y, m_tInfo.vSpeedMax.y);

	m_pZSpeeds = new _float[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pZSpeeds[i] = CUtile::Get_RandomFloat(m_tInfo.vSpeedMin.z, m_tInfo.vSpeedMax.z);

	m_pPositions = new _float3[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pPositions[i] = {
			CUtile::Get_RandomFloat(m_tInfo.vMinPos.x, m_tInfo.vMaxPos.x),
			CUtile::Get_RandomFloat(m_tInfo.vMinPos.y, m_tInfo.vMaxPos.y),
			CUtile::Get_RandomFloat(m_tInfo.vMinPos.z, m_tInfo.vMaxPos.z) };
	}



#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT* pVertices = new VTXPOINT;
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

	_ushort* pIndices = new _ushort[m_iNumPrimitive];
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

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_tInfo.iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_int i = 0; i < m_tInfo.iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(m_pPositions[i].x, m_pPositions[i].y, m_pPositions[i].z, 1.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick(_float TimeDelta)
{
	switch (m_tInfo.eType)
	{
	case POINTINFO::TYPE_HAZE:
		return Tick_Haze(TimeDelta);
	case POINTINFO::TYPE_GATHER:
		return Tick_Gather(TimeDelta);
	case POINTINFO::TYPE_PARABOLA:
		return Tick_Parabola(TimeDelta);
	case POINTINFO::TYPE_SPREAD:
		return Tick_Spread(TimeDelta);
	default:
		MSG_BOX("Invalid Type : Instancing S2");
		break;
	}

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pInstanceBuffer
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Update_Buffer(POINTINFO* pInfo)
{
	if (nullptr == pInfo)
		return E_FAIL;

	Reset();

	Initialize(pInfo, nullptr);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Haze(_float TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_tInfo.fTermAcc += m_tInfo.fPlaySpeed * TimeDelta;
	if (m_tInfo.fTermAcc >= 360.f)
		m_tInfo.fTermAcc -= 360.f;

	_float fMaxY = m_tInfo.fTerm * m_tInfo.vSpeedMax.y;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (fMaxY <= ((VTXMATRIX*)SubResource.pData)[i].vPosition.y)
		{
			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f + m_pPositions[i].x;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f + m_pPositions[i].y;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f + m_pPositions[i].z;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1.f;
		}
		else
		{
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y += _float(m_pYSpeeds[i] * TimeDelta);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_pPositions[i].x + m_pXSpeeds[i] * sinf(m_tInfo.fTermAcc);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_pPositions[i].z + m_pZSpeeds[i] * sinf(m_tInfo.fTermAcc);

			/* Life */
			((VTXMATRIX*)SubResource.pData)[i].vPosition.w = (1 - ((VTXMATRIX*)SubResource.pData)[i].vPosition.y / fMaxY);
		}

	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Gather(_float TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//m_tInfo.fTermAcc += TimeDelta;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (((VTXMATRIX*)SubResource.pData)[i].vPosition.w < 0.05f)
		{
			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f + m_pPositions[i].x;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f + m_pPositions[i].y;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f + m_pPositions[i].z;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1.f;
		}
		else
		{
			_float4 vPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
			_float4 vDir = XMVector3Normalize(-vPos);

			((VTXMATRIX*)SubResource.pData)[i].vPosition = vPos + m_pXSpeeds[i] * vDir;

			/* Life */
			((VTXMATRIX*)SubResource.pData)[i].vPosition.w =
				(XMVectorGetX(XMVector3Length(vPos + m_pXSpeeds[i] * vDir))
					/ XMVectorGetX(XMVector3Length(m_pPositions[i])));
		}

	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Parabola(_float TimeDelta)
{
	if (m_tInfo.fTermAcc > m_tInfo.fTerm)
	{
		m_bFinished = true;
		return S_OK;
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_tInfo.fTermAcc += TimeDelta;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float fDownSpeedY = m_tInfo.fPlaySpeed * m_tInfo.fTermAcc * m_tInfo.fTermAcc;

		///* Reset Condition */
		//if (m_tInfo.fTermAcc > m_tInfo.fTerm)
		//{
		//	((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_pPositions[i].x;
		//	((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_pPositions[i].y;
		//	((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_pPositions[i].z;
		//	((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1.f;

		//	if(i == m_iNumInstance -1)
		//		m_tInfo.fTermAcc = 0.0f;
		//}
		///* Normal Tick Action */
		//else
		{
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y += _float(m_pYSpeeds[i] * TimeDelta) - fDownSpeedY;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.x += _float(m_pXSpeeds[i] * TimeDelta);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z += _float(m_pZSpeeds[i] * TimeDelta);

			/* Life */
			((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1 - m_tInfo.fTermAcc / m_tInfo.fTerm;
		}

	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Spread(_float TimeDelta)
{
	if (m_tInfo.fTermAcc > m_tInfo.fTerm)
	{
		m_bFinished = true;
		return S_OK;
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_tInfo.fTermAcc += TimeDelta;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float fDownSpeedY = m_tInfo.fPlaySpeed * m_tInfo.fTermAcc * m_tInfo.fTermAcc;

		_float4 vDir = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
		vDir.Normalize();
		_float4 vMove = m_pXSpeeds[i] * TimeDelta * vDir;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += vMove.y - fDownSpeedY;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += vMove.x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += vMove.z;

		/* Life */
		((VTXMATRIX*)SubResource.pData)[i].vPosition.w = m_tInfo.fTermAcc / m_tInfo.fTerm;

	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

void CVIBuffer_Point_Instancing_S2::Safe_Delete_Arrays()
{
	Safe_Delete_Array(m_pXSpeeds);
	Safe_Delete_Array(m_pYSpeeds);
	Safe_Delete_Array(m_pZSpeeds);
	Safe_Delete_Array(m_pPositions);
}

void CVIBuffer_Point_Instancing_S2::Reset()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pInstanceBuffer);

	Safe_Delete_Arrays();

	m_bFinished = false;
}

CVIBuffer_Point_Instancing_S2* CVIBuffer_Point_Instancing_S2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Point_Instancing_S2* pInstance = new CVIBuffer_Point_Instancing_S2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Point_Instancing_S2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instancing_S2::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_Point_Instancing_S2* pInstance = new CVIBuffer_Point_Instancing_S2(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Point_Instancing_S2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point_Instancing_S2::Free()
{
	__super::Free();

	if (true == m_isCloned)
		Safe_Delete_Arrays();
}
