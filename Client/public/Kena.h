#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "UI_ClientManager.h"
#include "delegator.h"
#include "Model.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CStateMachine;
class CAnimationState;
END

BEGIN(Client)

class CUI_RotIcon;

class CKena final : public CGameObject
{
	friend class CKena_State;

public:
	enum DAMAGED_FROM { DAMAGED_FRONT, DAMAGED_BACK, DAMAGED_LEFT, DAMAGED_RIGHT, DAMAGED_FROM_END };
	enum COLLIDERTYPE { COLL_BODY, COLL_STAFF, COLLIDERTYPE_END };

private:
	CKena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena(const CKena& rhs);
	virtual ~CKena() = default;

public:
	class CKena_State*		Get_State() { return m_pKenaState; }
	class CKena_Parts*		Get_KenaPart(const _tchar* pCloneObjectTag);
	class CKena_Status*		Get_Status() { return m_pKenaStatus; }
	_double					Get_AnimationPlayTime();
	const string&				Get_AnimationState() const;

	const _bool&				Is_Attack() const { return m_bAttack; }
	const _bool&				Is_Bow() const { return m_bBow; }
	const _bool&				Is_ChargeLight() const { return m_bChargeLight; }


public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_AnimationProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual void				Update_Child() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void				Push_EventFunctions() override;
	virtual void				Calc_RootBoneDisplacement(_fvector vDisplacement) override;

public:
	void						Call_RotIcon(CGameObject* pTarget);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CAnimationState*		m_pAnimation = nullptr;
	class CKena_State*		m_pKenaState = nullptr;
	class CKena_Status*		m_pKenaStatus = nullptr;

	class CCamera_Player*	m_pCamera = nullptr;
	class CTerrain*			m_pTerrain = nullptr;
	class CRope_RotRock*	m_pRopeRotRock = nullptr;
	class CRot*				m_pFirstRot = nullptr;
	class CRotForMonster*	m_pRotForMonster[8] = { nullptr, };

private:
	vector<class CKena_Parts*>				m_vecPart;
	vector<class CSpiritArrow*>				m_vecArrow;
	class CSpiritArrow*							m_pCurArrow = nullptr;
	map<const string, class CEffect_Base*>	m_mapEffect;

private:
	_bool						m_bAttack = false;
	_bool						m_bCommonHit = false;
	_bool						m_bHeavyHit = false;
	_float4						m_vDamagedDir;
	DAMAGED_FROM		m_eDamagedDir;

	_bool					m_bChargeLight = false;
	_bool					m_bSprint = false;

	_bool					m_bAim = false;
	_bool					m_bBow = false;

	_float					m_fInertia = 5.f;
	_float					m_fVelocity = 0.f;

	_bool					m_bOnGround = true;
	_bool					m_bJump = false;
	_bool					m_bPulseJump = false;
	_float					m_fGravity = 9.81f;
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

	/* PhysX */
	vector<wstring>		m_vecColliderName;
	vector<_float3>		m_vecPivot;
	vector<_float3>		m_vecPivotScale;
	vector<_float3>		m_vecPivotRot;

	CUI_RotIcon*			m_pFocusRot;

private:
	HRESULT					Ready_Parts();
	HRESULT					Ready_Arrows();
	HRESULT					Ready_Effects();
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();
	HRESULT					SetUp_State();
	HRESULT					SetUp_UI();
	void						Update_Collider(_float fTimeDelta);

private:
	DAMAGED_FROM			Calc_DirToMonster(CGameObject* pTarget);
	DAMAGED_FROM			Calc_DirToMonster(const _float3& vCollisionPos);

private:	/* Animation Event Func */
	void					Test(_bool bIsInit, _float fTimeDelta);
	void					TurnOnAttack(_bool bIsInit, _float fTimeDelta);
	void					TurnOffAttack(_bool bIsInit, _float fTimeDelta);

	void					TurnOnCharge(_bool bIsInit, _float fTimeDelta);
	void					TurnOffCharge(_bool bIsInit, _float fTimeDelta);

	void					TurnOnPulseJump(_bool bIsInit, _float fTimeDelta);

public:
	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, _float>		m_PlayerDelegator;

	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, CKena*>		m_PlayerPtrDelegator;

	_float m_fTest = 0.f;

public:
	static CKena*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;

	virtual _int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex);

	void Test_Raycast();
	void Set_RopeRotRockPtr(class CRope_RotRock* pObject) { m_pRopeRotRock = pObject; }
	void Set_FirstRotPtr(class CRot* pFirstRot) { m_pFirstRot = pFirstRot; }
	
	class CKena_Status*	Get_KenaStatusPtr() { return m_pKenaStatus; }
};

END