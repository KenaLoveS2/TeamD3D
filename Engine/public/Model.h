#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tagMaterialPath
	{
		wstring strPath_NONE				; // aiTextureType_NONE = 0,
		wstring strPath_DIFFUSE				; // aiTextureType_DIFFUSE = 1,
		wstring strPath_SPECULAR			; // aiTextureType_SPECULAR = 2,
		wstring strPath_AMBIENT				; // aiTextureType_AMBIENT = 3,
		wstring strPath_EMISSIVE			; // aiTextureType_EMISSIVE = 4,
		wstring strPath_HEIGHT				; // aiTextureType_HEIGHT = 5,
		wstring strPath_NORMALS				; // aiTextureType_NORMALS = 6,
		wstring strPath_SHININESS			; // aiTextureType_SHININESS = 7,
		wstring strPath_OPACITY				; // aiTextureType_OPACITY = 8,
		wstring strPath_DISPLACEMENT		; // aiTextureType_DISPLACEMENT = 9,
		wstring strPath_LIGHTMAP			; // aiTextureType_LIGHTMAP = 10,
		wstring strPath_REFLECTION			; // aiTextureType_REFLECTION = 11,
		wstring strPath_BASE_COLOR			; // aiTextureType_BASE_COLOR = 12,
		wstring strPath_NORMAL_CAMERA		; // aiTextureType_NORMAL_CAMERA = 13,
		wstring strPath_EMISSION_COLOR		; // aiTextureType_EMISSION_COLOR = 14,
		wstring strPath_METALNESS			; // aiTextureType_METALNESS = 15,
		wstring strPath_DIFFUSE_ROUGHNESS	; // aiTextureType_DIFFUSE_ROUGHNESS = 16,
		wstring strPath_AMBIENT_OCCLUSION	; // aiTextureType_AMBIENT_OCCLUSION = 17,
		wstring strPath_UNKNOWN				; // aiTextureType_UNKNOWN = 18,

		/*
		= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			= TEXT("")
			*/

		tagMaterialPath::tagMaterialPath()
		{
			wstring *p = (wstring*)this;
			for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++, p++) 
				*p = TEXT("");
		}

	} MATERIAL_PATH;


private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_matrix Get_PivotMatrix() const {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}

	class CBone* Get_BonePtr(const char* pBoneName);

public:
	void Set_AnimIndex(_uint iAnimIndex);

public:	
	HRESULT Initialize_Prototype(const _tchar *pModelFilePath, _fmatrix PivotMatrix, MATERIAL_PATH* pMaterialPath);
	virtual HRESULT Initialize(void* pArg);

public:
	void Play_Animation(_float fTimeDelta);
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);	
	HRESULT Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName = nullptr, _uint iPassIndex = 0);

public:
	TYPE								m_eType = TYPE_END;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint								m_iNumMeshes = 0;
	vector<class CMesh*>				m_Meshes;	

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

	/* 전체 뼈의 갯수. */
	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;

	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

	_float4x4							m_PivotMatrix;

	_float m_fBlendDuration = 0.2f;
	_float m_fBlendCurTime = 0.2f;
	_uint m_iPreAnimIndex = 0;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const _tchar* pModelFilePath, _fmatrix PivotMatrix, MATERIAL_PATH* pMaterialPath = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


	HRESULT SetUp_BonesParentPtr();
	HRESULT SetUp_ClonedAnimations();
	HRESULT SetUp_ClonedMeshes();
};

END