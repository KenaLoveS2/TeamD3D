#pragma once
#include "Client_Defines.h"
#include "Effect_Base.h"

BEGIN(Client)

#define TESTPLAY

class CEffect_Trail final : public CEffect_Base
{
private:
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

public:
	void	Set_WorldMatrix(_matrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }
	void	Set_TrailPosition(_vector vPosition) { m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition); }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void	Set_ParticleIdx(_int iIndex) { m_iIndex = iIndex; }

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	_float					m_fTimeDelta = 0.0f;
	_int					m_iIndex = 0;

public:
	static  CEffect_Trail*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;

};

END