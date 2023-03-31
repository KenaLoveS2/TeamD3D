#pragma once
#include "Effect_Base_S2.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Point_Instancing_S2;
END 

BEGIN(Client)
class CEffect_Particle_Base final : public CEffect_Base_S2
{
private:
	CEffect_Particle_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Particle_Base(const CEffect_Particle_Base& rhs);
	virtual ~CEffect_Particle_Base() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg = nullptr) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual void					Imgui_RenderProperty() override;

public:
	virtual HRESULT					Save_Data() override;
	virtual HRESULT					Load_Data(_tchar* fileName) override;

public:
	virtual	void					Activate(_float4 vPos)			override;
	virtual	void					Activate(CGameObject* pTarget)	override;
	virtual void					DeActivate()					override;

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();
	HRESULT							SetUp_Buffer();

private:
	CShader*						m_pShaderCom;
	CRenderer*						m_pRendererCom;
	CTexture*						m_pTextureCom;
	CVIBuffer_Point_Instancing_S2*	m_pVIBufferCom;
	
public:
	static CEffect_Particle_Base*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*					Clone(void* pArg = nullptr) override;
	virtual void							Free()			override;

};
END