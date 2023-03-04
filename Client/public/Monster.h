#pragma once
#include "GameObject.h"
#include "Delegator.h"
#include "UI_ClientManager.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CRenderer;
class CNavigation;
class CFSMComponent;
END

BEGIN(Client)

class CMonster  : public CGameObject
{
protected:
	enum MonsterShaderPass
	{ DEFAULT,
		AO_R_M,
		AO_R_M_E,
		AO_R_M_G,
		AO_R_M_O,
		AO_R_M_EEM,
		SEPARATE_AO_R_M_E,
		MASK,
		PASS_END};

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

public:
	_bool	AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);
	_bool   AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate);
	_bool   DistanceTrigger(_float distance);
	_bool	TimeTrigger(_float Time1, _float Time2);
	_float	DistanceBetweenPlayer();

protected:
	CRenderer*					m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*						m_pRangeCol = nullptr;
	CNavigation*				m_pNavigationCom = nullptr;
	CFSMComponent*		m_pFSM = nullptr;
	class CGameObject*		m_pKena = nullptr;
	_float4							m_pKenaPos;

protected:
	_bool							m_bSpawn = false;
	_float							m_fIdletoAttackTime = 0.f;

protected:
	virtual	HRESULT			SetUp_State() PURE;
	virtual	HRESULT			SetUp_Components() PURE;
	virtual	HRESULT			SetUp_ShaderResources() PURE;
	virtual  HRESULT			SetUp_ShadowShaderResources() PURE;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr)  PURE;
	virtual void					Free() override;
};

END