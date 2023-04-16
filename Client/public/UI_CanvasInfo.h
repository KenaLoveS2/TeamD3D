#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasInfo final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_VIDEO, UI_END };
	enum INFO { INFO_FIGHTIN, INFO_END };
private:
	CUI_CanvasInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasInfo(const CUI_CanvasInfo& rhs);
	virtual ~CUI_CanvasInfo() = default;

public:
	void	Call_InfoWindow(INFO eInfo);

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
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, _float fValue);

	_bool m_bCapture = false;
	_bool m_bReturn = false;

public:
	static	CUI_CanvasInfo*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END