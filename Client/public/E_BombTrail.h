#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CE_BombTrail final : public CEffect_Trail
{
private:
	CE_BombTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_BombTrail(const CE_BombTrail& rhs);
	virtual ~CE_BombTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_ShaderResources();

private:
	class CTexture*			m_pTrailflowTexture = nullptr;
	class CTexture*			m_pTrailTypeTexture = nullptr;

private:
	_uint m_iTrailFlowTexture = 0;
	_uint m_iTrailTypeTexture = 0;

public:
	static  CE_BombTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END