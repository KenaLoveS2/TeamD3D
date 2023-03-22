#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Point_Instancing_S2;
END 

BEGIN(Client)
class CEffect_Particle_Base final : public CGameObject
{
private:
	CEffect_Particle_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Particle_Base(const CEffect_Particle_Base& rhs);
	virtual ~CEffect_Particle_Base() = default;

public:
	inline	void					Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg = nullptr) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual void					Imgui_RenderProperty() override;

public:
	HRESULT							Save_Data();
	HRESULT							Load_Data(char* fileName);

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();
	HRESULT							SetUp_Buffer();

private:	/* Tool Function */
	_float4		ColorCode();

private:
	CShader*						m_pShaderCom;
	CRenderer*						m_pRendererCom;
	CTexture*						m_pTextureCom;
	CVIBuffer_Point_Instancing_S2*	m_pVIBufferCom;
	
private: 
	CGameObject*					m_pTarget;
	char*							m_pfileName;

private: /* struct 로 뺄 수도 있는 목록 */
	_int							m_iRenderPass;
	_int							m_iTextureIndex;
	_float4							m_vColor;

public:
	static CEffect_Particle_Base*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*					Clone(void* pArg = nullptr) override;
	virtual void							Free()			override;

};
END