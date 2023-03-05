#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "UI_ClientManager.h"
#include "delegator.h"
#include "Model.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
class CStateMachine;
class CAnimationState;
END

BEGIN(Client)

class CKena final : public CGameObject
{
	friend class CKena_State;

private:
	CKena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena(const CKena& rhs);
	virtual ~CKena() = default;

public:
	_double					Get_AnimationPlayTime();

public:
	class CKena_State*		Get_State() { return m_pKenaState; }
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;
	virtual void					Update_Child() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void			Push_EventFunctions() override;
	virtual void				Calc_RootBoneDisplacement(_fvector vDisplacement) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*					m_pRangeCol = nullptr;
	CNavigation*				m_pNavigationCom = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CAnimationState*		m_pAnimation = nullptr;
	class CKena_State*		m_pKenaState = nullptr;

	class CCamera_Player*	m_pCamera = nullptr;

private:
	vector<class CKena_Parts*>	m_vecPart;
	map<const string, class CEffect_Base*>	m_mapEffects;

	// CloneTag, Effect
	map<const string, class CEffect_Base*>   m_mapEffect;

private:
	_bool					m_bAttack = false;

	_float					m_fInertia = 5.f;

	_bool					m_bJump = false;
	_bool					m_bDoubleJump = false;
	_float					m_fGravity;
	_float					m_fInitJumpSpeed;
	_float					m_fCurJumpSpeed;

	/* Shader */
	_float					m_fSSSAmount = 0.01f;
	_float4					m_vSSSColor = _float4(0.8f, 0.7f, 0.6f, 1.f);
	_float4					m_vMulAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float4					m_vEyeAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float					m_fLashWidth = 10.f;
	_float					m_fLashDensity = 10.f;
	_float					m_fLashIntensity = 10.f;

private:
	HRESULT					Ready_Parts();
	HRESULT					Ready_Effects();
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();
	HRESULT					SetUp_State();
	void					Test(_bool bIsInit, _float fTimeDelta);

public:
	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, _float>		m_PlayerDelegator;
	_float m_fTest = 0.f;

public:
	static CKena*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END