#pragma once
#include "Client_Defines.h"
#include "Effect_Trail.h"

BEGIN(Client)

class CE_EnemyWispTrail final : public CEffect_Trail
{
private:
	CE_EnemyWispTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_EnemyWispTrail(const CE_EnemyWispTrail& rhs);
	virtual ~CE_EnemyWispTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_ShaderResources();

private:
	class CTexture*			m_pTrailflowTexture[2] = { nullptr };

private:
	_uint m_iTrailFlowTexture[2] = { 0,9 };

	_float2  m_fUV = { 1.f,1.f };
	_float	 m_fTimeDelta = 0.0f;

public:
	static  CE_EnemyWispTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END