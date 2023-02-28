#include "stdafx.h"
#include "..\public\Mesh.h"
#include "Model.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
	, m_eType(rhs.m_eType)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_pNonAnimVertices(rhs.m_pNonAnimVertices)
	, m_pAnimVertices(rhs.m_pAnimVertices)
	, m_pIndices(rhs.m_pIndices)
	, m_bLodMesh(rhs.m_bLodMesh)
{
	if (rhs.m_Bones.size() || m_eType == CModel::TYPE_NONANIM)
	{
		m_pBoneNames = new string[rhs.m_iNumBones];
		for (_uint i = 0; i < rhs.m_iNumBones; i++)
		{
			m_pBoneNames[i] = rhs.m_Bones[i]->Get_Name();
		}
	}
}

HRESULT CMesh::Save_Mesh(HANDLE & hFile, DWORD & dwByte)
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
		for (_uint i = 0; i < m_iNumVertices; ++i)
			WriteFile(hFile, &m_pAnimVertices[i], sizeof(VTXANIMMODEL), &dwByte, nullptr);
	}

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
		WriteFile(hFile, &m_pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

	return S_OK;
}

HRESULT CMesh::Save_MeshBones(HANDLE & hFile, DWORD & dwByte)
{
	for (auto& pBone : m_Bones)
		pBone->Save_BoneName(hFile, dwByte);

	return S_OK;
}

HRESULT CMesh::Load_Mesh(HANDLE & hFile, DWORD & dwByte)
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

HRESULT CMesh::Initialize_Prototype(HANDLE hFile, CModel* pModel, _bool bIsLod)
{
	if (hFile == nullptr)
		return S_OK;
	m_bLodMesh = bIsLod;

	_ulong dwByte = 0;
	ReadFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
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

#pragma region VERTEX_BUFFER
	HRESULT	hr = 0;
	if (CModel::TYPE_NONANIM == m_eType)
		hr = Ready_VertexBuffer_NonAnimModel(hFile, pModel);
	else if (CModel::TYPE_ANIM == m_eType)
		hr = Ready_VertexBuffer_AnimModel(hFile, pModel);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER

	if (m_bLodMesh == true)
	{
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.StructureByteStride = 0;
		m_BufferDesc.MiscFlags = 0;

		m_pIndices = new FACEINDICES32[m_iNumPrimitive];
		ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
		ReadFile(hFile, m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive, &dwByte, nullptr);

		ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
		m_SubResourceData.pSysMem = m_pIndices;

		if (FAILED(__super::Create_IndexBuffer()))
			return E_FAIL;
	}
	else
	{
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.StructureByteStride = 0;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;

		m_pIndices = new FACEINDICES32[m_iNumPrimitive];
		ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
		ReadFile(hFile, m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive, &dwByte, nullptr);

		ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
		m_SubResourceData.pSysMem = m_pIndices;

		if (FAILED(__super::Create_IndexBuffer()))
			return E_FAIL;
#pragma endregion
	}
	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
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

HRESULT CMesh::SetUp_BonePtr(CModel* pModel)
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

HRESULT CMesh::SetUp_BonePtr(HANDLE & hFile, DWORD & dwByte, CModel * pModel)
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

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(HANDLE hFile, CModel* pModel)
{
	if (m_bLodMesh == true)
	{
		m_iMaterialIndex;
		m_iNumVertexBuffers = 1;
		m_iNumVertices;
		m_iNumPrimitive;
		m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
		m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
		m_iNumIndicesPerPrimitive = 3;
		//m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
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

	for (_uint i = 0; i < m_iNumVertices; ++i)			// pVertices  3개-> 1개로
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

HRESULT CMesh::Ready_VertexBuffer_AnimModel(HANDLE hFile, CModel* pModel)
{
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


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext, HANDLE hFile, CModel* pModel, _bool bIsLod)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel, bIsLod)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg, CGameObject * pOwner)
{
	CMesh* pInstance = new CMesh(*this);
	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	if (m_isCloned == false)
	{
		Safe_Delete_Array(m_pNonAnimVertices);
		Safe_Delete_Array(m_pAnimVertices);
		Safe_Delete_Array(m_pIndices);
	}
}
