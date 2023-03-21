#include "stdafx.h"
#include "..\public\Instancing_Mesh.h"
#include "Model.h"
#include "Bone.h"
#include "GameInstance.h"

CInstancing_Mesh::CInstancing_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer_Instancing(pDevice, pContext)
{
}

CInstancing_Mesh::CInstancing_Mesh(const CInstancing_Mesh & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_eType(rhs.m_eType)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_pNonAnimVertices(rhs.m_pNonAnimVertices)
	, m_pAnimVertices(rhs.m_pAnimVertices)
	, m_pIndices(rhs.m_pIndices)
	, m_bLodMesh(rhs.m_bLodMesh)
	, m_iOriginNumPrimitive(rhs.m_iOriginNumPrimitive)
	, m_iIncreaseInstancingNumber(rhs.m_iIncreaseInstancingNumber)
	, m_pInstancingPositions(rhs.m_pInstancingPositions)
	, m_iNumInstance_Origin(rhs.m_iNumInstance_Origin)
	
{

	m_Bones.reserve(rhs.m_Bones.size());

	if (rhs.m_Bones.size() || m_eType == CModel::TYPE_NONANIM)
	{
		m_pBoneNames = new string[rhs.m_iNumBones];
		for (_uint i = 0; i < rhs.m_iNumBones; i++)
		{
			m_pBoneNames[i] = rhs.m_Bones[i]->Get_Name();
		}
	}
}

HRESULT CInstancing_Mesh::Save_Mesh(HANDLE & hFile, DWORD & dwByte)
{
	if (m_eType == CModel::TYPE_END)
		return E_FAIL;

	WriteFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
	WriteFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iStride, sizeof(_uint), &dwByte, nullptr);

	if (m_eType == CModel::TYPE_NONANIM)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
			WriteFile(hFile, &m_pNonAnimVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);
	}
	else if (m_eType == CModel::TYPE_ANIM)
	{
		assert(!"this type Only Non_Anim");
	}

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
		WriteFile(hFile, &m_pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

	return S_OK;
}

HRESULT CInstancing_Mesh::Save_MeshBones(HANDLE & hFile, DWORD & dwByte)
{
	for (auto& pBone : m_Bones)
		pBone->Save_BoneName(hFile, dwByte);

	return S_OK;
}

HRESULT CInstancing_Mesh::Load_Mesh(HANDLE & hFile, DWORD & dwByte)
{
	if (m_eType == CModel::TYPE_END)
		return E_FAIL;

	ReadFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iStride, sizeof(_uint), &dwByte, nullptr);

	m_iNumVertexBuffers = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; //
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	if (m_eType == CModel::TYPE_NONANIM)
	{
		m_pNonAnimVertices = new VTXMODEL[m_iNumVertices];

		for (_uint i = 0; i < m_iNumVertices; ++i)
			ReadFile(hFile, &m_pNonAnimVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);

		ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		m_SubResourceData.pSysMem = m_pNonAnimVertices;
	}
	else if (m_eType == CModel::TYPE_ANIM)
	{
		m_pAnimVertices = new VTXANIMMODEL[m_iNumVertices];

		for (_uint i = 0; i < m_iNumVertices; ++i)
			ReadFile(hFile, &m_pAnimVertices[i], sizeof(VTXANIMMODEL), &dwByte, nullptr);

		ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		m_SubResourceData.pSysMem = m_pAnimVertices;
	}



	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
		ReadFile(hFile, &m_pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

void CInstancing_Mesh::Add_InstanceModel(vector<_float4x4*>	VecInstancingMatrix)
{
	m_iNumInstance = m_iNumInstance_Origin =(_uint)VecInstancingMatrix.size();
	m_iNumPrimitive = m_iOriginNumPrimitive * m_iNumInstance;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	/*Instancing_ Mesh*/
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		for (_uint j = 0; j < m_iOriginNumPrimitive; ++j)
		{
			pIndices[iNumFaces]._0 = m_pIndices[j]._0;
			pIndices[iNumFaces]._1 = m_pIndices[j]._1;
			pIndices[iNumFaces]._2 = m_pIndices[j]._2;
			++iNumFaces;
		}
	}

	Safe_Release(m_pIB);
	m_pIB = nullptr;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		assert(!"Instancing Create Issue");

	Safe_Delete_Array(pIndices);

#pragma endregion
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					//Rock UnLock을 하겠다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		//Rock UnLock을 하겠다.
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX)*m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		memcpy(&pInstanceVertices[i].vRight, &VecInstancingMatrix[i]->m[0],sizeof(_float4));
		memcpy(&pInstanceVertices[i].vUp, &VecInstancingMatrix[i]->m[1], sizeof(_float4));
		memcpy(&pInstanceVertices[i].vLook, &VecInstancingMatrix[i]->m[2], sizeof(_float4));
		memcpy(&pInstanceVertices[i].vPosition, &VecInstancingMatrix[i]->m[3], sizeof(_float4));
	}
	Safe_Release(m_pInstanceBuffer);
	m_pInstanceBuffer = nullptr;
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);


	Safe_Delete_Array(pInstanceVertices);
}

void CInstancing_Mesh::InstBuffer_Update(vector<_float4x4*> VecInstancingMatrix)
{
	/*메쉬_인스턴싱_이펙트 일때 여기 사용*/
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vRight, &VecInstancingMatrix[i]->m[0], sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vUp, &VecInstancingMatrix[i]->m[1], sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vLook, &VecInstancingMatrix[i]->m[2], sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vPosition, &VecInstancingMatrix[i]->m[3], sizeof(_float4));
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

}

void CInstancing_Mesh::InstBufferSize_Update(_int iSize)
{
	/*메쉬_인스턴싱_이펙트 일때 여기 사용*/
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float4 vRight = ((VTXMATRIX*)SubResource.pData)[i].vRight;
		_float4 vUp = ((VTXMATRIX*)SubResource.pData)[i].vUp;
		_float4 vLook = ((VTXMATRIX*)SubResource.pData)[i].vLook;

		XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4(&vRight)) * (_float)iSize);
		XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4(&vUp)) * (_float)iSize);
		XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4(&vLook)) *(_float)iSize);

		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vRight, &vRight, sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vUp, &vUp, sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[i].vLook, &vLook, sizeof(_float4));
	

	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CInstancing_Mesh::Set_PxTriangle(vector<_float4x4*> VecInstancingMatrix)
{

}

HRESULT CInstancing_Mesh::Initialize_Prototype(HANDLE hFile, CModel * pModel, _bool bIsLod,
	_bool bUseTriangleMeshActor, _uint iNumInstance)
{
	if (hFile == nullptr)
		return S_OK;
	m_bLodMesh = bIsLod;			// is_Lod Or NonLod????

	_ulong dwByte = 0;
	ReadFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	m_Bones.reserve(m_iNumBones);
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		_uint iLen = 0;
		ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
		char szBoneName[MAX_PATH] = { 0, };
		ReadFile(hFile, szBoneName, iLen + 1, &dwByte, nullptr);

		CBone* pBone = pModel->Get_BonePtr(szBoneName);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);
	}

	ReadFile(hFile, &m_iStride, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iIndicesSizePerPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumIndicesPerPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertexBuffers, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_eIndexFormat, sizeof(DXGI_FORMAT), &dwByte, nullptr);
	ReadFile(hFile, &m_eTopology, sizeof(D3D11_PRIMITIVE_TOPOLOGY), &dwByte, nullptr);

	m_pInstancingPositions.push_back(_float4(0.f, 0.f, 0.f, 1.f));			//Test 용 위치잡기
	m_pInstancingPositions.push_back(_float4(0.f, 0.f, 4.f, 1.f));

	m_iOriginNumPrimitive = m_iNumPrimitive;
	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 3 * m_iNumPrimitive;
	m_iNumVertexBuffers = 2;
	m_iNumPrimitive = m_iNumPrimitive * iNumInstance;
	

#pragma region VERTEX_BUFFER
	HRESULT	hr = 0;
	if (CModel::TYPE_NONANIM == m_eType)
		hr = Ready_VertexBuffer_NonAnimModel(hFile, pModel);
	else 
		hr = Ready_VertexBuffer_AnimModel(hFile, pModel);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	/*Origin _Mesh*/
	m_pIndices = new FACEINDICES32[m_iOriginNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32)*m_iOriginNumPrimitive);
	ReadFile(hFile, m_pIndices, sizeof(FACEINDICES32) * m_iOriginNumPrimitive, &dwByte, nullptr);

#pragma region Instancing_INDEX_BUFFER

	/*Instancing_ Mesh*/
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		for (_uint j = 0; j < m_iOriginNumPrimitive; ++j)
		{
			pIndices[iNumFaces]._0 = m_pIndices[j]._0;
			pIndices[iNumFaces]._1 = m_pIndices[j]._1;
			pIndices[iNumFaces]._2 = m_pIndices[j]._2;
			++iNumFaces;
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					//Rock UnLock을 하겠다.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		//Rock UnLock을 하겠다.
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX)*iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);				// 버퍼 하나의 행렬을 만들어서 쉐이더에게 전달해줘야한다.
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);		// 나중에 인스터닝을 할때 이포지션을 움직일 수 있게 락 언락구조를 짜야한다.
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;
	Safe_Delete_Array(pInstanceVertices);

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	
#pragma endregion
	if (bUseTriangleMeshActor)
	{
		if (m_pPxVertices == nullptr)
		{
			m_pPxVertices = new PxVec3[m_iNumVertices];
			ZeroMemory(m_pPxVertices, sizeof(PxVec3) * m_iNumVertices);

			for (_uint i = 0; i < m_iNumVertices; i++)
			{
				_float3 vPos = m_pNonAnimVertices[i].vPosition;
				m_pPxVertices[i] = CUtile::ConvertPosition_D3DToPx(vPos);
			}
		}

		if (m_pPxIndicies == nullptr)
		{
			m_pPxIndicies = new PxIndicies[m_iOriginNumPrimitive];
			ZeroMemory(m_pPxIndicies, sizeof(PxIndicies) * m_iOriginNumPrimitive);
			for (_uint i = 0; i < m_iOriginNumPrimitive; i++)
			{
				m_pPxIndicies[i]._0 = m_pIndices[i]._0;
				m_pPxIndicies[i]._1 = m_pIndices[i]._2;
				m_pPxIndicies[i]._2 = m_pIndices[i]._1;
			}
		}
		
	}

	return S_OK;
}

HRESULT CInstancing_Mesh::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

HRESULT CInstancing_Mesh::Tick(_float fTimeDelta)
{
	///*메쉬_인스턴싱_이펙트 일때 여기 사용*/
	//
	//D3D11_MAPPED_SUBRESOURCE			SubResource;
	//ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//for (_uint i = 0; i < m_iNumInstance; ++i)
	//{
	//	((VTXMATRIX*)SubResource.pData)[i].vPosition.y += _float(m_pSpeeds[i]) * fTimeDelta;

	//	if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y > 3.f)
	//		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
	//}

	//m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CInstancing_Mesh::Render()
{
	if (nullptr == m_pContext )
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

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);			// 인스턴싱을 위한  DX_ 함수 제공 

	m_iNumInstance = m_iNumInstance_Origin;

	return S_OK;

}

_int CInstancing_Mesh::Culling_InstancingMesh(_float fCameraDistanceLimit, vector<_float4x4*> InstanceMatrixVec, _fmatrix ParentMat)
{
	_float4 vCamPos	=	CGameInstance::GetInstance()->Get_CamPosition();

	list<_float4x4> InstPos;
	_float fCameDistance = 0.f;
	_float4 vTransPos;
	
	for (auto OriginPos : InstanceMatrixVec)
	{
		
		_matrix matTransWorld = XMLoadFloat4x4(OriginPos)* ParentMat;

		memcpy(&vTransPos, &matTransWorld.r[3], sizeof(_float4));
		
		_vector	 vDir =	XMLoadFloat4(&(vCamPos - vTransPos));

		fCameDistance  = XMVectorGetX(XMVector3Length(vDir));

		if (fCameDistance < fCameraDistanceLimit)
		{
			InstPos.push_back(*OriginPos);
		}
	}

	m_iNumInstance = (_int)InstPos.size();
	
	if (m_iNumInstance == 0)
		return 0;
	
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_int iIndex = 0;
	for (auto &Iter : InstPos)
	{
		memcpy(&((VTXMATRIX*)SubResource.pData)[iIndex].vRight, &Iter.m[0], sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[iIndex].vUp, &Iter.m[1], sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[iIndex].vLook, &Iter.m[2], sizeof(_float4));
		memcpy(&((VTXMATRIX*)SubResource.pData)[iIndex++].vPosition, &Iter.m[3], sizeof(_float4));
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	InstPos.clear();

	return 1;

}

void CInstancing_Mesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
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

_bool CInstancing_Mesh::Instaincing_MoveControl(CEnviromentObj::CHAPTER eChapterGimmcik,_float fTimeDelta)
{
	if (eChapterGimmcik == CEnviromentObj::CHAPTER::Gimmick_TYPE_GO_UP)
	{
		D3D11_MAPPED_SUBRESOURCE			SubResource;
		ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		_int iFinishCnt = 0;

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (m_vecOriginYPos[i] <= ((VTXMATRIX*)SubResource.pData)[i].vPosition.y)
			{
				iFinishCnt += 0;
				continue;
			}
			_float fMaxSpeed = fmax((i + 1)*1.5f, 3.f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (fMaxSpeed) * fTimeDelta;
			iFinishCnt += 1;
		}
		m_pContext->Unmap(m_pInstanceBuffer, 0);
	
	
		if (iFinishCnt == 0)
			return true;
		
	}



	return false;

}

void CInstancing_Mesh::InstaincingMesh_GimmkicInit(CEnviromentObj::CHAPTER eChapterGimmcik)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (eChapterGimmcik == CEnviromentObj::CHAPTER::Gimmick_TYPE_GO_UP)
	{
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_vecOriginYPos.push_back(((VTXMATRIX*)SubResource.pData)[i].vPosition.y);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y -= 10.f;
		}
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CInstancing_Mesh::InstaincingMesh_yPosControl(_float yPos)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = yPos;
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CInstancing_Mesh::Create_PxTriangle_InstMeshActor(CTransform* pParentTransform ,vector<_float4x4*> VecInstancingMatrix)
{
	size_t InstSize = VecInstancingMatrix.size();
	assert(InstSize > 0 && "CInstancing_Mesh::Create_PxTriangle_InstMeshActor");
		
	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	PxTriangleMeshDesc TriangleMeshDesc;
	
	_float4x4 MatPosTrans;
	_float4 vPos, vRight, vUp, vLook, vFloat4Len;
	_float  fXSize, fYSize, fZSize;
	for (size_t i = 0; i < InstSize; ++i)
	{
		ZeroMemory(&TriangleMeshDesc, sizeof(TriangleMeshDesc));
		TriangleMeshDesc.points.count = m_iNumVertices;
		TriangleMeshDesc.points.stride = sizeof(PxVec3);
		TriangleMeshDesc.points.data = m_pPxVertices;
		TriangleMeshDesc.triangles.count = m_iOriginNumPrimitive;
		TriangleMeshDesc.triangles.stride = 3 * sizeof(PxU32);
		TriangleMeshDesc.triangles.data = m_pPxIndicies;
		
		MatPosTrans = *VecInstancingMatrix[i];
		XMStoreFloat4x4(&MatPosTrans, XMLoadFloat4x4(&MatPosTrans) * pParentTransform->Get_WorldMatrix());

		memcpy(&vRight, &MatPosTrans.m[0], sizeof(_float4));
		memcpy(&vUp, &MatPosTrans.m[1], sizeof(_float4));
		memcpy(&vLook, &MatPosTrans.m[2], sizeof(_float4));
		memcpy(&vPos, &MatPosTrans.m[3], sizeof(_float4));

		fXSize = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vRight)));
		fYSize = XMVectorGetY(XMVector4Length(XMLoadFloat4(&vUp)));
		fZSize = XMVectorGetZ(XMVector4Length(XMLoadFloat4(&vLook)));

		XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4(&vRight)));
		XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4(&vUp)));
		XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4(&vLook)));
				
		
		_float4x4 matNew;
		XMStoreFloat4x4(&matNew, XMMatrixIdentity());

		memcpy(&matNew.m[0], &vRight, sizeof(_float4));
		memcpy(&matNew.m[1], &vUp, sizeof(_float4));
		memcpy(&matNew.m[2], &vLook, sizeof(_float4));
		memcpy(&matNew.m[3], &vPos, sizeof(_float4));

	
		PxRigidStatic *pStaticRigid = pPhysX->Create_TriangleMeshActor_Static(TriangleMeshDesc, nullptr
			,0.5f,0.5f, 0.1f,_float3(fXSize,fYSize,fZSize));
		
	
		pPhysX->Set_ActorMatrix(pStaticRigid, (matNew));		// 노말라이즈 매트릭스보내고
	}	
}

HRESULT CInstancing_Mesh::SetUp_BonePtr(CModel * pModel)
{
	if (m_pBoneNames == nullptr)
		return E_FAIL;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		CBone* pBone = pModel->Get_BonePtr(m_pBoneNames[i].c_str());
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);
	}

	Safe_Delete_Array(m_pBoneNames);

	return S_OK;
}

HRESULT CInstancing_Mesh::SetUp_BonePtr(HANDLE & hFile, DWORD & dwByte, CModel * pModel)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_uint		iBoneNameLength = 0;
		ReadFile(hFile, &iBoneNameLength, sizeof(_uint), &dwByte, nullptr);
		char*		pBoneName = new char[iBoneNameLength];
		ReadFile(hFile, pBoneName, sizeof(char) * iBoneNameLength, &dwByte, nullptr);

		CBone*		pBone = pModel->Get_BonePtr(pBoneName);

		Safe_Delete_Array(pBoneName);
		if (pBone == nullptr)
			continue;

		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);
	}

	return S_OK;
}

HRESULT CInstancing_Mesh::Ready_VertexBuffer_NonAnimModel(HANDLE hFile, CModel * pModel)
{
	if (m_bLodMesh == true)
	{
		m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
		m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
		m_iNumIndicesPerPrimitive = 3;
		m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
	}
	else
	{
		m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
		m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
		m_iNumIndicesPerPrimitive = 3;
		m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
	}

	m_iStride = sizeof(VTXMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;


	_matrix	PivotMatrix = pModel->Get_PivotMatrix();
	_ulong dwByte = 0;
	VTXMODEL* pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);
	ReadFile(hFile, pVertices, sizeof(VTXMODEL) * m_iNumVertices, &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pNonAnimVertices = new VTXMODEL[m_iNumVertices];
	memcpy(m_pNonAnimVertices, pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CInstancing_Mesh::Ready_VertexBuffer_AnimModel(HANDLE hFile, CModel * pModel)
{
	if (m_bLodMesh == true)
		assert(!"CInstancing_Mesh::Ready_VertexBuffer_AnimModel");

	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	m_iStride = sizeof(VTXANIMMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ulong dwByte = 0;
	m_pAnimVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);
	ReadFile(hFile, m_pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices, &dwByte, nullptr);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CInstancing_Mesh::Set_up_Instancing()
{
	return S_OK;
}

CInstancing_Mesh * CInstancing_Mesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,
	HANDLE hFile, CModel * pModel, _bool bIsLod, _bool bUseTriangleMeshActor,_uint iNumInstance)
{
	CInstancing_Mesh* pInstance = new CInstancing_Mesh(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel, bIsLod, bUseTriangleMeshActor,iNumInstance )))
	{
		MSG_BOX("Failed to Created : CInstancing_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CInstancing_Mesh::Clone(void * pArg, CGameObject * pOwner)
{
	CInstancing_Mesh* pInstance = new CInstancing_Mesh(*this);
	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInstancing_Mesh::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	if (m_isCloned == false)
	{
		Safe_Delete_Array(m_pAnimVertices);
		Safe_Delete_Array(m_pNonAnimVertices);
		Safe_Delete_Array(m_pIndices);
	}

	m_pInstancingPositions.clear();
}
