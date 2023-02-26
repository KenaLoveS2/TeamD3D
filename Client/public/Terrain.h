#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Client)
class CTerrain final : public CGameObject
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*						m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	CNavigation*				m_pNavigationCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END