#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasAim final : public CUI_Canvas
{
public:
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_LINELEFT, UI_LINERIGHT, 
				UI_ARROW1, UI_ARROW2, UI_ARROW3, UI_ARROW4, UI_ARROW5,
				UI_BOMB1, UI_BOMB2,
				UI_END };

	enum ARROW_ID { ARROW_1, ARROW_2, ARROW_3, ARROW_4, ARROW_5, ARROW_END };
	enum BOMB_ID { BOMB_1, BOMB_2, BOMB_END };

private:
	CUI_CanvasAim(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasAim(const CUI_CanvasAim& rhs);
	virtual ~CUI_CanvasAim() = default;


public:
	void	Set_Arrow(_int iIndex, _int iTag);

public: /* For. Events */

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

private:  /* Test .... Transform Event를 따로 뺄지 말지 생각중 */
	_bool		m_bStart;
	_float		m_fTime;
	_float		m_fTimeAcc;

private:
	_int	m_Arrows[ARROW_END];
	_int	m_Bombs[BOMB_END];

private: /* Bind Functions */
	void	Function(CUI_ClientManager::UI_PRESENT eType, _float fValue);
	

public:
	static	CUI_CanvasAim*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr);
	virtual void			Free() override;
};
END

