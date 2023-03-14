#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_KenaDust final : public CEffect
{
private:
	CE_KenaDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaDust(const CE_KenaDust& rhs);
	virtual ~CE_KenaDust() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static  CE_KenaDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END