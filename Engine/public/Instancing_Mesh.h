#pragma once

#include "VIBuffer_Instancing.h"
#include "Model.h"

/* 모델의 교체가능한 한 파츠. */
/* 이 메시를 그려내기위한 정점, 인덱스 버퍼를 보유한다. */
BEGIN(Engine)
class CInstancing_Mesh  final : public CVIBuffer_Instancing
{
private:
	CInstancing_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancing_Mesh(const CInstancing_Mesh& rhs);
	virtual ~CInstancing_Mesh() = default;

public:
	VTXMODEL* Get_NonAnimVertices() { return m_pNonAnimVertices; }
	_uint Get_NumVertices() {
		return m_iNumVertices;
	}
	_uint Get_NumPrimitive() {
		return m_iNumPrimitive;
	}
public:
	HRESULT		Save_Mesh(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Save_MeshBones(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Load_Mesh(HANDLE& hFile, DWORD& dwByte);
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
public:
	void			Add_InstanceModel(vector<_float4x4*>VecInstancingMatrix);
	void			InstBuffer_Update(vector<_float4x4*>VecInstancingMatrix);
	void			InstBufferSize_Update(_int iSize);
	void			Set_PxTriangle(vector<_float4x4*>VecInstancingMatrix);
public:
	virtual HRESULT Initialize_Prototype(HANDLE hFile, class CModel* pModel, _bool bIsLod, _bool bUseTriangleMeshActor, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT Render();


public:
	_bool	Instaincing_MoveControl(CEnviromentObj::CHAPTER eChapterGimmcik,_float fTimeDelta);
	void	InstaincingMesh_GimmkicInit(CEnviromentObj::CHAPTER eChapterGimmcik);
	void	InstaincingMesh_yPosControl(_float yPos);

	void	Create_PxTriangle_InstMeshActor(CTransform* pParentTransform, vector<_float4x4*> VecInstancingMatrix);

public:
	HRESULT SetUp_BonePtr(CModel* pModel);
	HRESULT SetUp_BonePtr(HANDLE& hFile, DWORD& dwByte, class CModel* pModel);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

private:				/*For.OriginMeshData*/
	CModel::TYPE						m_eType;
	_uint								m_iMaterialIndex = 0;  /* 이 메시는 m_iMaterialIndex번째 머테리얼을 사용한다. */
	_uint								m_iNumBones = 0; /* 이 메시의 정점들에게 영향을 주는 뼈의 갯수. */
	vector<class CBone*>				m_Bones;
	string*								m_pBoneNames = nullptr;
	VTXMODEL*							m_pNonAnimVertices = nullptr;
	VTXANIMMODEL*						m_pAnimVertices = nullptr;
	FACEINDICES32*						m_pIndices = nullptr;

private:		/*for.Lod */
	_bool								m_bLodMesh = false;

private:		/*For.Instancing*/
	vector<_float4>						m_pInstancingPositions;				// Instancing 한 포지션들의 벡터			
	_uint								m_iOriginNumPrimitive = 0;			// 기존 NumPrimitive Data
	_uint								m_iIncreaseInstancingNumber = 0;
	
	vector<_float>						m_vecOriginYPos;
	_float								m_fIncreaseYPos = 0.f;


private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(HANDLE hFile, class CModel* pModel);
private:
	HRESULT	Set_up_Instancing();


public:
	static CInstancing_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		HANDLE hFile, class CModel* pModel, _bool bIsLod = false, _bool bUseTriangleMeshActor = false, _uint iNumInstance =1 );
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free();
	
};

END