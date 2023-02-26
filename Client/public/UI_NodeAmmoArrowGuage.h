#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeAmmoArrowGuage final : public CUI_Node
{
public:
	enum EVENT_ID { EVENT_GUAGE, EVENT_END };

	CUI_NodeAmmoArrowGuage(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeAmmoArrowGuage(const CUI_NodeAmmoArrowGuage& rhs);
	virtual ~CUI_NodeAmmoArrowGuage() = default;

public:
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

private:
	_bool			m_bFullFilled;

public:
	static	CUI_NodeAmmoArrowGuage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr);
	virtual void						Free() override;
};
END