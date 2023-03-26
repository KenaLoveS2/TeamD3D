#pragma once
#include "Effect_Base_S2.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Mesh_Base final : public CEffect_Base_S2
{
private:
	CEffect_Mesh_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mesh_Base(const CEffect_Mesh_Base& rhs);
	virtual ~CEffect_Mesh_Base() = default;

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

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();

private: /* Tool Function */
	HRESULT							Set_ModelCom();

private:
	CShader*						m_pShaderCom;
	CRenderer*						m_pRendererCom;
	CTexture*						m_pDiffuseTextureCom[5];
	CTexture*						m_pMaskTextureCom[5];
	CVIBuffer_Rect*					m_pVIBufferCom;
	CModel*							m_pModelCom;

private:
	//aiTextureType					m_eTextureType;
	_float							m_fTest = 0.f;

public:
	static CEffect_Mesh_Base*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr) override;
	virtual void						Free()			override;
};
END