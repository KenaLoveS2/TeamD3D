#pragma once
#include "Component.h"
#include "Animation.h"
#include "PhysX_Defines.h"
#include "EnviromentObj.h"

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
	_uint						Get_NumMeshes() const { return m_iNumMeshes; }
	_matrix						Get_PivotMatrix() const { return XMLoadFloat4x4(&m_PivotMatrix); }
	_float4x4					Get_PivotFloat4x4() const { return m_PivotMatrix; }
	class CBone*				Get_BonePtr(const char* pBoneName);
	class CBone*				Get_BonePtr(_int iBoneIndex);
	const _double&				Get_PlayTime() const;
	const _double&				Get_LastPlayTime() const;
	const _float				Get_AnimationProgress() const;
	const _double				Get_AnimationDuration() const;
	const _bool&				Get_PausePlay() const { return m_bPausePlay; }
	const _bool&				Get_Preview() const { return m_bPreview; }
	const _uint&				Get_AnimIndex() const { return m_iCurrentAnimIndex; }
	const _uint&				Get_LastAnimIndex() const { return m_iPreAnimIndex; }
	const _bool&				Get_AnimationFinish() const;
	vector<KEYFRAME>*			Get_KeyFrames(const char* pBoneName);
	void						Set_PlayTime(_double dPlayTime);
	void						Set_PausePlay(_bool bPausePlay) { m_bPausePlay = bPausePlay; }
	void						Set_AnimIndex(_uint iAnimIndex);
	void						Set_PivotMatrix(_fmatrix matPivot) { XMStoreFloat4x4(&m_PivotMatrix, matPivot); }
	void						Set_BoneLocked(const char* pBoneName, _bool bLock);
	void						Set_AllBonesUnlock();
	void						Set_RootBone(const string & strRootBone) { m_strRootBone = strRootBone; }
	HRESULT						Set_BoneIndex(const _tchar * pFilePath);
	void						Set_AnimationBlendDuration(_uint iAnimIndex, _float fBlendDuration);

	void						FixedAnimIdx_PlayTime(_uint iAnimIndex, _float fTime);
	void						ResetAnimIdx_PlayTime(_uint iAnimIndex);
	void						Set_AdditiveAnimIndexForMonster(_uint iAnimIndex);
	void						Play_AdditiveAnimForMonster(_float fTimeDelta, _float fRatio, const string & strRootBone = "");

	vector<MODELMATERIAL>* Get_Material() { return &m_Materials; }
	vector<_float3>* Get_ColiderSize() { return	&m_VecInstancingColiderSize; }


	CModel::TYPE					Get_Type()const { return m_eType; }
	CAnimation* Find_Animation(const string & strAnimName);
	CAnimation* Find_Animation(_uint iAnimIndex);

	/*for.Instancing*/
	const	_bool			Get_IStancingModel() const { return m_bIsInstancing; }
	vector<_float4x4*>* 	Get_InstancePos() { return &m_pInstancingMatrix; }
	void					Set_InstancePos(vector<_float4x4>& InstanceMatrixVec);
	const _float4				    GetMeshCenter() const;

	/*~for.Instancing*/
	/*for.Lod*/
	const	_bool 			Get_IsLodModel()const { return m_bIsLodModel; }

public:
	HRESULT				Save_Model(const wstring & wstrSaveFileDirectory);
	HRESULT				Animation_Synchronization(CModel * pModelCom, const string & strRootNodeName);
	HRESULT				Synchronization_MeshBone(CModel * pModelCom);
	void						Reset_Animation();
	HRESULT				Add_Event(_uint iAnimIndex, _float fPlayTime, const string & strFuncName);
	void						Call_Event(const string & strFuncName);
	void						Compute_CombindTransformationMatrix();
	void						Compute_CombindTransformationMatrix(const string & RootBone);
	void						Update_BonesMatrix(CModel * pModel);
	void						Set_AllAnimCommonType();
	void						Print_Animation_Names(const string & strFilePath);
	void						InstanceModelPosInit(_fmatrix parentMatrix);

public:
	HRESULT 					Initialize_Prototype(const _tchar * pModelFilePath, _fmatrix PivotMatrix,
		const _tchar * pAdditionalFilePath, _bool bIsLod, _bool bIsInstancing , const char* JsonMatrial, _bool bUseTriangleMeshActor, _bool bPointBuffer);
	virtual HRESULT		Initialize(void* pArg, class CGameObject* pOwner);
	virtual void				Imgui_RenderProperty() override;

public:
	void						Play_Animation(_float fTimeDelta);
	HRESULT				Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT				Render(CShader * pShader, _uint iMeshIndex, const char* pBoneConstantName = nullptr, _uint iPassIndex = 0);
	void						Imgui_MaterialPath();


	void					Instaincing_GimmkicInit(CEnviromentObj::CHAPTER eChapterGimmcik);
	_bool					Instaincing_MoveControl(CEnviromentObj::CHAPTER eChapterGimmcik,_float fTimeDelta);

	_bool					Culling_InstancingMeshs(_float fCameraDistance, _fmatrix ParentMat);

private:
	void						MODELMATERIAL_Create_Model(const char* jSonPath);

public:
	void						Set_InstanceEffect_Info(CTransform * pParentTransform,_int iInstanceNum,_float fMinSpeed, _float fMaxSpeed);
	void						Instaincing_mesh_Effect_tick(_float yLimitPos, _float fTimeDelta);

private:
	TYPE									m_eType = TYPE_END;
	wstring									m_wstrModelFilePath = L"";
	DWORD								m_dwBeginBoneData = 0;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint										m_iNumMeshes = 0;
	vector<class CMesh*>						m_Meshes;

	_uint										m_iNumMaterials = 0;
	vector<MODELMATERIAL>						m_Materials;

	/* 전체 뼈의 갯수. */
	_uint										m_iNumBones = 0;
	vector<class CBone*>						m_Bones;
	string									m_strRootBone = "";

	_uint										m_iPreAnimIndex = 0;
	_uint										m_iCurrentAnimIndex = 0;
	_uint										m_iNumAnimations = 0;
	vector<class CAnimation*>	m_Animations;

	_float4x4								m_PivotMatrix;

	_float									m_fBlendDuration = 0.2f;
	_float									m_fBlendCurTime = 0.2f;

	_bool									m_bPreview = false;
	_bool									m_bPausePlay = false;
	_uint										m_iAdditiveAnimIndexForMonster = 0;

	/*For.Mesh_Instancing*/
	_bool											m_bIsInstancing = false;
	vector<class CInstancing_Mesh*>					m_InstancingMeshes;
	vector<_float4x4*>								m_pInstancingMatrix;
	vector<_float3>									m_VecInstancingColiderSize;

	/*For.Lod*/
	_bool												m_bIsLodModel = false;

	/*For.TriangleMeshColl*/
	_bool												m_bUseTriangleMeshActor = false;


private:
	HRESULT			Load_MeshMaterial(const wstring & wstrModelFilePath);
	HRESULT			Load_BoneAnimation(HANDLE & hFile, DWORD & dwByte);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,
	const _tchar * pModelFilePath, _fmatrix PivotMatrix, const _tchar * pAdditionalFilePath = nullptr,
	_bool bIsLod = false, _bool bIsInstancing = false, const char* JsonMatrial = nullptr, _bool bUseTriangleMeshActor = false, _bool bPointBuffer = false);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	HRESULT SetUp_BonesParentPtr();
	HRESULT SetUp_ClonedAnimations();
	HRESULT SetUp_ClonedMeshes();

	HRESULT SetUp_Material(_uint iMaterialIndex, aiTextureType eType, const _tchar * pTexturePath);

	const	_bool	Get_UseTriangleMeshActor() const { return m_bUseTriangleMeshActor; }


public:/*For.Mesh_Instancing*/
	void		Imgui_MeshInstancingPosControl(_fmatrix parentMatrix, _float4 vPickingPos, _fmatrix TerrainMatrix, _bool bPickingTerrain = false, _int iGroundCoverNum = 0, _float fBetween = 0.f, _bool IsMultipleCheck = false,_float fRaduis = 0.f);
	void		Imgui_Mesh_InstancingSort_EditOrCreate();
	void		Imgui_MeshInstancingyPosControl(_float yPos);

	/*Imgui*/
	void		Edit_InstModel_Collider(const _tchar * pActorName); // Only_Static
	void		InitPhysxData();

private:
	_uint									  		m_iSelectMeshInstace_Index = -1;		// -1이 아닐때 Instancing Pos 정하기
	class	CTransform* m_pInstanceTransform = nullptr;

	_bool						m_bUseEditOrder = false;
	_bool						m_bSort_MeshCreate = false;
	_bool						m_bExistEdit = false;

	_int						m_vSortObj_CreateNum = 0;
	_float3						m_vSortStartPos;
	_float3						m_vSortAngle;
	_float3						m_vIncreateDir;
	_bool						m_bXYZRatioUse[3];



public:
	void Create_PxTriangle(PX_USER_DATA * pUserData);
	void Set_PxPosition(_float3 vPosition);
	void Set_PxMatrix(_float4x4 & Matrix);

	void Calc_MinMax(_float * pMinX, _float * pMaxX, _float * pMinY, _float * pMaxY, _float * pMinZ, _float * pMaxZ);
	void Create_PxBox(const _tchar * pActorName, CTransform * pConnectTransform, _uint iColliderIndex);

	void Calc_InstMinMax(_float * pMinX, _float * pMaxX, _float * pMinY, _float * pMaxY, _float * pMinZ, _float * pMaxZ);
	void Create_InstModelPxBox(const _tchar * pActorName, CTransform * pConnectTransform, _uint iColliderIndex, _float3 vSize, _float3 _vPos = _float3(0.f,0.f,0.f), _bool bRotation = false);


	void Create_PxTriangle(const _tchar * szCloneName,class CTransform* pParentTransform, PX_USER_DATA * pUserData);
	void Create_Px_InstTriangle(class CTransform* pParentTransform);
	void SetUp_InstModelColider(vector<_float3>& vecColiderSize);
	
	void Set_DurationRate(_uint iAnimIndex, _float fRate);

	void Init_AnimIndex(_uint iAnimIndex);
};

END