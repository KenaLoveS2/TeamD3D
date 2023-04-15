#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"
#include "UI_ClientManager.h"

BEGIN(Client)
class CUI_CanvasMiniGame final : public CUI_Canvas
{
public:
	enum UI_ORDER { UI_END };

private:
	CUI_CanvasMiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasMiniGame(const CUI_CanvasMiniGame& rhs);
	virtual ~CUI_CanvasMiniGame() = default;

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
	
private:
	_bool	m_bResultShow;

public:
	static	CUI_CanvasMiniGame* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END

