#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEnemyWisp final : public CEffect_Mesh
{
public:
	enum CHILD { CHILD_TRAIL, CHILD_BACK, CHILD_GROUND, CHILD_PARTICLE, CHILD_END };

private:
	CEnemyWisp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyWisp(const CEnemyWisp& rhs);
	virtual ~CEnemyWisp() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	_bool	IsActiveState();

	/* Event Caller */
public: 
	virtual void			Push_EventFunctions() override;
	virtual void			Calc_RootBoneDisplacement(_fvector vDisplacement) override;

public:
	virtual void			ImGui_AnimationProperty() override;
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

	HRESULT					Set_WispTrail();
	HRESULT					Set_WispEffects();

	/* Animation Event Func */
private:	
	void					TurnOnTrail(_bool bIsInit, _float fTimeDelta);
	void					TurnOnBack(_bool bIsInit, _float fTimeDelta);
	void					TurnOnGround(_bool bIsInit, _float fTimeDelta);
	void					TurnOnParticle(_bool bIsInit, _float fTimeDelta);

	void					TurnOffTrail(_bool bIsInit, _float fTimeDelta);
	void					TurnOffBack(_bool bIsInit, _float fTimeDelta);
	void					TurnOffGround(_bool bIsInit, _float fTimeDelta);
	void					TurnOffParticle(_bool bIsInit, _float fTimeDelta);

public:
	_bool	Get_Dissolve() { return m_bDissolve; }

private:
	_bool m_bDissolve = false;
	_float m_fDissolveTimeDelta = 0.0f;

public:
	static CEnemyWisp*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void					Free() override;
};

END