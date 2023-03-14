#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CRotForMonster final : public CGameObject
{
public:
	typedef struct tagRotForMonsterDesc
	{
		_float4 vPivotPos;
	} DESC;


private:
	CRotForMonster(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRotForMonster(const CRotForMonster& rhs);
	virtual ~CRotForMonster() = default;

public:
	class CMonster*		Get_Target() { return m_pTarget; }
	void				Set_Target(CMonster* pMonster) { m_pTarget = pMonster; }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;
	virtual HRESULT Call_EventFunction(const string& strFuncName) override;
	virtual void Push_EventFunctions() override;

	void Bind(_bool bBind, class CMonster* pGameObject);


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT SetUp_FSM();
	_bool		AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);
private:
	CRenderer*			 m_pRendererCom = nullptr;
	CShader*				 m_pShaderCom = nullptr;
	CModel*				 m_pModelCom = nullptr;
	CFSMComponent* m_pFSM = nullptr;

	_float m_fLimit = 0.5f;

private:
	_bool						  m_bWakeUp = false;
	_bool						  m_bCreateStart = false;
	_bool						  m_bBind = false;
	class CMonster*		  m_pTarget = nullptr;
	DESC						  m_Desc;
public:
	static CRotForMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END