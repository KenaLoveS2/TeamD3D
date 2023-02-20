#include "..\public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pBone : rhs.m_Bones)
	{
		m_Bones.push_back((CBone*)pBone->Clone());
	}

	for (auto& pAnimation : rhs.m_Animations)
	{
		m_Animations.push_back((CAnimation*)pAnimation->Clone());
	}

	for (auto& pMesh : rhs.m_Meshes)
	{
		m_Meshes.push_back((CMesh*)pMesh->Clone());
	}
}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Initialize_Prototype(const _tchar *pModelFilePath, _fmatrix PivotMatrix, MATERIAL_PATH* pMaterialPath)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	HANDLE hFile = CreateFile(pModelFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;

	_ulong dwByte = 0;
	ReadFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		MODELMATERIAL ModelMatrial;
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			_bool bUseFlag = false;
			ReadFile(hFile, &bUseFlag, sizeof(_bool), &dwByte, nullptr);
			if (bUseFlag)
			{
				_uint iLen = 0;
				ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
				_tchar szPath[MAX_PATH] = { 0, };
				ReadFile(hFile, szPath, (iLen + 1) * sizeof(_tchar), &dwByte, nullptr);
				_uint iNumTextures = 0;
				ReadFile(hFile, &iNumTextures, sizeof(_uint), &dwByte, nullptr);

				ModelMatrial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szPath, iNumTextures);
			}
			else
				ModelMatrial.pTexture[j] = nullptr;
			
			// 수동 매터리얼 텍스처 입력 이상할 수도 있음 확인 필요
			if (pMaterialPath)
			{
				wstring* pPath = (wstring*)pMaterialPath;
				pPath += j;

				if (*pPath != TEXT(""))
				{
					if(ModelMatrial.pTexture[j]) 
						Safe_Release(ModelMatrial.pTexture[j]);

					ModelMatrial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, pPath->c_str());
				}
			}
		}

		m_Materials.push_back(ModelMatrial);
	}

	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		CBone* pBone = CBone::Create(hFile);
		if (pBone == nullptr)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	if (FAILED(SetUp_BonesParentPtr()))
		return E_FAIL;

	ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh *pMesh = CMesh::Create(m_pDevice, m_pContext, hFile, this);
		if (pMesh == nullptr) return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation *pAnimation = CAnimation::Create(hFile, this);
		if (pAnimation == nullptr) return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	if (FAILED(SetUp_BonesParentPtr()))
		return E_FAIL;

	if (FAILED(SetUp_ClonedAnimations()))
		return E_FAIL;

	if (FAILED(SetUp_ClonedMeshes()))
		return E_FAIL;

	return S_OK;
}


HRESULT CModel::SetUp_BonesParentPtr()
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		const char* pParentName = m_Bones[i]->Get_ParentName();
		if (!strcmp(pParentName, ""))
			continue;

		_uint j = 0;
		for (; j < m_iNumBones; j++)
		{
			if (i == j)
				continue;

			const char* pName = m_Bones[j]->Get_Name();
			if (!strcmp(pParentName, pName))
			{
				m_Bones[i]->SetParent(m_Bones[j]);
				break;
			}
		}

		if (j == m_iNumBones)
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::SetUp_ClonedAnimations()
{
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		if (FAILED(m_Animations[i]->SetUp_ChannelsBonePtr(this)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::SetUp_ClonedMeshes()
{
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		if (FAILED(m_Meshes[i]->SetUp_BonePtr(this)))
			return E_FAIL;
	}

	return S_OK;
}

void CModel::Set_AnimIndex(_uint iAnimIndex)
{
	m_iCurrentAnimIndex = iAnimIndex;
	
	/*
	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		m_iPreAnimIndex = m_iCurrentAnimIndex;
		m_fBlendCurTime = 0.f;
	}

	m_iCurrentAnimIndex = iAnimIndex;
	*/
}

void CModel::Play_Animation(_float fTimeDelta)
{
	if (TYPE_NONANIM == m_eType)
		return;
		
	if (m_fBlendCurTime < m_fBlendDuration)
	{
		_float fBlendRatio = m_fBlendCurTime / m_fBlendDuration;
		m_Animations[m_iPreAnimIndex]->Update_Bones(fTimeDelta);
		m_Animations[m_iCurrentAnimIndex]->Update_Bones_Blend(fTimeDelta, fBlendRatio);

		m_fBlendCurTime += fTimeDelta;
	}
	else
	{
		m_Animations[m_iCurrentAnimIndex]->Update_Bones(fTimeDelta);
	}
	
	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
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

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName, _uint iPassIndex)
{
	if (nullptr != m_Meshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[128];

			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			
			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 128);
		}		

		pShader->Begin(iPassIndex);

		m_Meshes[iMeshIndex]->Render();
	}
		

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath, _fmatrix PivotMatrix, MATERIAL_PATH* pMaterialPath)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix, pMaterialPath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

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
