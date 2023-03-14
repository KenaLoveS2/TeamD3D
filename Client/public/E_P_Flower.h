#pragma once
#include "Effect_Point_Instancing.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_P_Flower final : public CEffect_Point_Instancing
{
private:
	CE_P_Flower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Flower(const CE_P_Flower& rhs);
	virtual ~CE_P_Flower() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static  CE_P_Flower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END