#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	enum	TEXTURE_TYPE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_END };
	typedef struct tagUIDesc
	{
		wstring				fileName; 
		_float3				vPos;
		_float2				vSize;
		_float4x4			ViewMatrix;
		_float4x4			ProjMatrix;
	} UIDESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;


public:
	_fmatrix			Get_WorldMatrix();
	_fmatrix			Get_InitMatrix();
	void				Set_Parent(CUI* pUI);
	HRESULT				Set_Texture(TEXTURE_TYPE eType, wstring textureComTag);
	void				Set_RenderPass(_uint iPass) { m_iRenderPass = iPass; }
	void				Set_LocalMatrix(_float4x4 matLocal) { m_matLocal = matLocal; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

protected:
	void					Imgui_RenderingSetting();

protected:
	virtual HRESULT					Save_Data() { return S_OK; }
	virtual HRESULT					Load_Data(wstring fileName) { return S_OK; }



protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom[TEXTURE_END] = { nullptr, };

protected:
	UIDESC				m_tDesc;
	CUI*				m_pParent;
	_bool				m_bActive;
	_uint				m_iRenderPass;
	wstring				m_TextureComTag[TEXTURE_END];

	/* For. Node (mostly) */
	_float4x4			m_matInit;
	_float4x4			m_matParentInit;
	_float4x4			m_matLocal;

	_uint				m_iLastWinSizeX = 0, m_iLastWinSizeY = 0;
	_float				m_fAspectRatioX = 1.f, m_fAspectRatioY = 1.f;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free()	override;


};
END