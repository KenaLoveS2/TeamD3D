#include "stdafx.h"
#include "..\public\VIBuffer_Point_Instancing.h"
#include "Utile.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
	ZeroMemory(&m_ePointDesc, sizeof(POINTDESC));
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_pSpeeds(rhs.m_pSpeeds)
	, m_pVerices(rhs.m_pVerices)
	, m_SpeedMinMax(rhs.m_SpeedMinMax)
	, m_ePointDesc(rhs.m_ePointDesc)
{
}

HRESULT CVIBuffer_Point_Instancing::Set_ShapePosition(POINTDESC* ePointDesc)
{
	_float fDeg1 = 0.0f, fDeg2 = 0.0f, fRangeX = 0.0f, fRangeY = 0.0f, fRangeZ = 0.0f;

	_float fMoveRate = 0.5f, fStopRate = 0.35f;
	_uint  iFloorCount = 0, iFloorInstCount = 0, iCurFloor = 0;
	//_float fRange = 0.f, fMin = 0.f;

	if (ePointDesc->eShapeType == CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_CONE)
	{
		iFloorCount = (_uint)(ePointDesc->fRangeY / ePointDesc->fRangeOffset);
		iFloorInstCount = m_iNumInstance / iFloorCount;
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	switch (ePointDesc->eShapeType)
	{
	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_BOX:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			_float4 fRandomPos = CUtile::Get_RandomVector(ePointDesc->fMin, ePointDesc->fMax);
			fRandomPos.w = 1.f;

			((VTXMATRIX*)SubResource.pData)[i].vPosition = fRandomPos;
		}
		ePointDesc->eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_BOX;
		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_SPHERE:
// 		for (_uint i = 0; i < m_iNumInstance; ++i)
// 		{
// 			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
// 			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
// 			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
// 
// 			fDeg1 = CUtile::Get_RandomFloat(0.f, 180.f);
// 			fDeg2 = CUtile::Get_RandomFloat(0.f, 360.f);
// 			fRangeX = CUtile::Get_RandomFloat(0.25f, 0.65f);
// 			fRangeY = CUtile::Get_RandomFloat(0.25f, 0.65f);
// 			fRangeZ = CUtile::Get_RandomFloat(0.25f, 0.65f);
// 
// 			((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(
// 				ePointDesc.fChargeRange * fRangeX * sinf(XMConvertToRadians(fDeg1)) * cosf(XMConvertToRadians(fDeg2)),
// 				ePointDesc.fChargeRange * fRangeY * cosf(XMConvertToRadians(fDeg1)),
// 				ePointDesc.fChargeRange * fRangeZ * sinf(XMConvertToRadians(fDeg1)) * sinf(XMConvertToRadians(fDeg2)), 1.f);
// 
// 			i -= (XMVectorGetX(XMVector3Length(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition))) == 0.f);
// 		}
// 		ePointDesc.eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_SPHER
		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_CONE:
// 		iFloorCount = (_uint)(ePointDesc.fRangeY / ePointDesc.fRangeOffset);
// 		iFloorInstCount = m_iNumInstance / iFloorCount;
// 
// 		for (_uint i = 0; i < m_iNumInstance; ++i)
// 		{
// 			fMin = ePointDesc.fRangeOffset * iCurFloor;
// 			fRange = ePointDesc.fRangeOffset * (iCurFloor + 1);
// 
// 			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
// 			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
// 			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
// 			((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(0.f, CUtile::Get_RandomFloat(0.f, m_ePointDesc.fMinY), 0.f, 1.f);
// 
// 			iCurFloor = i / iFloorInstCount;
// 		}
// 		iFloorInstCount = m_iNumInstance / iFloorCount;
// 		iCurFloor = 0;
// 
// 		ePointDesc.eShapeType = CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_CON	
		break;

	case CVIBuffer_Point_Instancing::tagPointDesc::VIBUFFER_EXPLOSION:
#pragma region TestExplosion
		/*
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = cos(XMConvertToRadians(fDegree)) * ePointDesc.fRange;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = sin(XMConvertToRadians(fDegree)) * ePointDesc.fRange;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = CUtile::Get_RandomFloat(ePointDesc.fRange * -0.25f, ePointDesc.fRange * 0.25f);

			fDegree += fDegreeOffset;
			ePointDesc.fRange = (ePointDesc.fRange - fRangeOffset) < 0 ? ePointDesc.fRange : (ePointDesc.fRange - fRangeOffset);
			fSizeRate = (fSizeRate - fSizeRateOffset) < 0 ? 1.f : (fSizeRate - fSizeRateOffset);
		}
		*/
#pragma endregion TestExplosion

		// _float3 vPos = m_ePointDesc.vOriginPos;
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			ePointDesc[i].fRangeOffset = ePointDesc[i].fRange / (_float)m_iNumInstance;
			ePointDesc[i].fDegree = rand() % (360 * 3) * 1.0f;
			ePointDesc[i].fDegreeOffset = (360.f * 3.f) / (_float)m_iNumInstance;

			ePointDesc[i].fSizeRateOffset = 1.f / (_float)m_iNumInstance;
			ePointDesc[i].vExplosionDir = XMVector4Normalize(XMVectorSet(CUtile::Get_RandomFloat(-5.f, 5.f), CUtile::Get_RandomFloat(-5.f, 5.f), CUtile::Get_RandomFloat(-5.f, 5.f), 0.f));
			memcpy(m_ePointDesc, ePointDesc, sizeof(POINTDESC));

			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);

			// ((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(vPos.x, vPos.y, vPos.z, 1.f);
// 			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = cos(XMConvertToRadians(ePointDesc.fDegree)) * ePointDesc.fRange;
// 			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = sin(XMConvertToRadians(ePointDesc.fDegree)) * ePointDesc.fRange;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = CUtile::Get_RandomFloat(ePointDesc[i].fRange * -0.25f, ePointDesc[i].fRange * 0.25f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = CUtile::Get_RandomFloat(ePointDesc[i].fRange * -0.25f, ePointDesc[i].fRange * 0.25f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = CUtile::Get_RandomFloat(ePointDesc[i].fRange * -0.25f, ePointDesc[i].fRange * 0.25f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.w = 1.f;

			ePointDesc[i].fDegree += ePointDesc[i].fDegreeOffset;
			ePointDesc[i].fRange = (ePointDesc[i].fRange - ePointDesc[i].fRangeOffset) < 0 ? ePointDesc[i].fRange : (ePointDesc[i].fRange - ePointDesc[i].fRangeOffset);
			ePointDesc[i].fSizeRate = (ePointDesc[i].fSizeRate - ePointDesc[i].fSizeRateOffset) < 0 ? 1.f : (ePointDesc[i].fSizeRate - ePointDesc[i].fSizeRateOffset);
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
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

void CVIBuffer_Point_Instancing::Set_RandomSpeeds(_double fMin, _double fMax)
{
	for (_uint i = 0; i < m_iNumInstance; i++)
		m_pSpeeds[i] = CUtile::Get_RandomFloat((_float)fMin, (_float)fMax);

	m_SpeedMinMax = _float2((_float)fMin, (_float)fMax);
}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pSpeeds = new _double[iNumInstance];

	m_ePointDesc = new POINTDESC[iNumInstance];
	ZeroMemory(m_ePointDesc, sizeof(POINTDESC) * iNumInstance);

	m_iNumInstance = iNumInstance;
	Set_RandomSpeeds(0.5, 2.0);

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

	m_pVerices = new VTXPOINT;
	ZeroMemory(m_pVerices, sizeof(VTXPOINT));

	m_pVerices->vPosition = _float3(0.0f, 0.0f, 0.0f);
	m_pVerices->vPSize = _float2(0.2f, 0.2f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVerices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	//Safe_Delete_Array(pVertices);

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
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(_float(rand() % 5), 3.0f, _float(rand() % 5), 1.f);

		m_ePointDesc[i].vInitPos = pInstanceVertices[i].vPosition;
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

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	switch (m_ePointDesc[0].eShapeType)
	{
	case POINTDESC::VIBUFFER_BOX:

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			vNormalLook = XMVector3Normalize(m_ePointDesc[i].vDir);

			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition,
				XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) + vNormalLook * _float(m_pSpeeds[i]) * fTimeDelta);

			if (m_ePointDesc[0].eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::ROT_X)
			{
				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.x > m_ePointDesc[i].vInitPos.x * m_ePointDesc[i].fRange)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_ePointDesc[i].vInitPos.x;
				else if (((VTXMATRIX*)SubResource.pData)[i].vPosition.x < m_ePointDesc[i].vInitPos.x * m_ePointDesc[i].fRange * -1.f)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = m_ePointDesc[i].vInitPos.x;
			}
			else if (m_ePointDesc[0].eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::ROT_Y)
			{
				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y >m_ePointDesc[i].vInitPos.y * m_ePointDesc[i].fRange)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.y =m_ePointDesc[i].vInitPos.y;
				else if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y < m_ePointDesc[i].vInitPos.y * m_ePointDesc[i].fRange * -1.f)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_ePointDesc[i].vInitPos.y;
			}

			else if (m_ePointDesc[0].eRotXYZ == CVIBuffer_Point_Instancing::tagPointDesc::ROT_Z)
			{
				if (((VTXMATRIX*)SubResource.pData)[i].vPosition.z > m_ePointDesc->vInitPos.z * m_ePointDesc->fRange)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_ePointDesc->vInitPos.z;
				else if (((VTXMATRIX*)SubResource.pData)[i].vPosition.z < m_ePointDesc->vInitPos.z * m_ePointDesc->fRange * -1.f)
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = m_ePointDesc->vInitPos.z;
			}
		}
		//Box_Tick(SubResource, fTimeDelta);
		break;

	case POINTDESC::VIBUFFER_SPHERE:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{

		}
		break;

	case POINTDESC::VIBUFFER_CONE:
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{

		}
		break;

	case POINTDESC::VIBUFFER_EXPLOSION:

// 		for (_uint i = 0; i < m_iNumInstance; ++i)
// 		{
// 			if (m_ePointDesc.bIsAlive)
// 			{
// 				_vector vPos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition);
// 				vPos += XMLoadFloat4(&vVelocity) * fTimeDelta;
// 				XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition, vPos);
// 				m_ePointDesc.fDurationTime += fTimeDelta;
// 
// 				// 죽임
// 				if (m_ePointDesc.fDurationTime > m_ePointDesc.fLifeTime)
// 					m_ePointDesc.bIsAlive = false;
// 			}
// 		}

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			vMovePos = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i + m_iNumInstance].vPosition) + m_ePointDesc[i].vExplosionDir * _float(m_pSpeeds[i]) * fTimeDelta;
			fDistance = XMVectorGetX(XMVector3Length(vMovePos));
			fDistance > m_ePointDesc[i].fRange ? XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i + m_iNumInstance].vPosition,
				XMVectorSet(0.f, 0.f, 0.f, 1.f)) : XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i + m_iNumInstance].vPosition, vMovePos);

			//((VTXMATRIX*)SubResource.pData)[i].vPosition.x += _float(m_pSpeeds[i] * fTimeDelta);
			//((VTXMATRIX*)SubResource.pData)[i].vPosition.y += _float(m_pSpeeds[i] * fTimeDelta);
			//((VTXMATRIX*)SubResource.pData)[i].vPosition.z += _float(m_pSpeeds[i] * fTimeDelta);

			//if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y < m_ePointDesc.fRange)
			//	((VTXMATRIX*)SubResource.pData)[i].vPosition = m_ePointDesc.vInitPos;
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

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pVerices);
		Safe_Delete_Array(m_ePointDesc);
	}
}
