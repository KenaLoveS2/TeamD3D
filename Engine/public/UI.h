#pragma once
#include "GameObject.h"
#include "UI_Event.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CUI_Event;
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
		_uint				iIndex;
	} UIDESC;
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;


public: /* Get */
	_matrix					Get_WorldMatrix()			{ return m_pTransformCom->Get_WorldMatrix(); }
	_matrix					Get_InitMatrix()			{ return XMLoadFloat4x4(&m_matInit); }
	_float4x4				Get_InitMatrixFloat4x4()	{ return m_matInit; }
	_matrix					Get_LocalMatrix()			{ return XMLoadFloat4x4(&m_matLocal); }
	_float4x4				Get_LocalMatrixFloat4x4()	{ return m_matLocal; }
	_float3					Get_WorldScale()			{ return m_pTransformCom->Get_Scaled(); }
	_float3					Get_OriginalSettingScale()	{ return m_vOriginalSettingScale; }
	_uint					Get_RenderPass()			{ return m_iRenderPass; }
	CTexture*				Get_DiffuseTexture()		{ return m_pTextureCom[TEXTURE_DIFFUSE]; }
	_bool					Is_Active() { return m_bActive; }

public: /* Set */
	void					Set_Parent(CUI* pUI);
	void					Set_Active(_bool bActive) { m_bActive = bActive;}
	HRESULT					Set_Texture(TEXTURE_TYPE eType, wstring textureComTag);
	void					Set_RenderPass(_uint iPass)			{ m_iRenderPass = iPass; }
	void					Set_LocalMatrix(_float4x4 matLocal) { m_matLocal = matLocal; }
	void					Set_TextureIndex(_uint iIdx) {  /* For Diffuse Texture */
		if(m_pTextureCom[TEXTURE_DIFFUSE]!=nullptr)
			m_iTextureIdx = iIdx; 
	}
	void					Set_OriginalSettingScale(_float3 vScale) { m_vOriginalSettingScale = vScale; }
	void					Set_LocalTranslation(_float4 vPos)	{ 
		m_matLocal._41 = vPos.x;
		m_matLocal._42 = vPos.y;
		m_matLocal._43 = vPos.z;
	}
	void					Be_Active(_bool bActive) { m_bActive = bActive; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	HRESULT					Add_Event(CUI_Event* pEvent);
	HRESULT					Delete_Event();
	void					Change_DiffuseTexture(CTexture* pTexture, CTexture** ppOrigin = nullptr);

public:
	virtual HRESULT			Save_Data() { return S_OK; }
	virtual HRESULT			Load_Data(wstring fileName) { return S_OK; }



protected:
	virtual HRESULT			SetUp_Components() = 0;
	virtual HRESULT			SetUp_ShaderResources(); /* For. Events */
	void					Imgui_RenderingSetting();
	void					Imgui_EventSetting();

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom[TEXTURE_END] = { nullptr, };

protected:
	UIDESC					m_tDesc;
	CUI*					m_pParent;
	_bool					m_bActive;
	_uint					m_iRenderPass;
	wstring					m_TextureComTag[TEXTURE_END];
	_int					m_TextureListIndices[TEXTURE_END]; /* Save the index of texture for using save/load */

	_uint					m_iTextureIdx; /* Diffuse Texture's index (for render) */

										   /* For. Node (mostly) */
	_float4x4				m_matInit;
	_float4x4				m_matParentInit;
	_float4x4				m_matLocal;
	_float4x4				m_matLocalOriginal;

	/* For. Save_Data */
	/* Some Data is changed during the game...*/
	_uint					m_iOriginalRenderPass;


	/* For. Event */
	/* it's not a real original scale. it's the scale after the scale setting.*/
	_float3					m_vOriginalSettingScale;

protected: /* Event */
		   //_uint					m_iEventNum; /* Mostly, One UI gets One Events, but for extension */
	vector<CUI_Event*>		m_vecEvents;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free()	override;


};
END