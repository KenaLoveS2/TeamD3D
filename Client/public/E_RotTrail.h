#pragma once
#include "Client_Defines.h"
#include "Effect_Trail.h"

BEGIN(Client)

class CE_RotTrail final : public CEffect_Trail
{
private:
	CE_RotTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_RotTrail(const CE_RotTrail& rhs);
	virtual ~CE_RotTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_ShaderResources();

public:
	static  CE_RotTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END