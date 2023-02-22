#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instancing :public CComponent
{
public:
	typedef struct tag_LoadModel_Instancing_Desc
	{
		_tchar* szProtoName = TEXT("");
	} LOAD_Instancing_MODELDES;


private:
	CModel_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instancing(const CModel_Instancing& rhs);
	virtual ~CModel_Instancing() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	_matrix Get_PivotMatrix() const { return XMLoadFloat4x4(&m_PivotMatrix); }
	class CBone_Instacing* Get_BonePtr(const char* pBoneName);
	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);

public:
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(CShader* pShader, _uint iMeshIndex, _uint iShaderIndex = 0, const char* pBoneConstantName = nullptr, const char* pNoRenderName = nullptr);

	void	Add_IncreasePosition(_float4 vPos, _uint iIndex);
	void	Save_TreePos(const _tchar* pFileName);
	void	Load_TreePos(const _tchar* pFileName);

private:
	_uint								m_iNumMeshes = 0;
	vector<class CMeshInstancing*>		m_Meshes;
	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

	_uint								m_iNumBones = 0;
	vector<class CBone_Instacing*>		m_Bones;

	LOAD_Instancing_MODELDES			m_ModelDesc;
	_float4x4							m_PivotMatrix;

private:
	char								m_szModelPath[MAX_PATH] = "";
	char								m_szNoRenderMeshName[MAX_PATH] = "";

public:
	HRESULT Ready_Bones(HANDLE hFile, class CBone_Instacing* pParent);
	HRESULT Ready_MeshContainers(HANDLE hFile);
	HRESULT Ready_Materials(HANDLE hFile, const char* pModelFilePath);

public:
	static CModel_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END
