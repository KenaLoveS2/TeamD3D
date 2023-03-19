#pragma once
#include "Client_Defines.h"
#include "Effect_Trail.h"

BEGIN(Client)

class CE_SpiritArrowTrail final : public CEffect_Trail
{
private:
	CE_SpiritArrowTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_SpiritArrowTrail(const CE_SpiritArrowTrail& rhs);
	virtual ~CE_SpiritArrowTrail() = default;

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

	_float	 m_fTimeDelta = 0.0f;

public:
	static  CE_SpiritArrowTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END