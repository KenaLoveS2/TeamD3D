#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Engine)
class CUI_Canvas;
END

BEGIN(Client)
class CUI_NodeConfWindow final : public CUI_Node
{
private:
	CUI_NodeConfWindow(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeConfWindow(const CUI_NodeConfWindow& rhs);
	virtual ~CUI_NodeConfWindow() = default;

public:
	void	Set_Message(wstring msg, CUI_Canvas* pCaller);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_tchar*				m_Message;
	CUI_Canvas*			m_pCaller;

public:
	static	CUI_NodeConfWindow*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END