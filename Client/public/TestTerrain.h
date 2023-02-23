#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain_Tess;
class CVIBuffer_Terrain;
END


BEGIN(Client)
class CTestTerrain : public CGameObject
{
public:
	enum TEXTURE_TEST { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };
private:
	CTestTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestTerrain(const CTestTerrain& rhs);
	virtual ~CTestTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain_Tess*		m_pVIBufferCom = nullptr;
	//CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	//CNavigation*			m_pNavigationCom = nullptr;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTestTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
