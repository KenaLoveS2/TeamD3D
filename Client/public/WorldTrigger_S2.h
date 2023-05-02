#pragma once

#include "Client_Defines.h"
#include "UI_ClientManager.h"
#include "Delegator.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Client)
class CKena;
class CWorldTrigger_S2 final : public CGameObject
{
public:
	enum TYPE { TYPE_QUEST, TYPE_ALARM, TYPE_LINE, TYPE_END };

	typedef struct TriggerDesc
	{
		char		szName[MAX_PATH];
		TYPE		eType;
		_float		fData;
		char		szData[MAX_PATH];
		_tchar		wstrData[MAX_PATH];

		_float3		vPosition;
		_float3		vScale;
	}DESC;

private:
	CWorldTrigger_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWorldTrigger_S2(const CWorldTrigger_S2& rhs);
	virtual ~CWorldTrigger_S2() = default;

public:
	void	Set_TriggerOn()				{ m_bTriggerOn = true; }
	void	Set_Active(_bool bActive)	{ m_bActive = bActive; }

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual void					Imgui_RenderProperty() override;

public:
	HRESULT			Save_Data(Json* json, _uint iIndex);
	HRESULT			Load_Data(void* pArg);

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float>					m_TriggerDelegatorA;
	Delegator<CUI_ClientManager::UI_PRESENT, _bool, _float, wstring>	m_TriggerDelegatorB;

private:
	HRESULT			SetUp_Components();
	_bool			Check_Collision();

private:
	CRenderer*		m_pRendererCom;
	CCollider*		m_pColliderCom;

private:
	_bool			m_bActive;
	_bool			m_bTriggerOn;
	DESC			m_tDesc;
	CKena*			m_pPlayer;

public:
	static  CWorldTrigger_S2*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};
END
