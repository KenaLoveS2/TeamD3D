#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

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

class CRot final : public CGameObject
{
	friend class CRot_State;

private:
	static _uint m_iEveryRotCount;
	static vector<CRot*> m_EveryRotList;
	_uint m_iThisRotIndex = 0;
	

public:
	CRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRot(const CRot& rhs);
	virtual ~CRot() = default;

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

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_pRangeCol = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CStateMachine*		m_pStateMachine = nullptr;
	class CRot_State*		m_pRotState = nullptr;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					SetUp_ShadowShaderResources();
	void							Test(_bool bIsInit, _float fTimeDelta);

public:
	static CRot*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END