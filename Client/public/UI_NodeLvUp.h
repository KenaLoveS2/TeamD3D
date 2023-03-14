#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeLvUp final : public CUI_Node
{
private:
	CUI_NodeLvUp(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeLvUp(const CUI_NodeLvUp& rhs);
	virtual ~CUI_NodeLvUp() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void					Appear(_uint iLevel);

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_tchar*					m_szTitle;
	_tchar*					m_szLevel;

public:
	static	CUI_NodeLvUp*			Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END