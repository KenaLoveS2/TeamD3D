#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

/* This canvas give Data to Canvas Aim */

BEGIN(Client)
class CUI_CanvasAmmo final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_BOMBFRAME1, UI_BOMBGUAGE1, 
		UI_BOMBFRAME2, UI_BOMBGUAGE2,
		UI_ARROWGUAGE, 
		UI_BOMBEFFECT, UI_ARROWEFFECT, UI_END };
	enum AIM_UI { AIM_ARROW, AIM_BOMB, AIM_END };

	enum BOMB_ID { BOMB_1, BOMB_2 ,BOMB_END };


private:
	CUI_CanvasAmmo(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasAmmo(const CUI_CanvasAmmo& rhs);
	virtual ~CUI_CanvasAmmo() = default;

public: /* For. Events */
	_uint	Is_ArrowFull() {
		if (m_iNumArrows == m_iNumArrowNow) return true;
		else return false;
	}
	void	FillArrow() { m_iNumArrowNow += 1; }

	_uint	Is_BombFull() {
		if (m_iNumBombs == m_iNumBombNow) return true;
		else return false;
	}
	void	FillBomb() { m_iNumBombNow += 1; }


public: 
	void	ConnectToAimUI(AIM_UI eUIType, _int iParam, CUI* pUI = nullptr);


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
	void	Function(CUI_ClientManager::UI_PRESENT eType, CUI_ClientManager::UI_FUNCTION eFunc, _float fValue);
	void	Function(CUI_ClientManager::UI_PRESENT eType, _float fValue);

private:
	void	Default(CUI_ClientManager::UI_PRESENT eType, _float fValue);
	void	LevelUp(CUI_ClientManager::UI_PRESENT eType, _int iLevel);

private: 
	/* Arrow */
	_uint	m_iNumArrows;
	_uint	m_iNumArrowNow;

	/* Bomb */
	_uint	m_iNumBombs;
	_uint	m_iNumBombNow;
	_int	m_Bombs[BOMB_END];

public:
	static	CUI_CanvasAmmo*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END

