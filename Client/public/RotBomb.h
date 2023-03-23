#pragma once
#include "Effect_Mesh.h"

BEGIN(Engine)
class CAnimationState;
END

BEGIN(Client)

class CRotBomb final : public CEffect_Mesh
{
public:
	enum BOMBSTATE { BOMB_CHARGE, BOMB_INJECT_CHARGE, BOMB_READY, BOMB_RELEASE, BOMB_LAND, BOMB_BOOM, BOMBSTATE_END };
	enum ANIMATION { CHARGE, CHARGE_LOOP, INJECT, LAND, LAND_LOOP, LAND_RUMBLE_ADD, REFPOSE, ANIMATION_END };
	enum CHILD_TYPE { CHILD_COVER, CHILD_PARTICLE, CHILD_END };

public:
	CRotBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotBomb(const CRotBomb& rhs);
	virtual ~CRotBomb() = default;

public:
	const BOMBSTATE&		Get_CurrentState() const { return m_eCurState; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual void				ImGui_AnimationProperty() override;

public:
	void						Reset();
	void						Set_Child();

private:
	CAnimationState*		m_pAnimation = nullptr;
	class CKena*				m_pKena = nullptr;
	class CE_BombTrail*		m_pPathTrail = nullptr;

private:
	BOMBSTATE				m_eCurState = BOMBSTATE_END;
	BOMBSTATE				m_ePreState = BOMBSTATE_END;

	_bool						m_bInject = false;
	_float						m_fInitScale = 0.1f;
	_float						m_fMaxScale = 1.f;
	_float						m_fInjectScale = 1.3f;
	_float						m_fScale = 1.f;
	_float3					m_vAimPos;
	list<_float4>				m_PathList;

	_bool						m_bHit = false;
	CGameObject*			m_pTarget = nullptr;
	_float4					m_vHitPosition;

	_bool						m_bBoom = false;
	_float						m_fBoomTimer = 0.f;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_State();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();

	BOMBSTATE				Check_State();
	void						Update_State(_float fTimeDelta);
	void						Calculate_Path(_float fTimeDelta);
	void						Throw(_float fTimeDelta);

	virtual _int				Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;

public:
	static CRotBomb*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END