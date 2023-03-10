#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeRotLevel final : public CUI_Node
{
private:
	CUI_NodeRotLevel(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeRotLevel(const CUI_NodeRotLevel& rhs);
	virtual ~CUI_NodeRotLevel() = default;

public:
	void	Set_Info(_int iLevel, _int iTotal, _int iNow);

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
	_tchar*					m_szInfo;
	_int					m_iLevel;
	_int					m_iRotTotal;
	_int					m_iRotNow;

public:
	static	CUI_NodeRotLevel*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END