#pragma once
#include "VIBuffer_Instancing.h"
#include "Model_Instancing.h"

BEGIN(Engine)

class CMeshInstancing  final : public CVIBuffer_Instancing
{
protected:
	CMeshInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshInstancing(const CMeshInstancing& rhs);
	virtual ~CMeshInstancing() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void	Add_Position(_float4 vPos, _uint iIndex);
	void	Set_Position(vector<_float4>& vTreePos);

public:
	virtual HRESULT Initialize_Prototype(class CModel_Instancing* pModel, HANDLE hFile, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(_double TimeDelta);
	virtual HRESULT Render();

	void	Map_UnMapViBuffer(_uint iNumInstance);


	vector<_float4>&		Get_vPos() { return m_pPosition; }

public:
	void	LoadFile(HANDLE hFile,class CModel_Instancing* pModel);
	void	SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

private:
	char						m_szName[MAX_PATH] = "";
	_uint						m_iMaterialIndex = 0;
	_uint						m_iNumBones = 0;
	vector<class CBone_Instacing*>		m_Bones;

	_uint						m_iOriginNumPrimitive = 0;
	_uint						m_iPickingIndex = 0;
	_uint						m_iIncreaseInstancingNumber = 0;
	VTXMODEL*					m_pVertices = nullptr;
	FACEINDICES32*				m_pIndices = nullptr;
	
	vector<_float4>					m_pPosition;

private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel_Instancing* pModel);

public:
	static CMeshInstancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CModel_Instancing* pModel, HANDLE hFile, _uint iNumInstance =1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END