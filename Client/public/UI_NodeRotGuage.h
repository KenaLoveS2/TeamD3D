#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeRotGuage final : public CUI_Node
{
public:
	enum EVENT_ID { EVENT_GUAGE, EVENT_END };
private:
	CUI_NodeRotGuage(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeRotGuage(const CUI_NodeRotGuage& rhs);
	virtual ~CUI_NodeRotGuage() = default;

public: /* Events */
	void	Set_Guage(_float fGuage);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT		SetUp_Components() override;
	virtual HRESULT		SetUp_ShaderResources() override;

public:
	static	CUI_NodeRotGuage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END