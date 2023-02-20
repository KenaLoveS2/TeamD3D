#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
END

BEGIN(Client)

class CEffect_Base abstract : public CGameObject
{
public:
	typedef struct tagEffectDesc
	{
		// Effect Type = texture, texture_Particle , mesh
		enum EFFECTTYPE  { EFFECT_PLANE, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_END };
		enum TEXTURERENDERTYPE { TEX_ONE, TEX_SPRITE, TEX_END };
		enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_END };
		enum BLENDSTATE  { BLENDSTATE_DEFAULT, BLENDSTATE_ALPHA, BLENDSTATE_ONEEFFECT, BLENDSTATE_END };

		EFFECTTYPE        eEffectType = EFFECT_PLANE;
		TEXTURERENDERTYPE eTextureRenderType = TEX_ONE;
		TEXTURETYPE		  eTextureType = TYPE_END;
		BLENDSTATE        eBlendType = BLENDSTATE_DEFAULT;

		// Diffuse Frame ( Cur Texture Idx )
		_float		fFrame = 0.0f;
		// Mask Frame ( Cur Texture Idx )
		_float		fMaskFrame = 0.0f;

		// if ( eTextureType == TEX_SPRITE )
		_float	fWidthFrame = 0.0f, fHeightFrame = 0.0f;
		_int	iSeparateWidth = 0, iSeparateHeight = 0;
		_int	iWidthCnt = 0, iHeightCnt = 0;
		
		// Color & Scale
		_vector	vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		_vector	vScale;

		_bool		IsTrigger = false;

	}EFFECTDESC;

protected:
	CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base(const CEffect_Base& rhs);
	virtual ~CEffect_Base() = default;

public:
	virtual HRESULT      Initialize_Prototype() override;
	virtual HRESULT		 Initialize(void* pArg) override;
	virtual void		 Tick(_float fTimeDelta) override;
	virtual void		 Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		 Render() override;

protected:
	CShader*			  m_pShaderCom = nullptr;
	CRenderer*			  m_pRendererCom = nullptr;
	CTexture*			  m_pTextureCom = nullptr;
						  
	EFFECTDESC			  m_eEFfectDesc;

//  VIBuffer는 각자 클래스에서 정의
//	CVIBuffer_Rect*		  m_pVIBufferCom = nullptr;	

private:
	HRESULT				  SetUp_Components();
	HRESULT				  SetUp_ShaderResources();

public:
	virtual void          Free() override;
};

END