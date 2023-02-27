#pragma once

#include "VIBuffer_Instancing.h"
#include "Model.h"

/* ���� ��ü������ �� ����. */
/* �� �޽ø� �׷��������� ����, �ε��� ���۸� �����Ѵ�. */
BEGIN(Engine)
class CInstancing_Mesh  final : public CVIBuffer_Instancing
{
private:
	CInstancing_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancing_Mesh(const CInstancing_Mesh& rhs);
	virtual ~CInstancing_Mesh() = default;

public:
	VTXMODEL* Get_NonAnimVertices() { return m_pNonAnimVertices; }

public:
	HRESULT		Save_Mesh(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Save_MeshBones(HANDLE& hFile, DWORD& dwByte);
	HRESULT		Load_Mesh(HANDLE& hFile, DWORD& dwByte);
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
public:
	void			Add_InstanceModel(vector<_float4x4*>VecInstancingMatrix);


public:
	virtual HRESULT Initialize_Prototype(HANDLE hFile, class CModel* pModel, _bool bIsLod, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

public:
	HRESULT SetUp_BonePtr(CModel* pModel);
	HRESULT SetUp_BonePtr(HANDLE& hFile, DWORD& dwByte, class CModel* pModel);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

private:				/*For.OriginMeshData*/
	CModel::TYPE				m_eType;
	_uint								m_iMaterialIndex = 0;  /* �� �޽ô� m_iMaterialIndex��° ���׸����� ����Ѵ�. */
	_uint								m_iNumBones = 0; /* �� �޽��� �����鿡�� ������ �ִ� ���� ����. */
	vector<class CBone*>	m_Bones;
	string*							m_pBoneNames = nullptr;
	VTXMODEL*					m_pNonAnimVertices = nullptr;
	FACEINDICES32*			m_pIndices = nullptr;

private:		/*for.Lod */
	_bool							m_bLodMesh = false;

private:		/*For.Instancing*/
	//_double*					m_pSpeeds = nullptr;
	vector<_float4>			m_pInstancingPositions;				// Instancing �� �����ǵ��� ����			
	_uint								m_iOriginNumPrimitive = 0;			// ���� NumPrimitive Data
	_uint								m_iIncreaseInstancingNumber = 0;
private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel* pModel);
private:
	HRESULT	Set_up_Instancing();


public:
	static CInstancing_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		HANDLE hFile, class CModel* pModel, _bool bIsLod = false, _uint iNumInstance =1 );
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free();
	
};

END