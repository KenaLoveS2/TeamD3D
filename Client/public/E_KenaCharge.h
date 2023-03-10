#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_KenaCharge final : public CEffect
{
private:
	CE_KenaCharge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaCharge(const CE_KenaCharge& rhs);
	virtual ~CE_KenaCharge() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Child();
	void    Set_InitMatrixScaled(_float3 vScale);

private:
	_float  m_fDurationActiveTime = 0.0f;
	_float  m_fScale = 1.0f;

	_float3 m_InitScale;
	_float3 m_InitColor;

private:
	HRESULT SetUp_ShaderResources();

public:
	static  CE_KenaCharge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END