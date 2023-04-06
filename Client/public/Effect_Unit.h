#pragma once
#include "Effect_Base_S2.h"

/* The Container of multiple Effects */

BEGIN(Client)
class CEffect_Unit final : public CEffect_Base_S2
{
private:
	CEffect_Unit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Unit(const CEffect_Unit& rhs);
	virtual ~CEffect_Unit() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg = nullptr) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual void					Imgui_RenderProperty() override;

public:
	virtual HRESULT					Save_Data() override;
	virtual HRESULT					Load_Data(_tchar* fileName) override;

public:
	virtual	void					Activate(_float4 vPos)			override;
	virtual	void					Activate(CGameObject* pTarget)	override;
	virtual void					DeActivate()					override;

private:
	vector<CEffect_Base_S2*>		m_vecEffects;

public:
	static CEffect_Unit*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free()			override;
};
END
