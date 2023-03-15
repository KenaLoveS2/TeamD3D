#pragma once
#include "Client_Defines.h"
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_KenaHeavyAttack_Into final : public CEffect_Point_Instancing
{
private:
	CE_P_KenaHeavyAttack_Into(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_KenaHeavyAttack_Into(const CE_P_KenaHeavyAttack_Into& rhs);
	virtual ~CE_P_KenaHeavyAttack_Into() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT SetUp_ShaderResources();

public:
	static  CE_P_KenaHeavyAttack_Into* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END