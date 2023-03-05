#include "stdafx.h"
#include "..\public\VIBuffer_Point_Instancing.h"
#include "Utile.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing & rhs)
	: CVIBuffer_Instancing(rhs)
{
	m_ePointDesc = new POINTDESC[m_iNumInstance];
	m_InstanceData = new INSTANCEDATA[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_ePointDesc[i] = rhs.m_ePointDesc[i];
		m_InstanceData[i] = rhs.m_InstanceData[i];
	}
}

HRESULT CVIBuffer_Point_Instancing::Set_ShapePosition()
{
	_float fDeg1 = 0.0f, fDeg2 = 0.0f;
	_float fRangeX = 0.0f, fRangeY = 0.0f, fRangeZ = 0.0f;
	_float fMoveRate = 0.5f, fStopRate = 0.35f;
	_uint  iFloorCount = 0, iFloorInstCount = 0, iCurFloor = 0;

	_vector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	switch (m_ePointDesc->eShapeType)
	{
	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_BOX:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			_float4 fRandomPos = CUtile::Get_RandomVector(m_ePointDesc->fMin, m_ePointDesc->fMax);
			fRandomPos.w = 1.f;

			m_ePointDesc[i].vOriginPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition = fRandomPos;
		}
		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_STRIGHT:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_ePointDesc[i].fRangeOffset = m_ePointDesc->fRange / (_float)m_iNumInstance;

			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(m_ePointDesc->vOriginPos.x, m_ePointDesc->vOriginPos.y, m_ePointDesc->vOriginPos.z, 1.f);

			m_ePointDesc[i].vDir = m_ePointDesc->vDir;
			m_ePointDesc[i].vDir.w = 0.f;

			m_ePointDesc[i].vOriginPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
			m_ePointDesc[i].fRange = (m_ePointDesc->fRange - m_ePointDesc[i].fRangeOffset) < 0 ? m_ePointDesc->fRange : (m_ePointDesc->fRange - m_ePointDesc[i].fRangeOffset);
		}
		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_PLANECIRCLE:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(0.0f, 0.f, 0.f, 1.f);

			m_ePointDesc[i].vDir = XMVector3Normalize(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)) * m_ePointDesc->fCreateRange;
			m_ePointDesc[i].vDir = XMVector3TransformNormal(m_ePointDesc[i].vDir, XMMatrixRotationY(XMConvertToRadians(_float(rand() % 360 * i))));

			if (m_ePointDesc->bMoveY)
				((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(0.0f, m_ePointDesc->fMoveY - 0.3f * i, 0.f, 1.f) + m_ePointDesc[i].vDir;
			else
				((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(0.0f, 0.f, 0.f, 1.f) + m_ePointDesc[i].vDir;
			m_ePointDesc[i].vOriginPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
		}
		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_CONE:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_ePointDesc[i].fRangeOffset = m_ePointDesc->fRange / (_float)m_iNumInstance;

			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(m_ePointDesc->vOriginPos.x, m_ePointDesc->vOriginPos.y, m_ePointDesc->vOriginPos.z, 1.f);

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_X)
			{
				m_ePointDesc[i].vDir = XMVectorSet(m_ePointDesc->fConeRange.y, CUtile::Get_RandomFloat(-m_ePointDesc->fConeRange.y, m_ePointDesc->fConeRange.y),
					CUtile::Get_RandomFloat(-m_ePointDesc->fConeRange.y, m_ePointDesc->fConeRange.y), 0.0f);
			}
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Y)
			{
				m_ePointDesc[i].vDir = XMVectorSet(CUtile::Get_RandomFloat(-m_ePointDesc->fConeRange.y, m_ePointDesc->fConeRange.y),
					m_ePointDesc->fConeRange.y, CUtile::Get_RandomFloat(-m_ePointDesc->fConeRange.y, m_ePointDesc->fConeRange.y), 0.0f);
			}
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::POINTDESC::DIRXYZ::DIR_Z)
			{
				m_ePointDesc[i].vDir = XMVectorSet(CUtile::Get_RandomFloat(-m_ePointDesc->fConeRange.y, m_ePointDesc->fConeRange.y),
					CUtile::Get_RandomFloat(-m_ePointDesc->fConeRange.y, m_ePointDesc->fConeRange.y), m_ePointDesc->fConeRange.y, 0.0f);
			}

			m_ePointDesc[i].vOriginPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
			m_ePointDesc[i].fRange = (m_ePointDesc->fRange - m_ePointDesc[i].fRangeOffset) < 0 ? m_ePointDesc->fRange : (m_ePointDesc->fRange - m_ePointDesc[i].fRangeOffset);
		}

		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_EXPLOSION:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_ePointDesc[i].fRangeOffset = m_ePointDesc->fRange / (_float)m_iNumInstance;

			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);

			if (m_ePointDesc->bSpread == true)
			{
				((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(m_ePointDesc->vOriginPos.x, m_ePointDesc->vOriginPos.y, m_ePointDesc->vOriginPos.z, 1.f);
				m_ePointDesc[i].vExplosionDir = XMVector4Normalize(XMVectorSet(CUtile::Get_RandomFloat(-5.f, 5.f), CUtile::Get_RandomFloat(-5.f, 5.f), CUtile::Get_RandomFloat(-5.f, 5.f), 0.f));
			}
			else
			{
				fDeg1 = CUtile::Get_RandomFloat(0.f, 180.f);
				fDeg2 = CUtile::Get_RandomFloat(0.f, 360.f);
				fRangeX = CUtile::Get_RandomFloat(0.25f, 0.65f);
				fRangeY = CUtile::Get_RandomFloat(0.25f, 0.65f);
				fRangeZ = CUtile::Get_RandomFloat(0.25f, 0.65f);

				((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(
					m_ePointDesc->fRange * fRangeX * sinf(XMConvertToRadians(fDeg1)) * cosf(XMConvertToRadians(fDeg2)),
					m_ePointDesc->fRange * fRangeY * cosf(XMConvertToRadians(fDeg1)),
					m_ePointDesc->fRange * fRangeZ * sinf(XMConvertToRadians(fDeg1)) * sinf(XMConvertToRadians(fDeg2)), 1.f);

				m_ePointDesc[i].vExplosionDir = XMVector4Normalize(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
			}

			m_ePointDesc[i].vOriginPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
			m_ePointDesc[i].fRange = (m_ePointDesc->fRange - m_ePointDesc[i].fRangeOffset) < 0 ? m_ePointDesc->fRange : (m_ePointDesc->fRange - m_ePointDesc[i].fRangeOffset);
		}
		break;
	}
	m_pContext->Unmap(m_pInstanceBuffer, 0);
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Set_Pos(_float3 fMin, _float3 fMax)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float4 fRandomPos = CUtile::Get_RandomVector(fMin, fMax);
		fRandomPos.w = 1.f;

		((VTXMATRIX*)SubResource.pData)[i].vPosition = fRandomPos;
		m_ePointDesc[i].vOriginPos = fRandomPos;
		m_InstanceData[i].fPos = fRandomPos;
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Set_PSize(_float2 PSize)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXPOINT*)SubResource.pData)[i].vPSize.x = PSize.x;
		((VTXPOINT*)SubResource.pData)[i].vPSize.y = PSize.y;
		m_InstanceData[i].fPSize = PSize;
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Set_RandomPSize(_float2 PSizeMinMax)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXPOINT*)SubResource.pData)[i].vPSize.x = CUtile::Get_RandomFloat(PSizeMinMax.x, PSizeMinMax.y);
		((VTXPOINT*)SubResource.pData)[i].vPSize.y = CUtile::Get_RandomFloat(PSizeMinMax.x, PSizeMinMax.y);
		m_InstanceData[i].fPSize = PSizeMinMax;
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

void CVIBuffer_Point_Instancing::Set_Speeds(_double pSpeed)
{
	for (_uint i = 0; i < m_iNumInstance; i++)
		m_InstanceData->pSpeeds = pSpeed;
}

void CVIBuffer_Point_Instancing::Set_RandomSpeeds(_double fMin, _double fMax)
{
	for (_uint i = 0; i < m_iNumInstance; i++)
		m_InstanceData->pSpeeds = CUtile::Get_RandomFloat((_float)fMin, (_float)fMax);
}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_ePointDesc = new POINTDESC[iNumInstance];
	ZeroMemory(m_ePointDesc, sizeof(POINTDESC) * iNumInstance);

	m_InstanceData = new INSTANCEDATA[iNumInstance];
	ZeroMemory(m_InstanceData, sizeof(INSTANCEDATA) * iNumInstance);
	for (_uint i = 0; i < iNumInstance; ++i)
	{
		m_ePointDesc[i].iCreateInstance = iNumInstance;

		m_InstanceData[i].fPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		m_InstanceData[i].SpeedMinMax = _float2((_float)m_InstanceData->pSpeeds, (_float)m_InstanceData->pSpeeds);
		m_InstanceData[i].fPSize = _float2(0.2f, 0.2f);
	}
	Set_RandomSpeeds(0.5, 2.0);

	m_iNumInstance = iNumInstance;

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
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT*		pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);
	pVertices->vPSize = _float2(0.2f, 0.2f);

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

		m_ePointDesc[i].vOriginPos = pInstanceVertices[i].vPosition;
		m_ePointDesc[i].iInstanceIndex = i;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Initialize(void * pArg, CGameObject * pOwner)
{
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Tick(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	_vector    vNormalLook;
	_vector    vMovePos;
	_float     fDistance = 0.f;
	_vector    vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	switch (m_ePointDesc->eShapeType)
	{
	case POINTDESC::VIBUFFER_BOX:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_X)
				m_ePointDesc->vDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Y)
				m_ePointDesc->vDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Z)
				m_ePointDesc->vDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

			vNormalLook = XMVector3Normalize(m_ePointDesc->vDir);

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_X ||
				m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Z)
			{
				if (m_ePointDesc->bSpread == true)
					XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
						XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - vNormalLook * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta);
				else
					XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
						XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + vNormalLook * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta);
			}

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_X)
			{
				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.x < m_ePointDesc->fMin.x)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_ePointDesc->fMax.x;

				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.x > m_ePointDesc->fMax.x)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_ePointDesc->fMin.x;
			}

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Y)
			{
				if (m_ePointDesc->bSpread == true)
					XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
						XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + vNormalLook * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta);
				else
					XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
						XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - vNormalLook * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta);

				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y < m_ePointDesc->fMin.y)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_ePointDesc->fMax.y;

				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y > m_ePointDesc->fMax.y)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_ePointDesc->fMin.y;
			}

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Z)
			{
				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.z < m_ePointDesc->fMin.z)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_ePointDesc->fMax.z;

				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.z > m_ePointDesc->fMax.z)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_ePointDesc->fMin.z;
			}

			m_InstanceData[i].fPos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition);
		}
		break;

	case POINTDESC::VIBUFFER_STRIGHT:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_X ||
				m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Z)
			{
				if (m_ePointDesc->bSpread == true)
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				else
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;

				fDistance = XMVectorGetY(XMVector3Length(vMovePos));
				fDistance > m_ePointDesc[i].fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Y)
			{
				if (m_ePointDesc->bSpread == true)
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				else
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;

				fDistance = XMVectorGetY(XMVector3Length(vMovePos));
				fDistance > m_ePointDesc[i].fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}
			m_InstanceData[i].fPos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition);
		}
		break;

	case POINTDESC::VIBUFFER_PLANECIRCLE:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (m_ePointDesc->bSpread == true)
			{
				if (m_ePointDesc->bRotation)
				{
					_vector vMoveDir = XMVector3Cross(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) - XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition));
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + XMVector3Normalize(vMoveDir) *_float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				}
				else
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + XMVector3Normalize(m_ePointDesc[i].vDir) *_float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;

				fDistance = XMVectorGetX(XMVector3Length(vMovePos));
				fDistance > m_ePointDesc->fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}
			else
			{
				if (m_ePointDesc->bRotation)
				{
					_vector vMoveDir = XMVector3Cross(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) - XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition));
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMVector3Normalize(vMoveDir) *_float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				}
				else
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMVector3Normalize(m_ePointDesc[i].vDir) *_float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;

				fDistance = XMVectorGetX(XMVector3Length(vMovePos));
				fDistance < 0.05f ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}
			// 			m_ePointDesc->fDurationTime += m_ePointDesc->fTimeDelta;
			// 			if (m_ePointDesc->fDurationTime > m_ePointDesc->fMaxTime)
			// 			{
			// 				((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(0.0f, 0.f, 0.f, 1.f);
			// 				m_ePointDesc[i].vDir = XMVector3Normalize(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)) * m_ePointDesc->fCreateRange;
			// 				m_ePointDesc[i].vDir = XMVector3TransformNormal(m_ePointDesc[i].vDir, XMMatrixRotationY(XMConvertToRadians(rand() % 360)));
			// 				((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(0.0f, 0.f, 0.f, 1.f) + m_ePointDesc[i].vDir;
			// 				m_ePointDesc->fDurationTime = 0.0f;
			// 			}
			// 			else
			// 				XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			m_InstanceData[i].fPos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition);
		}
		break;

	case POINTDESC::VIBUFFER_CONE:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_X ||
				m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Z)
			{
				if (m_ePointDesc->bSpread == true)
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				else
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;

				fDistance = XMVectorGetY(XMVector3Length(vMovePos));
				fDistance > m_ePointDesc[i].fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}

			if (m_ePointDesc->eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::DIR_Y)
			{
				if (m_ePointDesc->bSpread == true)
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				else
					vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - XMLoadFloat4(&m_ePointDesc[i].vDir) * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;

				fDistance = XMVectorGetY(XMVector3Length(vMovePos));
				fDistance > m_ePointDesc[i].fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}
			m_InstanceData[i].fPos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition);
		}
		break;

	case POINTDESC::VIBUFFER_EXPLOSION:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (m_ePointDesc->bSpread == true)
			{
				vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + m_ePointDesc[i].vExplosionDir * _float(m_InstanceData->pSpeeds) * m_ePointDesc->fTimeDelta;
				fDistance = XMVectorGetX(XMVector3Length(vMovePos));
				fDistance > m_ePointDesc[i].fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}
			else
			{
				vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - m_ePointDesc[i].vExplosionDir * _float(m_InstanceData->pSpeeds) * fTimeDelta;
				fDistance = XMVectorGetX(XMVector3Length(vMovePos));
				fDistance < 0.05f ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
					m_ePointDesc[i].vOriginPos) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vMovePos);
			}
			m_InstanceData[i].fPos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition);
		}
		break;
	}
	m_pContext->Unmap(m_pInstanceBuffer, 0);
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

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

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

CVIBuffer_Point_Instancing * CVIBuffer_Point_Instancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Point_Instancing::Clone(void * pArg, CGameObject * pOwner)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point_Instancing::Free()
{
	__super::Free();

	Safe_Delete_Array(m_InstanceData);
	Safe_Delete_Array(m_ePointDesc);
}
