#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_Rain final : public CEffect_Point_Instancing
{
private:
	CE_P_Rain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Rain(const CE_P_Rain& rhs);
	virtual ~CE_P_Rain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	void			Reset();

public:
	static  CE_P_Rain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END