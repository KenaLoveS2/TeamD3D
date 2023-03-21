#pragma once
#include "Client_Defines.h"
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_Sapling final : public CEffect_Point_Instancing
{
private:
	CE_P_Sapling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Sapling(const CE_P_Sapling& rhs);
	virtual ~CE_P_Sapling() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static  CE_P_Sapling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END