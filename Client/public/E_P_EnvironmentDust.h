#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_EnvironmentDust final : public CEffect_Point_Instancing
{
private:
	CE_P_EnvironmentDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_EnvironmentDust(const CE_P_EnvironmentDust& rhs);
	virtual ~CE_P_EnvironmentDust() = default;

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
	void	Reset();
	_bool	m_bDissolve = false;
	_bool	m_bTurn = false;

public:
	static  CE_P_EnvironmentDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END