#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_MousePointer final : public CGameObject
{
private:
	CUI_MousePointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MousePointer(const CUI_MousePointer& rhs);
	virtual ~CUI_MousePointer() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void			Set_Active(_bool bActive) { m_bActive = bActive; }

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	CShader*		m_pShaderCom	= nullptr;
	CRenderer*		m_pRendererCom	= nullptr;
	CVIBuffer_Rect* m_pVIBufferCom	= nullptr;
	CTexture*		m_pTextureCom	= nullptr;

private:
	_bool			m_bActive;
	_float4x4		m_matView;
	_float4x4		m_matProj;
	_uint			m_iRenderPass;

public:
	static	CUI_MousePointer*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END

