#include "Model_Instancing.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"


#include "Bone_Instacing.h"
#include "Mesh_Instancing.h"

#include "Utile.h"
#include <string>


#include "GameInstance.h"


CModel_Instancing::CModel_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instancing::CModel_Instancing(const CModel_Instancing & rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Bones(rhs.m_Bones)
	, m_iNumBones(rhs.m_iNumBones)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	strcpy_s(m_szModelPath, MAX_PATH, rhs.m_szModelPath);

	//Test ¾èÀºº¹»ç
	for (auto& m_Bones : m_Bones)
	{
		Safe_AddRef(m_Bones);
	}
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pMesh : rhs.m_Meshes)
		Safe_AddRef(pMesh);
}

CBone_Instacing * CModel_Instancing::Get_BonePtr(const char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone_Instacing* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

HRESULT CModel_Instancing::Initialize_Prototype(const char * pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile)
{
	_uint			iFlag = 0;
	strcpy_s(m_szModelPath, MAX_PATH, pModelFilePath);
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_MeshContainers(hFile)))
		return E_FAIL;

	if (FAILED(Ready_Materials(hFile, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Bones(hFile, nullptr)))	// ±íÀºº¹»ç ÇÊ¿ä
		return E_FAIL;

	for (auto& pMesh : m_Meshes)
	{
		pMesh->LoadFile(hFile, this);
	}

	DWORD   dwByte = 0;
	_uint iTemp = 0;
	ReadFile(hFile, &iTemp, sizeof(_uint), &dwByte, nullptr);


	return S_OK;
}

HRESULT CModel_Instancing::Initialize(void * pArg)
{
	ZeroMemory(&m_ModelDesc, sizeof(m_ModelDesc));

	if (nullptr != pArg)
		memcpy(&m_ModelDesc, pArg, sizeof(m_ModelDesc));
	return S_OK;
}

HRESULT CModel_Instancing::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pTexture[eType])
	{
		m_Materials[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShader, pConstantName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Instancing::Render(CShader * pShader, _uint iMeshIndex, _uint iShaderIndex, const char * pBoneConstantName, const char * pNoRenderName)
{	
	if (nullptr != m_Meshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[256];
			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 256);
		}
		
		pShader->Begin(iShaderIndex);
		m_Meshes[iMeshIndex]->Render();
	}


	return S_OK;
}

void CModel_Instancing::Add_IncreasePosition(_float4 vPos, _uint iIndex)
{
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_Meshes[i]->Add_Position(vPos, iIndex);
	}

}

void CModel_Instancing::Save_TreePos(const _tchar* pFileName)
{
	_tchar szName[MAX_PATH] = TEXT("../../Data/");

	lstrcat(szName, pFileName);
	lstrcat(szName, TEXT(".dat"));

	HANDLE      hFile = CreateFile(szName,
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}
	DWORD   dwByte = 0;

	vector<_float4> vTreePosSize = m_Meshes[0]->Get_vPos();

	_uint iTreeSize =(_uint)(vTreePosSize.size());

	WriteFile(hFile, &iTreeSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iTreeSize; ++i)
	{
		
		WriteFile(hFile, &vTreePosSize[i], sizeof(_float4), &dwByte, nullptr);
	}

	MSG_BOX("SaveClear");
	CloseHandle(hFile);


}

void CModel_Instancing::Load_TreePos(const _tchar* pFileName)
{
	_tchar szName[MAX_PATH] = TEXT("../../Data/");

	lstrcat(szName, pFileName);
	lstrcat(szName, TEXT(".dat"));

	HANDLE      hFile = CreateFile(szName,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}
	DWORD   dwByte = 0;

	vector<_float4> vTreePosSize;
	_float4		temp;
	_uint iTreeSize = 0;

	ReadFile(hFile, &iTreeSize, sizeof(_uint), &dwByte, nullptr);
	
	while (true)
	{
		ReadFile(hFile, &temp, sizeof(_float4), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		vTreePosSize.push_back(temp);
	}
	CloseHandle(hFile);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_Meshes[i]->Set_Position(vTreePosSize);
	}

}

HRESULT CModel_Instancing::Ready_Bones(HANDLE hFile, CBone_Instacing * pParent)
{
	DWORD   dwByte = 0;

	CBone_Instacing* pBone = CBone_Instacing::Create(this, hFile);
	if (nullptr == pBone)
		assert("CLoadModel_Ready_Bones");

	if (pParent != nullptr)
		pBone->Set_Parent(pParent);
	
	m_Bones.push_back(pBone);

	_uint NumChildren = 0;
	ReadFile(hFile, &NumChildren, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < NumChildren; ++i)
	{
		Ready_Bones(hFile, pBone);

	}

	return S_OK;
}

HRESULT CModel_Instancing::Ready_MeshContainers(HANDLE hFile)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshInstancing *pMesh = CMeshInstancing::Create(m_pDevice, m_pContext, this, hFile);

		if (nullptr == pMesh)
			assert("CLoadModel::Ready_MeshContainers");

		m_Meshes.push_back(pMesh);
	}


	return S_OK;
}

HRESULT CModel_Instancing::Ready_Materials(HANDLE hFile, const char * pModelFilePath)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MODELMATERIAL ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_uint iTextureIndex = 0;
			ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
			if (AI_TEXTURE_TYPE_MAX == iTextureIndex)
			{
				continue;
			}

			_tchar szFilePath[MAX_PATH] = TEXT("");
			_uint TextureFilePathLen = 0;
			ReadFile(hFile, &TextureFilePathLen, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, szFilePath, sizeof(_tchar)* TextureFilePathLen, &dwByte, nullptr);

			if (!lstrcmp(szFilePath, TEXT("")))
				assert("CLoadModel: Ready_Materials issue");

			ModelMaterial.pTexture[iTextureIndex] = CTexture::Create(m_pDevice, m_pContext, szFilePath);
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

CModel_Instancing * CModel_Instancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile)
{
	CModel_Instancing*		pInstance = new CModel_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix, hFile)))
	{
		MSG_BOX("Failed to Created : CModel_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel_Instancing::Clone(void * pArg)
{
	CModel_Instancing*		pInstance = new CModel_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Instancing::Free()
{
	__super::Free();


	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();
}
