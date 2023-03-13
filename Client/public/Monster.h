#pragma once
#include "GameObject.h"
#include "Delegator.h"
#include "UI_ClientManager.h"
#include "Monster_Status.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

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

public:
	_bool							AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);
	_bool							AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate);
	_bool							DistanceTrigger(_float distance);
	_bool							IntervalDistanceTrigger(_float min, _float max);
	_bool							TimeTrigger(_float Time1, _float Time2);
	_float							DistanceBetweenPlayer();
	_float							Calc_PlayerLookAtDirection();

	virtual void					AdditiveAnim(_float fTimeDelta);

protected:
	PLAYERLOOKAT_DIR	m_PlayerLookAt_Dir = PLAYERLOOKAT_DIREND;

	CRenderer*					m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CMonster_Status*			m_pMonsterStatusCom = nullptr;

	CFSMComponent*		m_pFSM = nullptr;
	class CKena*		m_pKena = nullptr;	
	_float4							m_vKenaPos;

	

protected:
	_bool	m_bWeaklyHit = false;
	_bool	m_bStronglyHit = false;
	_bool	m_bBind = false;

protected:
	virtual void					Update_Collider(_float fTimeDelta) PURE;
	virtual	HRESULT			SetUp_State() PURE;
	virtual	HRESULT			SetUp_Components();
	virtual	HRESULT			SetUp_ShaderResources() PURE;
	virtual  HRESULT			SetUp_ShadowShaderResources() PURE;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr)  PURE;
	virtual void					Free() override;
	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);

	CMonster_Status* Get_MonsterStatusPtr() { return m_pMonsterStatusCom; }
};

END