#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeChat final :public CUI_Node
{
private:
	CUI_NodeChat(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeChat(const CUI_NodeChat& rhs);
	virtual ~CUI_NodeChat() = default;

public:
	void	Set_String(wstring wstr, _float fCorrectY = 0.f, _bool bTypeLine = false);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_tchar*					m_szChat;
	_float					m_fCorrectX;
	_float					m_fCorrectY;

	_bool					m_bTypeLine;

public:
	static	CUI_NodeChat*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END