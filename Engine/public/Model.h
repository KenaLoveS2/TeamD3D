#pragma once
#include "Component.h"

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
	_uint				Get_NumMeshes() const { return m_iNumMeshes; }
	_matrix			Get_PivotMatrix() const { return XMLoadFloat4x4(&m_PivotMatrix); }
	_float4x4			Get_PivotFloat4x4() const { return m_PivotMatrix; }
	class CBone*		Get_BonePtr(const char* pBoneName);
	const _double&	Get_PlayTime();
	const _bool&		Get_PausePlay() const { return m_bPausePlay; }
	const _uint&		Get_AnimIndex() const { return m_iCurrentAnimIndex; }
	const _int&		Get_BlendAnimIndex() const { return m_iBlendAnimIndex; }
	const _bool&		Get_AnimationFinish() const;
	void				Set_PlayTime(_double dPlayTime);
	void				Set_PausePlay(_bool bPausePlay) { m_bPausePlay = bPausePlay; }
	void				Set_AnimIndex(_uint iAnimIndex, _int iBlendAnimIndex = -1);
	void				Set_PivotMatrix(_fmatrix matPivot) { XMStoreFloat4x4(&m_PivotMatrix, matPivot); }

public:
	HRESULT			Save_Model(const wstring& wstrSaveFileDirectory);
	HRESULT			Animation_Synchronization(CModel* pModelCom, const string& strRootNodeName);
	void				Reset_Animation();
	HRESULT			Add_Event(_uint iAnimIndex, _float fPlayTime, const string& strFuncName);
	void				Call_Event(const string& strFuncName);

public:	
	HRESULT 		Initialize_Prototype(const _tchar *pModelFilePath, _fmatrix PivotMatrix, const _tchar* pAdditionalFilePath, _bool bIsLod);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);
	virtual void	Imgui_RenderProperty() override;

public:
	void				Play_Animation(_float fTimeDelta);
	HRESULT			Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);	
	HRESULT			Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName = nullptr, _uint iPassIndex = 0);

private:
	TYPE								m_eType = TYPE_END;
	wstring							m_wstrModelFilePath = L"";
	DWORD							m_dwBeginBoneData = 0;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint								m_iNumMeshes = 0;
	vector<class CMesh*>			m_Meshes;	

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>		m_Materials;

	/* 전체 뼈의 갯수. */
	_uint								m_iNumBones = 0;
	vector<class CBone*>			m_Bones;

	_uint								m_iPreAnimIndex = 0;
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iAdditiveAnimIndex = 0;
	_int								m_iPreBlendAnimIndex = -1;
	_int								m_iBlendAnimIndex = -1;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>		m_Animations;

	_float4x4							m_PivotMatrix;

	_float								m_fBlendDuration = 0.2f;
	_float								m_fBlendCurTime = 0.2f;
	_float								m_fAdditiveCurTime = 0.f;

	_bool								m_bPausePlay = false;

private:
	HRESULT			Load_MeshMaterial(const wstring& wstrModelFilePath);
	HRESULT			Load_BoneAnimation(HANDLE& hFile, DWORD& dwByte);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _tchar* pModelFilePath, _fmatrix PivotMatrix, const _tchar* pAdditionalFilePath = nullptr, _bool bIsLod = false);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;


	HRESULT SetUp_BonesParentPtr();
	HRESULT SetUp_ClonedAnimations();
	HRESULT SetUp_ClonedMeshes();

	HRESULT SetUp_Material(_uint iMaterialIndex, aiTextureType eType, const _tchar *pTexturePath);
};

END