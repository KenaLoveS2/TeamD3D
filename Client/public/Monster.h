#pragma once
#include "GameObject.h"
#include "Monster_Status.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)
class CUI_MonsterHP;
class CMonster  : public CGameObject
{
protected:
	enum MonsterShaderPass
	{
		DEFAULT,
		AO_R_M,
		AO_R_M_E,
		AO_R_M_G,
		AO_R_M_O,
		AO_R_M_EEM,
		SEPARATE_AO_R_M_E,
		MASK,
		SAPLING_BOMBUP,
		ALPHA_AO_R_M,
		SHADOW,
		PASS_END
	};

	enum PLAYERLOOKAT_DIR
	{
		LEFT,
		FRONT,
		RIGHT,
		BACK,
		PLAYERLOOKAT_DIREND
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	const _double&			Get_AnimationPlayTime();
	_fvector				Get_Position();

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
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void					Push_EventFunctions() override;
	virtual void					Calc_RootBoneDisplacement(_fvector vDisplacement) override;

	void								Bind() { m_bBind = true; }
	void								Spawn() { m_bSpawn = true; };
	void								StringlyHit() { m_bStronglyHit = true; }
	void								WeakleyHit() { m_bWeaklyHit = true; }

public:
	_bool							AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);
	_bool							AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate);
	_bool							DistanceTrigger(_float distance);
	_bool							IntervalDistanceTrigger(_float min, _float max);
	_bool							TimeTrigger(_float Time1, _float Time2);
	_float							DistanceBetweenPlayer();
	_float							Calc_PlayerLookAtDirection();

	virtual void					AdditiveAnim(_float fTimeDelta);
	void							Call_RotIcon();
	virtual HRESULT					Ready_EnemyWisp(const _tchar* szEnemyWispCloneTag);

protected:
	PLAYERLOOKAT_DIR	m_PlayerLookAt_Dir = PLAYERLOOKAT_DIREND;

	CRenderer*					m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pDissolveTextureCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CMonster_Status*			m_pMonsterStatusCom = nullptr;

	CFSMComponent*		m_pFSM = nullptr;
	class CKena*		m_pKena = nullptr;	

	CEffect_Base*		m_pEnemyWisp = nullptr;
	_float4							m_vKenaPos;

	CUI_MonsterHP*		m_pUIHPBar;

protected:
	_bool	m_bWeaklyHit = false;
	_bool	m_bStronglyHit = false;
	_bool	m_bBind = false;
	_bool	m_bSpawn = false;
	_bool	m_bDying = false;
	_bool	m_bDeath = false;

	_float m_fDissolveTime = 0.0f;


protected:
	virtual void					Update_Collider(_float fTimeDelta) PURE;
	virtual	HRESULT			SetUp_State() PURE;
	virtual	HRESULT			SetUp_Components();
	virtual	HRESULT			SetUp_ShaderResources() PURE;
	virtual  HRESULT			SetUp_ShadowShaderResources() PURE;
	virtual HRESULT			SetUp_UI();

public:
	virtual CGameObject*	Clone(void* pArg = nullptr)  PURE;
	virtual void					Free() override;
	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);

	CMonster_Status* Get_MonsterStatusPtr() { return m_pMonsterStatusCom; }
	HRESULT Bind_Dissolove(class CShader* pShader);
};

END