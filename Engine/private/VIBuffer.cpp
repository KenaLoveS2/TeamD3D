#include "stdafx.h"
#include "..\public\VIBuffer.h"
#include "PhysX_Manager.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iStride(rhs.m_iStride)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iIndicesSizePerPrimitive(rhs.m_iIndicesSizePerPrimitive)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_iNumIndices(rhs.m_iNumIndices)
	,m_pPxVertices(rhs.m_pPxVertices)
	,m_pPxIndicies(rhs.m_pPxIndicies)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg, CGameObject* pOwner)
{
	if (FAILED(__super::Initialize(pArg, pOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점버퍼들을 장치에 바인딩한다.(복수를 바인딩한다.)  */

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB, 			
	};

	_uint					iStrides[] = {
		m_iStride,
	};

	_uint					iOffsets[] = {
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);	

	m_pContext->IASetPrimitiveTopology(m_eTopology);

 	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}



HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr != m_pVB)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVB);	
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr != m_pIB)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	// kbj physx
	if (m_isCloned == false)
	{
		Safe_Delete_Array(m_pPxVertices);
		Safe_Delete_Array(m_pPxIndicies);
	}
}

// kbj physx
HRESULT CVIBuffer::Create_PxTriangleMeshActor(PX_USER_DATA* pUserData)
{	
	if (m_pPxActor) return E_FAIL;

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	PxTriangleMeshDesc TriangleMeshDesc;
	TriangleMeshDesc.points.count = m_iNumVertices;
	TriangleMeshDesc.points.stride = sizeof(PxVec3);
	TriangleMeshDesc.points.data = m_pPxVertices;
	TriangleMeshDesc.triangles.count = m_iNumPrimitive;
	TriangleMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	TriangleMeshDesc.triangles.data = m_pPxIndicies;
		
	m_pPxActor = pPhysX->Create_TriangleMeshActor_Static(TriangleMeshDesc, pUserData);

	return S_OK;
}

void CVIBuffer::Set_PxPosition(_float3 vPos)
{
	if (m_pPxActor == nullptr) return;

	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	pPhysX->Set_ActorPosition(m_pPxActor, vPos);	
}

void CVIBuffer::Set_PxMatrix(_float4x4& Matrix)
{
	if (m_pPxActor == nullptr) return;
	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	pPhysX->Set_ActorMatrix(m_pPxActor, Matrix);
}