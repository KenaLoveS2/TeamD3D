#include "stdafx.h"
#include "..\public\Mesh.h"
#include "Model.h"
#include "Bone.h"
#include "Utile.h"
#include "PhysX_Manager.h"
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

HRESULT CMesh::Initialize_Prototype(HANDLE hFile, CModel* pModel, _bool bIsLod, _bool bUseTriangleMeshActor)
{
	if (hFile == nullptr)
		return S_OK;
	m_bLodMesh = bIsLod;

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

	if (bUseTriangleMeshActor)
	{
		if (m_pPxVertices == nullptr)
		{
			m_pPxVertices = new PxVec3[m_iNumVertices];
			ZeroMemory(m_pPxVertices, sizeof(PxVec3) * m_iNumVertices);

			if (m_eType == CModel::TYPE_ANIM)
			{
				for (_uint i = 0; i < m_iNumVertices; i++)
				{
					_float3 vPos = m_pAnimVertices[i].vPosition;
					m_pPxVertices[i] = CUtile::ConvertPosition_D3DToPx(vPos);
				}
			}
			else if (m_eType == CModel::TYPE_NONANIM)
			{
				for (_uint i = 0; i < m_iNumVertices; i++)
				{
					_float3 vPos = m_pNonAnimVertices[i].vPosition;
					m_pPxVertices[i] = CUtile::ConvertPosition_D3DToPx(vPos);
				}
			}
			else return E_FAIL;
		}

		if (m_pPxIndicies == nullptr)
		{
			m_pPxIndicies = new PxIndicies[m_iNumPrimitive];
			ZeroMemory(m_pPxIndicies, sizeof(PxIndicies) * m_iNumPrimitive);
			for (_uint i = 0; i < m_iNumPrimitive; i++)
			{
				m_pPxIndicies[i]._0 = m_pIndices[i]._0;
				m_pPxIndicies[i]._1 = m_pIndices[i]._2;
				m_pPxIndicies[i]._2 = m_pIndices[i]._1;
			}
		}
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

//  	if (17 == m_iNumBones)
//  	{
//  		//_matrix	matPivot = PivotMatrix * XMMatrixTranslation(0.07f, 0.7f, 0.35f);
//  
//  		for (auto& pBone : m_Bones)
//  		{
//  			// BoneMatrix = 오프셋매트릭스 * 컴바인드매트릭스;
//  			XMStoreFloat4x4(&pBoneMatrices[iNumBones++],
// 				pBone->Get_OffsetMatrix()
// 				* pBone->Get_CombindMatrix()
//  				* PivotMatrix);
//  		}
//  		return;
//  	}

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

HRESULT CMesh::Synchronization_BonePtr(CModel * pModel)
{
	NULL_CHECK_RETURN(pModel, E_FAIL);

	char**		ppBoneName = new char*[m_iNumBones];
	_uint		iLength = 0;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		iLength = (_uint)strlen(m_Bones[i]->Get_Name()) + 1;
		ppBoneName[i] = new char[iLength];
		strcpy_s(ppBoneName[i], iLength, m_Bones[i]->Get_Name());
	}

	//for (auto pBone : m_Bones)
	//	Safe_Release(pBone);

	_uint j = 0;
	CBone*	pRootBone = nullptr;
	for (auto iter = m_Bones.begin(); iter != m_Bones.end();)
	{
		Safe_Release(*iter);

		CBone*	pBone = pModel->Get_BonePtr(ppBoneName[j++]);
		if (pBone == nullptr)
		{
// 			if (pRootBone != nullptr)
// 			{
// 				m_Bones[j - 1]->SetParent(pRootBone);
// 				pRootBone->Add_Child(m_Bones[j - 1]);
// 				pRootBone = nullptr;
// 			}
			iter = m_Bones.erase(iter);
			continue;
		}

		if (iter == m_Bones.begin())
			pRootBone = pBone;

		//Safe_Release(*iter);
		*iter = pBone;
		Safe_AddRef(pBone);

		iter++;
	}

// 	for (_uint i = 0; i < m_iNumBones; ++i)
// 	{
// 		CBone*	pBone = pModel->Get_BonePtr(ppBoneName[i]);
// 		if (pBone == nullptr)
// 			continue;
// 
// 		Safe_Release(m_Bones[i]);
// 		m_Bones[i] = pBone;
// 
// 		//m_Bones.push_back(pBone);
// 		Safe_AddRef(pBone);
// 
// 	}

	for (_uint i = 0; i < m_iNumBones; ++i)
		Safe_Delete_Array(ppBoneName[i]);
	Safe_Delete_Array(ppBoneName);

	m_iNumBones = (_uint)m_Bones.size();

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


void CMesh::Create_PxTriangle_size(const _tchar * szCloneName, CTransform * pTransform, PX_USER_DATA* pUserData)
{
	CPhysX_Manager *pPhysX = CPhysX_Manager::GetInstance();

	PxTriangleMeshDesc TriangleMeshDesc;

	_float4x4 MatPosTrans;
	_float4 vPos, vRight, vUp, vLook, vFloat4Len;
	_float  fXSize, fYSize, fZSize;
	MatPosTrans = pTransform->Get_WorldMatrixFloat4x4();

	ZeroMemory(&TriangleMeshDesc, sizeof(TriangleMeshDesc));
	TriangleMeshDesc.points.count = m_iNumVertices;
	TriangleMeshDesc.points.stride = sizeof(PxVec3);
	TriangleMeshDesc.points.data = m_pPxVertices;
	TriangleMeshDesc.triangles.count = m_iNumPrimitive;
	TriangleMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	TriangleMeshDesc.triangles.data = m_pPxIndicies;

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

	memcpy(&MatPosTrans.m[0], &vRight, sizeof(_float4));
	memcpy(&MatPosTrans.m[1], &vUp, sizeof(_float4));
	memcpy(&MatPosTrans.m[2], &vLook, sizeof(_float4));
	memcpy(&MatPosTrans.m[3], &vPos, sizeof(_float4));

	PxRigidStatic *pStaticRigid = pPhysX->Create_TriangleMeshActor_Static(TriangleMeshDesc, pUserData
		, 0.5f, 0.5f, 0.1f, _float3(fXSize, fYSize, fZSize));


	pPhysX->Set_ActorMatrix(pStaticRigid, (MatPosTrans));		// 노말라이즈 매트릭스보내고
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext, HANDLE hFile, CModel* pModel, _bool bIsLod, _bool bUseTriangleMeshActor)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel, bIsLod, bUseTriangleMeshActor)))
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
