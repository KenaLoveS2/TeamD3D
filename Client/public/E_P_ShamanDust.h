#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_ShamanDust final : public CEffect_Point_Instancing
{
private:
	CE_P_ShamanDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_ShamanDust(const CE_P_ShamanDust& rhs);
	virtual ~CE_P_ShamanDust() = default;

public:
	virtual HRESULT       Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT       Initialize(void* pArg) override;
	virtual HRESULT		  Late_Initialize(void* pArg = nullptr)override;
	virtual void          Tick(_float fTimeDelta) override;
	virtual void          Late_Tick(_float fTimeDelta) override;
	virtual HRESULT       Render() override;
	
public:
	HRESULT				  SetUp_ShaderResources();
	
private:
	class CBossShaman*    m_pShaman = nullptr;
	_bool				  m_bTurn = false;

public:
	static  CE_P_ShamanDust*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END