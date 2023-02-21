#pragma once

#include "Component.h"

/* ȭ�鿡 �׷����� ��ü���� �׸��� ������� ���������Ѵ�. */
/* �����ϰ� �ִ� ������� ��ü���� ��ο���(�����Լ��� ȣ���Ѵ�)�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {
		RENDER_PRIORITY,
		RENDER_SHADOW,
		RENDER_NONALPHABLEND,
		RENDER_NONLIGHT,
		RENDER_ALPHABLEND,
		RENDER_UI,
		RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Initialize_ShadowResources(_uint iWidth, _uint iHeight);

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);

#ifdef _DEBUG
	HRESULT Add_DebugRenderGroup(class CComponent* pComponent);
	_bool								m_bDebugRender = false;
#endif

	HRESULT Draw_RenderGroup();

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

private:
	list<class CComponent*>				m_DebugObject;
	typedef list<class CComponent*>		DEBUGOBJECTS;
	
private:
	class CTarget_Manager*			m_pTarget_Manager = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;
	class CLevel_Manager*			m_pLevel_Manager = nullptr;
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	class CShader*							m_pShader = nullptr;
	class CShader*							m_pShader_PostProcess = nullptr;
	_float4x4									m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	ID3D11DepthStencilView*			m_pShadowDepthStencilView = nullptr;

	_uint											m_iShadowWidth = 0, m_iShadowHeight = 0;

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_HDR();
	HRESULT Render_PostProcess();
	HRESULT Render_UI();

#ifdef _DEBUG
private:
	HRESULT Render_DebugObject();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRenderer* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END