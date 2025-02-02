#pragma once
#include "Kena_Parts.h"

BEGIN(Client)

class CKena_Staff final : public CKena_Parts
{
private:
	CKena_Staff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_Staff(const CKena_Staff& rhs);
	virtual ~CKena_Staff() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;
	virtual void		Imgui_RenderProperty() override;
	virtual void		ImGui_AnimationProperty() override;
	virtual void		ImGui_ShaderValueProperty() override;

	virtual HRESULT		RenderCine() override;
	HRESULT				SetUp_CineShaderResources();

private:
	virtual HRESULT		SetUp_Components() override;
	virtual HRESULT		SetUp_ShaderResource() override;
	virtual HRESULT		SetUp_ShadowShaderResources() override;

public:
	HRESULT		Ready_Effects();

public:
	void		Tick_TrailOption(_float4 vPosition);
	void		Tick_ChargeOption(_float4 vPosition);
	void    	Tick_EffectWorld(OUT _float4& vWeaponPosition);

private:
	map<const string, class CEffect_Base*>   m_mapEffect;

	_float  m_fTimeDelta = 0.0f;
	_float  m_fBowDurationTime = 0.0f;
	_float4 m_vStaffPosition = _float4(0.0f, 0.0f, 0.0f, 1.0f);

public:
	static CKena_Staff*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END