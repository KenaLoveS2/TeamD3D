#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

#define MAX_HAT_COUNT		16

BEGIN(Client)
class CKena;
class CUI_CanvasHatCart final : public CUI_Canvas
{
public:
	enum UI_ORDER {
		UI_BG = 0,
		UI_ITEMBAR0 = 1, UI_ITEMBAR15 = 16,
		UI_SCROLLLINE = 17, UI_SCROLLBAR = 18, 
		UI_UPSIDE = 19, UI_DOWNSIDE = 20,
		UI_CRYSTAL = 21,
		UI_END };
private:
	CUI_CanvasHatCart(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasHatCart(const CUI_CanvasHatCart& rhs);
	virtual ~CUI_CanvasHatCart() = default;

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
	void	BindFunction(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, CKena* pPlayer);
	void	Picking();
	void	Shopping();

private:
	vector<pair<wstring, _int>>		m_vecHats;
	CKena*	m_pPlayer;
	_int	m_iPickedIndex;
	_int	m_iHatCount[MAX_HAT_COUNT];

public:
	static	CUI_CanvasHatCart*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END