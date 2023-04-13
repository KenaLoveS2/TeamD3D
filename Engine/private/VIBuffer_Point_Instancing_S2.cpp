#include "stdafx.h"
#include "..\public\VIBuffer_Point_Instancing_S2.h"

#include "GameInstance.h"
#include "Utile.h"

CVIBuffer_Point_Instancing_S2::CVIBuffer_Point_Instancing_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
	, m_pXSpeeds(nullptr)
	, m_pYSpeeds(nullptr)
	, m_pZSpeeds(nullptr)
	, m_pVariables(nullptr)
	, m_pPositions(nullptr)
	, m_bFinished(false)
	, m_bStop(false)
{
	ZeroMemory(&m_tInfo, sizeof(m_tInfo));
}

CVIBuffer_Point_Instancing_S2::CVIBuffer_Point_Instancing_S2(const CVIBuffer_Point_Instancing_S2& rhs)
	: CVIBuffer_Instancing(rhs)
	, m_pXSpeeds(nullptr)
	, m_pYSpeeds(nullptr)
	, m_pZSpeeds(nullptr)
	, m_pVariables(nullptr)
	, m_pPositions(nullptr)
	, m_bFinished(false)
	, m_bStop(false)
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

	m_pVariables = new _float3[m_iNumInstance];
	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pVariables[i] = { 0.0f, 0.0f, 0.0f };

	//if (POINTINFO::TYPE_SPREADREPEAT == m_tInfo.eType)
	//{
		/* x: StartTime, y: TimeAcc, z: NewTimeAcc */
		for (_uint i = 0; i < m_iNumInstance; ++i)
			m_pVariables[i] = { CUtile::Get_RandomFloat(0.0f, m_tInfo.fTerm),0.0f, 0.0f };
	//}


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

	m_vecInstances.reserve(m_iNumInstance);

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
	case POINTINFO::TYPE_TRAIL:
		return Tick_Trail(TimeDelta);
	case POINTINFO::TYPE_SPREADREPEAT:
		return Tick_SpreadRepeat(TimeDelta);
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
	Reset();

	if (nullptr == pInfo)
	{
		POINTINFO tInfo;
		ZeroMemory(&tInfo, sizeof POINTINFO);
		memcpy(&tInfo, &m_tInfo, sizeof POINTINFO);
		Initialize(&tInfo, nullptr);
	}
	else
		Initialize(pInfo, nullptr);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Bind_ShaderResouce(CShader* pShaderCom, const char* pConstanctName)
{
	if (pShaderCom == nullptr)
		return E_FAIL;

	return pShaderCom->Set_MatrixArray(pConstanctName, (_float4x4*)m_vecInstances.data(), (_uint)m_vecInstances.size());
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Haze(_float TimeDelta)
{
	if (m_bFinished)
		return S_OK;

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CONTEXT_LOCK
	HRESULT hr = m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (SUCCEEDED(hr))
	{
		_bool isAllStop = true;
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			//x :  StartTime, y : StartTimeAcc
			if (m_pVariables[i].y < m_pVariables[i].x)
			{
				isAllStop = false;
				m_pVariables[i].y += TimeDelta;
				continue;
			}

			// z : TimeAcc
			if (((VTXMATRIX*)SubResource.pData)[i].vPosition.w <= 0.f)
			{
				((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_pPositions[i].x;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_pPositions[i].y;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_pPositions[i].z;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1.f;
				m_pVariables[i].z = 0.0f;
				m_pVariables[i].y = 0.0f;

				if (m_bStop)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 0.0f;
				else
					isAllStop = false;
			}
			else
			{
				m_pVariables[i].z += m_tInfo.fPlaySpeed * TimeDelta;
				_float fAngle = fmod(m_pVariables[i].z, 360.f);

				((VTXMATRIX*)SubResource.pData)[i].vPosition.y += _float(m_pYSpeeds[i] * TimeDelta);
				((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_pPositions[i].x + m_pXSpeeds[i] * sinf(fAngle);
				((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_pPositions[i].z + m_pZSpeeds[i] * sinf(fAngle);

				/* Life */
				((VTXMATRIX*)SubResource.pData)[i].vPosition.w = (1 - m_pVariables[i].z / m_tInfo.fTerm);

			}
		}

		if (isAllStop == true)
			m_bFinished = true;

		m_pContext->Unmap(m_pInstanceBuffer, 0);
	}
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Gather(_float TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CONTEXT_LOCK
	HRESULT hr = m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (SUCCEEDED(hr))
	{
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
	}

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
	CONTEXT_LOCK
	HRESULT hr = m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (SUCCEEDED(hr))
	{
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
	}
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
	CONTEXT_LOCK
	HRESULT hr =m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (SUCCEEDED(hr))
	{
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
	}
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_Trail(_float TimeDelta)
{
	if (m_pOwner == nullptr)
	{
		MSG_BOX("Owner Needed");
		return S_OK;
	}

	//if (m_vecInstances.empty())
	//	return S_OK;

	for (auto& mat : m_vecInstances)
		mat.vPosition.w -= m_tInfo.fPlaySpeed * TimeDelta;

	m_vecInstances.erase(remove_if(m_vecInstances.begin(), m_vecInstances.end(), [](const VTXMATRIX& Info) {
		return Info.vPosition.w <= 0.0f;
		}), m_vecInstances.end());

	if (m_vecInstances.size() < m_tInfo.iNumInstance)
	{
		m_tInfo.fTermAcc += TimeDelta;
		if (m_tInfo.fTermAcc > m_tInfo.fTerm)
		{
			VTXMATRIX matrix;
			memcpy(&matrix, &m_pOwner->Get_TransformCom()->Get_WorldMatrixFloat4x4(), sizeof(_float4x4));
			m_vecInstances.push_back(matrix);
			m_tInfo.fTermAcc = 0.0f;
		}
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CONTEXT_LOCK									//D3D11_MAP_WRITE_DISCARD
		HRESULT hr = m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (SUCCEEDED(hr))
	{
		memcpy(SubResource.pData, m_vecInstances.data(), sizeof(VTXMATRIX) * m_vecInstances.size());
		m_pContext->Unmap(m_pInstanceBuffer, 0);
	}

	m_iNumInstance = (_uint)m_vecInstances.size();
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing_S2::Tick_SpreadRepeat(_float TimeDelta)
{
	if (m_bFinished)
		return S_OK;

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CONTEXT_LOCK
		HRESULT hr = m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (SUCCEEDED(hr))
	{
		_bool isAllStop = true;
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			//x :  StartTime, y : StartTimeAcc
			if (m_pVariables[i].y < m_pVariables[i].x)
			{
				isAllStop = false;
				m_pVariables[i].y += TimeDelta;
				continue;
			}
			
			// z : TimeAcc
			if (((VTXMATRIX*)SubResource.pData)[i].vPosition.w >= 1.f)
			{
				((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_pPositions[i].x;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_pPositions[i].y;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_pPositions[i].z;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 0.f;
				m_pVariables[i].z = 0.0f;
				m_pVariables[i].y = 0.0f;

				if (m_bStop)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1.0f;
				else
					isAllStop = false;
			}
			else
			{
				isAllStop = false;

				m_pVariables[i].z += TimeDelta;
				_float fDownSpeedY = m_tInfo.fPlaySpeed * m_pVariables[i].z * m_pVariables[i].z;

				_float4 vDir = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
				vDir.Normalize();
				_float4 vMove = m_pXSpeeds[i] * TimeDelta * vDir;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.y += vMove.y - fDownSpeedY;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.x += vMove.x;
				((VTXMATRIX*)SubResource.pData)[i].vPosition.z += vMove.z;

				/* Life */
				((VTXMATRIX*)SubResource.pData)[i].vPosition.w = m_pVariables[i].z / m_tInfo.fTerm;
			}
		}

		if (isAllStop == true)
			m_bFinished = true;

		m_pContext->Unmap(m_pInstanceBuffer, 0);
	}
	return S_OK;
}

void CVIBuffer_Point_Instancing_S2::Safe_Delete_Arrays()
{
	Safe_Delete_Array(m_pXSpeeds);
	Safe_Delete_Array(m_pYSpeeds);
	Safe_Delete_Array(m_pZSpeeds);
	Safe_Delete_Array(m_pPositions);
	Safe_Delete_Array(m_pVariables);
}

void CVIBuffer_Point_Instancing_S2::Reset()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pInstanceBuffer);

	Safe_Delete_Arrays();

	m_vecInstances.clear();

	m_bFinished = false;
	m_bStop = false;
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