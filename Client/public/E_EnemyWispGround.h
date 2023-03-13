#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_EnemyWispGround final : public CEffect
{
private:
	CE_EnemyWispGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_EnemyWispGround(const CE_EnemyWispGround& rhs);
	virtual ~CE_EnemyWispGround() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_ShaderResources();

private:
	_float	m_fDissolveTime = 0.0f;

public:
	static  CE_EnemyWispGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END