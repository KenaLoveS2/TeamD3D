#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasAmmo final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_BOMBFRAME, UI_BOMBGUAGE, UI_ARROWGUAGE, UI_END };


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
	void	Function(CUI_ClientManager::UI_PRESENT eType, _float fValue);

private: /* Arrow */
	_uint	m_iNumArrows;
	_uint	m_iNumArrowNow;

public:
	static	CUI_CanvasAmmo*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END

