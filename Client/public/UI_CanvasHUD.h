#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasHUD final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER	{ UI_HPBAR, UI_HPGUAGE, UI_SHIELDBAR, UI_SHIELD, UI_ROT, 
		UI_PIPBAR1, UI_PIPGAUGE1, UI_PIPBAR2, UI_PIPGAUGE2, UI_PIPBAR3, UI_PIPGAUGE3,
		UI_PIPEFFECT, UI_END };
	enum PIP_ID		{ PIP_1, PIP_2, PIP_3, PIP_END };
		
private:
	CUI_CanvasHUD(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasHUD(const CUI_CanvasHUD& rhs);
	virtual ~CUI_CanvasHUD() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public: /* For. Events */
	_bool	Is_PipAllFull() {
		if (m_iNumPips == m_iNumPipsNow) return true;
		else return false;
	}
	void	PlusPipCount() { m_iNumPipsNow += 1; }

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;

private: /* Bind Functions */
	void	Function(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue);
	void	Function(CUI_ClientManager::UI_PRESENT eType, _float fValue);

private:
	void	Default(CUI_ClientManager::UI_PRESENT eType, _float fValue);
	void	LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel);

private:
	_uint	m_iNumPips;
	_uint	m_iNumPipsNow; /* Full Pip Left */
	_int	m_Pips[PIP_END];

public:
	static	CUI_CanvasHUD*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END
