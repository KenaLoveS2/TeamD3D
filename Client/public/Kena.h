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
class CMonster;
class CUI_RotIcon;
class CUI_FocusMonster;
class CKena final : public CGameObject
{
	friend class CKena_State;

public:
	enum DAMAGED_FROM { DAMAGED_FRONT, DAMAGED_BACK, DAMAGED_LEFT, DAMAGED_RIGHT, DAMAGED_FROM_END };
	enum COLLIDERTYPE { COLL_BODY, COLL_STAFF, COLLIDERTYPE_END };
	enum STATERETURN { STATE_ATTACK, STATE_HEAVYATTACK, STATE_PERFECTATTACK, STATE_COMMONHIT, STATE_HEAVYHIT, STATE_SPRINT, STATE_AIM, STATE_BOW, STATE_INJECTBOW, STATE_BOMB, STATE_INJECTBOMB,  STATE_PULSE, STATE_PARRY, STATE_JUMP, STATERETURN_END };

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
	const _uint				Get_AnimationStateIndex() const;
	vector<_float4>*			Get_WeaponPositions() { return &m_vecWeaposPos; }

	const _bool				Get_State(STATERETURN eState) const;
	const _bool&				Is_Attack() const { return m_bAttack; }
	const _bool&				Is_Bow() const { return m_bBow; }
	const _bool&				Is_TrailON() const { return m_bTrailON; }
	const _bool&				Is_ChargeLight() const { return m_bChargeLight; }
	void						TurnOn_TeleportFlower() { m_bTeleportFlower = true; }
	void						Set_State(STATERETURN eState, _bool bValue);
	void						Set_AttackObject(CGameObject* pObject) { m_pAttackObject = pObject; }
	void						Set_DamagedDir(DAMAGED_FROM eDir) { m_eDamagedDir = eDir; }

	void						Set_RotWispInteractable(_bool bInteractable) { m_bRotWispInteractable = bInteractable; }
	void						Add_HitStopTime(_float fTime) { m_fHitStopTime += fTime; }

	const _bool&				Is_StateLock() const{ return m_bStateLock; }
	void						Set_StateLock(_bool bLock) { m_bStateLock = bLock; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;

#ifdef _DEBUG
	virtual HRESULT		RenderCine() override;
	HRESULT					SetUp_CineShaderResources();
#endif

	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_AnimationProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	virtual void				Update_Child() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void				Push_EventFunctions() override;
	virtual void				Calc_RootBoneDisplacement(_fvector vDisplacement) override;

public:
	void						Smooth_Targeting(class CMonster* pMonster);

	//void						Call_FocusIcon(CGameObject* pTarget);
	void						Call_FocusRotIcon(CGameObject* pTarget);
	void						Call_FocusMonsterIcon(CGameObject* pTarget);
	
	void						Dead_FocusRotIcon(CGameObject* pTarget);

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
	class CMonster* m_pTargetMonster = nullptr;

private:
	vector<class CKena_Parts*>				m_vecPart;
	vector<class CSpiritArrow*>				m_vecArrow;
	class CSpiritArrow*							m_pCurArrow = nullptr;
	vector<class CRotBomb*>					m_vecBomb;
	class CRotBomb*								m_pCurBomb = nullptr;
	map<const string, class CEffect_Base*>	m_mapEffect;
	vector<_float4>								m_vecWeaposPos;

private:
	/* State variables*/
	_bool						m_bAttack = false;
	_bool						m_bHeavyAttack = false;
	_bool						m_bPerfectAttack = false;
	_bool						m_bParry = false;
	_bool						m_bParryLaunch = false;
	_uint						m_iCurParryFrame = 12;
	_uint						m_iParryFrameCount = 12;
	CGameObject*			m_pAttackObject = nullptr;
	_float						m_fHitStopTime = 0.f;
	_bool						m_bLocalMoveLock = false;
	_bool						m_bCommonHit = false;
	_bool						m_bHeavyHit = false;
	_float4					m_vDamagedDir;
	DAMAGED_FROM			m_eDamagedDir;

	_bool						m_bSprint = false;
	_bool						m_bTeleportFlower = false;

	_bool						m_bAim = false;
	_bool						m_bBow = false;
	_bool						m_bInjectBow = false;
	_bool						m_bBomb = false;
	_bool						m_bInjectBomb = false;
	_bool						m_bPulse = false;

	_float						m_fInertia = 5.f;

	_bool						m_bOnGround = true;
	_bool						m_bJump = false;
	_bool						m_bPulseJump = false;
	_float						m_fGravity = 9.81f;
	_float						m_fInitJumpSpeed;
	_float						m_fCurJumpSpeed;

	_bool						m_bRotWispInteractable = false;

	/* Effect Control */
	_bool						m_bTrailON = false;
	_bool						m_bChargeLight = false;

	/* Shader */
	_float						m_fSSSAmount = 0.01f;
	_float4					m_vSSSColor = _float4(0.8f, 0.7f, 0.6f, 1.f);
	_float4					m_vMulAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float4					m_vEyeAmbientColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float						m_fLashWidth = 10.f;
	_float						m_fLashDensity = 10.f;
	_float						m_fLashIntensity = 10.f;

	/* UI */
	CUI_RotIcon*				m_pUI_FocusRot;
	CUI_FocusMonster*		m_pUI_FocusMonster;

private:
	HRESULT					Ready_Parts();
	HRESULT					Ready_Arrows();
	HRESULT					Ready_Bombs();
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
	void						TurnOnAttack(_bool bIsInit, _float fTimeDelta);
	void						TurnOffAttack(_bool bIsInit, _float fTimeDelta);
	void						TurnOnTrail(_bool bIsInit, _float fTimeDelta);
	void						TurnOffTrail(_bool bIsInit, _float fTimeDelta);
	void						TurnOnFootStep_Left(_bool bIsInit, _float fTimeDelta);
	void						TurnOnFootStep_Right(_bool bIsInit, _float fTimeDelta);
	void						TurnOnCharge(_bool bIsInit, _float fTimeDelta);
	void						TurnOffCharge(_bool bIsInit, _float fTimeDelta);
	void						TurnOnPulseJump(_bool bIsInit, _float fTimeDelta);
	void						TurnOnHeavyAttack_Into(_bool bIsInit, _float fTimeDelta);
	void						TurnOnInteractStaff(_bool bIsInit, _float fTimeDelta);

	void						TurnOnPulseParry(_bool bIsInit, _float fTimeDelta);
	void						TurnOnPulseParryHand(_bool bIsInit, _float fTimeDelta);
	void						TurnOnPulseParryRange(_bool bIsInit, _float fTimeDelta);

public:
	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, _float>		m_PlayerDelegator;
	Delegator<CUI_ClientManager::UI_PRESENT, CUI_ClientManager::UI_FUNCTION, CKena*>		m_PlayerPtrDelegator;
	//Delegator<CUI_ClientManager::UI_PRESENT, _float, _float, _float, _float>				m_PlayerAmmoDelegator;


	_float m_fTest = 0.f;
	_bool	m_bStateLock;

public:
	static CKena*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;

	virtual _int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

	void Test_Raycast();
	void Set_RopeRotRockPtr(class CRope_RotRock* pObject) { m_pRopeRotRock = pObject; }
	void Set_FirstRotPtr(class CRot* pFirstRot) { m_pFirstRot = pFirstRot; }
	
	class CKena_Status*	Get_KenaStatusPtr() { return m_pKenaStatus; }
};

END