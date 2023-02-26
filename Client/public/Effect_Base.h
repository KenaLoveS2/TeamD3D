#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
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
		enum EFFECTTYPE        { EFFECT_PLANE, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_END };
		enum PARTICLETYPE	   { PARTICLE_BOX, PARTICLE_SPHERE, PARTICLE_CONE, PARTICLE_SPREAD, PARTICLE_END };
		enum TEXTURERENDERTYPE { TEX_ONE, TEX_SPRITE, TEX_END };
		enum TEXTURETYPE       { TYPE_DIFFUSE, TYPE_MASK, TYPE_END };
		enum BLENDSTATE        { BLENDSTATE_DEFAULT, BLENDSTATE_ALPHA, BLENDSTATE_ONEEFFECT, BLENDSTATE_MIX, BLENDSTATE_END };
		enum MOVEDIR           { MOVE_FRONT, MOVE_BACK, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, DIR_END };
		enum ROTXYZ            { ROT_X, ROT_Y, ROT_Z, ROT_END };

		EFFECTTYPE        eEffectType = EFFECT_PLANE;
		PARTICLETYPE	  eParticleType = PARTICLE_BOX;
		TEXTURERENDERTYPE eTextureRenderType = TEX_ONE;
		TEXTURETYPE		  eTextureType = TYPE_END;
		BLENDSTATE        eBlendType = BLENDSTATE_DEFAULT;
		MOVEDIR           eMoveDir = MOVE_FRONT;
		ROTXYZ            eRotation = ROT_Y;

		// Diffuse Frame ( Cur Texture Idx )
		_float		fFrame[MAX_TEXTURECNT] = { 0.0f };
		// Mask Frame ( Cur Texture Idx )
		_float		fMaskFrame[MAX_TEXTURECNT] = { 0.0f };

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

		// Option // 
		_bool	IsBillboard = false;
		_bool	IsMask = false;
		_bool	IsTrigger = false;
		_bool	IsMovingPosition = false;
		_bool	bUseChild = false;
		_bool	bSpread = false;

	}EFFECTDESC;

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& rhs);
	virtual ~CEffect_Base() = default;

public:
	void	             Set_EffectDesc(EFFECTDESC eEffectDesc) { 
		memcpy(&m_eEFfectDesc, &eEffectDesc, sizeof(EFFECTDESC)); }
	EFFECTDESC           Get_EffectDesc() { return m_eEFfectDesc; }

	// Effect Desc
	void	Set_EffectDescDTexture(_int iSelectIdx, _float fDTextureframe) { m_eEFfectDesc.fFrame[iSelectIdx] = fDTextureframe; }
	void	Set_EffectDescMTexture(_int iSelectIdx, _float fMTextureframe) { m_eEFfectDesc.fMaskFrame[iSelectIdx] = fMTextureframe; }

	_uint					     Get_ChildCnt() { return (_uint)m_vecChild.size(); }
	vector<class CEffect_Base*>* Get_vecChild() { return &m_vecChild; }

	class CEffect_Base* Get_Parent() { return m_pParent; }
	void				Set_Parent(class CEffect_Base* pParrent) { m_pParent = pParrent; }

	void				Set_Matrix();
	void				Set_InitMatrix(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_InitWorldMatrix, WorldMatrix);
	}
	_float4x4 Get_InitMatrix() { return m_InitWorldMatrix; }

public: // Texture Cnt
	_int    Get_TotalDTextureCnt() { return m_iTotalDTextureComCnt; }
	_int    Get_TotalMTextureCnt() { return m_iTotalMTextureComCnt; }

public:
	void				 BillBoardSetting(_float3 vScale);

public:
	virtual HRESULT      Initialize_Prototype() override;
	virtual HRESULT		 Initialize(void* pArg) override;
	virtual void		 Tick(_float fTimeDelta) override;
	virtual void		 Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		 Render() override;

public:
	virtual HRESULT		 Set_Child(EFFECTDESC eEffectDesc, _int iCreateCnt, char* ProtoTag) { return S_OK; }
	virtual HRESULT	     Edit_Child(const _tchar * ProtoTag) { return S_OK; }

public:
	HRESULT				 Edit_TextureComponent(_uint iDTextureComCnt, _uint iMTextureComCnt);

protected: 
	CShader*				    m_pShaderCom = nullptr;
	CRenderer*				    m_pRendererCom = nullptr;
	CVIBuffer_Rect*			    m_pVIBufferCom = nullptr;
	CVIBuffer_Point_Instancing*	m_pVIInstancingBufferCom = nullptr;

	CTexture*					m_pDTextureCom[MAX_TEXTURECNT] = { nullptr };
	CTexture*					m_pMTextureCom[MAX_TEXTURECNT] = { nullptr };

	/* Texture Setting */
	_uint	m_iTotalDTextureComCnt = 0;
	_uint	m_iTotalMTextureComCnt = 0;
	/* ~Texture Setting */

protected:
	 /* Child */
	_uint m_iHaveChildCnt = 0;
	vector<class CEffect_Base*> m_vecChild;

	class CEffect_Base*			m_pParent = nullptr;
	_float4x4					m_InitWorldMatrix;
	_float4x4					m_WorldWithParentMatrix;
	/* ~Child */

protected:
	EFFECTDESC			  m_eEFfectDesc;

public:
	virtual void          Free() override;
};

END