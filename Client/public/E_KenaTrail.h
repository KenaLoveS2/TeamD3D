#pragma once
#include "Client_Defines.h"
#include "Effect_Trail.h"

BEGIN(Client)

class CE_KenaTrail final : public CEffect_Trail
{
private:
	CE_KenaTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaTrail(const CE_KenaTrail& rhs);
	virtual ~CE_KenaTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_ShaderResources();

private:
	class CTexture*			m_pTrailflowTexture = nullptr;
	class CTexture*			m_pTrailTypeTexture = nullptr;

	class CKena*			m_pKena = nullptr;
	_vector					m_vInitColor;
	_vector					m_vHeavyAttacktColor;

private:
	_uint m_iTrailFlowTexture = 0;
	_uint m_iTrailTypeTexture = 0;

	_float2  m_fUV = { 1.f,1.f };
	_float	 m_fTimeDelta = 0.0f;

public:
	static  CE_KenaTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END