#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeTitle final : public CUI_Node
{
private:
	CUI_NodeTitle(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeTitle(const CUI_NodeTitle& rhs);
	virtual ~CUI_NodeTitle() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void					Appear(wstring title, _uint iIndex);

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_tchar*					m_szTitle;
	_bool					m_bStart;
	_float					m_fAlpha;
	_float					m_fSpeed;
	_float					m_fDurTime;
	_float					m_fDurTimeAcc;

public:
	static	CUI_NodeTitle*			Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END