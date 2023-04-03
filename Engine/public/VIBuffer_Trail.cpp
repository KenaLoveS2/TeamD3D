#include "stdafx.h"
#include "..\public\VIBuffer_Trail.h"
#include "Utile.h"
#include "Shader.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_iInitNumInstance(rhs.m_iInitNumInstance)
	, m_pSpeeds(rhs.m_pSpeeds)
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pSpeeds = new _float[iNumInstance];

	for (_uint i = 0; i < iNumInstance; ++i)
		m_pSpeeds[i] = rand() % 1 + 1.0f;

	m_iNumInstance = iNumInstance;
	m_iInitNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumPrimitive = iNumInstance;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(_ushort);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT*		pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);
	pVertices->vPSize = _float2(0.05f, 0.05f);

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

	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
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

	m_vecInstanceInfo.reserve(m_iInitNumInstance);
	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Tick(_float fTimeDelta)
{
	for (VTXMATRIX& pInfo : m_vecInstanceInfo)
		pInfo.vPosition.w -= fTimeDelta;

	m_vecInstanceInfo.erase(remove_if(m_vecInstanceInfo.begin(), m_vecInstanceInfo.end(), [](const VTXMATRIX& Info) {
		return Info.vPosition.w <= 0.0f;
	}), m_vecInstanceInfo.end());

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, m_vecInstanceInfo.data(), sizeof(VTXMATRIX) * m_vecInstanceInfo.size());
	m_pContext->Unmap(m_pInstanceBuffer, 0);

	m_iNumInstance = (_uint)m_vecInstanceInfo.size();

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점버퍼들을 장치에 바인딩한다.(복수를 바인딩한다.)  */

	ID3D11Buffer*			pVertexBuffers[] = {
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

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/*m_pContext->DrawIndexed(m_iNumIndices, 0, 0);*/
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Trail_Tick(_int iObjectType,CGameObject* pGameObject, _float fTimeDelta)
{
	if(iObjectType == (_int)OBJ_MONSTER)
	{
		_float fRange = 3.f;
		_float4 vDir = XMVectorSet(fRange, CUtile::Get_RandomFloat(-fRange, fRange), CUtile::Get_RandomFloat(-fRange, fRange), 0.0f);

		for (_uint i = 0; i < m_iNumInstance; ++i)
			XMStoreFloat4(&m_vecInstanceInfo[i].vPosition, XMLoadFloat4(&m_vecInstanceInfo[i].vPosition) - XMLoadFloat4(&vDir) * m_pSpeeds[i] * fTimeDelta);
	}
	else if (iObjectType == (_int)OBJ_TRAIL)
	{
		_float4 vDir = pGameObject->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);

		for (_uint i = 0; i < m_iNumInstance; ++i)
			XMStoreFloat4(&m_vecInstanceInfo[i].vPosition, XMLoadFloat4(&m_vecInstanceInfo[i].vPosition) - XMLoadFloat4(&vDir) * m_pSpeeds[i] * fTimeDelta);
	}
	else
	{
		for (_uint i = 0; i < m_iNumInstance; ++i)
			XMStoreFloat4(&m_vecInstanceInfo[i].vPosition, XMLoadFloat4(&m_vecInstanceInfo[i].vPosition));
	}

	return S_OK;
}

void CVIBuffer_Trail::Add_Instance(_float4x4 InfoMatrix)
{
	if (m_vecInstanceInfo.size() >= m_iInitNumInstance)
		return;

	VTXMATRIX Info;
	memcpy(&Info, &InfoMatrix, sizeof(_float4x4));
	m_vecInstanceInfo.push_back(Info);
}

HRESULT CVIBuffer_Trail::Bind_ShaderResouce(CShader * pShaderCom, const char * pConstanctName)
{
	if (pShaderCom == nullptr)
		return E_FAIL;

	return pShaderCom->Set_MatrixArray(pConstanctName, (_float4x4*)m_vecInstanceInfo.data(), (_uint)m_vecInstanceInfo.size());
}

HRESULT CVIBuffer_Trail::Bind_RawValue(CShader * pShaderCom, const char * pConstanctName)
{
	if (pShaderCom == nullptr)
		return E_FAIL;

	_float fSize = (_float)m_vecInstanceInfo.size();
	return pShaderCom->Set_RawValue(pConstanctName, &fSize, sizeof(_float));
}

void CVIBuffer_Trail::Refresh_InstanceCount()
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, m_vecInstanceInfo.data(), sizeof(VTXMATRIX) * m_vecInstanceInfo.size());
	m_pContext->Unmap(m_pInstanceBuffer, 0);

	m_iNumInstance = (_uint)m_vecInstanceInfo.size();
}

CVIBuffer_Trail * CVIBuffer_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Trail*		pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg, CGameObject * pOwner)
{
	CVIBuffer_Trail*		pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

	m_vecInstanceInfo.clear();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeeds);
}
