#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델의 교체가능한 한 파츠. */
/* 이 메시를 그려내기위한 정점, 인덱스 버퍼를 보유한다. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	HRESULT		Save_Mesh(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Save_MeshBones(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Load_Mesh(HANDLE& hFile, DWORD& dwByte);
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(HANDLE hFile, class CModel* pModel);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;

public:
	HRESULT SetUp_BonePtr(CModel* pModel);
	HRESULT SetUp_BonePtr(HANDLE& hFile, DWORD& dwByte, class CModel* pModel);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

private:
	CModel::TYPE		m_eType;
	/* 이 메시는 m_iMaterialIndex번째 머테리얼을 사용한다. */
	_uint				m_iMaterialIndex = 0;

	/* 이 메시의 정점들에게 영향을 주는 뼈의 갯수. */
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;

	string* m_pBoneNames = nullptr;
	VTXMODEL* m_pNonAnimVertices = nullptr;
	VTXANIMMODEL*		m_pAnimVertices = nullptr;
	FACEINDICES32*			m_pIndices = nullptr;

private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(HANDLE hFile, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE hFile, class CModel* pModel);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free();
	VTXMODEL* Get_NonAnimVertices() { return m_pNonAnimVertices; }
};

END