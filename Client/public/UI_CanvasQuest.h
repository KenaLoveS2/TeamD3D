#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasQuest final : public CUI_Canvas
{
	/* should be same with the order of m_vecNode.push_back()*/
	enum UI_ORDER { UI_MAIN, UI_END	};

private:
	CUI_CanvasQuest(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasQuest(const CUI_CanvasQuest& rhs);
	virtual ~CUI_CanvasQuest() = default;

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

public:
	static	CUI_CanvasQuest*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END