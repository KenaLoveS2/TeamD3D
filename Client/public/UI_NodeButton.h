#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeButton final : public CUI_Node
{
public:
	enum TYPE { TYPE_CONFIRM, TYPE_END };
private:
	CUI_NodeButton(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeButton(const CUI_NodeButton& rhs);
	virtual ~CUI_NodeButton() = default;

public:
	void	Setting(wstring wstrName, TYPE eType);

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
	_tchar*					m_szName;
	TYPE					m_eType;
	_float					m_fAlpha;

public:
	static	CUI_NodeButton*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
