#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSpiritArrow final : public CEffect_Mesh
{
public:
	enum ARROWSTATE { ARROW_CHARGE, ARROW_READY, ARROW_FIRE, ARROW_HIT, ARROWSTATE_END };

private:
	CSpiritArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpiritArrow(const CSpiritArrow& rhs);
	virtual ~CSpiritArrow() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual void				Push_EventFunctions() override;

private:
	class CKena*				m_pKena = nullptr;
	class CKena_Staff*		m_pStaff = nullptr;
	class CCamera_Player*	m_pCamera = nullptr;

private:
	_bool						m_bActive = false;

	ARROWSTATE			m_eCurState = ARROWSTATE_END;
	ARROWSTATE			m_ePreState = ARROWSTATE_END;
	_float						m_fScale = 1.f;
	_float						m_fMaxScale = 2.f;
	_float						m_fScalePosRate = 0.35f;

	_float						m_fMinDistance = 10.f;
	_float						m_fMaxDistance = 30.f;
	_float						m_fDistance = 10.f;
	_bool						m_bReachToAim = false;

	_float4					m_vFirePosition;
	_float4					m_vFireCamLook;
	_float4					m_vFireCamPos;
	_bool						m_bHit = false;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();
	ARROWSTATE			Check_State();
	void						Update_State(_float fTimeDelta);
	virtual _int				Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	inline void					Reset();

public:
	static CSpiritArrow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END