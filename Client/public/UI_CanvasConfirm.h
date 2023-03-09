#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasConfirm final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER {	UI_YES, UI_NO, UI_END };
private:
	CUI_CanvasConfirm(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasConfirm(const CUI_CanvasConfirm& rhs);
	virtual ~CUI_CanvasConfirm() = default;

public:
	void	Set_Message(wstring msg, CUI_Canvas* pCaller);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;

private:
	void		Picking();
private: /* Bind Functions */
	void		BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue);
private:
	_bool		m_bConfirm;
	_tchar*		m_Message;
	CUI_Canvas* m_pCaller;

public:
	static	CUI_CanvasConfirm*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
