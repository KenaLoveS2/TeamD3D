#pragma once
#include "Component.h"
#include "Animation.h"

BEGIN(Engine)
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint					Get_NumMeshes() const { return m_iNumMeshes; }
	_matrix				Get_PivotMatrix() const { return XMLoadFloat4x4(&m_PivotMatrix); }
	_float4x4			Get_PivotFloat4x4() const { return m_PivotMatrix; }
	class CBone*	Get_BonePtr(const char* pBoneName);
	_double			Get_PlayTime();
	const _bool&	Get_PausePlay() const { return m_bPausePlay; }
	const _uint&		Get_AnimIndex() const { return m_iCurrentAnimIndex; }
	const _uint&		Get_LastAnimIndex() const { return m_iPreAnimIndex; }
	const _bool&	Get_AnimationFinish() const;
	void					Set_PlayTime(_double dPlayTime);
	void					Set_PausePlay(_bool bPausePlay) { m_bPausePlay = bPausePlay; }
	void					Set_AnimIndex(_uint iAnimIndex);
	void					Set_PivotMatrix(_fmatrix matPivot) { XMStoreFloat4x4(&m_PivotMatrix, matPivot); }
	void					Set_BoneLocked(const char* pBoneName, _bool bLock);
	void					Set_AllBonesUnlock();

	void					ResetAnimIdx_PlayTime(_uint iAnimIndex);

	CModel::TYPE		Get_Type()const { return m_eType; }
	CAnimation*		Find_Animation(const string& strAnimName);
	CAnimation*		Find_Animation(_uint iAnimIndex);

	/*for.Instancing*/
	const	_bool		Get_IStancingModel() const { return m_bIsInstancing; }
	vector<_float4x4*>*	Get_InstancePos() { return &m_pInstancingMatrix; }
	void						Set_InstancePos(vector<_float4x4> InstanceMatrixVec);
	/*~for.Instancing*/
	/*for.Lod*/
	const	_bool 		Get_IsLodModel()const { return m_bIsLodModel; }
	
public:
	HRESULT			Save_Model(const wstring& wstrSaveFileDirectory);
	HRESULT			Animation_Synchronization(CModel* pModelCom, const string& strRootNodeName);
	HRESULT			Synchronization_MeshBone(CModel* pModelCom);
	void				Reset_Animation();
	HRESULT			Add_Event(_uint iAnimIndex, _float fPlayTime, const string& strFuncName);
	void				Call_Event(const string& strFuncName);
	void				Compute_CombindTransformationMatrix();
	void				Update_BonesMatrix(CModel* pModel);
	void				Set_AllAnimCommonType();

public:	
	HRESULT 				Initialize_Prototype(const _tchar *pModelFilePath, _fmatrix PivotMatrix, 
		const _tchar* pAdditionalFilePath, _bool bIsLod, _bool bIsInstancing , const char* JsonMatrial);
	virtual HRESULT 	Initialize(void* pArg, class CGameObject* pOwner);
	virtual void			Imgui_RenderProperty() override;

public:
	void						Play_Animation(_float fTimeDelta);
	HRESULT				Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);	
	HRESULT				Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName = nullptr, _uint iPassIndex = 0);

private:
	TYPE					m_eType = TYPE_END;
	wstring					m_wstrModelFilePath = L"";
	DWORD				m_dwBeginBoneData = 0;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint						m_iNumMeshes = 0;
	vector<class CMesh*>		m_Meshes;	

	_uint						m_iNumMaterials = 0;
	vector<MODELMATERIAL>		m_Materials;			

	/* 전체 뼈의 갯수. */
	_uint						m_iNumBones = 0;
	vector<class CBone*>		m_Bones;

	_uint						m_iPreAnimIndex = 0;
	_uint						m_iCurrentAnimIndex = 0;
	_uint						m_iNumAnimations = 0;
	vector<class CAnimation*>	m_Animations;

	_float4x4					m_PivotMatrix;

	_float						m_fBlendDuration = 0.2f;
	_float						m_fBlendCurTime = 0.2f;

	_bool						m_bPausePlay = false;

/*For.Mesh_Instancing*/
	_bool						m_bIsInstancing = false;
	vector<class CInstancing_Mesh*>		m_InstancingMeshes;
	vector<_float4x4*>			m_pInstancingMatrix;				// Instancing 한 포지션들의 벡터			
	_uint						m_iSelectMeshInstace_Index = -1;		// -1이 아닐때 Instancing Pos 정하기
	
	/*For.Lod*/
	_bool														m_bIsLodModel = false;
#ifdef _DEBUG
	class	CTransform*										m_pInstanceTransform = nullptr;
#endif
private:
	HRESULT			Load_MeshMaterial(const wstring& wstrModelFilePath);
	HRESULT			Load_BoneAnimation(HANDLE& hFile, DWORD& dwByte);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _tchar* pModelFilePath, _fmatrix PivotMatrix, const _tchar* pAdditionalFilePath = nullptr, _bool bIsLod = false, _bool bIsInstancing = false, const char* JsonMatrial = nullptr);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	HRESULT SetUp_BonesParentPtr();
	HRESULT SetUp_ClonedAnimations();
	HRESULT SetUp_ClonedMeshes();

	HRESULT SetUp_Material(_uint iMaterialIndex, aiTextureType eType, const _tchar *pTexturePath);

#ifdef _DEBUG
	/*For.Mesh_Instancing*/
public:
	void		 Imgui_MeshInstancingPosControl(_fmatrix parentMatrix);
#endif
};

END