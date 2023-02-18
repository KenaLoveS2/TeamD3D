#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect_Instancing;
END

BEGIN(Client)

class CEffect_Rect_Instancing final : public CGameObject
{
private:
	CEffect_Rect_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Rect_Instancing(const CEffect_Rect_Instancing& rhs);
	virtual ~CEffect_Rect_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect_Instancing*	m_pVIBufferCom = nullptr;	




private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CEffect_Rect_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END