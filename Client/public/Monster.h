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
class CStateMachine;
END

BEGIN(Client)

class CMonster  : public CGameObject
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	const _double&			Get_AnimationPlayTime();

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual HRESULT			Call_EventFunction(const string& strFuncName) override;
	virtual void					Push_EventFunctions() override;

protected:
	CRenderer*					m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*						m_pRangeCol = nullptr;
	CNavigation*				m_pNavigationCom = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;

protected:
	virtual	HRESULT			SetUp_Components() PURE;
	virtual	HRESULT			SetUp_ShaderResources() PURE;
	virtual  HRESULT			SetUp_ShadowShaderResources() PURE;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr)  PURE;
	virtual void					Free() override;
};

END