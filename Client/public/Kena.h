#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Delegator.h"

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

class CKena final : public CGameObject
{
	friend class CKena_State;

private:
	CKena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena(const CKena& rhs);
	virtual ~CKena() = default;

public:
	const _double&		Get_AnimationPlayTime();

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void			Imgui_RenderProperty() override;
	virtual void			ImGui_AnimationProperty() override;
	virtual void			Update_Child() override;
	virtual HRESULT		Call_EventFunction(const string& strFuncName) override;
	virtual void			Push_EventFunctions() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*				m_pRangeCol = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CStateMachine*		m_pStateMachine = nullptr;
	class CKena_State*	m_pKenaState = nullptr;

	class CCamera_Player*	m_pCamera = nullptr;

private:
	vector<class CKena_Parts*>	m_vecPart;

private:
	_bool					m_bAttack = false;

private:
	HRESULT				Ready_Parts();
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();
	HRESULT				SetUp_State();
	void					Test(_bool bIsInit, _float fTimeDelta);

public:
	Delegator<_int>		m_PlayerDelegator;


public:
	static CKena*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END