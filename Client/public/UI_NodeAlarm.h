#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeAlarm final : public CUI_Node
{
public:
	enum EVENT { EVENT_FADE, EVENT_END };
	enum TYPE { TYPE_CHEST, TYPE_HAT, TYPE_MASK_HUNTER, TYPE_MASK_WARRIOR, TYPE_MASK_SHAMAN, TYPE_END };

private:
	CUI_NodeAlarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeAlarm(const CUI_NodeAlarm& rhs);
	virtual ~CUI_NodeAlarm() = default;

public:
	void	Set_Alarm(TYPE eType, _float fTerm = 0.0f);

public:
	virtual HRESULT			Initialize_Prototype()					override;
	virtual HRESULT			Initialize(void* pArg)					override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr)	override;
	virtual void			Tick(_float fTimeDelta)					override;
	virtual void			Late_Tick(_float fTimeDelta)			override;
	virtual HRESULT			Render()								override;

private:
	virtual HRESULT			SetUp_Components()						override;
	virtual HRESULT			SetUp_ShaderResources()					override;

private:
	_tchar*					m_szTitle;
	_tchar*					m_szDesc;

	/* Appear */
	_float					m_fTime;
	_float					m_fTimeAcc;
	_float					m_fTerm;


public:
	static	CUI_NodeAlarm*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END
