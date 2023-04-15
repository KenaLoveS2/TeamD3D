#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeTimeAtk final : public CUI_Node
{
private:
	CUI_NodeTimeAtk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeTimeAtk(const CUI_NodeTimeAtk& rhs);
	virtual ~CUI_NodeTimeAtk() = default;

public:
	void	Timer_On() { m_bStart = true; m_fTime = 0.f; m_fTimeAcc = 0.f; }

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

	_bool					m_bStart = false;
	_float					m_fTime;
	_float					m_fTimeAcc;

public:
	static	CUI_NodeTimeAtk*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
