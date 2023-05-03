#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeHUDRot final : public CUI_Node
{
public:
	enum	EVENT_ID { EVENT_TEXCHANGE, EVENT_ANIM, EVENT_END };
private:
	CUI_NodeHUDRot(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeHUDRot(const CUI_NodeHUDRot& rhs);
	virtual ~CUI_NodeHUDRot() = default;

public:
	void	Change_RotIcon(_float fIcon);
	_bool	Is_ChangeFinished();
public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	static	CUI_NodeHUDRot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
