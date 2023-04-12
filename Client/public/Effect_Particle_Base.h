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
	virtual	void					Activate(_float4 vPos)											override;
	virtual	void					Activate(CGameObject* pTarget)									override;
	virtual void					Activate(CGameObject* pTarget, _float4 vCorrectPos)				override;
	virtual void					Activate(CGameObject* pTarget, const char* pBoneName)			override;
	virtual void					Activate_Reflecting(_float4 vLook, _float4 vPos, _float fAngle) override;
	virtual void					DeActivate()													override;

public:
	void	Activate_BufferUpdate();
	void	Stop_Buffer();

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();
	HRESULT							SetUp_Buffer();

private:
	CShader*						m_pShaderCom;
	CRenderer*						m_pRendererCom;
	CTexture*						m_pTextureCom;
	CVIBuffer_Point_Instancing_S2*	m_pVIBufferCom;


	/* For. Recording */
private:
	void							RecordPath();
private:
	vector<_float4>					m_vecPos;
	_float							m_fTime;
	_float							m_fTimeAcc;
	/* ~ For. Recording */

	/* For. UI Type */
private:
	_bool							m_bUI;
	_float4x4						m_matView;
	_float4x4						m_matProj;


public:
	static CEffect_Particle_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void							Free()			override;

};
END