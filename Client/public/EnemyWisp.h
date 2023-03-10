#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEnemyWisp final : public CEffect_Mesh
{
private:
	CEnemyWisp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyWisp(const CEnemyWisp& rhs);
	virtual ~CEnemyWisp() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

	HRESULT					Set_WispTrail();

private:
	_float2 m_fUV;

public:
	static CEnemyWisp*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END