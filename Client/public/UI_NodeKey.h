#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeKey final : public CUI_Node
{
public:
	enum EVENT		{ EVENT_FADE ,EVENT_END };
	enum KEYTYPE	{ TYPE_R, TYPE_B, TYPE_Q, TYPE_LB, TYPE_RB, TYPE_END };

private:
	CUI_NodeKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeKey(const CUI_NodeKey& rhs);
	virtual ~CUI_NodeKey() = default;

public:
	void	Set_Key(wstring wstr, KEYTYPE eType, _float fTerm = 0.0f);
	void	SetOff_Key();

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
	//KEY					m_eState;
	_tchar*					m_szKeyDesc;

	/* Appear */
	_float					m_fTime;
	_float					m_fTimeAcc;
	_float					m_fTerm;

	/* Idle */
	_float					m_fRotSpeed;
	_float					m_fRotAngle; /* ring ring ring */

public:
	static	CUI_NodeKey*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END