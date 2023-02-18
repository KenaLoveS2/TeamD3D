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
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	/*, m_Meshes(rhs.m_Meshes)*/
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	/*, m_Bones(rhs.m_Bones)*/
	, m_iNumBones(rhs.m_iNumBones)
	/*, m_Animations(rhs.m_Animations)*/
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	/*for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);*/

	//for (auto& pAnimation : m_Animations)
	//	Safe_AddRef(pAnimation);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
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


	return nullptr;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	_uint			iFlag = 0;

	if(TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_eType = eType;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* 뼈를 로드한다. / */
	/*if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;*/

	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;



 	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	/* 모델의 전체뼈를 로드하여 보관한다. 메시별로 구분하지않는다. */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	/* 전체뼈를 보관하고 있느 ㄴm_Bones에서 각 메시에 영향을 주느 ㄴ뼈들을 찾아서. 
	메시 안에 다시 재보관한다(참조) */
	for (auto& pMesh : m_Meshes)
	{
		pMesh->SetUp_MeshBones(this);
	}

	/* 애니메이션을 구동하는데 필요한 뼈대를 찾는 작업이 이뤄진다. 
	애니메이션을 구동하는데 필요한 뼈대 : 채널, 
	특정 채널(뼈)에 대해서 현재 시간에 맞는 키프레임상태를 얻어오고, 이걸 행렬로 만들고. 
	이 행렬을 전체뼈를 보관하고 있느 ㄴm_Bones중 같은 이름을 가진 뼈에 m_TransformMarix로 교체해주기 위해서. 
	*/

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta)
{
	if (TYPE_NONANIM == m_eType)
		return;

	/* 현재 애니메이션에 맞는 뼈들의 TranformMAtrix를 갱신하낟. */
	m_Animations[m_iCurrentAnimIndex]->Update_Bones(fTimeDelta);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
	int a = 10;
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

HRESULT CModel::Ready_Bones(aiNode * pAINode, CBone* pParent)
{
	/* 생성하고자해ㅑㅆ떤 뼈의 정보. */
	CBone*		pBone = CBone::Create(pAINode, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	/* 이뼈의 자식뼈를 만든다.  */
	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pAIMesh = m_pAIScene->mMeshes[i];

		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, pAIMesh, this);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}	

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	char		szDirectory[MAX_PATH] = "";

	_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	/* 텍스쳐에 기록되어있는 픽셀단위 재질 정보를 로드한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		char		szTextureFileName[MAX_PATH] = "";
		char		szExt[MAX_PATH] = "";
		char		szTexturePath[MAX_PATH] = "";

		MODELMATERIAL			ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{			
			/*j == 0 = > aiTextureType_NONE;
			j == 1 = > aiTextureType_DIFFUSE;
			j == 2 = > aiTextureType_SPECULAR;*/

			aiString			strTexturePath;			
			
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szTextureFileName);
			strcat_s(szTexturePath, szExt);

			_tchar			szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, strlen(szTexturePath), szFullPath, MAX_PATH);
			
			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*			pAnim = CAnimation::Create(pAIAnimation, this);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
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

	m_Importer.FreeScene();
}
