#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_TeleportRot final : public CEffect
{
private:
	CE_TeleportRot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_TeleportRot(const CE_TeleportRot& rhs);
	virtual ~CE_TeleportRot() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static  CE_TeleportRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END