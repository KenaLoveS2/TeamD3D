#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CSpiritArrow final : public CEffect_Mesh
{
public:
	enum ARROWSTATE { ARROW_CHARGE, ARROW_INJECT_CHARGE, ARROW_READY, ARROW_FIRE, ARROW_INJECT_FIRE, ARROW_HIT, ARROWSTATE_END };
	enum EFFECTS { EFFECT_POSITION, EFFECT_HIT, EFFECT_TRAIL, EFFECT_END };

private:
	CSpiritArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpiritArrow(const CSpiritArrow& rhs);
	virtual ~CSpiritArrow() = default;

public:
	const ARROWSTATE&			Get_CurrentState() const { return m_eCurState; }
	const _int&					Get_Damage() const { return m_iDamage; }
	const _bool&				IsActive() const { return m_bActive; }
	virtual void				Set_Active(_bool bActive) override { m_bActive = bActive; }
	void						Set_Damage(_int fDamage) { m_iDamage = fDamage; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual HRESULT				Late_Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				RenderShadow() override;
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual void				Push_EventFunctions() override;

public:
	void						Reset();
	void						Set_Child();

private:
	class CKena*				m_pKena = nullptr;
	class CKena_Staff*			m_pStaff = nullptr;
	class CCamera_Player*		m_pCamera = nullptr;

private:
	_bool						m_bActive = false;

	ARROWSTATE					m_eCurState = ARROWSTATE_END;
	ARROWSTATE					m_ePreState = ARROWSTATE_END;
	_float						m_fScale = 1.f;
	_float						m_fMaxScale = 2.f;
	_float						m_fScalePosRate = 0.35f;
	_float						m_fInjectScale = 1.f;
	_float						m_fMaxInjectScale = 3.f;
	_float						m_fInjectFireTime = 0.f;

	_float						m_fMinDistance = 10.f;
	_float						m_fMaxDistance = 30.f;
	_float						m_fDistance = 10.f;
	_bool						m_bReachToAim = false;

	_int						m_iDamage = 20;

	_float4						m_vFirePosition;
	_float4						m_vFireCamLook;
	_float4						m_vFireCamPos;
	_bool						m_bHit = false;

private:
	_float		m_fWaveHeight = 1.f;
	_float		m_fSpeed = 10.f;
	_float		m_fWaveFrequency = 5.f;
	_float		m_fUVSpeed = 8.f;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();
	ARROWSTATE				Check_State();
	void					Update_State(_float fTimeDelta);
	virtual _int			Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;

private:
	void					PlaySound_Charge();
	void					PlaySound_Charge_Full();
	void					PlaySound_Fire();
	void					PlaySound_Inject_Charge();
	void					PlaySound_Inject_Charge_Full();
	void					PlaySound_Inject_Fire();
	void					PlaySound_Fly();
	void					PlaySound_Hit();

public:
	static CSpiritArrow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END