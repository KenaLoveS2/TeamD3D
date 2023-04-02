#pragma once
#include "GameObject.h"
#include "Monster_Status.h"
#include "EnemyWisp.h"
#include "UI_MonsterHP.h"

#include "Kena.h"
#include "E_KenaHit.h"
#include "Kena_Status.h"
#include "RotForMonster.h"
#include "Camera.h"
#include "Camera_Player.h"

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
public:
	typedef  struct tagMonsterDesc
	{
		_int			iRoomIndex;
		_float4x4 WorldMatrix;
	}DESC;

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
		BOSS_AO_R_M,
		BOSS_AO_R_M_E,
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
	_vector						Get_Position();
	DESC							Get_Desc() { return m_Desc; };
	virtual _vector				Get_FocusPosition();
	const _bool&					Is_Dead() const { return m_bDeath; }

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
	virtual void					Calc_RootBoneDisplacement(_fvector vDisplacement) override;

	void								Bind(class CRotForMonster* pGameObject[], _int iRotCnt);
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

	CModel*						Get_Model() { return m_pModelCom; }

	virtual void					AdditiveAnim(_float fTimeDelta);
	void								Call_RotIcon();
	void								Call_MonsterFocusIcon();
	virtual HRESULT			Ready_EnemyWisp(const _tchar* szEnemyWispCloneTag);

	_bool							Get_Bind() { return m_bBind; }

	/* MovementTrail Particle */
	HRESULT						SetUp_MovementTrail();
	void								Update_MovementTrail(const char* pBoneTag);

protected:
	class CGameInstance*		m_pGameInstance = nullptr;

	PLAYERLOOKAT_DIR		m_PlayerLookAt_Dir = PLAYERLOOKAT_DIREND;
	DESC								m_Desc;
	CRenderer*						m_pRendererCom = nullptr;
	CShader*							m_pShaderCom = nullptr;
	CTexture*							m_pDissolveTextureCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CMonster_Status*			m_pMonsterStatusCom = nullptr;

	CFSMComponent*			m_pFSM = nullptr;
	class CKena*					m_pKena = nullptr;	

	CUI_MonsterHP*				m_pUIHPBar;
	class CEnemyWisp*			m_pEnemyWisp = nullptr;
	static _float4					m_vKenaPos;

	class CRotForMonster*	m_pRotForMonster[8] = { nullptr, };
	class CE_KenaHit*			m_pKenaHit = nullptr;
	class CE_RectTrail*			m_pMovementTrail = nullptr;

	_uint m_iNumMeshes = 0;
	_float m_fKenaDistance = 0.f;

protected:
	_bool	m_bWeaklyHit = false;
	_bool	m_bStronglyHit = false;
	_bool	m_bBind = false;
	_bool	m_bReadySpawn = false;
	_bool	m_bSpawn = false;
	_bool	m_bDying = false;
	_bool	m_bDeath = false;
	
	_bool   m_bRealAttack = false;

	_bool   m_bIsFightReady = false;
	_bool	m_bDissolve = false;
	_bool	m_bWispEnd = false;

	_float	m_fHitStopTime = 0.f;
	_float  m_fDissolveTime = 0.0f;
	_float  m_fSpawnRange   = 8.f;

protected:
	virtual void					Update_Collider(_float fTimeDelta) PURE;
	virtual	HRESULT			SetUp_State() PURE;
	virtual	HRESULT			SetUp_Components();
	virtual	HRESULT			SetUp_ShaderResources() PURE;
	virtual HRESULT			SetUp_ShadowShaderResources() PURE;
	virtual HRESULT			SetUp_UI(_float fOffsetY = 0.2f);

public:
	virtual CGameObject*	Clone(void* pArg = nullptr)  PURE;
	virtual void					Free() override;
	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);

	CMonster_Status* Get_MonsterStatusPtr() { return m_pMonsterStatusCom; }
	HRESULT Bind_Dissolove(class CShader* pShader);
	_bool IsRealAttack() { return m_bRealAttack; }

	void Set_DeathFlag(_bool bFlag) { m_bDeath = bFlag; }
	void Set_Dying(_uint iDeathAnimIndex);
	void Clear_Death();

	void Start_Bind(_uint iBindAnimIndex);
	void End_Bind();

	static void Set_MonsterUseKenaPos(_float4 vKenaPos) { m_vKenaPos = vKenaPos ; }
	static _float4 Get_MonsterUseKenaPos() { return m_vKenaPos; }

	_bool IsParried();
	_float Get_DissolveTime() { return m_fDissolveTime; }

	void Attack_Start(_uint iAnimIndex);
	void Attack_End(_uint iAnimIndex);

	virtual void Spawn_ByMaster(CMonster* pMaster, _float4 vPos) {}
	virtual void Clear_ByMinion(CMonster* pMinion) {}

	inline _bool IsFightReady() { return m_bIsFightReady; }
	void Execute_Dying();

	void Start_Spawn();
	void Tick_Spawn(_float fTimeDelta);
	void End_Spawn();
};

END