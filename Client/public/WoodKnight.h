#pragma once
#include "Monster.h"

BEGIN(Client)

class CWoodKnight : public CMonster
{
private:
	enum ANIMATION
	{
		ATTACK90_L,
		ATTACK90_R,
		ALERT,
		ATTACK180,
		BIND,
		BLOCK_COUNTERATTACK,
		BLOCK_EXIT,
		BLOCK_INTO,
		BLOCK_LOOP,
		BLOCKATTACK_180,
		CHARGEATTACK,
		COMBOATTACK_LUNGE,
		COMBOATTACK_OVERHEAD,
		DEATH,
		DOUBLEATTACK,
		HITDEFLECT,
		IDLE,
		IDLE_ADDITIVE, // ADDITIVE
		INTOCHARGE,
		INTOCHARGE_BACKUP,
		JUMPBACK,
		LOOK_DOWN,
		LOOK_LEFT,
		LOOK_RIGHT,
		LOOK_UP,
		OVERHEADSLAMATTACK,
		PUSHBACK,
		RANGEDATTACK,
		RUN,
		RUN_ROOTMOTION,
		SLEEP,
		STAGGER_ALT,
		STAGGER_B,
		STAGGER_L,
		STAGGER_R,
		STRAFE_L,
		STRAFE_R,
		TAKEDAMAGE, //ADDITIVE
		TAUNT,
		TURN_180,
		TURNL_90,
		TURNR_90,
		UPPERCUTATTACK,
		WALK,
		WALKBACKWARDS,
		ANIMATION_END
	};

	enum ATTACKTYPE
	{
		AT_RANGEDATTACK,
		AT_CHARGEATTACK,
		AT_COMBOATTACK_LUNGE,
		AT_COMBOATTACK_OVERHEAD,
		AT_DOUBLEATTACK,
		AT_UPPERCUTATTACK,
		ATTACKTYPE_END
	};

	enum BLOCKAFTERTYPE
	{
		BA_FRONT,
		BA_BACK,
		BA_NONE,
		BLOCKAFTERTYPE_END
	};

	enum COLLIDERTYPE
	{
		COLL_WEAPON = 0,
		COLL_PUNCH,
		COLL_END
	};

private:
	CWoodKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWoodKnight(const CWoodKnight& rhs);
	virtual ~CWoodKnight() = default;

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

private:
	virtual  HRESULT			SetUp_State() override;
	virtual	HRESULT			SetUp_Components() override;
	virtual	HRESULT			SetUp_ShaderResources() override;
	virtual  HRESULT			SetUp_ShadowShaderResources() override;

private:
	void	Update_Collider(_float fTimeDelta) override;
	void	AdditiveAnim(_float fTimeDelta) override;

private:
	void Set_AttackType();
	void Reset_Attack();
	void Tick_Attack(_float fTimeDelta);

	void Set_BlockAfterType();
	void Reset_BlockAfterType();

private:
	_float	m_fIdletoAttackTime = 0.f;

	_int		m_iAttackType = ATTACKTYPE_END;
	
	_bool	m_bRangedAttack = false;
	_bool	m_bChargeAttack = false;
	_bool   m_bComboAttack_Lunge = false;
	_bool	m_bComboAttack_Overhead = false;
	_bool	m_bDoubleAttack = false;
	_bool	m_bUppercutAttack = false;

	_bool	m_isCloseRange = false;
	_bool	m_isMiddleRange = false;
	_bool	m_isFarRange = false;

	_bool	m_bBlock = false;

	_float	m_fBlocktoAttackTime = 0.f;
	_int		m_iBlockType = BLOCKAFTERTYPE_END;

	_bool	m_bBlockAfterFront = false;
	_bool	m_bBlockAfterBack = false;
	_bool	m_bBlockNone = false;

private:
	vector<wstring> m_vecColliderName;
	vector<_float3> m_vecPivot;
	vector<_float3> m_vecPivotScale;
	vector<_float3> m_vecPivotRot;

	enum COPY_SOUND_KEY {
		CSK_TENSE, CSK_ATTACK_START, CSK_ATTACK, CSK_IDLE, CSK_WOOSH, CSK_WALK, CSK_SLAM, CSK_DIE, CSK_CATCH, CSK_HURT,
		COPY_SOUND_KEY_END,
	};

	_tchar* m_pCopySoundKey[COPY_SOUND_KEY_END] = { nullptr, };

public:
	static CWoodKnight*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;

	virtual void Create_CopySoundKey() override;

	void Play_WoooshSound(_bool bIsInit, _float fTimeDelta);
	void Play_WalkSound(_bool bIsInit, _float fTimeDelta);
	void Play_SlamSound(_bool bIsInit, _float fTimeDelta);
	void Play_CatchSound(_bool bIsInit, _float fTimeDelta);
	void Play_HurtSound(_bool bIsInit, _float fTimeDelta);
	void LookAt_Kena(_bool bIsInit, _float fTimeDelta);
};

END