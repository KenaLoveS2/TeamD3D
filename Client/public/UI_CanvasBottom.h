#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasBottom final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_LETTERBOX, UI_CHAT, UI_KEY1, UI_KEY2, UI_END };
private:
	CUI_CanvasBottom(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasBottom(const CUI_CanvasBottom& rhs);
	virtual ~CUI_CanvasBottom() = default;

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

private: /* Bind Functions */
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, _bool bValue, _float fValue, wstring wstr);
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue);

public:
	static	CUI_CanvasBottom*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END
