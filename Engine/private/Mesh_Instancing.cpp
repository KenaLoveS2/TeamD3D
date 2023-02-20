#include "..\public\Mesh_Instancing.h"
#include "Model_Instancing.h"
#include "Bone_Instacing.h"
#include "GameInstance.h"

CMeshInstancing::CMeshInstancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CMeshInstancing::CMeshInstancing(const CMeshInstancing & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_Bones(rhs.m_Bones)
	, m_iOriginNumPrimitive(rhs.m_iOriginNumPrimitive)
	, m_pIndices(rhs.m_pIndices)
	, m_pVertices(rhs.m_pVertices)
	, m_iIncreaseInstancingNumber(rhs.m_iIncreaseInstancingNumber)
	, m_pPosition(rhs.m_pPosition)
{
	strcpy_s(m_szName, MAX_PATH,rhs.m_szName);
}

void CMeshInstancing::Add_Position(_float4 vPos,_uint iIndex)
{
	m_pPosition.push_back(vPos);
	Map_UnMapViBuffer(iIndex);
}

void CMeshInstancing::Set_Position(vector<_float4>& vTreePos)
{
	m_pPosition.clear();

	_uint iSize = (_uint)vTreePos.size();
	
	for (_uint i = 0; i < iSize; ++i)
	{
		m_pPosition.push_back(vTreePos[i]);
	}
	
	Map_UnMapViBuffer(iSize);
}

HRESULT CMeshInstancing::Initialize_Prototype(CModel_Instancing* pModel, HANDLE hFile, _uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pPosition.push_back(_float4(0.f, 0.f, 0.f, 1.f));

	DWORD   dwByte = 0;
	_uint	iType = 0;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	m_iOriginNumPrimitive = m_iNumPrimitive;
	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 3 * m_iNumPrimitive;

	m_iMaterialIndex = m_iMaterialIndex;
	
	m_iNumVertexBuffers = 2;
	m_iNumVertices = m_iNumVertices;
	m_iNumPrimitive = m_iNumPrimitive * iNumInstance;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;			
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
	
	Ready_VertexBuffer_NonAnimModel(hFile, pModel);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	 m_pIndices = new FACEINDICES32[m_iOriginNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32));

	for (_uint i = 0; i < m_iOriginNumPrimitive; ++i)
	{
		ReadFile(hFile, &m_pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);
	}
	
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;
	_uint		iNumOriginFaces = 0;
	for (_uint i = 0; i < iNumInstance; ++i)			
	{
		pIndices[iNumFaces]._0 = m_pIndices[iNumOriginFaces]._0;
		pIndices[iNumFaces]._1 = m_pIndices[iNumOriginFaces]._1;
		pIndices[iNumFaces]._2 = m_pIndices[iNumOriginFaces]._2;
		++iNumFaces;
		++iNumOriginFaces;

		pIndices[iNumFaces]._0 = m_pIndices[iNumOriginFaces]._0;
		pIndices[iNumFaces]._1 = m_pIndices[iNumOriginFaces]._1;
		pIndices[iNumFaces]._2 = m_pIndices[iNumOriginFaces]._2;
		++iNumFaces;
		++iNumOriginFaces;

		if (iNumOriginFaces == m_iOriginNumPrimitive)
			iNumOriginFaces = 0;
				
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					//Rock UnLock을 하겠다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		//Rock UnLock을 하겠다.
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);				// 버퍼 하나의 행렬을 만들어서 쉐이더에게 전달해줘야한다.
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(_float(rand() % 5), 0.f, _float(rand() % 5), 1.f);		// 나중에 인스터닝을 할때 이포지션을 움직일 수 있게 락 언락구조를 짜야한다.
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);
	return S_OK;

}

HRESULT CMeshInstancing::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMeshInstancing::Tick(_double TimeDelta)
{
	
	return S_OK;
}



HRESULT CMeshInstancing::Render()
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

	CONTEXT_LOCK;
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);			// 인스턴싱을 위한  DX_ 함수 제공 

	return S_OK;
}

void CMeshInstancing::Map_UnMapViBuffer(_uint iNumInstance)
{
	CONTEXT_LOCK;
	m_iNumInstance = iNumInstance;
	m_iNumPrimitive = m_iOriginNumPrimitive * m_iNumInstance;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;
	_uint		iNumOriginFaces = 0;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumFaces]._0 = m_pIndices[iNumOriginFaces]._0;
		pIndices[iNumFaces]._1 = m_pIndices[iNumOriginFaces]._1;
		pIndices[iNumFaces]._2 = m_pIndices[iNumOriginFaces]._2;
		++iNumFaces;
		++iNumOriginFaces;

		pIndices[iNumFaces]._0 = m_pIndices[iNumOriginFaces]._0;
		pIndices[iNumFaces]._1 = m_pIndices[iNumOriginFaces]._1;
		pIndices[iNumFaces]._2 = m_pIndices[iNumOriginFaces]._2;
		++iNumFaces;
		++iNumOriginFaces;

		if (iNumOriginFaces == m_iOriginNumPrimitive)
			iNumOriginFaces = 0;

	}


	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;
	Safe_Release(m_pIB);
	m_pIB = nullptr;
	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pIB);
	Safe_Delete_Array(pIndices);


	//D3D11_MAPPED_SUBRESOURCE		SubResources;
	//ZeroMemory(&SubResources, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResources);
	//memcpy(SubResources.pData, pIndices, sizeof(FACEINDICES32) * iNumFaces);
	//m_pContext->Unmap(m_pIB, 0);
	//Safe_Delete_Array(pIndices);




	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					//Rock UnLock을 하겠다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		//Rock UnLock을 하겠다.
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX)* m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);				// 버퍼 하나의 행렬을 만들어서 쉐이더에게 전달해줘야한다.
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = m_pPosition[i];
		//pInstanceVertices[i].vPosition = _float4(m_pPosition[i].x+ 0.5f, m_pPosition[i].y  , m_pPosition[i].z+ 0.4f,1.f);		// 나중에 인스터닝을 할때 이포지션을 움직일 수 있게 락 언락구조를 짜야한다.
	}
	

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;
	Safe_Release(m_pInstanceBuffer);
	m_pInstanceBuffer = nullptr;
	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);
	Safe_Delete_Array(pInstanceVertices);
}

void CMeshInstancing::LoadFile(HANDLE hFile, CModel_Instancing* pModel)
{
	DWORD dwByte = 0;

	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		char pName[MAX_PATH] = "";

		ReadFile(hFile, pName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		CBone_Instacing*	pBone = pModel->Get_BonePtr(pName);
		if (nullptr == pBone)
			assert("CMesh::LoadFile");

		_float4x4 OffSetMatrix;
		ReadFile(hFile, &OffSetMatrix, sizeof(_float4x4), &dwByte, nullptr);
		pBone->Set_OffsetMatrix(OffSetMatrix);

		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);
	}

}

void CMeshInstancing::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	_uint		iNumBones = 0;

	if (0 == m_iNumBones)
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());

	for (auto& pBone : m_Bones)
	{
		// BoneMatrix = 오프셋매트릭스 * 컴바인드매트릭스;
		XMStoreFloat4x4(&pBoneMatrices[iNumBones++],
			pBone->Get_OffsetMatrix()
			* pBone->Get_CombindMatrix()
			* PivotMatrix);
	}
}

HRESULT CMeshInstancing::Ready_VertexBuffer_NonAnimModel(HANDLE hFile,  CModel_Instancing* pModel)
{
	DWORD   dwByte = 0;

	m_iStride = sizeof(VTXMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;


	m_pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMODEL));

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(hFile, &m_pVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);
	}
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	

	return S_OK;
}

CMeshInstancing * CMeshInstancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel_Instancing* pModel, HANDLE hFile, _uint iNumInstance )
{
	CMeshInstancing*		pInstance = new CMeshInstancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel,hFile,iNumInstance)))
	{
		MSG_BOX("Failed to Created : CMeshInstancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshInstancing::Clone(void * pArg)
{
	CMeshInstancing*		pInstance = new CMeshInstancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeshInstancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshInstancing::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pIndices);	

	m_pPosition.clear();
		
}