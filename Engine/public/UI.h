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
	} UIDESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;


public: /* Get */
	_fmatrix				Get_WorldMatrix();
	_fmatrix				Get_InitMatrix();
	_float3					Get_WorldScale();
	_float3					Get_OriginalSettingScale() { return m_vOriginalSettingScale; }
	_uint					Get_RenderPass() { return m_iRenderPass; }
	CTexture*				Get_DiffuseTexture() { return m_pTextureCom[TEXTURE_DIFFUSE]; }
	
public: /* Set */
	void					Set_Parent(CUI* pUI);
	HRESULT					Set_Texture(TEXTURE_TYPE eType, wstring textureComTag);
	void					Set_RenderPass(_uint iPass) { m_iRenderPass = iPass; }
	void					Set_LocalMatrix(_float4x4 matLocal) { m_matLocal = matLocal; }
	void					Set_TextureIndex(_uint iIdx) { m_iTextureIdx = iIdx; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	HRESULT					Add_Event(CUI_Event* pEvent);
	HRESULT					Delete_Event();
public:
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