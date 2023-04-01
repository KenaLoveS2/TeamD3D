#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CVIBuffer_Trail;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)
#define  MAX_TEXTURECNT 5

class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct tagEffectDesc
	{
		// Effect Type = texture, texture_Particle , mesh
		enum EFFECTTYPE { EFFECT_PLANE, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_TRAIL, EFFECT_END };
		enum MESHTYPE { MESH_PLANE, MESH_CUBE, MESH_CONE, MESH_SPHERE, MESH_CYLINDER, MESH_ETC, MESH_END };
		enum PARTICLETYPE { PARTICLE_BOX, PARTICLE_SPHERE, PARTICLE_CONE, PARTICLE_SPREAD, PARTICLE_END };
		enum TEXTURERENDERTYPE { TEX_ONE, TEX_SPRITE, TEX_END };
		enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_END };
		enum BLENDSTATE { BLENDSTATE_DEFAULT, BLENDSTATE_ALPHA, BLENDSTATE_ONEEFFECT, BLENDSTATE_MIX, BLENDSTATE_TRAIL, BLENDSTATE_END };
		enum MOVEDIR { MOVE_FRONT, MOVE_BACK, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, DIR_END };
		enum ROTXYZ { ROT_X, ROT_Y, ROT_Z, ROT_END };

		EFFECTTYPE        eEffectType = EFFECT_PLANE;
		MESHTYPE		  eMeshType = MESH_PLANE;
		PARTICLETYPE	  eParticleType = PARTICLE_BOX;
		TEXTURERENDERTYPE eTextureRenderType = TEX_ONE;
		TEXTURETYPE		  eTextureType = TYPE_END;
		BLENDSTATE        eBlendType = BLENDSTATE_DEFAULT;
		MOVEDIR           eMoveDir = MOVE_FRONT;
		ROTXYZ            eRotation = ROT_Y;

		_int			  iPassCnt = 0;

		// Diffuse Frame ( Cur Texture Idx )
		_float		fFrame[MAX_TEXTURECNT] = { 0.0f };
		// Mask Frame ( Cur Texture Idx )
		_float		fMaskFrame[MAX_TEXTURECNT] = { 0.0f };
		// Normal Frame ( Cur Texture Idx )
		_float		fNormalFrame = 0.0f;

		// if ( eTextureType == TEX_SPRITE )
		_float	fWidthFrame = 0.0f, fHeightFrame = 0.0f;
		_int	iSeparateWidth = 0, iSeparateHeight = 0;
		_int	iWidthCnt = 0, iHeightCnt = 0;

		// Color & Scale
		_vector	vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		_vector	vScale = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Time 
		_float  fTimeDelta = 0.0f;
		_float  fPlayBbackTime = 0.0f;

		// Move Position // 
		_float  fCreateRange = 1.0f;
		_float  fRange = 0.0f;
		_float  fAngle = 0.0f;
		_float  fMoveDurationTime = 0.0f;
		_bool   bStart = false;
		_vector vInitPos = { 0.0f, 0.0f, 0.0f, 1.0f };
		_vector vPixedDir = { 0.0f, 0.0f, 0.0f, 0.0f };

		/* Trail  */
		_bool   IsTrail = false;
		_bool   bActive = true;
		_bool   bAlpha = false;
		_float  fLife = 0.0f;
		_float  fWidth = 0.0f;
		_float  fSegmentSize = 0.0f;
		_float  fAlpha = 0.0f;
		/* ~Trail */

		// Option // 
		_bool	IsBillboard = false;
		_bool	IsNormal = false;
		_bool	IsMask = false;
		_bool	IsTrigger = false;
		_bool	IsMovingPosition = false;
		_bool	bUseChild = false;
		_bool	bSpread = false;
		// ~Option // 

		_bool   bFreeMove = false;

	}EFFECTDESC;

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& rhs);
	virtual ~CEffect_Base() = default;

public:
	HRESULT   Load_E_Desc(const _tchar* pFilePath);
	_float4x4 Get_InitMatrix() { return m_InitWorldMatrix; }
	vector<_float4>*	Get_PrePos() { return &m_vecProPos; }
	HRESULT   Set_InitTrail(const _tchar* pPrototypeTag, _int iCnt);
	HRESULT   Set_InitChild(EFFECTDESC eEffectDesc, _int iCreateCnt, const char* ProtoTag, _matrix worldmatrix);

public:
	void	             Set_EffectDesc(EFFECTDESC eEffectDesc) {
		memcpy(&m_eEFfectDesc, &eEffectDesc, sizeof(EFFECTDESC));
	}
	EFFECTDESC           Get_EffectDesc() { return m_eEFfectDesc; }

	// Effect Desc
	void						 Set_EffectDescDTexture(_int iSelectIdx, _float fDTextureframe) {
		m_eEFfectDesc.fFrame[iSelectIdx] = fDTextureframe;
	}
	void						 Set_EffectDescMTexture(_int iSelectIdx, _float fMTextureframe) {
		m_eEFfectDesc.fMaskFrame[iSelectIdx] = fMTextureframe;
	}
	void						 Set_EffectDescNTexture(_float fNTextureframe) { m_eEFfectDesc.fNormalFrame = fNTextureframe; }

	_uint					     Get_ChildCnt() { return (_uint)m_vecChild.size(); }
	vector<class CEffect_Base*>* Get_vecChild() { return &m_vecChild; }

	class CGameObject*			 Get_Parent() { return m_pParent; }
	void						 Set_Parent(class CGameObject* pParrent) { m_pParent = pParrent; }

	void						 Set_Matrix();
	void						 Set_InitMatrix(_fmatrix WorldMatrix) { XMStoreFloat4x4(&m_InitWorldMatrix, WorldMatrix); }

	virtual void    Set_Active(_bool bActive) { m_eEFfectDesc.bActive = bActive; }
	//void    Set_AddScale(_float fScale) { m_eEFfectDesc.vScale *= fScale; }
	void    Set_AddScale(_float fScale) {
		_float4	vScale = m_eEFfectDesc.vScale;
		vScale.x += fScale;
		vScale.y += fScale;
		vScale.z += fScale;
		m_eEFfectDesc.vScale = vScale;
	}
	void    Set_Scale(_vector vScale) { m_eEFfectDesc.vScale = vScale; }
	_bool   Get_Active() { return m_eEFfectDesc.bActive; }

	void	ResetSprite(){
		m_eEFfectDesc.fWidthFrame = m_fInitSpriteCnt.x;
		m_eEFfectDesc.fHeightFrame = m_fInitSpriteCnt.y;
		m_bFinishSprite = false;
	}
	void SetSprite(_float2 InitSprit) {
		m_fInitSpriteCnt = InitSprit;
	}
	void	TurnOffSystem(_float fDurationTime, _float fTimeDelta);

public: // Texture Cnt
	_int    Get_TotalDTextureCnt() { return m_iTotalDTextureComCnt; }
	_int    Get_TotalMTextureCnt() { return m_iTotalMTextureComCnt; }
	void    Set_TotalDTextureCnt(_int iTextureCnt) { m_iTotalDTextureComCnt = iTextureCnt; }
	void    Set_TotalMTextureCnt(_int iTextureCnt) { m_iTotalMTextureComCnt = iTextureCnt; }

public:
	_bool   Get_HaveTrail() { return m_eEFfectDesc.IsTrail; }
	void    Set_HaveTrail(_bool bHave) { m_eEFfectDesc.IsTrail = bHave; }
	void    Set_TrailDesc();

public:
	virtual void				 Set_FreePos() {}
	virtual _bool				 Play_FreePos(_float4& vPos) { return true; }
	virtual vector<_float4>*	 Get_FreePos() { return nullptr; }
	void						 Set_InitPos(vector<_float4> vecFreePos) { memcpy(&m_vecFreePos, &vecFreePos, sizeof(vecFreePos.size())); }

public:
	void						 BillBoardSetting(_float3 vScale);

public:
	virtual HRESULT				 Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT				 Initialize(void* pArg) override;
	virtual void				 Tick(_float fTimeDelta) override;
	virtual void				 Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				 Render() override;
	virtual void				 Imgui_RenderProperty()override;

public:
	virtual HRESULT				 Set_Child(EFFECTDESC eEffectDesc, _int iCreateCnt, char* ProtoTag) { return S_OK; }
	virtual HRESULT				 Edit_Child(const _tchar * ProtoTag) { return S_OK; }

	virtual HRESULT				 Set_Trail(class CEffect_Base* pEffect, const _tchar* pProtoTag) { return S_OK; }
	virtual class CEffect_Trail* Get_Trail() { return nullptr; }
	virtual void				 Delete_Trail(const _tchar* pProtoTag) {}

public:
	HRESULT	Edit_TextureComponent(_uint iDTextureComCnt, _uint iMTextureComCnt);

protected:
	CModel*						m_pModelCom = nullptr;
	CShader*				    m_pShaderCom = nullptr;
	CRenderer*				    m_pRendererCom = nullptr;
	CVIBuffer_Rect*			    m_pVIBufferCom = nullptr;
	CVIBuffer_Point_Instancing*	m_pVIInstancingBufferCom = nullptr;
	CVIBuffer_Trail*			m_pVITrailBufferCom = nullptr;

	CTexture*					m_pDTextureCom[MAX_TEXTURECNT] = { nullptr };
	CTexture*					m_pMTextureCom[MAX_TEXTURECNT] = { nullptr };
	CTexture*					m_pNTextureCom = nullptr;

	EFFECTDESC			        m_eEFfectDesc;

protected:
	/* Child */
	_uint						m_iHaveChildCnt = 0;
	_float4x4					m_InitWorldMatrix;
	vector<class CEffect_Base*> m_vecChild;

	class CGameObject*			m_pParent = nullptr;
	_float4x4					m_WorldWithParentMatrix;
	/* ~Child */

	/* Trail  */
	vector<_float4>				m_vecProPos;
	class CEffect_Base*		    m_pEffectTrail = nullptr;
	/* ~Trail */

	/* Texture Setting */
	_uint	m_iTotalDTextureComCnt = 1;
	_uint	m_iTotalMTextureComCnt = 1;
	/* ~Texture Setting */

	/* FreeMoving */
	_float m_fFreePosTimeDelta = 0.0f;
	vector<_float4>  m_vecFreePos;

	_vector m_vPrePos;
	_vector m_vCurPos;

	_float m_fLerp = 0;
	_bool  m_bLerp = false;
	/* FreeMoving */

	_bool  m_bFinishSprite = false;
	_float2 m_fInitSpriteCnt = { 0.f,0.f };
	_float  m_fHDRValue = 0.0f;
	_float  m_fTurnOffTime = 0.0f;

public:
	virtual void          Free() override;
};

END